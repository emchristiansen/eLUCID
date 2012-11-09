// Author: Andrew Ziegler <andrewzieg@gmail.com>
// An abstraction for feature detection.

#ifndef LUCID_FEATURE_DETECTOR_H
#define LUCID_FEATURE_DETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <string>

namespace lucid
{
  class FeatureDetector
  {
  public:
    FeatureDetector(const std::string& detector_name);
    virtual ~FeatureDetector();
    
    /**
     * Returns keypoints that are detected in the given image.
     * The features with the highest response are returned up to
     * the maximum specified. Attempts to detect multiplier times
     * the number of maximum features desired and then selects
     * best features (based on response) up to max_features.
     */
    virtual void detectFeatures(const cv::Mat& image,
                                int max_features,
                                std::vector<cv::KeyPoint> *features) const = 0;

    virtual bool assignsAngle() { return false; }

    virtual bool assignsScale() { return false; }

    virtual std::string name() const { return _detector_name; }

  private:
    std::string _detector_name;
  };

} // namespace lucid

#endif
