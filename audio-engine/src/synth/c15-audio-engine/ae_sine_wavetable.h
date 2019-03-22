#pragma once

#include "io/Log.h"
#include <stdlib.h>
#include <type_traits>
#include <tuple>
#include <math.h>
#include <glib.h>

namespace detail
{
  template <size_t mask> struct CountBits;

  template <> struct CountBits<0>
  {
    static constexpr size_t value = 0;
  };

  template <size_t mask> struct CountBits
  {
    static constexpr size_t value = CountBits<(mask >> 1)>::value + (mask & 1 ? 1 : 0);
  };
}

class ae_sine_wavetable
{
 public:
  template <typename Derived> void doTests(const Derived &a)
  {
    auto offsets = { 0, 1, 2, 3, 5, 7, 9, 17, 1000, 10000 };

    for(float offset : offsets)
    {
      assertFloat(a.wrapped(offset), 0.0f);
      assertFloat(a.wrapped(-offset), 0.0f);
      assertFloat(a.wrapped(offset + 0.5f), 0.0f);
      assertFloat(a.wrapped(offset - 0.5f), 0.0f);
      assertFloat(a.wrapped(-offset + 0.5f), 0.0f);
      assertFloat(a.wrapped(-offset - 0.5f), 0.0f);
      assertFloat(a.wrapped(offset + 0.25f), 1.0f);
      assertFloat(a.wrapped(offset - 0.25f), -1.0f);
      assertFloat(a.wrapped(-offset + 0.25f), 1.0f);
      assertFloat(a.wrapped(-offset - 0.25f), -1.0f);
    }

    assertFloat(a.unwrapped(-1), 0.0f);
    assertFloat(a.unwrapped(-0.75), 1.0f);
    assertFloat(a.unwrapped(-0.5f), 0.0f);
    assertFloat(a.unwrapped(-0.25f), -1.0f);
    assertFloat(a.unwrapped(0.0f), 0.0f);
    assertFloat(a.unwrapped(0.25f), 1.0f);
    assertFloat(a.unwrapped(0.5f), 0.0f);
    assertFloat(a.unwrapped(0.75f), -1.0f);
    assertFloat(a.unwrapped(1), 0.0f);

    float maxErr = 0;
    for(int i = -48000; i < 48000; i++)
    {
      auto phase = i / 48000.0f;
      auto sineIn = 2 * static_cast<float>(M_PI) * phase;
      auto sine = std::sin(sineIn);
      auto approx = a.wrapped(phase);
      auto err = std::abs(sine - approx);
      maxErr = std::max(maxErr, err);
    }

    Log::info(__PRETTY_FUNCTION__, "Max Error:", maxErr);
  }

  void assertFloat(float value, float expected)
  {
    auto diff = std::abs(value - expected);
    g_assert(diff < std::numeric_limits<float>::epsilon());
  }
};

template <size_t size> class ae_sine_wavetable_no_interpolation : ae_sine_wavetable
{
 public:
  ae_sine_wavetable_no_interpolation()
  {
    static_assert(detail::CountBits<size>::value == 1, "This implementation works for sizes of power of two only!");

    for(size_t i = 0; i < size; i++)
      m_samples[i] = std::sin(2 * M_PI * i / size);

    doTests(*this);
  }

  inline float wrapped(double phase) const
  {
    const size_t unwrappedIdx = phase * size + (size * 10000);
    const auto idx = unwrappedIdx & (size - 1);
    return m_samples[idx];
  }

  inline float unwrapped(float phase) const
  {
    if(phase < 0)
      phase++;
    return m_samples[phase * size];
  }

 private:
  std::array<float, size> m_samples;
};

struct SineWaveTables
{
  static ae_sine_wavetable_no_interpolation<32768> sine32768;
  static ae_sine_wavetable_no_interpolation<16384> sine16384;
  static ae_sine_wavetable_no_interpolation<8192> sine8192;
  static ae_sine_wavetable_no_interpolation<4096> sine4096;
};
