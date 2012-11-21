// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/descriptors/elucid_binary_descriptor_extractor.h"
#include "lucid/descriptors/elucid_descriptor_pattern.h"
#include "lucid/tools/util.h"
#include "lucid/tools/timer.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <assert.h>

#include <iostream>

namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  ELucidBinaryDescriptorExtractor::ELucidBinaryDescriptorExtractor
  (bool useWideDesc, bool normalize_rotation)
    : DescriptorExtractor(useWideDesc ? "eLUCID 512 bit" : "eLUCID 256 bit"),
      _useWideDesc(useWideDesc), _normalize_rotation(normalize_rotation)
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

    const int num_channels = image.channels();
    
    uchar pixels[num_samples];

    valid_descriptors->reserve(key_points.size());

    // TODO: Clean this up and don't use redundant code
    if(!_useWideDesc)
    {
      int desc_width = 32;
      assert(desc_width == num_samples / 2);
      cv::Mat descs(key_points.size(),
                    desc_width,
                    CV_8UC1);

      /* Unary Representation Lookup Tables*/
      uchar lut_lower[4] =
        {
          0x00,
          0x01,
          0x03,
          0x07,
        };

      uchar lut_upper[4] =
        {
          0x00,
          0x10,
          0x30,
          0x70,
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

          int bin_width = 16;
          uchar temp_desc[num_samples];
          Util::getRankVectors2(num_samples,
                                bin_width,
                                pixels,
                                &(temp_desc[0]));

          int next_idx = 0;
          for(int i = 0; i < num_samples; i+=2)
          {
            cur_desc[next_idx++] =
              lut_lower[temp_desc[i]] | lut_upper[temp_desc[i+1]];
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
      assert(desc_width == num_samples);
      cv::Mat descs(key_points.size(),
                    desc_width,
                    CV_8UC1);

      /* Sampled Unary Representation Lookup Tables*/
      uchar lut[8] =
        {
          0x00,      
          0x01,
          0x03,
          0x07,
          0x0f,
          0x1f,
          0x3f,
          0x7f,
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

          int bin_width = 8;
          uchar temp_desc[num_samples];
          Util::getRankVectors2(num_samples,
                                bin_width,
                                pixels,
                                &(temp_desc[0]));

          for(int i = 0; i < num_samples; i++)
          {
            cur_desc[i] = lut[temp_desc[i]];
          }

        }
      }

      for(int k = 0; k < key_points.size(); ++k)
      {

        if((*valid_descriptors)[k])
        {
          uchar *cur_desc = descs.ptr<uchar>(k);

          // Number of times to rotate outer most pattern
          float turns = (360 - key_points[k].angle) / base_rotation_angle;

           // Rotate pattern elements
           Util::rotateDescriptor(turns, cur_desc);
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

    matches->clear();
    matches->reserve(test_descriptors.rows);
    for(int i = 0; i < test_descriptors.rows; ++i)
    {
      if(valid_test_descriptors[i])
      {
        const uchar *test_desc = test_descriptors.ptr<uchar>(i);
        std::vector<cv::DMatch> cur_matches;
        for(int j = 0; j < train_descriptors.rows; ++j)
        {
          if(valid_train_descriptors[j])
          {
            const uchar *train_desc = train_descriptors.ptr<uchar>(j);
            unsigned long int cur_dist = cv::normHamming(test_desc, train_desc, desc_width);
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
            const uchar *train_desc = train_descriptors.ptr<uchar>(j);
            unsigned long int cur_dist = cv::normHamming(test_desc, train_desc, desc_width);

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
