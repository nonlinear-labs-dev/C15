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

struct exponentiator
{
    /* table position variables */
    uint32_t m_position_step;
    float m_position_fine;
    /* constants (table offsets, bases, exponent scalings, hyperbolic floor parameters) */
    const float m_freqExponent_offset = 69.f;                           // pitch reference offset (where resulting frequency factor equals 1)
    const float m_freqBase = 2.f;                                       // base for pitch to frequency conversion
    const float m_scaleFreqExponent = 1.f / 12.f;                       // exponent normalization for pitch conversion
    const float m_gainBase = 10.f;                                      // base for level to amplitude and time conversion
    const float m_scaleGainExponent = 1.f / 20.f;                       // exponent normalization for level and time conversion
    const float m_hyperfloor[2] = {(300.f / 13.f), (280.f / 13.f)};     // two parameters needed to generate the hyperbolic floor function of oscillator pitches
    /* constant input value ranges for (clipped) table access */        // (range values provided by pe_defines_config.h)
    const float m_linear_pitch_from = dsp_expon_lin_pitch_from;
    const float m_linear_pitch_to = dsp_expon_lin_pitch_range + dsp_expon_lin_pitch_from;
    const float m_oscillator_pitch_from = dsp_expon_osc_pitch_from;
    const float m_oscillator_pitch_to = dsp_expon_osc_pitch_range + dsp_expon_osc_pitch_from;
    const float m_level_from = dsp_expon_level_from;
    const float m_level_to = dsp_expon_level_range + dsp_expon_level_from;
    const float m_time_from = dsp_expon_time_from;
    const float m_time_to = dsp_expon_time_range + dsp_expon_time_from;
    /* conversion tables (constructed on initialization) */             // (note: the additional table element (size + 1) prevents interpolation issues)
    float m_linear_pitch_table[dsp_expon_lin_pitch_range + 1];          // linear pitch table: [-150, 150] semitones
    float m_oscillator_pitch_table[dsp_expon_osc_pitch_range + 1];      // nonlinear pitch table for oscillators: [-20, 130] semitones
    float m_level_table[dsp_expon_level_range + 1];                     // level conversion table: [-300, 100] decibel (first element is zero)
    float m_time_table[dsp_expon_time_range + 1];                       // time conversion table: [-20, 90] decibel (first element is zero)
    /* proper init */
    void init();                                                        // perform construction of all four conversion tables
    /* hyperbolic osc pitch function (nonlinear pitch floor) */
    float hyperfloor(float _value);
    /* table position clipping */
    float clip(float _min, float _max, float _value);                   // return clipped value according to min and max parameters
    /* update (clipped) table position */
    void setTablePos(float _value);                                     // transform a valid (clipped) table position into integer and fractional parts
    /* table interpolation */
    float getInterpolated(float _fade, float _first, float _second);    // perform linear interpolation of two values (cross-fade value provided)
    /* run-time conversion methods (using table interpolation) */
    float eval_lin_pitch(float _value);                                 // linear pitch conversion (into frequency factor - multiples of 440 Hz - or different reference)
    float eval_osc_pitch(float _value);                                 // oscillator pitch conversion (into frequency factor)
    float eval_level(float _value);                                     // gain/level conversion (into amplitude factor)
    float eval_time(float _value);                                      // time conversion (into milliseconds)
};
