// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A tool to setup and run our experiments.

#ifndef LUCID_EXPERIMENT_DRIVER_H
#define LUCID_EXPERIMENT_DRIVER_H

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>
#include "lucid/descriptors/descriptor_extractor.h"
#include "lucid/detectors/feature_detector.h"
#include <vector>
#include <string>

namespace lucid
{
  class ExperimentDriver
  {
  public:

    /**
     * Finds features in the reference and test images seperately and 
     * determines true matches by proximity using the ground truth
     * homography. The test image is rotated by the given angles as
     * well as the detected features in the image. The recognition
     * rates for each descriptor are returned as a function of angle.
     */
    static void rotationMatchingExperiment(
      const int max_features,
      const float radius,
      const std::vector<float>& angles,
      const cv::Mat& reference_image,
      const cv::Mat& test_image,
      const cv::Matx33f homography,
      const lucid::FeatureDetector& detector,
      const std::vector<lucid::DescriptorExtractor*>& extractors,
      std::vector<std::vector<float> > *recognition_rates);


    /**
     * Detects the best interest points in each image seperately up to
     * max_features. Uses the ground truth homographies to determine 
     * ground truth matches. A pair of points is considered corresponding
     * if they are within the specified radius of each other according
     * to the homography mapping. Outputs labels and distances that can 
     * be used to compute ROC and PR curves to files starting with the
     * fiven file name base.
     */
    static void detectionMatchingExperiment(
      const int max_features,
      const float radius,
      const cv::Mat& reference_image,
      const std::vector<cv::Mat>& test_images,
      const std::vector<cv::Matx33f> homographies,
      const lucid::FeatureDetector& detector,
      const std::vector<lucid::DescriptorExtractor*>& extractors,
      std::vector<std::vector<float> > *recognition_rates,
      const std::string output_file_base);

    /**
     * The best interest points upto max_features are detected in the reference
     * image. Ground truth homographies are used to warp the features into
     * each of the test images. percent_non_matching percent of the warped
     * features are given a random correspondence in the test images. This
     * effectively simulates a detector that has repeatability rate of 
     * percent_non_matching. Nearest neighbor recognition rates are returned
     * for each pair of reference image and test image. Where the recognition
     * rate is the ratio of true matches to valid matches for the particular
     * descriptor.
     */
    static void symmetricMatchingExperiment(
      double percent_non_matching,
      const int max_features,
      const cv::Mat& reference_image,
      const std::vector<cv::Mat>& test_images,
      const std::vector<cv::Matx33f> homographies,
      const lucid::FeatureDetector& detector,
      const std::vector<lucid::DescriptorExtractor*>& extractors,
      std::vector<std::vector<float> > *recognition_rates);
  };

} // namespace lucid

#endif
