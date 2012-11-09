// Author: Andrew Ziegler <andrewzieg@gmail.com>
// An abstraction for feature description.

#ifndef LUCID_DESCRIPTOR_EXTRACTOR_H
#define LUCID_DESCRIPTOR_EXTRACTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <string>
#include <vector>

namespace lucid
{
  class DescriptorExtractor
  {
  public:

    DescriptorExtractor(const std::string& _extractor_name);
    virtual ~DescriptorExtractor();

    /**
     * Returns a mxd matrix of descriptors where m is the number of
     * descriptors and d is the dimensionality. Since some feature
     * descriptors use larger windows than others features near image
     * boundaries may be unusable. Therefore the method returns a bit vector
     * indicating which descriptors are valid. This is a fix to OpenCV
     * which has a const correctness error allowing mutation of the provided
     * keypoints vector.
     */
    virtual void computeDescriptors(const cv::Mat& image,
                                    const std::vector<cv::KeyPoint>& key_points,
                                    std::vector<bool> *valid_descriptors,
                                    cv::Mat *descriptors) const = 0;

    /**
     * Finds the nearest neighbor in the training image for each descriptor
     * in the testing image.
     */
    virtual void matchDescriptors(
      const cv::Mat& test_descriptors,
      const cv::Mat& train_descriptors,
      const std::vector<bool>& valid_test_descriptors,
      const std::vector<bool>& valid_train_descriptors,
      std::vector<cv::DMatch> *matches) const = 0;

    /**
     * Finds the k nearests neighbor in the training image for each descriptor
     * in the testing image. Returns up to k matches if possible.
     */
    virtual void knnMatchDescriptors(
      const int k,
      const cv::Mat& test_descriptors,
      const cv::Mat& train_descriptors,
      const std::vector<bool>& valid_test_descriptors,
      const std::vector<bool>& valid_train_descriptors,
      std::vector<std::vector<cv::DMatch> > *matches) const = 0;

    virtual std::string name() const { return _extractor_name; }

  private:
    std::string _extractor_name;
  };

} // namespace lucid

#endif
