// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A wrapper for Brisk interest points.

#ifndef LUCID_BRISK_FEATURE_DETECTOR_H
#define LUCID_BRISK_FEATURE_DETECTOR_H

#include "lucid/detectors/feature_detector.h"

namespace lucid
{
  class BriskFeatureDetector : public FeatureDetector
  {
  public:
    BriskFeatureDetector();
    ~BriskFeatureDetector();

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
