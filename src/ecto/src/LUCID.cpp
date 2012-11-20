#include <ecto/ecto.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "interfaces.h"
#include <opencv_candidate/hamming.h>
#include <numeric>

#include <lucid/descriptors/elucid_descriptor_extractor.h>

/** Cell for LUCID feature detection and descriptor extraction
 */
struct LUCID
{
  static void
  declare_params(tendrils& p)
  {
    p.declare<int>("n_features", "The number of desired features", 1000);
  }

  static void
  declare_io(const tendrils& params, tendrils& inputs, tendrils& outputs)
  {
    descriptor_extractor_interface::declare_inputs(inputs);
    feature_detector_interface::declare_outputs(outputs);
    descriptor_extractor_interface::declare_outputs(outputs);
  }

  void
  configure(const tendrils& params, const tendrils& inputs, const tendrils& outputs)
  {
    lucid_ = lucid::ELucidDescriptorExtractor(true);
  }

  int
  process(const tendrils& inputs, const tendrils& outputs)
  {
    std::vector<cv::KeyPoint> keypoints;
    inputs["keypoints"] >> keypoints;
    cv::Mat image, mask;
    inputs["image"] >> image;
    inputs["mask"] >> mask;

    if (keypoints.empty()) {
      cv::OrbFeatureDetector detector(100, 1, 1, 50);
      detector.detect(image, keypoints);
    }

    std::vector<bool> valid_desc;
    cv::Mat desc;
    lucid_.computeDescriptors(image, keypoints, &valid_desc, &desc); //use the provided keypoints if they were given.

    outputs["keypoints"] << keypoints;
    outputs["descriptors"] << desc;

    return ecto::OK;
  }

  
  lucid::ELucidDescriptorExtractor lucid_;
};

ECTO_CELL(lucid, LUCID, "LUCID",
          "A LUCID detector. Takes a image and a mask, and computes keypoints and descriptors(32 byte binary).");
