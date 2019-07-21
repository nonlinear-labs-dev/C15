#pragma once

#include "pe_defines_config.h"
#include "nltoolbox.h"
#include <stdlib.h>
#include <type_traits>
#include <array>
#include <omp.h>
#include <x86intrin.h>

template <typename T, size_t size> class ParallelData
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

  template <typename TOut> inline explicit operator ParallelData<TOut, size>() const
  {
    ParallelData<TOut, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = static_cast<TOut>(m_data[i]);

    return ret;
  }

  inline const T *getDataPtr() const
  {
    return m_data;
  }

  inline T *getDataPtr()
  {
    return m_data;
  }

  alignas(32) T m_data[size];
};

using FloatVector = ParallelData<float, dsp_number_of_voices>;
using Int32Vector = ParallelData<int32_t, dsp_number_of_voices>;
using UInt32Vector = ParallelData<uint32_t, dsp_number_of_voices>;

#define BINARY_P_P_OPERATOR(operation)                                                                                 \
  template <typename T, size_t size>                                                                                   \
  inline ParallelData<T, size> operator operation(const ParallelData<T, size> &l, const ParallelData<T, size> &r)      \
  {                                                                                                                    \
    ParallelData<T, size> ret;                                                                                         \
                                                                                                                       \
    for(size_t i = 0; i < size; i++)                                                                                   \
      ret[i] = l[i] operation r[i];                                                                                    \
                                                                                                                       \
    return ret;                                                                                                        \
  }

#define BINARY_P_S_OPERATOR(operation)                                                                                 \
  template <typename T, size_t size>                                                                                   \
  inline ParallelData<T, size> operator operation(const ParallelData<T, size> &l, T r)                                 \
  {                                                                                                                    \
    ParallelData<T, size> ret;                                                                                         \
                                                                                                                       \
    for(size_t i = 0; i < size; i++)                                                                                   \
      ret[i] = l[i] operation r;                                                                                       \
                                                                                                                       \
    return ret;                                                                                                        \
  }

#define BINARY_S_P_OPERATOR(operation)                                                                                 \
  template <typename T, size_t size>                                                                                   \
  inline ParallelData<T, size> operator operation(T l, const ParallelData<T, size> &r)                                 \
  {                                                                                                                    \
    ParallelData<T, size> ret;                                                                                         \
                                                                                                                       \
    for(size_t i = 0; i < size; i++)                                                                                   \
      ret[i] = l operation r[i];                                                                                       \
                                                                                                                       \
    return ret;                                                                                                        \
  }

#define BINARY_P_P_ASSIGNEMT_OPERATOR(operation)                                                                       \
  template <typename T, size_t size>                                                                                   \
  inline ParallelData<T, size> &operator operation##=(ParallelData<T, size> &l, const ParallelData<T, size> &r)        \
  {                                                                                                                    \
    for(size_t i = 0; i < size; i++)                                                                                   \
      l[i] operation## = r[i];                                                                                         \
                                                                                                                       \
    return l;                                                                                                          \
  }

#define BINARY_P_S_ASSIGNEMT_OPERATOR(operation)                                                                       \
  template <typename T, size_t size>                                                                                   \
  inline ParallelData<T, size> &operator operation##=(ParallelData<T, size> &l, T r)                                   \
  {                                                                                                                    \
    for(size_t i = 0; i < size; i++)                                                                                   \
      l[i] operation## = r;                                                                                            \
                                                                                                                       \
    return l;                                                                                                          \
  }

#define VECTOR_P_S_CMP_OPERATOR(operation, imm)                                                                        \
  template <typename T, size_t size>                                                                                   \
  inline ParallelData<uint32_t, size> operator operation(const ParallelData<T, size> &l, T r)                          \
  {                                                                                                                    \
    constexpr auto iterations = size / 4;                                                                              \
                                                                                                                       \
    ParallelData<uint32_t, size> ret;                                                                                  \
    __m128 b = { r, r, r, r };                                                                                         \
                                                                                                                       \
    for(size_t i = 0; i < iterations; i++)                                                                             \
    {                                                                                                                  \
      auto a = reinterpret_cast<const __m128 *>(l.getDataPtr() + 4 * i);                                               \
      auto t = reinterpret_cast<__m128 *>(ret.getDataPtr() + 4 * i);                                                   \
      *t = _mm_cmp_ps(*a, b, imm);                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
    return ret;                                                                                                        \
  }

#define DEFINE_OPERATORS(operation)                                                                                    \
  BINARY_P_P_OPERATOR(operation)                                                                                       \
  BINARY_P_S_OPERATOR(operation)                                                                                       \
  BINARY_S_P_OPERATOR(operation)                                                                                       \
  BINARY_P_P_ASSIGNEMT_OPERATOR(operation)                                                                             \
  BINARY_P_S_ASSIGNEMT_OPERATOR(operation)

DEFINE_OPERATORS(+)
DEFINE_OPERATORS(-)
DEFINE_OPERATORS(*)
DEFINE_OPERATORS(/)

BINARY_P_S_OPERATOR(&)
BINARY_P_S_OPERATOR(|)
BINARY_P_S_ASSIGNEMT_OPERATOR(&)
BINARY_P_S_ASSIGNEMT_OPERATOR(|)

VECTOR_P_S_CMP_OPERATOR(<, _CMP_LT_OS)
VECTOR_P_S_CMP_OPERATOR(>, _CMP_GT_OS)
VECTOR_P_S_CMP_OPERATOR(<=, _CMP_LE_OS)
VECTOR_P_S_CMP_OPERATOR(>=, _CMP_GE_OS)

template <typename T1, typename T2, size_t size>
inline ParallelData<T1, size> operator&(const ParallelData<T1, size> &l, const ParallelData<T2, size> &r)
{
  ParallelData<T1, size> ret;

  auto intL = reinterpret_cast<const uint32_t *>(l.getDataPtr());
  auto intR = reinterpret_cast<const uint32_t *>(r.getDataPtr());
  auto retPtr = reinterpret_cast<uint32_t *>(ret.getDataPtr());

  for(size_t i = 0; i < size; i++)
    retPtr[i] = intL[i] & intR[i];

  return ret;
}

template <typename T1, typename T2, size_t size>
inline ParallelData<T1, size> &operator&=(ParallelData<T1, size> &l, const ParallelData<T2, size> &r)
{
  for(size_t i = 0; i < size; i++)
    l[i] &= r[i];

  return l;
}

namespace std
{
  template <typename T, size_t size> inline ParallelData<T, size> abs(const ParallelData<T, size> &in)
  {
    ParallelData<T, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = abs(in[i]);

    return ret;
  }

  template <typename T, size_t size> inline ParallelData<T, size> min(const ParallelData<T, size> &in, T a)
  {
    ParallelData<T, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = std::min(in[i], a);

    return ret;
  }

  template <typename T, size_t size> inline ParallelData<T, size> max(const ParallelData<T, size> &in, T a)
  {
    ParallelData<T, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = std::max(in[i], a);

    return ret;
  }

  template <typename T, size_t size> inline ParallelData<T, size> clamp(const ParallelData<T, size> &in, T min, T max)
  {
    ParallelData<T, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = std::clamp(in[i], min, max);

    return ret;
  }

  // with the new state variable filter, we need a parallel vector max
  template <typename T, size_t size> inline ParallelData<T, size> clamp(const ParallelData<T, size> &in,
                                                                        T min, const ParallelData<T, size> &max)
  {
    ParallelData<T, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = std::clamp(in[i], min, max[i]);

    return ret;
  }

  template <typename TOut, typename T, size_t size>
  inline ParallelData<TOut, size> round(const ParallelData<T, size> &in)
  {
    ParallelData<TOut, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = static_cast<TOut>(std::round(in[i]));

    return ret;
  }
}

template <typename T, size_t size>
inline ParallelData<T, size> unipolarCrossFade(const ParallelData<T, size> &_sample1,
                                               const ParallelData<T, size> &_sample2, const float &_mix)
{
  return (1.f - _mix) * _sample1 + _mix * _sample2;
}

template <typename T, size_t size> inline ParallelData<T, size> keepFractional(const ParallelData<T, size> &in)
{
  ParallelData<T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = in[i] - NlToolbox::Conversion::float2int(in[i]);

  return ret;
}

template <typename T, size_t size> inline ParallelData<T, size> sinP3_wrap(ParallelData<T, size> _x)
{
  ParallelData<T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = NlToolbox::Math::sinP3_wrap(_x[i]);

  return ret;
}

template <typename T, size_t size> inline ParallelData<T, size> sinP3_noWrap(ParallelData<T, size> _x)
{
  ParallelData<T, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = NlToolbox::Math::sinP3_noWrap(_x[i]);

  return ret;
}

template <typename T, size_t size>
inline ParallelData<T, size> threeRanges(const ParallelData<T, size> &sample, const ParallelData<T, size> &ctrlSample,
                                         const float &foldAmnt)
{
  ParallelData<T, size> ret;

  for(size_t i = 0; i < size; i++)
  {
    if(ctrlSample[i] < -0.25f)
    {
      ret[i] = (sample[i] + 1.f) * foldAmnt - 1.f;
    }
    else if(ctrlSample[i] > 0.25f)
    {
      ret[i] = (sample[i] - 1.f) * foldAmnt + 1.f;
    }
    else
    {
      ret[i] = sample[i];
    }
  }

  return ret;
}

template <typename T, size_t size>
inline ParallelData<T, size> parAsym(const ParallelData<T, size> &sample, const ParallelData<T, size> &sample_square,
                                     const float &asymAmnt)
{
  return ((1.f - asymAmnt) * sample) + (2.f * asymAmnt * sample_square);
}

template <typename T, size_t size>
inline ParallelData<T, size> bipolarCrossFade(const ParallelData<T, size> &_sample1,
                                              const ParallelData<T, size> &_sample2, const T &_mix)
{
  return (1.f - std::abs(_mix)) * _sample1 + _mix * _sample2;
}

template <typename T, size_t size>
inline ParallelData<T, size> interpolRT(const ParallelData<T, size> &fract, const ParallelData<T, size> &sample_tm1,
                                        const ParallelData<T, size> &sample_t0, const ParallelData<T, size> &sample_tp1,
                                        const ParallelData<T, size> &sample_tp2)
{
  auto fract_square = fract * fract;
  auto fract_cube = fract_square * fract;

  auto a = 0.5f * (sample_tp1 - sample_tm1);
  auto b = 0.5f * (sample_tp2 - sample_t0);
  auto c = sample_t0 - sample_tp1;

  return sample_t0 + fract * a + fract_cube * (a + b + 2.f * c) - fract_square * (2.f * a + b + 3.f * c);
}

template <typename T, size_t size> inline T sumUp(const ParallelData<T, size> &in)
{
  T ret = {};

  for(size_t i = 0; i < size; i++)
    ret += in[i];

  return ret;
}
