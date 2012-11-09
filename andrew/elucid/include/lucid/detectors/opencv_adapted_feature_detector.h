// Author: Andrew Ziegler <andrewzieg@gmail.com>
// An wrapper for OpenCV feature detectors with adapters.

#ifndef LUCID_OPENCV_ADAPTED_FEATURE_DETECTOR_H
#define LUCID_OPENCV_ADAPTED_FEATURE_DETECTOR_H

#include "lucid/detectors/feature_detector.h"

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <string>

namespace lucid
{
  class OpenCvAdaptedFeatureDetector : public FeatureDetector
  {
  public:

    /**
     * Takes an OpenCV AdjusterAdapter so that many features can be detected
     * and the best can be returned.
     */
    OpenCvAdaptedFeatureDetector(
      const std::string& detector_name,
      int multiplier,
      const cv::Ptr<cv::AdjusterAdapter>& detector_adapter);

    virtual ~OpenCvAdaptedFeatureDetector();

    /**
     * Returns keypoints that are detected in the given image.
     * The features with the highest response are returned up to
     * the maximum specified. Attempts to detect multiplier times
     * the number of maximum features desired and then selects
     * best features (based on response) up to max_features.
     */
    virtual void detectFeatures(const cv::Mat& image,
                                int max_features,
                                std::vector<cv::KeyPoint> *features) const;


  private:
    cv::Ptr<cv::AdjusterAdapter> _detector_adapter;
    int _multiplier;
  };

} // namespace lucid

#endif
