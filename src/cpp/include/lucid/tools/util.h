// Author: Andrew Ziegler <andrewzieg@gmail.com>
// An static utility class.

#ifndef LUCID_UTIL_H
#define LUCID_UTIL_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <string>

#include <vector>

namespace lucid
{
  class Util
  {
  public:
    static void rotateDescriptor(float turns, uchar* desc);

    /**
     * Extracts square image patches centered about the subpixel location
     * of each key point. If any key points are too close to the image 
     * boundary they are removed from the provided vector and are not
     * considered. 
     */
    static void getImagePatches(int patch_edge_length,
                                const cv::Mat& image,
                                const std::vector<cv::KeyPoint>& key_points,
                                std::vector<bool> *valid_patches,
                                std::vector<cv::Mat> *patches);

    /**
     * Performs a linear time stable sort on the input vector. Returns the
     * permuation of the vector indices and optionally the sorted list.
     */
    static void countingSort(int num_elements,
                             const uchar unsorted[],
                             uchar *idxs,
                             uchar *sorted = NULL);

    /**
     * Performs a linear time stable sort on the input vector. Returns the
     * permuation of the vector indices and optionally the sorted list.
     */
    static void countingSort2(int num_elements,
                             const uchar unsorted[],
                             ushort *idxs,
                             ushort *sorted = NULL);

    static void getRankVectors(int num_elements,
                               const ushort bin_width,
                               const uchar unsorted[],
                               ushort *idxs);

    static void getRankVectors2(int num_elements,
                               const uchar bin_width,
                               const uchar unsorted[],
                               uchar *idxs);

    static bool readHomographyFromFile(const char *file_name,
                                       cv::Matx33f *homog);

    /**
     * Rotates a given image patch by the specified angle in radians and 
     * crops the resulting patch to be at a scale of 1/sqrt(2) the orginal size.
     */
    static void rotateImagePatch(const cv::Mat& patch,
                                 float angle,
                                 cv::Mat *rotated_patch);

    /**
     * Comparison function for sorting KeyPoints by response.
     */
    static bool compareKeyPoints(cv::KeyPoint k1, cv::KeyPoint k2)
    {
      return k1.response > k2.response;
    }

    /**
     * Comparison function for sorting DMatches by distance.
     */
    static bool compareMatches(cv::DMatch d1, cv::DMatch d2)
    {
      return d1.distance < d2.distance;
    }

  };

} // namespace lucid

#endif
