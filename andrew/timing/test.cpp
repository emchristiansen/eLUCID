#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>

#include "timer.h"
#include <iostream>
#include <vector>

// For pop count.
#include <nmmintrin.h>
#include <inttypes.h>

// For equality operator.
#include <emmintrin.h>

// For rand
#include <stdlib.h>

// For setaffinity
#include <sched.h>


union __oword_t {
  __m128i m128i;
  uint64_t m128i_u64[2];
  uint8_t m128i_u8[16];
};

typedef union __oword_t __oword;

// Hamming LUT.
int lut[256];

string l1SSEMatch(const cv::Mat& test_descriptors,
                  const cv::Mat& train_descriptors,
                  std::vector<cv::DMatch> *matches)
{
  register __oword xmm0;
  register __oword xmm1;
  register __oword xmm2;
  register __oword xmm3;

  int desc_width = test_descriptors.cols;
  matches->reserve(test_descriptors.cols * train_descriptors.cols);

  for(int i = 0; i < test_descriptors.rows; ++i)
  {
    const __m128i *test_desc = test_descriptors.ptr<__m128i>(i);
    for(int j = 0; j < train_descriptors.rows; ++j)
    {
      const __m128i *train_desc = train_descriptors.ptr<__m128i>(j);
      unsigned long int cur_dist = 0;
      for(int d = 0; d < desc_width / 16; ++d)
      {
        // Load descriptor elements for comparison.
        xmm0.m128i = _mm_load_si128(&(test_desc[d]));
        xmm1.m128i = _mm_load_si128(&(train_desc[d]));
            
        // Find difference
        xmm0.m128i = _mm_sad_epu8(xmm0.m128i, xmm1.m128i);
              
        // Sum upper and lower halfs 
        cur_dist += xmm0.m128i_u64[0] + xmm0.m128i_u64[1];
      }
      matches->push_back(cv::DMatch(i, j, cur_dist));
    }
  }
  return "L1 SSE";
}

string l1NoSSEMatch(const cv::Mat& test_descriptors,
                    const cv::Mat& train_descriptors,
                    std::vector<cv::DMatch> *matches)
{
  int desc_width = test_descriptors.cols;
  matches->reserve(test_descriptors.cols * train_descriptors.cols);

  for(int i = 0; i < test_descriptors.rows; ++i)
  {
    const char *test_desc = test_descriptors.ptr<char>(i);
    for(int j = 0; j < train_descriptors.rows; ++j)
    {
      const char *train_desc = train_descriptors.ptr<char>(j);
      unsigned long int cur_dist = 0;
      for(int d = 0; d < desc_width; ++d)
      {
        // Get Difference
        int diff = test_desc[d] - train_desc[d];
        
        // Setup mask for branch free absolute value.
        int mask = diff >> (sizeof(int)*8 - 1);
        
        // Get absolute value and increment distance.
        cur_dist += (diff ^ mask) - mask;
      }
      matches->push_back(cv::DMatch(i, j, cur_dist));
    }
  }
  return "L1 No SSE";
}

string hammingSSEMatch(const cv::Mat& test_descriptors,
                       const cv::Mat& train_descriptors,
                       std::vector<cv::DMatch> *matches)
{
  register __oword xmm0;
  register __oword xmm1;
  register __oword xmm2;
  register __oword xmm3;
 
  int desc_width = test_descriptors.cols;
  matches->reserve(test_descriptors.cols * train_descriptors.cols);

  for(int i = 0; i < test_descriptors.rows; ++i)
  {
    const __m128i *test_desc = test_descriptors.ptr<__m128i>(i);
    for(int j = 0; j < train_descriptors.rows; ++j)
    {
      const __m128i *train_desc = train_descriptors.ptr<__m128i>(j);
      unsigned long int cur_dist = 0;
      for(int d = 0; d < desc_width / 16; ++d)
      {
        // Load descriptor elements for comparison.
        xmm0.m128i = _mm_load_si128(&(test_desc[d]));
        xmm1.m128i = _mm_load_si128(&(train_desc[d]));
        
        // Find exor
        xmm0.m128i = _mm_xor_si128(xmm0.m128i, xmm1.m128i);
              
        // Sum upper and lower halfs 
        cur_dist += _mm_popcnt_u64(xmm0.m128i_u64[0]) +
          _mm_popcnt_u64(xmm0.m128i_u64[1]);
      }
      matches->push_back(cv::DMatch(i, j, cur_dist));
    }
  }
  return "Hamming SSE";
}


string hammingSSEMatchNoPopcnt(const cv::Mat& test_descriptors,
                               const cv::Mat& train_descriptors,
                               std::vector<cv::DMatch> *matches)
{
  register __oword xmm0;
  register __oword xmm1;
  register __oword xmm2;
  register __oword xmm3;

  int desc_width = test_descriptors.cols;
  matches->reserve(test_descriptors.cols * train_descriptors.cols);

  for(int i = 0; i < test_descriptors.rows; ++i)
  {
    const __m128i *test_desc = test_descriptors.ptr<__m128i>(i);
    for(int j = 0; j < train_descriptors.rows; ++j)
    {
      const __m128i *train_desc = train_descriptors.ptr<__m128i>(j);
      unsigned long int cur_dist = 0;
      for(int d = 0; d < desc_width / 16; ++d)
      {
        // Load descriptor elements for comparison.
        xmm0.m128i = _mm_load_si128(&(test_desc[d]));
        xmm1.m128i = _mm_load_si128(&(train_desc[d]));
        
        // Find exor
        xmm0.m128i = _mm_xor_si128(xmm0.m128i, xmm1.m128i);

        // Use Lookup table to find popcount
        for(int b = 0; b < 16; ++b)
        {
          cur_dist += lut[xmm0.m128i_u8[b]];
        }
      }
      matches->push_back(cv::DMatch(i, j, cur_dist));
    }
  }
  return "Hamming SSE No POPCNT";
}

string hammingNoSSE(const cv::Mat& test_descriptors,
                    const cv::Mat& train_descriptors,
                    std::vector<cv::DMatch> *matches)
{
  int desc_width = test_descriptors.cols;
  matches->reserve(test_descriptors.cols * train_descriptors.cols);

  for(int i = 0; i < test_descriptors.rows; ++i)
  {
    const uchar *test_desc = test_descriptors.ptr<uchar>(i);
    for(int j = 0; j < train_descriptors.rows; ++j)
    {
      const uchar *train_desc = train_descriptors.ptr<uchar>(j);
      unsigned long int cur_dist = 0;
      for(int d = 0; d < desc_width; ++d)
      {
        // Get Hamming distance from lookup table.
        cur_dist += lut[test_desc[d] ^ train_desc[d]];
      }
      matches->push_back(cv::DMatch(i, j, cur_dist));
    }
  }
  return "Hamming No SSE";
}

string l2MatchNoSSE(const cv::Mat& test_descriptors,
                    const cv::Mat& train_descriptors,
                    std::vector<cv::DMatch> *matches)
{
  int desc_width = test_descriptors.cols;
  matches->reserve(test_descriptors.cols * train_descriptors.cols);

  for(int i = 0; i < test_descriptors.rows; ++i)
  {
    const float *test_desc = test_descriptors.ptr<float>(i);
    for(int j = 0; j < train_descriptors.rows; ++j)
    {
      const float *train_desc = train_descriptors.ptr<float>(j);
      unsigned long int cur_dist = 0;
      for(int d = 0; d < desc_width; ++d)
      {
        float diff = test_desc[d] - train_desc[d];
        cur_dist += diff*diff;
      }
      matches->push_back(cv::DMatch(i, j, cur_dist));
    }
  }
  return "L2 No SSE";
}


double timeMethod(const int num_trials,
                  const int num_descs,
                  const int desc_width,
                  const bool use_float, 
                  string (*matchingMethod)(const cv::Mat&,
                                           const cv::Mat&,
                                           std::vector<cv::DMatch>*))
{
  // Setup descriptors for matching.
  cv::Mat descs1, descs2;

  if(use_float)
  {
    descs1 = cv::Mat(num_descs,
                     desc_width,
                     CV_32FC1);

    descs2 = cv::Mat(num_descs,
                     desc_width,
                     CV_64FC1);    

    for(int i = 0; i < descs1.rows; ++i)
    {
      for(int j = 0; j < descs1.cols; ++j)
      {
        descs1.at<float>(i, j) =
          static_cast<float>(rand()) /
          static_cast<float>(RAND_MAX);
        
        descs2.at<float>(i, j) =
          static_cast<float>(rand()) /
          static_cast<float>(RAND_MAX);
      }
    }
  }
  else
  {
    descs1 = cv::Mat(num_descs,
                     desc_width,
                     CV_8UC1);

    descs2 = cv::Mat(num_descs,
                     desc_width,
                     CV_8UC1);

    // Initialize descriptors with random values since cv::Mat starts with
    // all zeros and this may be optimized out by the compiler.
    srand( time(NULL) );
    for(int i = 0; i < descs1.rows; ++i)
    {
      for(int j = 0; j < descs1.cols; ++j)
      {
        descs1.at<uchar>(i, j) = rand() % (0xff);
        
        descs2.at<uchar>(i, j) = rand() % (0xff);
      }
    }
  }

  Timer timer;
  timer.reset();
  string method_name;
  for(int t = 0; t < num_trials; ++t)
  {
    std::vector<cv::DMatch> matches;
    timer.start();
    method_name = matchingMethod(descs1,
                                 descs2,
                                 &matches);
    timer.stop();

    assert(!timer.timerInvalid());
    // Do something with the DMatches so the compiler can't optimize
    // things out.
    double sum = 0;
    for(int i = 0; i < matches.size(); ++i)
    {
      sum += matches[i].distance;
      sum += matches[i].queryIdx;
      sum += matches[i].trainIdx;
    }
    std::cerr << "Sum of match values (avoid compiler optimization) = "
              << sum
              << std::endl;
  }

  double average_run_time = timer.getAverageDelta() / (num_descs * num_descs);

  std::cout << "Average comparison time for "
            << method_name
            << " with "
            << desc_width
            << " dimensions "
            << " = "
            << average_run_time
            << std::endl;
  
  return average_run_time;
}


int main(int argc, char *argv[])
{

  // Tie everything to one CPU so timings are accurate.
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);
  assert(sched_setaffinity(0, sizeof(mask),&mask) != -1);

  // Set values for Hamming LUT.
  // This doesn't really matter since we are only testing timings.
  // However, the compiler may optimize better if it knows what
  // values are in the array.
  for(int i = 0; i < 256; ++i)
  {
    uchar val = static_cast<uchar>(i);
    lut[i] = 0;
    int mask = 0x1;
    for(int b = 0; b < 8; ++b)
    {
      lut[i] += (((mask << b) & val) >> b) == 1 ? 1 : 0;
    }
  }

  const int num_descs = 1500;
  const int num_trials = 5;
  double average_run_time;

////////////////////////////////////////////////////////////////////////////////
// L1
////////////////////////////////////////////////////////////////////////////////
  timeMethod(num_trials,
             num_descs,
             96, // desc_width
             false, // use unsigned bytes
             &l1SSEMatch);

  timeMethod(num_trials,
             num_descs,
             96, // desc_width
             false, // use unsigned bytes
             &l1NoSSEMatch);

////////////////////////////////////////////////////////////////////////////////
// Hamming
////////////////////////////////////////////////////////////////////////////////

  timeMethod(num_trials,
             num_descs,
             32, // desc_width
             false, // use unsigned bytes
             &hammingSSEMatch);

  timeMethod(num_trials,
             num_descs,
             32, // desc_width
             false, // use unsigned bytes
             &hammingSSEMatchNoPopcnt);

  timeMethod(num_trials,
             num_descs,
             32, // desc_width
             false, // use unsigned bytes
             &hammingNoSSE);

  timeMethod(num_trials,
             num_descs,
             64, // desc_width
             false, // use unsigned bytes
             &hammingSSEMatch);

  timeMethod(num_trials,
             num_descs,
             64, // desc_width
             false, // use unsigned bytes
             &hammingSSEMatchNoPopcnt);

  timeMethod(num_trials,
             num_descs,
             64, // desc_width
             false, // use unsigned bytes
             &hammingNoSSE);

////////////////////////////////////////////////////////////////////////////////
// L2
////////////////////////////////////////////////////////////////////////////////

  timeMethod(num_trials,
             num_descs,
             64, // desc_width
             true, // use floats
             &l2MatchNoSSE);

  timeMethod(num_trials,
             num_descs,
             128, // desc_width
             true, // use floats
             &l2MatchNoSSE);


  return 0;
}
