// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A class to wrap Orb Descriptors

#ifndef LUCID_ORB_DESCRIPTOR_EXTRACTOR_H
#define LUCID_ORB_DESCRIPTOR_EXTRACTOR_H

#include "lucid/descriptors/opencv_descriptor_extractor.h"

namespace lucid
{
  class OrbDescriptorExtractor : public OpenCvDescriptorExtractor
  {
  public:
    OrbDescriptorExtractor();
    virtual ~OrbDescriptorExtractor();
  };  
} // namespace lucid

#endif
