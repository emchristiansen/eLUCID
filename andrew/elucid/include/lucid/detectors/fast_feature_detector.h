// Author: Andrew Ziegler <andrewzieg@gmail.com>
// An wrapper for FAST features.

#ifndef LUCID_FAST_FEATURE_DETECTOR_H
#define LUCID_FAST_FEATURE_DETECTOR_H

#include "lucid/detectors/opencv_adapted_feature_detector.h"

namespace lucid
{
  class FastFeatureDetector : public OpenCvAdaptedFeatureDetector
  {
  public:
    FastFeatureDetector(int multiplier);
    ~FastFeatureDetector();
  };

}  // namespace lucid

#endif
