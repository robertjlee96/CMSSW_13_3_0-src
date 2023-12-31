#ifndef CommonTools_Statistics_lms_1d_h
#define CommonTools_Statistics_lms_1d_h

#include "CommonTools/Statistics/interface/StatisticsException.h"

#include <vector>
#include <algorithm>
#include <cmath>

/**
 *  Least Median Sum of Squares in one dimension.
 *  T must be sortable, 'addable', and dividable by 2.
 */
template <class T>
T lms_1d(std::vector<T> values) {
  switch (values.size()) {
    case 0:
      throw StatisticsException("Lms of empty vector undefined");
    case 1:
      return (T) * (values.begin());
    case 2:
      return (T)((*(values.begin()) + *(values.end() - 1)) / 2);
  };
  const int size = values.size();
  const int half = size / 2;
  const int n_steps = (size + 1) / 2;

  sort(values.begin(), values.end());

  T i_begin = *(values.begin());
  T i_end = *(values.begin() + half);
  T div = (T)fabs(i_begin - i_end);
  for (typename std::vector<T>::const_iterator i = values.begin(); i != (values.begin() + n_steps); ++i) {
    if (fabs((*i) - (*(i + half))) < div) {
      i_begin = (*i);
      i_end = *(i + half);
      div = (T)fabs(i_begin - i_end);
    }
  }

  return (T)((i_begin + i_end) / 2);
}

#endif
