// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A class to compute LUCID descriptors.

#ifndef LUCID_LUCID_DESCRIPTOR_EXTRACTOR_H
#define LUCID_LUCID_DESCRIPTOR_EXTRACTOR_H

#include "lucid/descriptors/descriptor_extractor.h"

namespace lucid
{
  class LucidDescriptorExtractor : public DescriptorExtractor
  {
  public:
    /**
     * Creates a descriptors with the specified image patch size.
     */
    LucidDescriptorExtractor(uint patch_size = 24, uint blur_radius = 5);
    ~LucidDescriptorExtractor();

    /**
     * Returns a mxd matrix of descriptors where m is the number of
     * key_poitns and d is the dimensionality of the descriptor.
     * Since some feature descriptors use larger windows than others
     * features near image boundaries may be unusable. There are other
     * reasons that features may be culled as well such as a low key point 
     * response. Therefore the method returns a bit vector indicating which
     * descriptors are valid and which correspond to culled key points.
     * This is a fix to OpenCV which has a const correctness error allowing
     * mutation of the provided keypoints vector.
     */
    virtual void computeDescriptors(const cv::Mat& image,
                                    const std::vector<cv::KeyPoint>& key_points,
                                    std::vector<bool> *valid_descriptors,
                                    cv::Mat *descriptors) const;
    /**
     * Finds the nearest neighbor in the training image for each descriptor
     * in the testing image.
     */
    virtual void matchDescriptors(
      const cv::Mat& test_descriptors,
      const cv::Mat& train_descriptors,
      const std::vector<bool>& valid_test_descriptors,
      const std::vector<bool>& valid_train_descriptors,
      std::vector<cv::DMatch> *matches) const;

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
      std::vector<std::vector<cv::DMatch> > *matches) const;


  private:
    uint _patch_size;
    uint _blur_radius;
  };

} // namespace lucid

#endif
