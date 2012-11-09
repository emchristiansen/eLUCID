// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/detectors/harris_feature_detector.h"
#include "lucid/tools/util.h"

#include <ctime> // TODO: Make a Timable interface
#include <iostream>


namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  HarrisFeatureDetector::HarrisFeatureDetector(int multiplier)
    : FeatureDetector("Harris"), _multiplier(multiplier)
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  HarrisFeatureDetector::~HarrisFeatureDetector()
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

 void HarrisFeatureDetector::detectFeatures(
   const cv::Mat& image,
   int max_features,
   std::vector<cv::KeyPoint> *features) const
  {

    cv::GoodFeaturesToTrackDetector detector(_multiplier * max_features,.001,5);
    std::clock_t start = clock();

     detector.detect(image, *features);
     std::sort(features->begin(), features->end(), Util::compareKeyPoints);    

     if(features->size() > max_features)
     {
       features->resize(max_features);
     }

     // End timing before finding valid descriptors.
     std::clock_t stop = std::clock();
     std::cout << "Time to detect Harris Interest Points "
               << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
               << "ms"
               << std::endl;
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


}  // namespace lucid
