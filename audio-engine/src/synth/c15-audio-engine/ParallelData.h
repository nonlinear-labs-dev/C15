#pragma once

#include "pe_defines_config.h"
#include "nltoolbox.h"
#include <stdlib.h>
#include <type_traits>
#include <array>
#include <omp.h>

enum class DataMode
{
  Owned,
  Pointer,
  ConstPointer
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

  template <typename TOut> inline explicit operator ParallelData<DataMode::Owned, TOut, size>() const
  {
    ParallelData<DataMode::Owned, TOut, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = static_cast<TOut>(m_data[i]);

    return ret;
  }

  alignas(32) T m_data[size];
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

  template <typename TOut> inline explicit operator ParallelData<DataMode::Owned, TOut, size>() const
  {
    ParallelData<DataMode::Owned, TOut, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = static_cast<TOut>(m_data[i]);

    return ret;
  }

  T *m_data;
};

template <typename T, size_t size> class ParallelData<DataMode::ConstPointer, T, size>
{
 public:
  inline ParallelData()
  {
  }

  inline ParallelData(const T *d)
  {
    m_data = d;
  }

  inline const T &operator[](size_t i) const
  {
    return m_data[i];
  }

  template <typename TOut> inline explicit operator ParallelData<DataMode::Owned, TOut, size>() const
  {
    ParallelData<DataMode::Owned, TOut, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = static_cast<TOut>(m_data[i]);

    return ret;
  }

  const T *m_data;
};

using FloatVector = ParallelData<DataMode::Owned, float, dsp_number_of_voices>;
using Int32Vector = ParallelData<DataMode::Owned, int32_t, dsp_number_of_voices>;
using UInt32Vector = ParallelData<DataMode::Owned, uint32_t, dsp_number_of_voices>;
using FloatVectorWrapper = ParallelData<DataMode::Pointer, float, dsp_number_of_voices>;
using ConstFloatVectorWrapper = ParallelData<DataMode::ConstPointer, float, dsp_number_of_voices>;

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

  template <DataMode rhsDataMode, typename T, size_t size>
  inline ParallelData<DataMode::Owned, T, size> min(const ParallelData<rhsDataMode, T, size> &in, T a)
  {
    ParallelData<DataMode::Owned, T, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = std::min(in[i], a);

    return ret;
  }

  template <DataMode rhsDataMode, typename T, size_t size>
  inline ParallelData<DataMode::Owned, T, size> clamp(const ParallelData<rhsDataMode, T, size> &in, T min, T max)
  {
    ParallelData<DataMode::Owned, T, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = std::clamp(in[i], min, max);

    return ret;
  }

  template <typename TOut, DataMode rhsDataMode, typename T, size_t size>
  inline ParallelData<DataMode::Owned, TOut, size> round(const ParallelData<rhsDataMode, T, size> &in)
  {
    ParallelData<DataMode::Owned, TOut, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = static_cast<TOut>(std::round(in[i]));

    return ret;
  }
}

template <DataMode lhsDataMode, DataMode rhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator&(const ParallelData<lhsDataMode, T, size> &l,
                                                        const ParallelData<rhsDataMode, T, size> &r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l[i] & r[i];

  return ret;
}

template <DataMode lhsDataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> operator&(const ParallelData<lhsDataMode, T, size> &l, T r)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = l[i] & r;

  return ret;
}

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
inline ParallelData<lhsDataMode, T, size> &operator&=(ParallelData<lhsDataMode, T, size> &l,
                                                      const ParallelData<rhsDataMode, T, size> &r)
{
  for(size_t i = 0; i < size; i++)
    l[i] &= r[i];

  return l;
}

template <DataMode lhsDataMode, typename T, size_t size>
inline ParallelData<lhsDataMode, T, size> &operator&=(ParallelData<lhsDataMode, T, size> &l, T r)
{
  for(size_t i = 0; i < size; i++)
    l[i] &= r;

  return l;
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
    l[i] -= r;

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

template <DataMode dataMode1, DataMode dataMode2, DataMode dataMode3, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> unipolarCrossFade(const ParallelData<dataMode1, T, size> &_sample1,
                                                                const ParallelData<dataMode2, T, size> &_sample2,
                                                                const ParallelData<dataMode3, T, size> &_mix)
{
  return (1.f - _mix) * _sample1 + _mix * _sample2;
}

template <DataMode dataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> keepFractional(const ParallelData<dataMode, T, size> &in)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = in[i] - NlToolbox::Conversion::float2int(in[i]);

  return ret;
}

template <DataMode dataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> sinP3_noWrap(const ParallelData<dataMode, T, size> &_x)
{
  auto a = _x + _x;
  a = std::abs(a);
  a = 0.5f - a;

  auto x_square = a * a;
  return a * ((2.26548f * x_square - 5.13274f) * x_square + 3.14159f);
}

template <DataMode dataMode, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> sinP3_wrap(ParallelData<dataMode, T, size> _x)
{
  _x += -0.25f;

  for(size_t i = 0; i < size; i++)
  {
    if(_x[i] >= 0.f)
      _x[i] -= static_cast<int>(_x[i] + 0.5f);
    else
      _x[i] -= static_cast<int>(_x[i] - 0.5f);
  }

  return sinP3_noWrap(_x);
}

template <DataMode dataMode1, DataMode dataMode2, DataMode dataMode3, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> threeRanges(const ParallelData<dataMode1, T, size> &sample,
                                                          const ParallelData<dataMode2, T, size> &ctrlSample,
                                                          const ParallelData<dataMode3, T, size> &foldAmnt)
{
  ParallelData<DataMode::Owned, T, size> ret;

  for(size_t i = 0; i < size; i++)
  {
    if(ctrlSample[i] < -0.25f)
    {
      ret[i] = (sample[i] + 1.f) * foldAmnt[i] - 1.f;
    }
    else if(ctrlSample[i] > 0.25f)
    {
      ret[i] = (sample[i] - 1.f) * foldAmnt[i] + 1.f;
    }
    else
    {
      ret[i] = sample[i];
    }
  }

  return ret;
}

template <DataMode dataMode1, DataMode dataMode2, DataMode dataMode3, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> parAsym(const ParallelData<dataMode1, T, size> &sample,
                                                      const ParallelData<dataMode2, T, size> &sample_square,
                                                      const ParallelData<dataMode3, T, size> &asymAmnt)
{
  return ((1.f - asymAmnt) * sample) + (2.f * asymAmnt * sample_square);
}

template <DataMode dataMode1, DataMode dataMode2, DataMode dataMode3, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size> bipolarCrossFade(const ParallelData<dataMode1, T, size> &_sample1,
                                                               const ParallelData<dataMode2, T, size> &_sample2,
                                                               const ParallelData<dataMode3, T, size> &_mix)
{
  return (1.f - std::abs(_mix)) * _sample1 + _mix * _sample2;
}

template <DataMode m1, DataMode m2, DataMode m3, DataMode m4, DataMode m5, typename T, size_t size>
inline ParallelData<DataMode::Owned, T, size>
    interpolRT(const ParallelData<m1, T, size> &fract, const ParallelData<m2, T, size> &sample_tm1,
               const ParallelData<m3, T, size> &sample_t0, const ParallelData<m4, T, size> &sample_tp1,
               const ParallelData<m5, T, size> &sample_tp2)
{
  auto fract_square = fract * fract;
  auto fract_cube = fract_square * fract;

  auto a = 0.5f * (sample_tp1 - sample_tm1);
  auto b = 0.5f * (sample_tp2 - sample_t0);
  auto c = sample_t0 - sample_tp1;

  return sample_t0 + fract * a + fract_cube * (a + b + 2.f * c) - fract_square * (2.f * a + b + 3.f * c);
}

template <DataMode dataMode, typename T, size_t size> inline T sumUp(const ParallelData<dataMode, T, size> &in)
{
  T ret = {};

  for(size_t i = 0; i < size; i++)
    ret += in[i];

  return ret;
}
