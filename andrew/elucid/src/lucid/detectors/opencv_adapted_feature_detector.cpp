// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/detectors/opencv_adapted_feature_detector.h"
#include "lucid/tools/util.h"

#include <ctime> // TODO: Make a Timable interface
#include <iostream>

namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  OpenCvAdaptedFeatureDetector::OpenCvAdaptedFeatureDetector(
    const std::string& detector_name,
    int multiplier,
    const cv::Ptr<cv::AdjusterAdapter>& detector_adapter)
    : FeatureDetector(detector_name),
      _detector_adapter(detector_adapter),
      _multiplier(multiplier)
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  OpenCvAdaptedFeatureDetector::~OpenCvAdaptedFeatureDetector()
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

 void OpenCvAdaptedFeatureDetector::detectFeatures(
   const cv::Mat& image,
   int max_features,
   std::vector<cv::KeyPoint> *features) const
  {
    std::clock_t start = clock();

    cv::DynamicAdaptedFeatureDetector 
      detector(_detector_adapter, max_features, _multiplier * max_features);
    
    detector.detect(image, *features);
    // Sort by largest response.
    std::sort(features->begin(), features->end(), Util::compareKeyPoints);    

    if(features->size() > max_features)
    {
      features->resize(max_features);
    }

    // End timing before finding valid descriptors.
    std::clock_t stop = std::clock();
    std::cout << "Time to detect OpenCV "
              << name() << " interest points. "
              << (1000.0*(stop - start)) / CLOCKS_PER_SEC 
              << "ms"
              << std::endl;
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

}  // namespace lucid
