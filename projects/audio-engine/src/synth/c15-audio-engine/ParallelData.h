#pragma once

#include "pe_defines_config.h"
#include "nltoolbox.h"
#include <stdlib.h>
#include <type_traits>
#include <array>
#include <omp.h>
#include <x86intrin.h>

#include "ae_potential_improvements.h"

#if POTENTIAL_IMPROVEMENT_COMB_REDUCE_VOICE_LOOP_3
#include <vector>
#endif

template <typename T, size_t size> class ParallelData
{
 public:
  inline ParallelData(const T *d)
  {
    for(size_t i = 0; i < size; i++)
      m_data[i] = d[i];
  }

  inline ParallelData(T d = {})
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
    constexpr auto parallelism = 4;                                                                                    \
    constexpr auto iterations = size / parallelism;                                                                    \
    static_assert((size % parallelism) == 0, "Cannot use this operator with this type!");                              \
    ParallelData<uint32_t, size> ret;                                                                                  \
    __m128 b = { r, r, r, r };                                                                                         \
                                                                                                                       \
    for(size_t i = 0; i < iterations; i++)                                                                             \
    {                                                                                                                  \
      auto a = reinterpret_cast<const __m128 *>(l.getDataPtr() + parallelism * i);                                     \
      auto t = reinterpret_cast<__m128 *>(ret.getDataPtr() + parallelism * i);                                         \
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

// the following (std::abs, std::min, std::max, std::clamp, std::round) parallel operations are now only operable with floats
namespace std
{
  template <size_t size> inline ParallelData<float, size> abs(const ParallelData<float, size> &in)
  {
#if POTENTIAL_IMPROVEMENT_PARALLEL_DATA_STD_ABS
    // unfortunately, I did not manage to establish a reduced operator like:
    //   template <typename T1, size_t size>
    //   inline ParallelData<T1, size> operator&(const ParallelData<T1, size> &l, uint32_t &r) {...}
    // - so, for now this is the only way I can come up with
    ParallelData<float, size> ret = in & ParallelData<uint32_t, size>(0x7FFFFFFF);  // masking sign bit
    return ret;
    // first tests revealed no audible difference
#else
    ParallelData<float, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = abs(in[i]);

    return ret;
#endif
  }

  template <size_t size> inline ParallelData<float, size> min(const ParallelData<float, size> &in, float a)
  {
#if POTENTIAL_IMPROVEMENT_PARALLEL_DATA_STD_MINMAX_FLOAT
    constexpr auto parallelism = 4;
    constexpr auto iterations = size / parallelism;
    static_assert((size % parallelism) == 0, "Cannot use std::min with this type!");
    ParallelData<float, size> ret;
    __m128 cmp = { a, a, a, a };

    for(size_t i = 0; i < iterations; i++)
    {
      auto val = reinterpret_cast<const __m128 *>(in.getDataPtr() + parallelism * i);
      auto tmp = reinterpret_cast<__m128 *>(ret.getDataPtr() + parallelism * i);
      *tmp = _mm_min_ps(*val, cmp);
    }

    return ret;
    // first tests revealed no audible difference
#else
    ParallelData<float, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = std::min(in[i], a);

    return ret;
#endif
  }

  template <size_t size> inline ParallelData<float, size> max(const ParallelData<float, size> &in, float a)
  {
#if POTENTIAL_IMPROVEMENT_PARALLEL_DATA_STD_MINMAX_FLOAT
    constexpr auto parallelism = 4;
    constexpr auto iterations = size / parallelism;
    static_assert((size % parallelism) == 0, "Cannot use std::max with this type!");
    ParallelData<float, size> ret;
    __m128 cmp = { a, a, a, a };

    for(size_t i = 0; i < iterations; i++)
    {
      auto val = reinterpret_cast<const __m128 *>(in.getDataPtr() + parallelism * i);
      auto tmp = reinterpret_cast<__m128 *>(ret.getDataPtr() + parallelism * i);
      *tmp = _mm_max_ps(*val, cmp);
    }

    return ret;
    // first tests revealed no audible difference
#else
    ParallelData<float, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = std::max(in[i], a);

    return ret;
#endif
  }

  template <size_t size>
  inline ParallelData<float, size> clamp(const ParallelData<float, size> &in, float _min, float _max)
  {
#if POTENTIAL_IMPROVEMENT_PARALLEL_DATA_STD_MINMAX_FLOAT
    ParallelData<float, size> ret;

    ret = max(min(in, _max), _min);

    return ret;
    // first tests revealed no audible difference
#else
    ParallelData<float, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = std::clamp(in[i], _min, _max);

    return ret;
#endif
  }

  // with the new state variable filter, we need a parallel vector max
  template <size_t size>
  inline ParallelData<float, size> clamp(const ParallelData<float, size> &in, float _min,
                                         const ParallelData<float, size> &_max)
  {
#if POTENTIAL_IMPROVEMENT_PARALLEL_DATA_STD_MINMAX_FLOAT
    constexpr auto parallelism = 4;
    constexpr auto iterations = size / parallelism;
    static_assert((size % parallelism) == 0, "Cannot use std::clamp with this type!");
    ParallelData<float, size> ret = max(in, _min);

    for(size_t i = 0; i < iterations; i++)
    {
      auto val = reinterpret_cast<__m128 *>(ret.getDataPtr() + parallelism * i);
      auto cmpMax = reinterpret_cast<const __m128 *>(_max.getDataPtr() + parallelism * i);
      *val = _mm_min_ps(*val, *cmpMax);
    }

    return ret;
    // first tests revealed no audible difference
#else
    ParallelData<float, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = std::clamp(in[i], _min, _max[i]);

    return ret;
#endif
  }

  template <typename TOut, size_t size> inline ParallelData<TOut, size> round(const ParallelData<float, size> &in)
  {
#if POTENTIAL_IMPROVEMENT_PARALLEL_DATA_STD_ROUND
    // assuming that the ROUNDING paradigm should fit nltoolbox.h[line 359]::float2int()
    const ParallelData<float, size> wrap = 0.5f
        + static_cast<ParallelData<float, size>>(ParallelData<int32_t, size>(
              (in < 0.0f)));  // negative: -0.5, positive: 0.5
    ParallelData<TOut, size> ret = static_cast<ParallelData<TOut, size>>(in + wrap);
    return ret;
    // first tests revealed no audible difference
#else
    ParallelData<TOut, size> ret;

    for(size_t i = 0; i < size; i++)
      ret[i] = static_cast<TOut>(std::round(in[i]));

    return ret;
#endif
  }
}

// the following (unipolarCrossFade, keepFractional, sinP3_wrap, sinP3_noWrap, threeRanges, parAsym, bipolarCrossFade, interpolRT) ...
// ... parallel operations are now only operable with floats
template <size_t size>
inline ParallelData<float, size> unipolarCrossFade(const ParallelData<float, size> &_sample1,
                                                   const ParallelData<float, size> &_sample2, const float &_mix)
{
  return (1.f - _mix) * _sample1 + _mix * _sample2;
}

template <size_t size> inline ParallelData<float, size> keepFractional(const ParallelData<float, size> &in)
{
#if POTENTIAL_IMPROVEMENT_PARALLEL_DATA_KEEP_FRACTIONAL
  const ParallelData<float, size> wrap = 0.5f
      + static_cast<ParallelData<float, size>>(ParallelData<int32_t, size>(
            (in < 0.0f)));  // negative: -0.5, positive: 0.5
  ParallelData<float, size> ret
      = in - static_cast<ParallelData<float, size>>(static_cast<ParallelData<int32_t, size>>(in + wrap));
  return ret;
  // first tests revealed no audible difference
#else
  ParallelData<float, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = in[i] - NlToolbox::Conversion::float2int(in[i]);

  return ret;
#endif
}

template <size_t size> inline ParallelData<float, size> sinP3_wrap(ParallelData<float, size> _x)
{
#if POTENTIAL_IMPROVEMENT_PARALLEL_DATA_SINP3_WRAP
  _x += -0.25f;
  const ParallelData<float, size> wrap = 0.5f
      + static_cast<ParallelData<float, size>>(ParallelData<int32_t, size>(
            (_x < 0.0f)));  // negative: -0.5, positive: 0.5
  _x -= static_cast<ParallelData<float, size>>(static_cast<ParallelData<int32_t, size>>(_x + wrap));
  // similar to sinP3_noWrap now...
  const ParallelData<float, size> x = 0.5f - std::abs(_x + _x), squared = x * x;
  ParallelData<float, size> ret = x * ((2.26548f * squared - 5.13274f) * squared + 3.14159f);
  return ret;
  // first tests revealed no audible difference
#else
  ParallelData<float, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = NlToolbox::Math::sinP3_wrap(_x[i]);

  return ret;
#endif
}

template <size_t size> inline ParallelData<float, size> sinP3_noWrap(ParallelData<float, size> _x)
{
#if POTENTIAL_IMPROVEMENT_PARALLEL_DATA_SINP3_NOWRAP
  const ParallelData<float, size> x = 0.5f - std::abs(_x + _x), squared = x * x;
  ParallelData<float, size> ret = x * ((2.26548f * squared - 5.13274f) * squared + 3.14159f);
  return ret;
  // first tests revealed no audible difference
#else
  ParallelData<float, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = NlToolbox::Math::sinP3_noWrap(_x[i]);

  return ret;
#endif
}

template <size_t size>
inline ParallelData<float, size> threeRanges(const ParallelData<float, size> &sample,
                                             const ParallelData<float, size> &ctrlSample, const float &foldAmnt)
{
#if POTENTIAL_IMPROVEMENT_PARALLEL_DATA_THREE_RANGES
  const ParallelData<float, size> abs = std::abs(ctrlSample);
  // fold: contains -1 (true) or 0 (false)
  const ParallelData<float, size> fold
      = static_cast<ParallelData<float, size>>(ParallelData<int32_t, size>((abs > 0.25f)));
  // sign: properly reflects sign (negative: -1, positive: 1)
  const ParallelData<float, size> sign
      = (-2.0f * static_cast<ParallelData<float, size>>(ParallelData<int32_t, size>((ctrlSample >= 0.0f)))) - 1.0f;
  const ParallelData<float, size> shaped
      = ((sample - sign) * foldAmnt) + sign - sample;  // expressed as difference to "unshaped" sample
  ParallelData<float, size> ret = sample - (fold * shaped);
  return ret;
  // first tests revealed no audible difference
#else
  ParallelData<float, size> ret;

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
#endif
}

template <size_t size>
inline ParallelData<float, size> parAsym(const ParallelData<float, size> &sample,
                                         const ParallelData<float, size> &sample_square, const float &asymAmnt)
{
  return ((1.f - asymAmnt) * sample) + (2.f * asymAmnt * sample_square);
}

template <size_t size>
inline ParallelData<float, size> bipolarCrossFade(const ParallelData<float, size> &_sample1,
                                                  const ParallelData<float, size> &_sample2, const float &_mix)
{
  return (1.f - std::abs(_mix)) * _sample1 + _mix * _sample2;
}

template <size_t size>
inline ParallelData<float, size>
    interpolRT(const ParallelData<float, size> &fract, const ParallelData<float, size> &sample_tm1,
               const ParallelData<float, size> &sample_t0, const ParallelData<float, size> &sample_tp1,
               const ParallelData<float, size> &sample_tp2)
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

#if POTENTIAL_IMPROVEMENT_COMB_REDUCE_VOICE_LOOP_3
// this function is NOT compliant with SIMD, however it abstracts the voice loop away from the Comb Filter,
// fitting the established parallel paradigm (no voice loops necessary in poly components)
template <typename TScalar, typename TIntegral, size_t size>
inline ParallelData<TScalar, size> polyVectorIndex(const std::vector<ParallelData<TScalar, size>> &_vector,
                                                   const ParallelData<TIntegral, size> &_index)
{
  ParallelData<TScalar, size> ret;

  for(size_t i = 0; i < size; i++)
  {
    ret[i] = _vector[_index[i]][i];
  }

  return ret;
}
#endif
