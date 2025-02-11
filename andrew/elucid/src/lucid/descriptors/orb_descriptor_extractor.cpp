// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/descriptors/orb_descriptor_extractor.h"

namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  OrbDescriptorExtractor::OrbDescriptorExtractor()
    : OpenCvDescriptorExtractor("ORB",
                                new cv::OrbDescriptorExtractor(),
                                new cv::BFMatcher(cv::NORM_HAMMING))
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  OrbDescriptorExtractor::~OrbDescriptorExtractor()
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // namespace lucid
