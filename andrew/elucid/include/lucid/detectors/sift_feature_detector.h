// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A wrapper for Sift interest points.

#ifndef LUCID_SIFT_FEATURE_DETECTOR_H
#define LUCID_SIFT_FEATURE_DETECTOR_H

#include "lucid/detectors/feature_detector.h"

namespace lucid
{
  class SiftFeatureDetector : public FeatureDetector
  {
  public:
    SiftFeatureDetector();
    ~SiftFeatureDetector();

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
