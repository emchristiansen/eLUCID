// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/detectors/surf_feature_detector.h"
#include <opencv2/nonfree/features2d.hpp>

namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  SurfFeatureDetector::SurfFeatureDetector(int multiplier)
    : OpenCvAdaptedFeatureDetector("SURF", multiplier, new cv::SurfAdjuster())
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  SurfFeatureDetector::~SurfFeatureDetector()
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

}  // namespace lucid
