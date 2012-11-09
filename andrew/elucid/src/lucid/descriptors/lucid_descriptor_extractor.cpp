// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/descriptors/lucid_descriptor_extractor.h"
#include "lucid/tools/util.h"
#include "lucid/tools/timer.h"
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

// For pop count.
#include <nmmintrin.h>
#include <inttypes.h>

// For equality operator.
#include <emmintrin.h>


union __oword_t {
  __m128i m128i;
  uint64_t m128i_u64[2];
};

typedef union __oword_t __oword;

namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  LucidDescriptorExtractor::LucidDescriptorExtractor(uint patch_size,
                                                     uint blur_radius)
    : DescriptorExtractor("LUCID"),
      _patch_size(patch_size),
      _blur_radius(blur_radius)
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  LucidDescriptorExtractor::~LucidDescriptorExtractor()
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void LucidDescriptorExtractor::computeDescriptors(
    const cv::Mat& image,
    const std::vector<cv::KeyPoint>& key_points,
    std::vector<bool> *valid_descriptors,
    cv::Mat *descriptors) const
  {    

    cv::Mat blurred_image;
    cv::blur(image,
             blurred_image,
             cv::Size(_blur_radius, _blur_radius));

    std::clock_t start = std::clock();
    std::vector<cv::Mat> patches;
    Util::getImagePatches(_patch_size,
                          blurred_image,
                          key_points,
                          valid_descriptors,
                          &patches);

    std::clock_t stop = std::clock();
    std::cout << "Time to get image patches "
              << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
              << "ms"
              << std::endl;

    start = std::clock();
    int desc_width = patches[0].cols * patches[0].rows * patches[0].channels();
    cv::Mat descs;
    // if(desc_width < 2<<8) 
    // {
    //   // 8 bits supports a window size of 8x8 grayscale.
    //   descs = cv::Mat(patches.size(),
    //                   desc_width,
    //                   CV_8UC1);

    //   for(int i = 0; i < patches.size(); ++i)
    //   {
    //     if((*valid_descriptors)[i])
    //     {
    //       uchar *cur_desc = descs.ptr<uchar>(i);
    //       // Vectorize the image patch, an O(1) operation for cv::Mat.
    //       uchar *cur_patch = patches[i].data;
    //       Util::countingSort(desc_width, cur_patch, cur_desc);
    //     }
    //   }
    // } else if(desc_width < 2<<16)
    // {
      // 16 bits supports window sizes up to 256x256 grayscale and 147x147 rgb.
      descs = cv::Mat(patches.size(),
                      desc_width,
                      CV_16UC1);
      
      for(int i = 0; i < patches.size(); ++i)
      {
        if((*valid_descriptors)[i])
        {
          ushort *cur_desc = descs.ptr<ushort>(i);
          // Vectorize the image patch, an O(1) operation for cv::Mat.
          uchar *cur_patch = patches[i].data;
          
          Util::countingSort2(desc_width, cur_patch, cur_desc);
        }
      }
//    }
      stop = std::clock();
      std::cout << "Time to compute LUCID descriptors "
                << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
                << "ms"
                << std::endl;
      *descriptors = descs;
  }


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void LucidDescriptorExtractor::knnMatchDescriptors(
    const int k,
    const cv::Mat& test_descriptors,
    const cv::Mat& train_descriptors,
    const std::vector<bool>& valid_test_descriptors,
    const std::vector<bool>& valid_train_descriptors,
    std::vector<std::vector<cv::DMatch> > *matches) const
  {
    std::clock_t start = clock();
    int desc_width = test_descriptors.cols;

    for(int i = 0; i < test_descriptors.rows; ++i)
    {
      const ushort *test_desc = test_descriptors.ptr<ushort>(i);
      std::vector<cv::DMatch> cur_matches;
      
      for(int j = 0; j < train_descriptors.rows; ++j)
      {
        const ushort *train_desc = train_descriptors.ptr<ushort>(j);
        unsigned int cur_dist = 0;
        if(valid_test_descriptors[i] && valid_train_descriptors[j])
        {
          // for(int l = 0; l < desc_width; ++l)
          // {
          //   cur_dist += train_desc[l] != test_desc[l] ? 1 : 0;
          // }

          // Kendall's Tau
          for(int l1 = 0; l1 < desc_width; ++l1)
          {
            for(int l2 = l1 + 1; l2 < desc_width; ++l2)
            {
              cur_dist +=
                (train_desc[l1] < train_desc[l2]) &&
                (test_desc[l1] >= test_desc[l2]);
            }
          }

          cur_matches.push_back(cv::DMatch(i, j, cur_dist));
        }
      }
      
      // Sort by smallest distance.
      std::sort(cur_matches.begin(), cur_matches.end(), Util::compareMatches);    
      if(cur_matches.size() > k)
      {
        cur_matches.resize(k);
      }
      
      matches->push_back(cur_matches);

    }

    std::clock_t stop = std::clock();
    std::cout << "Time to match LUCID descriptors "
              << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
              << "ms"
              << std::endl;
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  void LucidDescriptorExtractor::matchDescriptors(
    const cv::Mat& test_descriptors,
    const cv::Mat& train_descriptors,
    const std::vector<bool>& valid_test_descriptors,
    const std::vector<bool>& valid_train_descriptors,
    std::vector<cv::DMatch> *matches) const
  {
    std::clock_t start = clock();
    int desc_width = test_descriptors.cols;

    register __oword xmm0;
    register __oword xmm1;
    
    // TODO: Compute mask for what descriptors to compare ahead of time
    // Figure out how to deal with descriptor lengths that are not divisible
    // by 8 or 16. Maybe just special case last bit of descriptor.
    
      for(int i = 0; i < test_descriptors.rows; ++i)
      {
        const ushort *test_desc = test_descriptors.ptr<ushort>(i);
        int best_match_idx = -1;
        uint best_match_distance = ~0;
        
        for(int j = 0; j < train_descriptors.rows; ++j)
        {
          if(valid_test_descriptors[i] && valid_test_descriptors[j])
          {
            const ushort *train_desc = train_descriptors.ptr<ushort>(j);
            uint cur_dist = 0;
            for(int k = 0; k < desc_width; ++k)
            {
              cur_dist += train_desc[k] != test_desc[k] ? 1 : 0;
            }

            if(cur_dist < best_match_distance)
            {
              best_match_distance = cur_dist;
              best_match_idx = j;
            }
          }
        }
        matches->push_back(cv::DMatch(i, best_match_idx, best_match_distance));
      }

    // if(desc_width < 2<<8)
    // {
    //   for(int i = 0; i < test_descriptors.rows; ++i)
    //   {
    //     const __m128i *test_desc = test_descriptors.ptr<__m128i>(i);
    //     int best_match_idx = -1;
    //     uint best_match_distance = ~0;
        
    //     for(int j = 0; j < train_descriptors.rows; ++j)
    //     {
    //       if(valid_test_descriptors[i] && valid_train_descriptors[j])
    //       {
    //         const __m128i *train_desc = train_descriptors.ptr<__m128i>(j);
    //         unsigned long int cur_dist = 0;
    //         for(int k = 0; k < desc_width / 16; ++k)
    //         {
    //           // Load descriptor elements for comparison.
    //           xmm0.m128i = _mm_load_si128(&(test_desc[k]));
    //           xmm1.m128i = _mm_load_si128(&(train_desc[k]));
              
    //           // Find difference
    //           xmm0.m128i = _mm_sad_epu8(xmm0.m128i, xmm1.m128i);
              
    //           // Sum upper and lower halfs 
    //           cur_dist += xmm0.m128i_u64[0] + xmm0.m128i_u64[1];
    //         }

    //         if(cur_dist < best_match_distance)
    //         {
    //           best_match_distance = cur_dist;
    //           best_match_idx = j;
    //         }
    //       }
    //     }
    //     matches->push_back(cv::DMatch(i, best_match_idx, best_match_distance));
    //   }
    // }
    // else if(desc_width < 2<<16)
    // {
    //   for(int i = 0; i < test_descriptors.rows; ++i)
    //   {
    //     const __m128i *test_desc = test_descriptors.ptr<__m128i>(i);
    //     int best_match_idx = -1;
    //     uint best_match_distance = ~0;
        
    //     for(int j = 0; j < train_descriptors.rows; ++j)
    //     {
    //       if(valid_test_descriptors[i] && valid_test_descriptors[j])
    //       {
    //         const __m128i *train_desc = train_descriptors.ptr<__m128i>(j);
    //         unsigned long int cur_dist = 0;
    //         for(int k = 0; k < desc_width; ++k)
    //         {
    //           // Load descriptor elements for comparison.
    //           xmm0.m128i = _mm_load_si128(&(test_desc[k]));
    //           xmm1.m128i = _mm_load_si128(&(train_desc[k]));

    //           // Find absolute difference and sum (L1 norm).

    //           // TODO: This only supports bytes, we are working with shorts
    //           // shorts here ... figure out how to do this
    //           xmm0.m128i = _mm_sad_epu8(xmm0.m128i, xmm1.m128i);
              
    //           // Sum upper and lower halfs 
    //           cur_dist += xmm0.m128i_u64[0] + xmm0.m128i_u64[1];
    //         }

    //         if(cur_dist < best_match_distance)
    //         {
    //           best_match_distance = cur_dist;
    //           best_match_idx = j;
    //         }
    //       }
    //     }
    //     matches->push_back(cv::DMatch(i, best_match_idx, best_match_distance));
    //   }
    // }
    std::clock_t stop = std::clock();
    std::cout << "Time to match LUCID descriptors "
              << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
              << "ms"
              << std::endl;
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // namespace lucid
