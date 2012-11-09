// Author: Andrew Ziegler <andrewzieg@gmail.com

#include "lucid/tools/util.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "lucid/descriptors/elucid_descriptor_pattern.h"

#include <iostream>
#include <fstream>
#include <assert.h>

namespace lucid
{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void Util::getImagePatches(int patch_edge_length,
                             const cv::Mat& image,
                             const std::vector<cv::KeyPoint>& key_points,
                             std::vector<bool> *valid_patches,
                             std::vector<cv::Mat> *patches)
  {
    int patch_size = patch_edge_length * patch_edge_length * image.channels();
    std::vector<unsigned char> cur_patch;

    for(int k = 0; k < key_points.size(); ++k)
    {
      float x = key_points[k].pt.x;
      float y = key_points[k].pt.y;

      valid_patches->push_back(
        (x - patch_edge_length/2) > 0 &&
        (y - patch_edge_length/2) > 0 &&
        (x + patch_edge_length/2) < image.cols &&
        (y + patch_edge_length/2) < image.rows);

        cv::Mat patch;
        getRectSubPix(image,
                      cv::Size(patch_edge_length, patch_edge_length),
                      key_points[k].pt,
                      patch);
        
        patches->push_back(patch);
    }
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void Util::countingSort(int num_elements,
                          const uchar unsorted[],
                          uchar *idxs,
                          uchar *sorted)
  {
    // TODO: Create versions of this for different size elements
    // i.e. short, int, uchar
    static const int MAX_VALUE = 256;
    static int counts[MAX_VALUE];

    // Reset counts.
    for(int i = 0; i < MAX_VALUE; ++i)
    {
      counts[i] = 0;
    }

    // Initialize counts.
    for(int i = 0; i < num_elements; ++i)
    {
      counts[unsorted[i]] += 1;
    }

    // Compute running totals.
    int running_total = 0;
    for(int i = 0; i < MAX_VALUE; ++i)
    {
      int cur_count = counts[i];
      counts[i] = running_total;
      running_total += cur_count;
    }

    // Find the rank permutation.
    for(int i = 0; i < num_elements; ++i)
    {
//      idxs[counts[unsorted[i]]] = i;
      idxs[i] = counts[unsorted[i]];
      counts[unsorted[i]] += 1;
    }

    if(sorted != NULL)
    {
      // Sort list.
      for(int i = 0; i < num_elements; ++i)
      {
        sorted[i] = unsorted[idxs[i]];
      }
    }
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void Util::getRankVectors(int num_elements,
                            const ushort bin_width,
                            const uchar unsorted[],
                            ushort *idxs)
  {
    static const int MAX_VALUE = 256;
    static int counts[MAX_VALUE];

    // Reset counts.
    for(int i = 0; i < MAX_VALUE; ++i)
    {
      counts[i] = 0;
    }

    // Initialize counts.
    for(int i = 0; i < num_elements; ++i)
    {
      assert(unsorted[i] < MAX_VALUE);
      counts[unsorted[i]] += 1;
    }

    // Compute running totals.
    int running_total = 0;
    for(int i = 0; i < MAX_VALUE; ++i)
    {
      int cur_count = counts[i];
      counts[i] = running_total;
      running_total += cur_count;
    }

    // Find the rank permutation.
    for(int i = 0; i < num_elements; ++i)
    {
      idxs[i] = counts[unsorted[i]] / bin_width;
      counts[unsorted[i]] += 1;
    }
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void Util::getRankVectors2(int num_elements,
                            const uchar bin_width,
                            const uchar unsorted[],
                            uchar *idxs)
  {
    static const int MAX_VALUE = 256;
    static int counts[MAX_VALUE];

    // Reset counts.
    for(int i = 0; i < MAX_VALUE; ++i)
    {
      counts[i] = 0;
    }

    // Initialize counts.
    for(int i = 0; i < num_elements; ++i)
    {
      assert(unsorted[i] < MAX_VALUE);
      counts[unsorted[i]] += 1;
    }

    // Compute running totals.
    int running_total = 0;
    for(int i = 0; i < MAX_VALUE; ++i)
    {
      int cur_count = counts[i];
      counts[i] = running_total;
      running_total += cur_count;
    }

    // Find the rank permutation.
    for(int i = 0; i < num_elements; ++i)
    {
      idxs[i] = counts[unsorted[i]] / bin_width;
      counts[unsorted[i]] += 1;
    }
  }



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  void Util::countingSort2(int num_elements,
                           const uchar unsorted[],
                           ushort *idxs,
                           ushort *sorted)
  {
    // i.e. short, int, uchar
    static const int MAX_VALUE = 256;
    static int counts[MAX_VALUE];


    // Reset counts.
    for(int i = 0; i < MAX_VALUE; ++i)
    {
      counts[i] = 0;
    }

    // Initialize counts.
    for(int i = 0; i < num_elements; ++i)
    {
      assert(unsorted[i] < MAX_VALUE);
      counts[unsorted[i]] += 1;
    }

    // Compute running totals.
    int running_total = 0;
    for(int i = 0; i < MAX_VALUE; ++i)
    {
      int cur_count = counts[i];
      counts[i] = running_total;
      running_total += cur_count;
    }

    // Find the rank permutation.
    for(int i = 0; i < num_elements; ++i)
    {
//      idxs[counts[unsorted[i]]] = i;
      idxs[i] = counts[unsorted[i]];
      counts[unsorted[i]] += 1;
    }

    if(sorted != NULL)
    {
      // Sort list.
      for(int i = 0; i < num_elements; ++i)
      {
        sorted[i] = unsorted[idxs[i]];
      }
    }
  }


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

  bool Util::readHomographyFromFile(const char *file_name,
                                    cv::Matx33f *homog)
  {
    std::ifstream homog_file;
    homog_file.open(file_name);
    if(homog_file.fail())
    {
      return false;
    }

    for(int i = 0; i < 3; ++i)
    {
      for(int j = 0; j < 3; ++j)
      {
        homog_file >> (*homog)(i, j);
      }
    }

    homog_file.close();
    return true;
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
  
  void Util::rotateImagePatch(const cv::Mat& patch,
                              float angle,
                              cv::Mat *rotated_patch)
  {
  }

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

} // namespace lucid
