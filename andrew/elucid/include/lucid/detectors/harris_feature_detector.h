// Author: Andrew Ziegler <andrewzieg@gmail.com>
// An wrapper for Harris corners.

#ifndef LUCID_HARRIS_FEATURE_DETECTOR_H
#define LUCID_HARRIS_FEATURE_DETECTOR_H

#include "lucid/detectors/feature_detector.h"

namespace lucid
{
  class HarrisFeatureDetector : public FeatureDetector
  {
  public:
    HarrisFeatureDetector(int multiplier);
    ~HarrisFeatureDetector();

    /**
     * Returns keypoints that are detected in the given image.
     * The features with the highest response are returned up to
     * the maximum specified.
     */
    virtual void detectFeatures(const cv::Mat& image,
                                int max_features,
                                std::vector<cv::KeyPoint> *features) const;

  private:
    int _multiplier;
  };

} // namespace lucid

#endif
