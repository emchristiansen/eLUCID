// Author: Andrew Ziegler <andrewzieg@gmail.com>
// Predefined eLUCID pixel sampling pattern.

#ifndef LUCID_LUCID_DESCRIPTOR_PATTERN_H
#define LUCID_LUCID_DESCRIPTOR_PATTERN_H

namespace lucid
{
//  const unsigned int patch_size = 45;
  // FIXME put constanst to upper case
  const unsigned int num_samples = 96;
  const unsigned int num_polygons = 5;
  const unsigned int polygon_start_idxs[num_polygons] = { 0, 8, 16, 32, 64 };
  const unsigned int polygon_sizes[num_polygons] = { 8, 8, 16, 32, 32 };
  const unsigned int rotation_ratios[num_polygons] = { 4, 4, 2, 1, 1 };
  const float base_rotation_angle = 360.0 / 32;
  const unsigned char pattern[num_samples][2] =
    {
      /* Polygon 1 */
      24, 24, 
      22, 25, 
      20, 24, 
      19, 22, 
      20, 20, 
      22, 19, 
      24, 20, 
      25, 22, 
      /* Polygon 2 */
      24, 27, 
      20, 27, 
      17, 24, 
      17, 20, 
      20, 17, 
      24, 17, 
      27, 20, 
      27, 24, 
      /* Polygon 3 */
      31, 26, 
      29, 29, 
      26, 31, 
      22, 32, 
      18, 31, 
      15, 29, 
      13, 26, 
      12, 22, 
      13, 18, 
      15, 15, 
      18, 13, 
      22, 12, 
      26, 13, 
      29, 15, 
      31, 18, 
      32, 22, 
      /* Polygon 4 */
      38, 25, 
      37, 28, 
      36, 31, 
      34, 34, 
      31, 36, 
      28, 37, 
      25, 38, 
      22, 39, 
      19, 38, 
      16, 37, 
      13, 36, 
      10, 34, 
      8, 31, 
      7, 28, 
      6, 25, 
      5, 22, 
      6, 19, 
      7, 16, 
      8, 13, 
      10, 10, 
      13, 8, 
      16, 7, 
      19, 6, 
      22, 5, 
      25, 6, 
      28, 7, 
      31, 8, 
      34, 10, 
      36, 13, 
      37, 16, 
      38, 19, 
      39, 22, 
      /* Polygon 5 */
      43, 28,
      41, 32,
      39, 36,
      36, 39,
      32, 41,
      28, 43,
      24, 44,
      20, 44,
      16, 43,
      12, 41,
      8, 39,
      5, 36,
      3, 32,
      1, 28,
      0, 24,
      0, 20,
      1, 16,
      3, 12,
      5, 8,
      8, 5,
      12, 3,
      16, 1,
      20, 0,
      24, 0,
      28, 1,
      32, 3,
      36, 5,
      39, 8,
      41, 12,
      43, 16,
      44, 20,
      44, 24
    };

} // namespace lucid

#endif
