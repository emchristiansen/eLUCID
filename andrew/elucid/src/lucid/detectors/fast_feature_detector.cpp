// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/detectors/fast_feature_detector.h"

namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  FastFeatureDetector::FastFeatureDetector(int multiplier)
    : OpenCvAdaptedFeatureDetector("FAST", multiplier, new cv::FastAdjuster())
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  FastFeatureDetector::~FastFeatureDetector()
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

}  // namespace lucid
