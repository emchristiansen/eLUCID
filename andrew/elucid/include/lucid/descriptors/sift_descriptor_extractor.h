// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A class to wrap Sift Descriptors

#ifndef LUCID_SIFT_DESCRIPTOR_EXTRACTOR_H
#define LUCID_SIFT_DESCRIPTOR_EXTRACTOR_H

#include "lucid/descriptors/opencv_descriptor_extractor.h"

namespace lucid
{
  class SiftDescriptorExtractor : public OpenCvDescriptorExtractor
  {
  public:
    SiftDescriptorExtractor();
    virtual ~SiftDescriptorExtractor();
  };  
} // namespace lucid

#endif
