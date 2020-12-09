/******************************************************************************/
/** @file       pe_defines_config.h
    @date       2020-11-17
    @version    1.7B-7
    @author     Matthias Seeber
    @brief      crucial definitions of the param engine architecture
                removed all pre 1.7 legacy code
*******************************************************************************/

#pragma once

/* DSP Helper Values */

#define dsp_expon_osc_pitch_from -20  // lowest logarithmic Pitch value for Oscillator unit
#define dsp_expon_osc_pitch_range                                                                                      \
  150  // range of logarithmic Pitch value for Oscillator unit ([-20 ... 130] ST = 150 ST)
#define dsp_expon_lin_pitch_from -150  // lowest logarithmic Pitch value for linear conversion
#define dsp_expon_lin_pitch_range                                                                                      \
  300  // range of logarithmic Pitch value for linear conversion ([-150 ... 150] ST = 300 ST)
#define dsp_expon_level_from -300  // lowest logarithmic Level value for gain conversion (-300 dB)
#define dsp_expon_level_range 400  // range of logarithmic Level value for gain conversion ([-300 ... 100] dB = 400 dB)
#define dsp_expon_time_from -20    // lowest logarithmic Time value for time conversion (-20 dB)
#define dsp_expon_time_range 110   // range of logarithmic Time value for time conversion ([-20 ... 90] dB = 110 dB)
#define dsp_expon_time_factor 104.0781f  // measured value to produce exactly time of 16000 (equals highest time)
#define dsp_comb_max_freqFactor                                                                                        \
  19.0166f  // measured value of highest frequency factor for the Comb Filter to run without bypass (corresponding to Pitch of 119.99 ST)

#define env_clip_peak 1.412537545f  // measured value for LevelKT Clipping, equals +3 dB (candidate)
#define env_init_gateRelease                                                                                           \
  10  // release time of gate envelopes (in milliseconds) -- 1ms problematic (key up noise), 10ms okay (like prototype) \
      // (due to current comb decay gate implementation)
#define env_highest_finite_time 16000.f  // highest allowed finite time
