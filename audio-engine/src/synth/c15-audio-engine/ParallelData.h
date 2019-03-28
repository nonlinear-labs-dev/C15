#pragma once

#include "pe_defines_config.h"
#include <stdlib.h>
#include <type_traits>
#include <array>

enum class DataMode
{
  Owned,
  Pointer
};

template <DataMode mode, typename T, size_t size> class ParallelData;

template <typename T, size_t size> class ParallelData<DataMode::Owned, T, size>
{
 public:
  inline ParallelData()
  {
  }

  inline ParallelData(const T *d)
  {
    for(size_t i = 0; i < size; i++)
      m_data[i] = d[i];
  }

  inline ParallelData(T d)
  {
    for(size_t i = 0; i < size; i++)
      m_data[i] = d;
  }

  inline const T &operator[](size_t i) const
  {
    return m_data[i];
  }

  inline T &operator[](size_t i)
  {
    return m_data[i];
  }

 private:
  std::array<T, size> m_data;
};

template <typename T, size_t size> class ParallelData<DataMode::Pointer, T, size>
{
 public:
  inline ParallelData()
  {
  }

  inline ParallelData(T *d)
  {
    m_data = d;
  }

  inline const T &operator[](size_t i) const
  {
    return m_data[i];
  }

  inline T &operator[](size_t i)
  {
    return m_data[i];
  }

 private:
  T *m_data;
};

using FloatVector = ParallelData<DataMode::Owned, float, dsp_number_of_voices>;
using Int32Vector = ParallelData<DataMode::Owned, int32_t, dsp_number_of_voices>;
using UInt32Vector = ParallelData<DataMode::Owned, uint32_t, dsp_number_of_voices>;
using FloatVectorWrapper = ParallelData<DataMode::Pointer, float, dsp_number_of_voices>;

template <DataMode lhsDataMode, DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator*(const ParallelData<lhsDataMode, T, size> &l,
                                                        const ParallelData<rhsDataMode, T, size> &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l[i] * r[i];

  return ret;
}

template <DataMode lhsDataMode, DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator/(const ParallelData<lhsDataMode, T, size> &l,
                                                        const ParallelData<rhsDataMode, T, size> &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l[i] / r[i];

  return ret;
}

template <DataMode lhsDataMode, DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator+(const ParallelData<lhsDataMode, T, size> &l,
                                                        const ParallelData<rhsDataMode, T, size> &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l[i] + r[i];

  return ret;
}

template <DataMode lhsDataMode, DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator-(const ParallelData<lhsDataMode, T, size> &l,
                                                        const ParallelData<rhsDataMode, T, size> &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l[i] - r[i];

  return ret;
}

template <DataMode lhsDataMode, DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<lhsDataMode, T, size> &operator*=(ParallelData<lhsDataMode, T, size> &l,
                                                      const ParallelData<rhsDataMode, T, size> &r)
{
  for(size_t i = 0; i < size; i++)
    l[i] *= r[i];

  return l;
}

template <DataMode lhsDataMode, DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<lhsDataMode, T, size> &operator/=(ParallelData<lhsDataMode, T, size> &l,
                                                      const ParallelData<rhsDataMode, T, size> &r)
{
  for(size_t i = 0; i < size; i++)
    l[i] /= r[i];

  return l;
}

template <DataMode lhsDataMode, DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<lhsDataMode, T, size> &operator+=(ParallelData<lhsDataMode, T, size> &l,
                                                      const ParallelData<rhsDataMode, T, size> &r)
{
  for(size_t i = 0; i < size; i++)
    l[i] += r[i];

  return l;
}

template <DataMode lhsDataMode, DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<lhsDataMode, T, size> &operator-=(ParallelData<lhsDataMode, T, size> &l,
                                                      const ParallelData<rhsDataMode, T, size> &r)
{
  for(size_t i = 0; i < size; i++)
    l[i] -= r[i];

  return l;
}

// scalars rhs

template <DataMode lhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator*(const ParallelData<lhsDataMode, T, size> &l, const T &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l[i] * r;

  return ret;
}

template <DataMode lhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator/(const ParallelData<lhsDataMode, T, size> &l, const T &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l[i] / r;

  return ret;
}

template <DataMode lhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator+(const ParallelData<lhsDataMode, T, size> &l, const T &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l[i] + r;

  return ret;
}

template <DataMode lhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator-(const ParallelData<lhsDataMode, T, size> &l, const T &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l[i] - r;

  return ret;
}

template <DataMode lhsDataMode, typename T, size_t size>
inline ParallelData<lhsDataMode, T, size> &operator*=(ParallelData<lhsDataMode, T, size> &l, const T &r)
{
  for(size_t i = 0; i < size; i++)
    l[i] *= r;

  return l;
}

template <DataMode lhsDataMode, typename T, size_t size>
inline ParallelData<lhsDataMode, T, size> &operator/=(ParallelData<lhsDataMode, T, size> &l, const T &r)
{
  for(size_t i = 0; i < size; i++)
    l[i] /= r;

  return l;
}

template <DataMode lhsDataMode, typename T, size_t size>
inline ParallelData<lhsDataMode, T, size> &operator+=(ParallelData<lhsDataMode, T, size> &l, const T &r)
{
  for(size_t i = 0; i < size; i++)
    l[i] += r;

  return l;
}

template <DataMode lhsDataMode, typename T, size_t size>
inline ParallelData<lhsDataMode, T, size> &operator-=(ParallelData<lhsDataMode, T, size> &l, const T &r)
{
  for(size_t i = 0; i < size; i++)
    l[i] -= r;

  return l;
}

// scalars lhs

template <DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator*(const T &l, const ParallelData<rhsDataMode, T, size> &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l * r[i];

  return ret;
}

template <DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator/(const T &l, const ParallelData<rhsDataMode, T, size> &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l / r[i];

  return ret;
}

template <DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator+(const T &l, const ParallelData<rhsDataMode, T, size> &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l + r[i];

  return ret;
}

template <DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator-(const T &l, const ParallelData<rhsDataMode, T, size> &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l - r[i];

  return ret;
}

namespace std
{
  template <DataMode rhsDataMode, typename T, size_t size>
  inline ParallelData<DataMode::Owned, T, size> abs(const ParallelData<rhsDataMode, T, size> &in)
  {
    ParallelData<DataMode::Owned, T, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = abs(in[i]);

    return ret;
  }
}
