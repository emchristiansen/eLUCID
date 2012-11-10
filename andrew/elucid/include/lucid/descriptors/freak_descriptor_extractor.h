// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A class to wrap Freak Descriptors

#ifndef LUCID_FREAK_DESCRIPTOR_EXTRACTOR_H
#define LUCID_FREAK_DESCRIPTOR_EXTRACTOR_H

#include "lucid/descriptors/opencv_descriptor_extractor.h"

namespace lucid
{
  class FreakDescriptorExtractor : public OpenCvDescriptorExtractor
  {
  public:
    FreakDescriptorExtractor();
    virtual ~FreakDescriptorExtractor();
  };  
} // namespace lucid

#endif
