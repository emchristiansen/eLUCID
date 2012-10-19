#include <iostream>
#include <nmmintrin.h>
#include <stdint.h>
#include <stddef.h>

#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

// Computes the permutation which sorts |unsortedElements|.
// The permutation is stored in |sortPermutation|.
// Optionally returns the sort of the elements, stored in |sortedElements|.
void countingSort(const int numElements,
		  const uint8_t unsortedElements[],
		  uint16_t *sortPermutation,
		  uint8_t *sortedElements) {
  const int MAX_VALUE = 256;
  int counts[MAX_VALUE];

  // Reset counts.
  for (int i = 0; i < MAX_VALUE; ++i) {
    counts[i] = 0;
  }

  // Initialize counts.
  for (int i = 0; i < numElements; ++i) {
    counts[unsortedElements[i]] += 1;
  }

  // Compute running totals.
  int runningTotal = 0;
  for (int i = 0; i < MAX_VALUE; ++i) {
    const int currentCount = counts[i];
    counts[i] = runningTotal;
    runningTotal += currentCount;
  }

  // Find the rank permutation.
  for (int i = 0; i < numElements; ++i) {
    sortPermutation[counts[unsortedElements[i]]] = i;
    counts[unsortedElements[i]] += 1;
  }

  if (sortedElements != NULL) {
    // Sort list.
    for(int i = 0; i < numElements; ++i) {
      sortedElements[i] = unsortedElements[sortPermutation[i]];
    }
  }
}

// Computes the l0 distance between |left| and |right|, where the arguments
// are vectors of the same length, stored as single-row Mats of type uint16_t.
int distanceL0(const Mat& left, const Mat& right) {
  assert(left.rows == 1);
  assert(right.rows == 1);
  assert(left.cols == right.cols);
  assert(left.type() == CV_16UC1);
  assert(right.type() == CV_16UC1);

  int numDifferent = 0;
  MatConstIterator_<uint16_t> l = left.begin<uint16_t>();
  MatConstIterator_<uint16_t> r = right.begin<uint16_t>();
  for (; l != left.end<uint16_t>(); ++l, ++r) {
    numDifferent += *l != *r ? 1 : 0;
  }
  return numDifferent;
}

union __oword_t {
  __m128i m128i;
  uint64_t m128i_u64[2];
};

typedef union __oword_t __oword;

// The same as |distanceL0|, but sped up with SSE.
int distanceL0SSE(const Mat& left, const Mat& right) {
  assert(left.rows == 1);
  assert(right.rows == 1);
  assert(left.cols == right.cols);
  assert(left.type() == CV_16UC1);
  assert(right.type() == CV_16UC1);

  const int descriptorSize = left.cols;
  // Assume the descriptorSize is a multiple of 8.
  assert(!(descriptorSize % 8));

  // TODO: "register" probably doesn't help.
  register __oword xmm0;
  register __oword xmm1;

  const __m128i *leftSSE = left.ptr<__m128i>(0);
  const __m128i *rightSSE = right.ptr<__m128i>(0);

  int numOnes = 0;
  for (int k = 0; k < descriptorSize / 8; ++k) {
    // Load descriptor elements for comparison.
    xmm0.m128i = _mm_loadu_si128(leftSSE + k);
    xmm1.m128i = _mm_load_si128(rightSSE + k);

    // Compare them.
    xmm0.m128i = _mm_cmpeq_epi16(xmm0.m128i, xmm1.m128i);

    // Do pop-count.
    numOnes += _mm_popcnt_u64(xmm0.m128i_u64[0]) + 
               _mm_popcnt_u64(xmm0.m128i_u64[1]);
  }

  // Obtain Hamming distance by dividing by 16 and taking the complement.
  const int distance = descriptorSize - (numOnes >> 4);
  return distance;
}

// Just a demo and a test that the distances agree.
int main () {
  uint8_t leftValues[] = {4, 1, 2, 3, 4, 5, 1, 6, 
			  3, 5, 2, 3, 1, 3, 4, 5};
  Mat leftPermutation(1, 16, CV_16UC1);
  countingSort(16, leftValues, leftPermutation.ptr<uint16_t>(0), NULL);

  uint8_t rightValues[] = {2, 2, 5, 1, 6, 2, 6, 7,
			   2, 7, 8, 4, 2, 3, 1, 6};
  Mat rightPermutation(1, 16, CV_16UC1);
  countingSort(16, rightValues, rightPermutation.ptr<uint16_t>(0), NULL);

  const int distance0 = distanceL0(leftPermutation, rightPermutation);
  const int distance1 = distanceL0SSE(leftPermutation, rightPermutation);

  assert(distance0 == distance1);

  cout << distance0 << endl;
}
