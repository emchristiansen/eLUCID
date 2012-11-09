// Author: Andrew Ziegler <andrewzieg@gmail.com>
// A class to wrap Brief Descriptors

#ifndef LUCID_BRIEF_DESCRIPTOR_EXTRACTOR_H
#define LUCID_BRIEF_DESCRIPTOR_EXTRACTOR_H

#include "lucid/descriptors/opencv_descriptor_extractor.h"

namespace lucid
{
  class BriefDescriptorExtractor : public OpenCvDescriptorExtractor
  {
  public:
    BriefDescriptorExtractor();
    virtual ~BriefDescriptorExtractor();
  };  
} // namespace lucid

#endif
