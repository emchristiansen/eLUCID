// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A wrapper for SURF features.

#ifndef LUCID_SURF_FEATURE_DETECTOR_H
#define LUCID_SURF_FEATURE_DETECTOR_H

#include "lucid/detectors/opencv_adapted_feature_detector.h"

namespace lucid
{
  class SurfFeatureDetector : public OpenCvAdaptedFeatureDetector
  {
  public:
    SurfFeatureDetector(int multiplier);
    virtual ~SurfFeatureDetector();

    virtual bool assignsAngle() { return true; }

    virtual bool assignsScale() { return true; }
  };

}  // namespace lucid

#endif
