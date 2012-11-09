// Author: Andrew Ziegler <andrewzieg@gmail.com>
// An wrapper for CENSURE features.

#ifndef LUCID_CENSURE_FEATURE_DETECTOR_H
#define LUCID_CENSURE_FEATURE_DETECTOR_H

#include "lucid/detectors/opencv_adapted_feature_detector.h"

namespace lucid
{
  class CensureFeatureDetector : public OpenCvAdaptedFeatureDetector
  {
  public:
    CensureFeatureDetector(int multiplier);
    ~CensureFeatureDetector();
  };

}  // namespace lucid

#endif
