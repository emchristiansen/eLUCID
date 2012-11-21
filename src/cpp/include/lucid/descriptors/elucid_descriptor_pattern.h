// Author: Andrew Ziegler <andrewzieg@gmail.com>
// Predefined eLUCID pixel sampling pattern.

#ifndef LUCID_LUCID_DESCRIPTOR_PATTERN_H
#define LUCID_LUCID_DESCRIPTOR_PATTERN_H

namespace lucid
{
  // FIXME put constanst to upper case
  const unsigned int num_samples = 64;
  const int patch_size = 29;
  const unsigned int num_polygons = 4;
  const unsigned int polygon_start_idxs[num_polygons] = { 0, 8, 16, 32};
  const unsigned int polygon_sizes[num_polygons] = { 8, 8, 16, 32};
  const unsigned int rotation_ratios[num_polygons] = { 4, 4, 2, 1};
  const unsigned int num_rotations = 32;
  const float base_rotation_angle = 360.0 / num_rotations;
  const unsigned char pattern[num_samples][2] =
    {
      /* Polygon 1 */
      16, 16,
      14, 17,
      12, 16,
      11, 14,
      12, 12,
      14, 11,
      16, 12,
      17, 14,
      /* Polygon 2 */
      16, 19,
      12, 19,
      9, 16,
      9, 12,
      12, 9,
      16, 9,
      19, 12,
      19, 16,
      /* Polygon 3 */
      23, 18,
      21, 21,
      18, 23,
      14, 24,
      10, 23,
      7, 21,
      5, 18,
      4, 14,
      5, 10,
      7, 7,
      10, 5,
      14, 4,
      18, 5,
      21, 7,
      23, 10,
      24, 14,
      /* Polygon 4 */
      28, 17,
      27, 19,
      26, 22,
      24, 24,
      22, 26,
      19, 27,
      17, 28,
      14, 28,
      11, 28,
      9, 27,
      6, 26,
      4, 24,
      2, 22,
      1, 19,
      0, 17,
      0, 14,
      0, 11,
      1, 9,
      2, 6,
      4, 4,
      6, 2,
      9, 1,
      11, 0,
      14, 0,
      17, 0,
      19, 1,
      22, 2,
      24, 4,
      26, 6,
      27, 9,
      28, 11,
      28, 14
    };
} // namespace lucid

#endif
