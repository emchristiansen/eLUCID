// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A class to compute ELUCID descriptors.

#ifndef LUCID_ELUCID_BINARY_DESCRIPTOR_EXTRACTOR_H
#define LUCID_ELUCID_BINARY_DESCRIPTOR_EXTRACTOR_H

#include "lucid/descriptors/descriptor_extractor.h"

namespace lucid
{
  class ELucidBinaryDescriptorExtractor : public DescriptorExtractor
  {
  public:
    /**
     * Creates a binary valued elucid  descriptor extractor.
     */
    ELucidBinaryDescriptorExtractor(bool useWideDesc, bool normalize_rotation);
    ~ELucidBinaryDescriptorExtractor();

    /**
     * Setups up internal unary lookup tables.
     */
//    void initialize();

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
//    const ushort _bin_width = 16;
    bool _useWideDesc;
    bool _normalize_rotation;
//    uchar lut[_bin_width];
  };

} // namespace lucid

#endif
