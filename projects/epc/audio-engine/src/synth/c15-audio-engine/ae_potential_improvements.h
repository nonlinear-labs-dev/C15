#pragma once

/******************************************************************************/
/** @file       ae_potential_improvements.h
    @date       2020-11-13
    @version    1.7B-7
    @author     M. Seeber

    @brief      contains define flags enabling potential dsp improvements

                - primarily attempting to lower permanent cpu usage
                - if possible, maybe reduce some event-related cpu usage as well

                (this file is intended to only be temporary and should be deleted after everything is implemented, tested, decided and cleaned up)

*******************************************************************************/

// two basic states (disabled, enabled) are provided (disabled refers to the current implementation)

#define __POTENTIAL_IMPROVEMENT_DISABLED__ 0
#define __POTENTIAL_IMPROVEMENT_ENABLED__ 1

/******************************************************************************/
/** @details
 *
 * - permanent improvements (April 2020)
 *   - comb improvements: reduced voice loops by using paralleldata
 *   - dnc improvements: got rid of denormal floats, avoiding lots of additions (Nltoolbox::Contants::DNC_const)
 *   - paralleldata improvements (made the following SIMD-compliant):
 *     - std::abs, std::min, std::max, std::clamp, std::round
 *     - keepFractional, sinP3_wrap, sinP3_nowrap, threeRanges
 *
 * - permanent improvements (October/November 2020)
 *   - output mixer: filter simplification of static mono HP
 *   - part muting: reduced to only affect final part volume (cross feedback and fx send not affected anymore)
 *
*******************************************************************************/

/******************************************************************************/
/** @todo
 *
 * - remaining potential improvements (November 2020 - still in development)
 *   - fast pitch processing: process pitches faster (resolving env c and glide effects by fast(er) clock)
 *     --> new 2.4 kHz clock and parameter-db update required
 *   - gain curve smoothing: use a pre-calculated fade out/in buffer to
 *     - avoid sudden "gain"-related signal changes (producing audible clicks - part: voicelevel (fade from/range), mixers: poly keypan, keytrk)
 *     - avoid sudden "phase"-related signal changes (comb filter delay buffer)
 *     --> poly "steal" detection (env-signals), poly fade mechanism and delayed keydown application required
 *
*******************************************************************************/

// PROCESSING IMPROVEMENTS

#define POTENTIAL_IMPROVEMENT_FAST_PITCH_PROCESSING __POTENTIAL_IMPROVEMENT_DISABLED__
// - disabled: slow clock / enabled: fast clock for pitches

// GAIN-RELATED SMOOTHING (avoiding clicks)
#define POTENTIAL_IMPROVEMENT_GAIN_CURVE __POTENTIAL_IMPROVEMENT_DISABLED__
constexpr float POTENTIAL_SETTING_GAIN_CURVE_TIME_MS = 1.0f;  // later: migrate to parameter-db
// - introduces a lookup-based s-curve smoothing of gain-related signals (currently at 1ms, smoothing only voice_level for now)
