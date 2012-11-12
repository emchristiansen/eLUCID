// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A wrapper for Orb interest points.

#ifndef LUCID_ORB_FEATURE_DETECTOR_H
#define LUCID_ORB_FEATURE_DETECTOR_H

#include "lucid/detectors/feature_detector.h"

namespace lucid
{
  class OrbFeatureDetector : public FeatureDetector
  {
  public:
    OrbFeatureDetector();
    ~OrbFeatureDetector();

    /**
     * Returns keypoints that are detected in the given image.
     * The features with the highest response are returned up to
     * the maximum specified.
     */
    virtual void detectFeatures(const cv::Mat& image,
                                int max_features,
                                std::vector<cv::KeyPoint> *features) const;

  };

} // namespace lucid

#endif
