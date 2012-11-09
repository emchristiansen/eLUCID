// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A class to wrap Surf Descriptors

#ifndef LUCID_SURF_DESCRIPTOR_EXTRACTOR_H
#define LUCID_SURF_DESCRIPTOR_EXTRACTOR_H

#include "lucid/descriptors/opencv_descriptor_extractor.h"

namespace lucid
{
  class SurfDescriptorExtractor : public OpenCvDescriptorExtractor
  {
  public:
    SurfDescriptorExtractor();
    virtual ~SurfDescriptorExtractor();
  };  
} // namespace lucid

#endif
