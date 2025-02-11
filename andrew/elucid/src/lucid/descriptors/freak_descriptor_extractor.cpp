// Author: Andrew Ziegler <andrewzieg@gmail.com>

#include "lucid/descriptors/freak_descriptor_extractor.h"

namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  FreakDescriptorExtractor::FreakDescriptorExtractor()
    : OpenCvDescriptorExtractor("FREAK",
                                new cv::FREAK(),
                                new cv::BFMatcher(cv::NORM_HAMMING))
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  FreakDescriptorExtractor::~FreakDescriptorExtractor()
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // namespace lucid
