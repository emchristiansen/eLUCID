// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/descriptors/elucid_descriptor_extractor.h"
#include "lucid/descriptors/elucid_descriptor_pattern.h"
#include "lucid/tools/util.h"
#include "lucid/tools/timer.h"
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

// For pop count.
#if not CV_NEON
#include <nmmintrin.h>
#include <inttypes.h>

// For equality operator.
#include <emmintrin.h>


union __oword_t {
  __m128i m128i;
  uint64_t m128i_u64[2];
};

typedef union __oword_t __oword;
#endif
namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  ELucidDescriptorExtractor::ELucidDescriptorExtractor(bool normalize_rotation)
    : DescriptorExtractor(normalize_rotation ?
                          "eLUCID-Rank-Vector" :
                          "eLUCID-Rank-Vector-No-Rot"),
      _normalize_rotation(normalize_rotation)
  {
    
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  ELucidDescriptorExtractor::~ELucidDescriptorExtractor()
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void ELucidDescriptorExtractor::computeDescriptors(
    const cv::Mat& image,
    const std::vector<cv::KeyPoint>& key_points,
    std::vector<bool> *valid_descriptors,
    cv::Mat *descriptors) const
  {    
    std::clock_t start = std::clock();
    cv::Mat blurred_image;
    int _blur_radius = 5; //FIXME: move this to class member
    cv::blur(image,
             blurred_image,
             cv::Size(_blur_radius, _blur_radius));

    // TODO: Make this always grayscale
    const int num_channels = image.channels();
    
    uchar pixels[num_samples];

    int desc_width = num_samples * num_channels;
    cv::Mat descs(key_points.size(),
                  desc_width,
                  CV_8UC1);

    valid_descriptors->reserve(key_points.size());

    const ushort bin_width = 2; //FIXME: move this to class member
    for(int k = 0; k < key_points.size(); ++k)
    {
      uchar *cur_desc = descs.ptr<uchar>(k);
      float x = key_points[k].pt.x;
      float y = key_points[k].pt.y;

      valid_descriptors->push_back(
        (x - patch_size/2) > 0 &&
        (y - patch_size/2) > 0 &&
        (x + patch_size/2) < image.cols &&
        (y + patch_size/2) < image.rows);
      
      if((*valid_descriptors)[k])
      {
        // TODO: Replace this by directly accessing pattern pixels
        cv::Mat patch;
        getRectSubPix(blurred_image,
                      cv::Size(patch_size, patch_size),
                      key_points[k].pt,
                      patch);
      
        uchar* patch_ptr = patch.data;
        int next_idx = 0;
        for(int p = 0; p < num_samples; ++p)
        {
          pixels[p] = patch_ptr[pattern[p][1] * patch_size + pattern[p][0]];
        }

        Util::getRankVectors2(desc_width,
                              bin_width,
                              pixels,
                              cur_desc);
      }
    }

    if(_normalize_rotation)
    {
      // Rotate descriptors based on orientation
      std::clock_t start_2 = std::clock();
      
      for(int k = 0; k < key_points.size(); ++k)
      {
     
        if((*valid_descriptors)[k])
        {
          uchar *cur_desc = descs.ptr<uchar>(k);

          // Number of times to rotate outer most pattern
          uint turns = static_cast<uint>((360-round(key_points[k].angle) / base_rotation_angle));
                
//        std::cout << "angle = " << key_points[k].angle << std::endl;
//         std::cout << "octave = " << key_points[k].octave << std::endl;

           // Rotate pattern elements
           for(int r = 1; r <= turns; ++r)
           {
             for(int p = 0; p < num_polygons; ++p)
             {
              if(!(r % rotation_ratios[p]))
              {
                // TODO: Use lookup table with precomputed sigmas
                // Also try to use bit-shifting instead
                int start_idx = polygon_start_idxs[p];
                int end_idx = start_idx + polygon_sizes[p] - 1;
                uchar last_value = cur_desc[start_idx];
                cur_desc[start_idx] = cur_desc[end_idx];
                for(int s = start_idx+1; s <= end_idx; ++s)
                {
                  uchar temp = cur_desc[s];
                  cur_desc[s] = last_value;
                  last_value = temp;
                }
                cur_desc[start_idx] = last_value;
              }
            }
          }
        }
      }
      std::clock_t stop_2 = std::clock();
      std::cout << "Time to normalize rotation eLUCID Rank Vector descriptors "
                << (1000.0*(stop_2 - start_2)) / CLOCKS_PER_SEC 
                << "ms"
                << std::endl;
    }

    std::clock_t stop = std::clock();
    std::cout << "Time to compute eLUCID Rank Vector descriptors "
              << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
              << "ms"
              << std::endl;
    *descriptors = descs;
  }


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void ELucidDescriptorExtractor::knnMatchDescriptors(
    const int k,
    const cv::Mat& test_descriptors,
    const cv::Mat& train_descriptors,
    const std::vector<bool>& valid_test_descriptors,
    const std::vector<bool>& valid_train_descriptors,
    std::vector<std::vector<cv::DMatch> > *matches) const
  {
    std::clock_t start = clock();
    int desc_width = test_descriptors.cols;

    matches->clear();
    matches->reserve(test_descriptors.rows);

    for(int i = 0; i < test_descriptors.rows; ++i)
    {
      std::vector<cv::DMatch> cur_matches;
      const uchar *test_desc = test_descriptors.ptr<uchar>(i);
      for(int j = 0; j < train_descriptors.rows; ++j)
      {
        if(valid_test_descriptors[i] && valid_train_descriptors[j])
        {
          // TODO: Move this conditional outside of the loop ...
          // instead use a vector of pairs to compare. Should give a speedup
          const uchar *train_desc = train_descriptors.ptr<uchar>(j);
          unsigned int cur_dist = cv::normL1_(test_desc, train_desc, desc_width);
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
    std::cout << "Time to match eLUCID rank descriptors "
              << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
              << "ms"
              << std::endl;
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void ELucidDescriptorExtractor::matchDescriptors(
    const cv::Mat& test_descriptors,
    const cv::Mat& train_descriptors,
    const std::vector<bool>& valid_test_descriptors,
    const std::vector<bool>& valid_train_descriptors,
    std::vector<cv::DMatch> *matches) const
  {
    std::clock_t start = clock();
    int desc_width = test_descriptors.cols;

    uint weights[4] = {1,1,1,1};

    matches->clear();
    matches->reserve(test_descriptors.rows);
    for(int i = 0; i < test_descriptors.rows; ++i)
    {
      if(valid_test_descriptors[i])
      {
        const uchar *test_desc = test_descriptors.ptr<uchar>(i);
        int best_match_idx = -1;
        uint best_match_distance = ~0;
        for(int j = 0; j < train_descriptors.rows; ++j)
        {
          if(valid_train_descriptors[j])
          {
            // TODO: Move this conditional outside of the loop ...
            // instead use a vector of pairs to compare. Should give a speedup
            const uchar *train_desc = train_descriptors.ptr<uchar>(j);
            unsigned int cur_dist = cv::normL1_(test_desc, train_desc, desc_width);

            if(cur_dist < best_match_distance)
            {
              best_match_distance = cur_dist;
              best_match_idx = j;
            }
          }
        }
        matches->push_back(cv::DMatch(i, best_match_idx, best_match_distance));
      }
    }
    std::clock_t stop = std::clock();
    std::cout << "Time to match eLUCID rank descriptors "
              << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
              << "ms"
              << std::endl;
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // namespace lucid
