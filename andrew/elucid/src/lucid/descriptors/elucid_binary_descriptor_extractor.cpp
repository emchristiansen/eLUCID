// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/descriptors/elucid_binary_descriptor_extractor.h"
#include "lucid/descriptors/elucid_descriptor_pattern.h"
#include "lucid/tools/util.h"
#include "lucid/tools/timer.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <assert.h>

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

  ELucidBinaryDescriptorExtractor::ELucidBinaryDescriptorExtractor
  (bool useWideDesc)
    : DescriptorExtractor(useWideDesc ? "eLUCID 512 bit" : "eLUCID 256 bit"),
      _useWideDesc(useWideDesc)
  {
    
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  ELucidBinaryDescriptorExtractor::~ELucidBinaryDescriptorExtractor()
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void ELucidBinaryDescriptorExtractor::computeDescriptors(
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

    const int patch_size = 45;
    const int num_channels = image.channels();
    
    uchar pixels[num_samples];

    valid_descriptors->reserve(key_points.size());

    // TODO: Clean this up and don't use redundant code
    if(!_useWideDesc)
    {
      int desc_width = 32;
      cv::Mat descs(key_points.size(),
                    desc_width,
                    CV_8UC1);

      /* Sampled Unary Representation Lookup Tables*/
      uchar lut_lower_eight[8] =
        {
          0x00,
          0x01,
          0x03,
          0x07,
          0x0f,
          0x0e,
          0x0c,
          0x08,
        };

      uchar lut_upper_eight[8] =
        {
          0x00,
          0x10,
          0x30,
          0x70,
          0xf0,
          0xe0,
          0xc0,
          0x80,
        };

      uchar lut_lowest_four[4] =
        {
          0x00,
          0x01,
          0x03,
          0x02,
        };

      uchar lut_lower_four[4] =
        {
          0x00,
          0x04,
          0x0c,
          0x08,
        };

      uchar lut_upper_four[4] =
        {
          0x00,
          0x10,
          0x30,
          0x20,
        };

      uchar lut_upper_most_four[4] =
        {
          0x00,
          0x40,
          0xc0,
          0x80,
        };

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

          for(int p = 0; p < num_samples; ++p)
          {
            pixels[p] = patch_ptr[pattern[p][1] * patch_size + pattern[p][0]];
          } 

          int bin_width = 1;
          uchar temp_desc[num_samples];
          Util::getRankVectors2(num_samples,
                                bin_width,
                                pixels,
                                &(temp_desc[0]));

          int next_idx = 0;
          bin_width = 12;
          for(int i = 0; i < 32; i+=2)
          {
            cur_desc[next_idx++] =
              lut_upper_eight[temp_desc[i] / bin_width] |
              lut_lower_eight[temp_desc[i+1] / bin_width];
          }

          bin_width = 24;
          for(int i = 32; i < num_samples; i+=4)
          {
            cur_desc[next_idx++] =
              lut_lowest_four[temp_desc[i] / bin_width] | 
              lut_lower_four[temp_desc[i+1] / bin_width] |
              lut_upper_four[temp_desc[i+2] / bin_width] | 
              lut_upper_most_four[temp_desc[i+3] / bin_width];
          }
        }
      }

      std::clock_t stop = std::clock();
      std::cout << "Time to compute eLUCID 256 bit descriptors "
                << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
                << "ms"
                << std::endl;
      *descriptors = descs;
    }
    else
    {
      int desc_width = 64;
      cv::Mat descs(key_points.size(),
                    desc_width,
                    CV_8UC1);

      /* Sampled Unary Representation Lookup Tables*/
      uchar lut_sixteen[16] =
        {
          0x00,
          0x01,
          0x03,
          0x07,
          0x0f,
          0x1f,
          0x3f,
          0x7f,
          0xff,      
          0xfe,
          0xfc,
          0xf8,
          0xf0,
          0xe0,
          0xc0,
          0x80,
        };


      uchar lut_lower_eight[8] =
        {
          0x00,
          0x01,
          0x03,
          0x07,
          0x0f,
          0x0e,
          0x0c,
          0x08,
        };

      uchar lut_upper_eight[8] =
        {
          0x00,
          0x10,
          0x30,
          0x70,
          0xf0,
          0xe0,
          0xc0,
          0x80,
        };

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

          for(int p = 0; p < num_samples; ++p)
          {
            pixels[p] = patch_ptr[pattern[p][1] * patch_size + pattern[p][0]];
          } 

          int bin_width = 1;
          uchar temp_desc[num_samples];
          Util::getRankVectors2(num_samples,
                                bin_width,
                                pixels,
                                &(temp_desc[0]));

          int next_idx = 0;
          bin_width = 6;
          for(int i = 0; i < 32; i++)
          {
            cur_desc[next_idx++] = lut_sixteen[temp_desc[i] / bin_width];
          }


          bin_width = 12;
          for(int i = 32; i < num_samples; i+=2)
          {
            cur_desc[next_idx++] =
              lut_upper_eight[temp_desc[i] / bin_width] |
              lut_lower_eight[temp_desc[i+1] / bin_width];
          }
        }
      }
        std::clock_t stop = std::clock();
        std::cout << "Time to compute eLUCID 512 bit descriptors "
                  << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
                  << "ms"
                  << std::endl;
        *descriptors = descs;
    }
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void ELucidBinaryDescriptorExtractor::knnMatchDescriptors(
    const int k,
    const cv::Mat& test_descriptors,
    const cv::Mat& train_descriptors,
    const std::vector<bool>& valid_test_descriptors,
    const std::vector<bool>& valid_train_descriptors,
    std::vector<std::vector<cv::DMatch> > *matches) const
  {
    std::clock_t start = clock();
    int desc_width = test_descriptors.cols;

    register __oword xmm0;
    register __oword xmm1;
    register __oword xmm2;
    register __oword xmm3;

    for(int i = 0; i < test_descriptors.rows; ++i)
    {
      if(valid_test_descriptors[i])
      {
        const __m128i *test_desc = test_descriptors.ptr<__m128i>(i);
        std::vector<cv::DMatch> cur_matches;
        for(int j = 0; j < train_descriptors.rows; ++j)
        {
          if(valid_train_descriptors[j])
          {
            const __m128i *train_desc = train_descriptors.ptr<__m128i>(j);
            unsigned long int cur_dist = 0;
            for(int d = 0; d < desc_width / 16; ++d)
            {
              // Load descriptor elements for comparison.
              xmm0.m128i = _mm_load_si128(&(test_desc[d]));
              xmm1.m128i = _mm_load_si128(&(train_desc[d]));
            
              // Find exor
              xmm0.m128i = _mm_xor_si128(xmm0.m128i, xmm1.m128i);
              
              // Sum upper and lower halfs 
              cur_dist += _mm_popcnt_u64(xmm0.m128i_u64[0]) +
                _mm_popcnt_u64(xmm0.m128i_u64[1]);
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
    }

    std::clock_t stop = std::clock();
    if(_useWideDesc)
    {
      std::cout << "Time to match eLUCID 512 bit descriptors "
                << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
                << "ms"
                << std::endl;
    }
    else
    {
      std::cout << "Time to match eLUCID 256 bit descriptors "
                << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
                << "ms"
                << std::endl;

    }
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void ELucidBinaryDescriptorExtractor::matchDescriptors(
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
    register __oword xmm2;
    register __oword xmm3;

    for(int i = 0; i < test_descriptors.rows; ++i)
    {
      if(valid_test_descriptors[i])
      {
        const __m128i *test_desc = test_descriptors.ptr<__m128i>(i);
        int best_match_idx = -1;
        uint best_match_distance = ~0;
        for(int j = 0; j < train_descriptors.rows; ++j)
        {
          if(valid_train_descriptors[j])
          {
            const __m128i *train_desc = train_descriptors.ptr<__m128i>(j);
            unsigned long int cur_dist = 0;
            for(int k = 0; k < desc_width / 16; ++k)
            {
              // Load descriptor elements for comparison.
              xmm0.m128i = _mm_load_si128(&(test_desc[k]));
              xmm1.m128i = _mm_load_si128(&(train_desc[k]));
            
              // Find exor
              xmm0.m128i = _mm_xor_si128(xmm0.m128i, xmm1.m128i);
              
              // Sum upper and lower halfs 
              cur_dist += _mm_popcnt_u64(xmm0.m128i_u64[0]) +
                _mm_popcnt_u64(xmm0.m128i_u64[1]);
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
    }
    std::clock_t stop = std::clock();
    if(_useWideDesc)
    {
      std::cout << "Time to match eLUCID 512 bit descriptors "
                << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
                << "ms"
                << std::endl;
    }
    else
    {
      std::cout << "Time to match eLUCID 256 bit descriptors "
                << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
                << "ms"
                << std::endl;

    }
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // namespace lucid
