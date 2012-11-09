// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/descriptors/opencv_descriptor_extractor.h"

#include <iostream>
#include <ctime> // TODO: Make a Timable interface
#include <assert.h>
#include <cmath>

namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  OpenCvDescriptorExtractor::OpenCvDescriptorExtractor(
    const std::string& extractor_name,
    const cv::Ptr<cv::DescriptorExtractor>& extractor,
    const cv::Ptr<cv::DescriptorMatcher>& matcher)
    : DescriptorExtractor(extractor_name),
      _extractor(extractor),
      _matcher(matcher)
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  OpenCvDescriptorExtractor::~OpenCvDescriptorExtractor()
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void OpenCvDescriptorExtractor::computeDescriptors(
    const cv::Mat& image,
    const std::vector<cv::KeyPoint>& key_points,
    std::vector<bool>* valid_descriptors,
    cv::Mat* all_descriptors) const
  {
    std::vector<cv::KeyPoint> key_points_mutable_copy(key_points);
    
    for(int i = 0; i < key_points_mutable_copy.size(); ++i)
    {
      // Remember index to what is culled later.
      key_points_mutable_copy[i].class_id = i;
    }

    cv::Mat only_valid_descriptors;
    // Begin timing after setup.
    std::clock_t start = clock();

    _extractor->compute(image, key_points_mutable_copy, only_valid_descriptors);

    // Find valid descriptors.
    int num_rows = key_points.size();
    int desc_width = only_valid_descriptors.cols;
    *all_descriptors = cv::Mat(num_rows,
                               desc_width,
                               only_valid_descriptors.type());

    valid_descriptors->clear();
    valid_descriptors->resize(num_rows, false);

    for(int i = 0; i < key_points_mutable_copy.size(); ++i)
    {
      const cv::KeyPoint& cur_pt = key_points_mutable_copy[i];
      int cur_pt_index = cur_pt.class_id;

      // Assertions to make sure key_points were not mutated.
      assert(cur_pt_index >= 0 && cur_pt_index < num_rows);
      assert(abs(cur_pt.pt.x - key_points[cur_pt_index].pt.x) < 1 &&
             abs(cur_pt.pt.y - key_points[cur_pt_index].pt.y) < 1);
      assert(cur_pt.size == key_points[cur_pt_index].size);

      cv::Mat cur_desc = only_valid_descriptors.row(i);
      cur_desc.copyTo(all_descriptors->row(cur_pt_index));
      
      (*valid_descriptors)[cur_pt_index] = true;
    }

    // End timing after finding valid descriptors.
    std::clock_t stop = std::clock();
    std::cout << "Time to extract OpenCV "
              << name() << " descriptors "
              << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
              << "ms"
              << std::endl;
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void OpenCvDescriptorExtractor::knnMatchDescriptors(
    const int k,
    const cv::Mat& test_descriptors,
    const cv::Mat& train_descriptors,
    const std::vector<bool>& valid_test_descriptors,
    const std::vector<bool>& valid_train_descriptors,
    std::vector<std::vector<cv::DMatch> > *matches) const
  {
    assert(_matcher->isMaskSupported());

    // Setup up mask.
    cv::Mat mask = cv::Mat::zeros(test_descriptors.rows,
                                  train_descriptors.rows,
                                  CV_8UC1);

    for(int i = 0; i < valid_test_descriptors.size(); ++i)
    {
      for(int j = 0; j < valid_train_descriptors.size(); ++j)
      {
        if(valid_test_descriptors[i] && valid_train_descriptors[j])
        {
          mask.at<uchar>(i, j) = 1;
        }
      }
    }

     std::clock_t start = clock();
     _matcher->knnMatch(test_descriptors,
                        train_descriptors,
                        *matches,
                        k,
                        mask);

     std::clock_t stop = std::clock();
     std::cout << "Time to match OpenCV "
               << name() << " descriptors "
               << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
               << "ms"
               << std::endl;
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void OpenCvDescriptorExtractor::matchDescriptors(
    const cv::Mat& test_descriptors,
    const cv::Mat& train_descriptors,
    const std::vector<bool>& valid_test_descriptors,
    const std::vector<bool>& valid_train_descriptors,
    std::vector<cv::DMatch> *matches) const
   {
     assert(_matcher->isMaskSupported());

     // Setup up mask.
     cv::Mat mask = cv::Mat::zeros(test_descriptors.rows,
                                   train_descriptors.rows,
                                   CV_8UC1);

     for(int i = 0; i < valid_test_descriptors.size(); ++i)
     {
       for(int j = 0; j < valid_train_descriptors.size(); ++j)
       {
         if(valid_test_descriptors[i] && valid_train_descriptors[j])
         {
           mask.at<uchar>(i, j) = 1;
         }
       }
     }

     std::clock_t start = clock();
     _matcher->match(test_descriptors, train_descriptors, *matches, mask);

     std::clock_t stop = std::clock();
     std::cout << "Time to match OpenCV "
               << name() << " descriptors "
               << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
               << "ms"
               << std::endl;
   }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // namespace lucid
