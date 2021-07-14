#pragma once

#include <stdlib.h>
#include <type_traits>
#include <array>
#include <omp.h>
#include <vector>
#include <cstdint>

#ifdef __arm__

#include <arm_neon.h>

#define _CMP_LT_OS vcltq_f32
#define _CMP_GT_OS vcgtq_f32
#define _CMP_LE_OS vcleq_f32
#define _CMP_GE_OS vcgeq_f32

#else

#include <x86intrin.h>

using int32x4_t = __m128i;
using uint32x4_t = __m128i;
using float32x4_t = __m128;

#endif

void testParallelData();

namespace parallel_data_detail
{
  template <typename T, size_t simdSize, size_t parallelism> union v128
  {
    int32x4_t mmi[simdSize];
    uint32x4_t mmu[simdSize];
    float32x4_t mmf[simdSize];
    T raw[simdSize * parallelism];
  };

  template <size_t simdSize, size_t parallelism> struct Copy
  {
#ifdef __arm__

    static void copy(v128<int32_t, simdSize, parallelism> &out, const v128<float, simdSize, parallelism> &in)
    {
      for(int i = 0; i < simdSize; i++)
        out.mmi[i] = vcvtq_s32_f32(in.mmf[i]);
    }

    static void copy(v128<float, simdSize, parallelism> &out, const v128<int32_t, simdSize, parallelism> &in)
    {
      for(int i = 0; i < simdSize; i++)
        out.mmf[i] = vcvtq_f32_s32(in.mmi[i]);
    }

#else

    static void copy(v128<int32_t, simdSize, parallelism> &out, const v128<float, simdSize, parallelism> &in)
    {
      for(int i = 0; i < simdSize; i++)
        out.mmi[i] = _mm_cvttps_epi32(in.mmf[i]);
    }

    static void copy(v128<float, simdSize, parallelism> &out, const v128<int32_t, simdSize, parallelism> &in)
    {
      for(int i = 0; i < simdSize; i++)
        out.mmf[i] = _mm_cvtepi32_ps(in.mmi[i]);
    }

#endif

    static void copy(v128<uint32_t, simdSize, parallelism> &out, const v128<int32_t, simdSize, parallelism> &in)
    {
      for(int i = 0; i < simdSize; i++)
        out.mmi[i] = in.mmi[i];
    }

    static void copy(v128<int32_t, simdSize, parallelism> &out, const v128<uint32_t, simdSize, parallelism> &in)
    {
      for(int i = 0; i < simdSize; i++)
        out.mmi[i] = in.mmi[i];
    }
  };
}

template <typename T, size_t size> class ParallelData
{
 public:
  static constexpr auto parallelism = 4;
  static constexpr auto simdSize = (size % parallelism) ? (1 + size / parallelism) : size / parallelism;

  inline ParallelData(const std::array<T, size> &d)
  {
    std::copy(d.begin(), d.end(), m_data.raw);
  }

  inline ParallelData(const T *d)
  {
    for(size_t i = 0; i < size; i++)
      m_data.raw[i] = d[i];
  }

  inline ParallelData(T d = {})
  {
    for(size_t i = 0; i < size; i++)
      m_data.raw[i] = d;
  }

  inline const T &operator[](size_t i) const
  {
    return m_data.raw[i];
  }

  inline T &operator[](size_t i)
  {
    return m_data.raw[i];
  }

  template <typename TOut> inline explicit operator ParallelData<TOut, size>() const
  {
    ParallelData<TOut, size> ret;
    parallel_data_detail::Copy<simdSize, parallelism>::copy(ret.m_data, this->m_data);
    return ret;
  }

  parallel_data_detail::v128<T, simdSize, parallelism> m_data;
};

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

#ifdef __arm__

#define VECTOR_P_S_CMP_OPERATOR(operation, imm)                                                                        \
  template <typename T, size_t size>                                                                                   \
  inline ParallelData<uint32_t, size> operator operation(const ParallelData<T, size> &l, T r)                          \
  {                                                                                                                    \
    ParallelData<uint32_t, size> ret;                                                                                  \
    float32x4_t b = { r, r, r, r };                                                                                    \
                                                                                                                       \
    for(size_t i = 0; i < ParallelData<T, size>::simdSize; i++)                                                        \
      ret.m_data.mmu[i] = imm(l.m_data.mmf[i], b);                                                                     \
                                                                                                                       \
    return ret;                                                                                                        \
  }

#else

#define VECTOR_P_S_CMP_OPERATOR(operation, imm)                                                                        \
  template <typename T, size_t size>                                                                                   \
  inline ParallelData<uint32_t, size> operator operation(const ParallelData<T, size> &l, T r)                          \
  {                                                                                                                    \
    ParallelData<uint32_t, size> ret;                                                                                  \
    float32x4_t b = { r, r, r, r };                                                                                    \
                                                                                                                       \
    for(size_t i = 0; i < ParallelData<T, size>::simdSize; i++)                                                        \
      ret.m_data.mmf[i] = _mm_cmp_ps(l.m_data.mmf[i], b, imm);                                                         \
                                                                                                                       \
    return ret;                                                                                                        \
  }

#endif

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

  for(size_t i = 0; i < ParallelData<T1, size>::simdSize; i++)
    ret.m_data.mmi[i] = l.m_data.mmi[i] & r.m_data.mmi[i];

  return ret;
}

template <typename T1, typename T2, size_t size>
inline ParallelData<T1, size> &operator&=(ParallelData<T1, size> &l, const ParallelData<T2, size> &r)
{
  l = l & r;
  return l;
}

// the following (std::abs, std::min, std::max, std::clamp, std::round) parallel operations are now only operable with floats
namespace std
{
  template <size_t size> inline ParallelData<float, size> abs(const ParallelData<float, size> &in)
  {
    return in & ParallelData<uint32_t, size>(0x7FFFFFFF);  // masking sign bit
  }

#ifdef __arm__

  template <size_t size> inline ParallelData<float, size> min(const ParallelData<float, size> &in, float a)
  {
    ParallelData<float, size> ret;
    float32x4_t cmp = { a, a, a, a };

    for(size_t i = 0; i < ParallelData<float, size>::simdSize; i++)
      ret.m_data.mmf[i] = vminq_f32(in.m_data.mmf[i], cmp);

    return ret;
  }

  template <size_t size>
  inline ParallelData<float, size> min(const ParallelData<float, size> &l, const ParallelData<float, size> &r)
  {
    ParallelData<float, size> ret;

    for(size_t i = 0; i < ParallelData<float, size>::simdSize; i++)
      ret.m_data.mmf[i] = vminq_f32(r.m_data.mmf[i], l.m_data.mmf[i]);

    return ret;
  }

  template <size_t size> inline ParallelData<float, size> max(const ParallelData<float, size> &in, float a)
  {
    ParallelData<float, size> ret;
    float32x4_t cmp = { a, a, a, a };

    for(size_t i = 0; i < ParallelData<float, size>::simdSize; i++)
      ret.m_data.mmf[i] = vmaxq_f32(in.m_data.mmf[i], cmp);

    return ret;
  }

  // with the new state variable filter, we need a parallel vector max
  template <size_t size>
  inline ParallelData<float, size> clamp(const ParallelData<float, size> &in, float _min,
                                         const ParallelData<float, size> &_max)
  {
    return min(max(in, _min), _max);
  }

  template <size_t size> inline ParallelData<int32_t, size> round(const ParallelData<float, size> &in)
  {
    ParallelData<float, size> ret;

    for(size_t i = 0; i < ParallelData<float, size>::simdSize; i++)
      ret.m_data.mmf[i] = vrndnq_f32(in.m_data.mmf[i]);

    return static_cast<ParallelData<int32_t, size>>(ret);
  }

#else

  template <size_t size> inline ParallelData<float, size> min(const ParallelData<float, size> &in, float a)
  {
    ParallelData<float, size> ret;
    float32x4_t cmp = { a, a, a, a };

    for(size_t i = 0; i < ParallelData<float, size>::simdSize; i++)
      ret.m_data.mmf[i] = _mm_min_ps(in.m_data.mmf[i], cmp);

    return ret;
  }

  template <size_t size> inline ParallelData<float, size> max(const ParallelData<float, size> &in, float a)
  {
    ParallelData<float, size> ret;
    float32x4_t cmp = { a, a, a, a };

    for(size_t i = 0; i < ParallelData<float, size>::simdSize; i++)
      ret.m_data.mmf[i] = _mm_max_ps(in.m_data.mmf[i], cmp);

    return ret;
  }

  // with the new state variable filter, we need a parallel vector max
  template <size_t size>
  inline ParallelData<float, size> clamp(const ParallelData<float, size> &in, float _min,
                                         const ParallelData<float, size> &_max)
  {
    ParallelData<float, size> ret = max(in, _min);

    for(size_t i = 0; i < ParallelData<float, size>::simdSize; i++)
      ret.m_data.mmf[i] = _mm_min_ps(ret.m_data.mmf[i], _max.m_data.mmf[i]);

    return ret;
  }

  template <size_t size> inline ParallelData<int32_t, size> round(const ParallelData<float, size> &in)
  {
    ParallelData<int32_t, size> ret;
    for(int i = 0; i < ParallelData<int32_t, size>::simdSize; i++)
      ret.m_data.mmi[i] = _mm_cvtps_epi32(in.m_data.mmf[i]);
    return ret;
  }

#endif

  template <size_t size>
  inline ParallelData<float, size> clamp(const ParallelData<float, size> &in, float _min, float _max)
  {
    return max(min(in, _max), _min);
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
  ParallelData<float, size> one(1.0f);
  auto wrap = (one & (in > 0.0f)) - 0.5f;
  return in - static_cast<ParallelData<float, size>>(static_cast<ParallelData<int32_t, size>>(in + wrap));
}

template <size_t size> inline ParallelData<float, size> sinP3_wrap(ParallelData<float, size> _x)
{
  _x += -0.25f;
  const ParallelData<float, size> wrap = 0.5f
      + static_cast<ParallelData<float, size>>(ParallelData<int32_t, size>(
          (_x < 0.0f)));  // negative: -0.5, positive: 0.5
  _x -= static_cast<ParallelData<float, size>>(static_cast<ParallelData<int32_t, size>>(_x + wrap));
  // similar to sinP3_noWrap now...
  const ParallelData<float, size> x = 0.5f - std::abs(_x + _x), squared = x * x;
  ParallelData<float, size> ret = x * ((2.26548f * squared - 5.13274f) * squared + 3.14159f);
  return ret;
}

template <size_t size> inline ParallelData<float, size> sinP3_noWrap(ParallelData<float, size> _x)
{
  const ParallelData<float, size> x = 0.5f - std::abs(_x + _x), squared = x * x;
  ParallelData<float, size> ret = x * ((2.26548f * squared - 5.13274f) * squared + 3.14159f);
  return ret;
}

template <size_t size>
inline ParallelData<float, size> threeRanges(const ParallelData<float, size> &sample,
                                             const ParallelData<float, size> &ctrlSample, const float &foldAmnt)
{
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

// this function is NOT compliant with SIMD, however it abstracts the voice loop away from the Comb Filter,
// fitting the established parallel paradigm (no voice loops necessary in poly components)
template <typename TScalar, typename TIntegral, size_t size>
inline ParallelData<TScalar, size> polyVectorIndex(const std::vector<ParallelData<TScalar, size>> &_vector,
                                                   const ParallelData<TIntegral, size> &_index)
{
  ParallelData<TScalar, size> ret;

  for(size_t i = 0; i < size; i++)
    ret[i] = _vector[_index[i]][i];

  return ret;
}
