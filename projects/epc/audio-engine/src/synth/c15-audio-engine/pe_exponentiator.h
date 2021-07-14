/******************************************************************************/
/** @file       pe_exponentiator.h
    @date       2018-03-16
    @version    1.0
    @author     Matthias Seeber
    @brief      exponential converter based on initialized tables
                (used for TCD scaling mechanism)
    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "pe_defines_config.h"
#include "nltoolbox.h"
#include <math.h>

struct exponentiator
{
  /* constants (table offsets, bases, exponent scalings, hyperbolic floor parameters) */
  constexpr static float m_freqExponent_offset
      = 69.0f;                               // pitch reference offset (where resulting frequency factor equals 1)
  constexpr static float m_freqBase = 2.0f;  // base for pitch to frequency conversion
  constexpr static float m_scaleFreqExponent = 1.0f / 12.0f;  // exponent normalization for pitch conversion
  constexpr static float m_gainBase = 10.0f;                  // base for level to amplitude and time conversion
  constexpr static float m_scaleGainExponent = 1.0f / 20.0f;  // exponent normalization for level and time conversion
  constexpr static float m_hyperfloor[2]
      = { (300.0f / 13.0f),
          (280.0f / 13.0f) };  // two parameters needed to generate the hyperbolic floor function of oscillator pitches
  /* constant input value ranges for (clipped) table access */  // (range values provided by pe_defines_config.h)
  constexpr static float m_linear_pitch_from = dsp_expon_lin_pitch_from;
  constexpr static float m_linear_pitch_to = dsp_expon_lin_pitch_range + dsp_expon_lin_pitch_from;
  constexpr static float m_oscillator_pitch_from = dsp_expon_osc_pitch_from;
  constexpr static float m_oscillator_pitch_to = dsp_expon_osc_pitch_range + dsp_expon_osc_pitch_from;
  constexpr static float m_level_from = dsp_expon_level_from;
  constexpr static float m_level_to = dsp_expon_level_range + dsp_expon_level_from;
  constexpr static float m_time_from = dsp_expon_time_from;
  constexpr static float m_time_to = dsp_expon_time_range + dsp_expon_time_from;
  /* conversion tables (constructed on initialization) */  // (note: the additional table element (size + 1) prevents interpolation issues)
  float m_linear_pitch_table[dsp_expon_lin_pitch_range + 1];  // linear pitch table: [-150, 150] semitones
  float m_oscillator_pitch_table[dsp_expon_osc_pitch_range
                                 + 1];  // nonlinear pitch table for oscillators: [-20, 130] semitones
  float
      m_level_table[dsp_expon_level_range + 1];  // level conversion table: [-300, 100] decibel (first element is zero)
  float m_time_table[dsp_expon_time_range + 1];  // time conversion table: [-20, 90] decibel (first element is zero)
  /* proper init */
  void init();  // perform construction of all four conversion tables
  /* hyperbolic osc pitch function (nonlinear pitch floor) */
  float hyperfloor(float _value);

  /* table interpolation */
  /* run-time conversion methods (using table interpolation) */
  float eval_lin_pitch(
      float _value);  // linear pitch conversion (into frequency factor - multiples of 440 Hz - or different reference)
  float eval_osc_pitch(float _value);  // oscillator pitch conversion (into frequency factor)
  float eval_level(float _value);      // gain/level conversion (into amplitude factor)
  float eval_time(float _value);       // time conversion (into milliseconds)
};

inline float eval(float _value, float from, float to, float* table)
{
  const auto v = std::clamp(_value, from, to) - from;
  const auto vFloor = floor(v);
  const auto position_step = static_cast<uint32_t>(vFloor);
  const auto position_fine = v - vFloor;
  return (((1 - position_fine) * table[position_step]) + (position_fine * table[position_step + 1]));
}

/* main, run-time conversion methods */
inline float exponentiator::eval_lin_pitch(float _value)
{
  return eval(_value, m_linear_pitch_from, m_linear_pitch_to, m_linear_pitch_table);
}

inline float exponentiator::eval_osc_pitch(float _value)
{
  return eval(_value, m_oscillator_pitch_from, m_oscillator_pitch_to, m_oscillator_pitch_table);
}

inline float exponentiator::eval_level(float _value)
{
  return eval(_value, m_level_from, m_level_to, m_level_table);
}

inline float exponentiator::eval_time(float _value)
{
  return eval(_value, m_time_from, m_time_to, m_time_table);
}
