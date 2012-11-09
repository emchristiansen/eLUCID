// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A class to wrap Brisk Descriptors

#ifndef LUCID_BRISK_DESCRIPTOR_EXTRACTOR_H
#define LUCID_BRISK_DESCRIPTOR_EXTRACTOR_H

#include "lucid/descriptors/opencv_descriptor_extractor.h"

namespace lucid
{
  class BriskDescriptorExtractor : public OpenCvDescriptorExtractor
  {
  public:
    BriskDescriptorExtractor();
    virtual ~BriskDescriptorExtractor();
  };  
} // namespace lucid

#endif
