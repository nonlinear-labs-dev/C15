
/******************************************************************************/
/** @file       pe_defines_params.h
    @date       2018-03-13
    @version    1.0
    @author     Matthias Seeber
    @brief      c15 parameter definition (according to parameter list)
                ..
    @todo
*******************************************************************************/

#pragma once

#include "pe_defines_config.h"
#include "pe_defines_labels.h"
#include "dsp_defines_signallabels.h"
#include "pe_utilities.h"

// define: param id, clock type, poly type, range, scale id, scale arg, postID (-1: ignore, > -1: array pos), spread (0, 1), polarity, tcd factor
// convention: define poly params with spread = 0 !!
// postId for direct copy/distribute post processing
// param id of -1 for placeholders (not part of parameter selection, use clock type 0 to prevent any rendering)

/* Parameter Definition */

struct ParameterDefinition
{
  int32_t id;
  Parameters paramId;
  ClockTypes clock;
  PolyTypes poly;
  int32_t range;
  uint32_t scale;
  float scaleArg;
  Signals postId;
  SpreadTypes spread;
  int32_t pol;
  int32_t factor;
};

const ParameterDefinition param_definition[sig_number_of_params] = {

  // - - - ENVELOPE A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 0, Parameters::P_EA_ATT, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 0   ENV_A_ATTACK_TIME
  { 2, Parameters::P_EA_DEC1, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 1   ENV_A_DECAY1_TIME
  { 4, Parameters::P_EA_BP, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 2   ENV_A_BREAKPOINT_LEVEL
  { 6, Parameters::P_EA_DEC2, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 3   ENV_A_DECAY2_TIME
  { 8, Parameters::P_EA_SUS, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 4   ENV_A_SUSTAIN_LEVEL
  { 10, Parameters::P_EA_REL, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16160 },  // 5   ENV_A_RELEASE_TIME
  { 12, Parameters::P_EA_GAIN, ClockTypes::Fast, PolyTypes::Mono, 300, 7, 0, Signals::Invalid, SpreadTypes::Single, 1,
    7200 },  // 6   ENV_A_GAIN
  { 14, Parameters::P_EA_LV, ClockTypes::Sync, PolyTypes::Mono, 256, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    15360 },  // 7   ENV_A_LEVEL_VELOCITY
  { 15, Parameters::P_EA_AV, ClockTypes::Sync, PolyTypes::Mono, 200, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 8   ENV_A_ATTACK_VELOCITY
  { 340, Parameters::P_EA_D1V, ClockTypes::Sync, PolyTypes::Mono, 100, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    6000 },  // 9   ENV_A_DECAY1_VELOCITY
  { 341, Parameters::P_EA_D2V, ClockTypes::Sync, PolyTypes::Mono, 100, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    6000 },  // 10  ENV_A_DECAY2_VELOCITY
  { 16, Parameters::P_EA_RV, ClockTypes::Sync, PolyTypes::Mono, 200, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 11  ENV_A_RELEASE_VELOCITY
  { 17, Parameters::P_EA_LKT, ClockTypes::Sync, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 12  ENV_A_LEVEL_KEYTRACK
  { 18, Parameters::P_EA_TKT, ClockTypes::Sync, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 13  ENV_A_TIME_KEYTRACK
  { 294, Parameters::P_EA_AC, ClockTypes::Sync, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 14  ENV_A_ATTACK_CURVE
  { -1, Parameters::P_INVALID, ClockTypes::Sync, PolyTypes::Mono, 1, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    0 },  // 15  (RETRIGGER HARDNESS PLACEHOLDER)
  { 328, Parameters::P_EA_SPL, ClockTypes::Sync, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 16  ENV_A_SPLIT

  // - - - ENVELOPE B - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 19, Parameters::P_EB_ATT, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 17  ENV_B_ATTACK_TIME
  { 21, Parameters::P_EB_DEC1, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 18  ENV_B_DECAY1_TIME
  { 23, Parameters::P_EB_BP, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 19  ENV_B_BREAKPOINT_LEVEL
  { 25, Parameters::P_EB_DEC2, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 20  ENV_B_DECAY2_TIME
  { 27, Parameters::P_EB_SUS, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 21  ENV_B_SUSTAIN_LEVEL
  { 29, Parameters::P_EB_REL, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16160 },  // 22  ENV_B_RELEASE_TIME
  { 31, Parameters::P_EB_GAIN, ClockTypes::Fast, PolyTypes::Mono, 300, 7, 0, Signals::Invalid, SpreadTypes::Single, 1,
    7200 },  // 23  ENV_B_GAIN
  { 33, Parameters::P_EB_LV, ClockTypes::Sync, PolyTypes::Mono, 256, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    15360 },  // 24  ENV_B_LEVEL_VELOCITY
  { 34, Parameters::P_EB_AV, ClockTypes::Sync, PolyTypes::Mono, 200, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 25  ENV_B_ATTACK_VELOCITY
  { 342, Parameters::P_EB_D1V, ClockTypes::Sync, PolyTypes::Mono, 100, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    6000 },  // 26  ENV_B_DECAY1_VELOCITY
  { 343, Parameters::P_EB_D2V, ClockTypes::Sync, PolyTypes::Mono, 100, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    6000 },  // 27  ENV_B_DECAY2_VELOCITY
  { 35, Parameters::P_EB_RV, ClockTypes::Sync, PolyTypes::Mono, 200, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 28  ENV_B_RELEASE_VELOCITY
  { 36, Parameters::P_EB_LKT, ClockTypes::Sync, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 29  ENV_B_LEVEL_KEYTRACK
  { 37, Parameters::P_EB_TKT, ClockTypes::Sync, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 30  ENV_B_TIME_KEYTRACK
  { 295, Parameters::P_EB_AC, ClockTypes::Sync, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 31  ENV_B_ATTACK_CURVE
  { -1, Parameters::P_INVALID, ClockTypes::Sync, PolyTypes::Mono, 1, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    0 },  // 32  (RETRIGGER HARDNESS PLACEHOLDER)
  { 330, Parameters::P_EB_SPL, ClockTypes::Sync, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 33  ENV_B_SPLIT

  // - - - ENVELOPE C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 38, Parameters::P_EC_ATT, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 34  ENV_C_ATTACK_TIME
  { 40, Parameters::P_EC_DEC1, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 35  ENV_C_DECAY1_TIME
  { 42, Parameters::P_EC_BP, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 36  ENV_C_BREAKPOINT_LEVEL
  { 44, Parameters::P_EC_DEC2, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 37  ENV_C_DECAY2_TIME
  { 297, Parameters::P_EC_SUS, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 38  ENV_C_SUSTAIN_LEVEL
  { 46, Parameters::P_EC_REL, ClockTypes::Slow, PolyTypes::Mono, 16000, 13, -20, Signals::Invalid, SpreadTypes::Single,
    0, 16160 },  // 39  ENV_C_RELEASE_TIME
  { -1, Parameters::P_INVALID, ClockTypes::Sync, PolyTypes::Mono, 1, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    0 },  // 40  (GAIN PLACEHOLDER)
  { 48, Parameters::P_EC_LV, ClockTypes::Sync, PolyTypes::Mono, 256, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    15360 },  // 41  ENV_C_LEVEL_VELOCITY
  { 49, Parameters::P_EC_AV, ClockTypes::Sync, PolyTypes::Mono, 200, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 42  ENV_C_ATTACK_VELOCITY
  { -1, Parameters::P_INVALID, ClockTypes::Sync, PolyTypes::Mono, 1, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    0 },  // 43  (D1V PLACEHOLDER)
  { -1, Parameters::P_INVALID, ClockTypes::Sync, PolyTypes::Mono, 1, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    0 },  // 44  (D2V PLACEHOLDER)
  { 50, Parameters::P_EC_RV, ClockTypes::Sync, PolyTypes::Mono, 200, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 45  ENV_C_RELEASE_VELOCITY
  { 51, Parameters::P_EC_LKT, ClockTypes::Sync, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 46  ENV_C_LEVEL_KEYTRACK
  { 52, Parameters::P_EC_TKT, ClockTypes::Sync, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 47  ENV_C_TIME_KEYTRACK
  { 296, Parameters::P_EC_AC, ClockTypes::Sync, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 48  ENV_C_ATTACK_CURVE
  { 332, Parameters::P_EC_RH, ClockTypes::Sync, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 49  ENV_C_RETRIGGER_HARDNESS
  { -1, Parameters::P_INVALID, ClockTypes::Sync, PolyTypes::Mono, 1, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    0 },  // 50  (SPLIT PLACEHOLDER)

  // - - - OSCILLATOR A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 53, Parameters::P_OA_P, ClockTypes::Slow, PolyTypes::Mono, 100, 0, -20, Signals::Invalid, SpreadTypes::Single, 0,
    15000 },  // 51  OSC_A_PITCH
  { 55, Parameters::P_OA_PKT, ClockTypes::Slow, PolyTypes::Mono, 10000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    10000 },  // 52  OSC_A_PITCH_KEYTRACK
  { 56, Parameters::P_OA_PEC, ClockTypes::Slow, PolyTypes::Mono, 100, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 53  OSC_A_PITCH_ENV_C
  { 57, Parameters::P_OA_F, ClockTypes::Slow, PolyTypes::Mono, 16000, 4, 0.95f, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 54  OSC_A_FLUCTUATION
  { 59, Parameters::P_OA_FEC, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 55  OSC_A_FLUCTUATION_ENV_C
  { 60, Parameters::P_OA_PMS, ClockTypes::Fast, PolyTypes::Mono, 8000, 4, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 8000 },  // 56  OSC_A_PM_SELF
  { 62, Parameters::P_OA_PMSEA, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 57  OSC_A_PM_SELF_ENV_A
  { 63, Parameters::P_OA_PMSSH, ClockTypes::Slow, PolyTypes::Mono, 8000, 0, 0, Signals::OSC_A_PMSSH,
    SpreadTypes::Spread, 1, 8000 },  // 58  OSC_A_PM_SELF_SHAPER
  { 64, Parameters::P_OA_PMB, ClockTypes::Fast, PolyTypes::Mono, 8000, 4, 1, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 59  OSC_A_PM_B
  { 66, Parameters::P_OA_PMBEB, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 60  OSC_A_PM_B_ENV_B
  { 67, Parameters::P_OA_PMBSH, ClockTypes::Slow, PolyTypes::Mono, 8000, 0, 0, Signals::OSC_A_PMBSH,
    SpreadTypes::Spread, 1, 8000 },  // 61  OSC_A_PM_B_SHAPER
  { 68, Parameters::P_OA_PMF, ClockTypes::Fast, PolyTypes::Mono, 8000, 4, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 8000 },  // 62  OSC_A_PM_FB
  { 70, Parameters::P_OA_PMFEC, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 63  OSC_A_PM_FB_ENV_C
  { 301, Parameters::P_OA_PHS, ClockTypes::Audio, PolyTypes::Mono, 14400, 0, 0, Signals::OSC_A_PHS, SpreadTypes::Spread,
    1, 7200 },  // 64  OSC_A_PHASE
  { 303, Parameters::P_OA_CHI, ClockTypes::Slow, PolyTypes::Mono, 200, 9, 80, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 65  OSC_A_CHIRP

  // - - - SHAPER A - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 71, Parameters::P_SA_DRV, ClockTypes::Fast, PolyTypes::Mono, 200, 10, 0.18f, Signals::Invalid, SpreadTypes::Single,
    0, 10000 },  // 66  SHP_A_DRIVE
  { 73, Parameters::P_SA_DEA, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 67  SHP_A_DRIVE_ENV_A (later maybe fast type)
  { 74, Parameters::P_SA_FLD, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::SHP_A_FLD, SpreadTypes::Spread,
    0, 16000 },  // 68  SHP_A_FOLD (later maybe fast type)
  { 75, Parameters::P_SA_ASM, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::SHP_A_ASM, SpreadTypes::Spread,
    0, 16000 },  // 69  SHP_A_ASYMETRY (later maybe fast type)
  { 76, Parameters::P_SA_MIX, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::SHP_A_MIX, SpreadTypes::Spread, 1,
    8000 },  // 70  SHP_A_MIX
  { 78, Parameters::P_SA_FBM, ClockTypes::Fast, PolyTypes::Mono, 16000, 5, 0, Signals::SHP_A_FBM, SpreadTypes::Spread,
    0, 16000 },  // 71  SHP_A_FEEDBACK_MIX
  { 80, Parameters::P_SA_FBEC, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Spread, 0,
    16000 },  // 72  SHP_A_FEEDBACK_ENV_C
  { 81, Parameters::P_SA_RM, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::SHP_A_RM, SpreadTypes::Spread, 0,
    16000 },  // 73  SHP_A_RINGMOD

  // - - - OSCILLATOR B - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 83, Parameters::P_OB_P, ClockTypes::Slow, PolyTypes::Mono, 100, 0, -20, Signals::Invalid, SpreadTypes::Single, 0,
    15000 },  // 74  OSC_B_PITCH
  { 85, Parameters::P_OB_PKT, ClockTypes::Slow, PolyTypes::Mono, 10000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    10000 },  // 75  OSC_B_PITCH_KEYTRACK
  { 86, Parameters::P_OB_PEC, ClockTypes::Slow, PolyTypes::Mono, 100, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 76  OSC_B_PITCH_ENV_C
  { 87, Parameters::P_OB_F, ClockTypes::Slow, PolyTypes::Mono, 16000, 4, 0.95f, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 77  OSC_B_FLUCTUATION
  { 89, Parameters::P_OB_FEC, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 78  OSC_B_FLUCTUATION_ENV_C
  { 90, Parameters::P_OB_PMS, ClockTypes::Fast, PolyTypes::Mono, 8000, 4, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 8000 },  // 79  OSC_B_PM_SELF
  { 92, Parameters::P_OB_PMSEB, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 80  OSC_B_PM_SELF_ENV_B
  { 93, Parameters::P_OB_PMSSH, ClockTypes::Slow, PolyTypes::Mono, 8000, 0, 0, Signals::OSC_B_PMSSH,
    SpreadTypes::Spread, 1, 8000 },  // 81  OSC_B_PM_SELF_SHAPER
  { 94, Parameters::P_OB_PMA, ClockTypes::Fast, PolyTypes::Mono, 8000, 4, 1, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 82  OSC_B_PM_A
  { 96, Parameters::P_OB_PMAEA, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 83  OSC_B_PM_A_ENV_A
  { 97, Parameters::P_OB_PMASH, ClockTypes::Slow, PolyTypes::Mono, 8000, 0, 0, Signals::OSC_B_PMASH,
    SpreadTypes::Spread, 1, 8000 },  // 84  OSC_B_PM_A_SHAPER
  { 98, Parameters::P_OB_PMF, ClockTypes::Fast, PolyTypes::Mono, 8000, 4, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 8000 },  // 85  OSC_B_PM_FB
  { 100, Parameters::P_OB_PMFEC, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 86  OSC_B_PM_FB_ENV_C
  { 302, Parameters::P_OB_PHS, ClockTypes::Audio, PolyTypes::Mono, 14400, 0, 0, Signals::OSC_B_PHS, SpreadTypes::Spread,
    1, 7200 },  // 87  OSC_B_PHASE
  { 304, Parameters::P_OB_CHI, ClockTypes::Slow, PolyTypes::Mono, 200, 9, 80, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 88  OSC_B_CHIRP

  // - - - SHAPER B - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 101, Parameters::P_SB_DRV, ClockTypes::Fast, PolyTypes::Mono, 200, 10, 0.18f, Signals::Invalid, SpreadTypes::Single,
    0, 10000 },  // 89  SHP_B_DRIVE
  { 103, Parameters::P_SB_DEB, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 90  SHP_B_DRIVE_ENV_B (later maybe fast type)
  { 104, Parameters::P_SB_FLD, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::SHP_B_FLD, SpreadTypes::Spread,
    0, 16000 },  // 91  SHP_B_FOLD (later maybe fast type)
  { 105, Parameters::P_SB_ASM, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::SHP_B_ASM, SpreadTypes::Spread,
    0, 16000 },  // 92  SHP_B_ASYMETRY (later maybe fast type)
  { 106, Parameters::P_SB_MIX, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::SHP_B_MIX, SpreadTypes::Spread,
    1, 8000 },  // 93  SHP_B_MIX
  { 108, Parameters::P_SB_FBM, ClockTypes::Fast, PolyTypes::Mono, 16000, 5, 0, Signals::SHP_B_FBM, SpreadTypes::Spread,
    0, 16000 },  // 94  SHP_B_FEEDBACK_MIX
  { 110, Parameters::P_SB_FBEC, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Spread,
    0, 16000 },  // 95  SHP_B_FEEDBACK_ENV_C
  { 111, Parameters::P_SB_RM, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::SHP_B_RM, SpreadTypes::Spread, 0,
    16000 },  // 96  SHP_B_RINGMOD

  // - - - COMB FILTER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 113, Parameters::P_CMB_AB, ClockTypes::Fast, PolyTypes::Mono, 16000, 6, 1, Signals::CMB_AB, SpreadTypes::Spread, 0,
    16000 },  // 97  CMB_AB
  { 115, Parameters::P_CMB_P, ClockTypes::Slow, PolyTypes::Mono, 100, 9, 0, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 98  CMB_PITCH
  { 117, Parameters::P_CMB_PKT, ClockTypes::Slow, PolyTypes::Mono, 10000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 10500 },  // 99  CMB_PITCH_KT
  { 118, Parameters::P_CMB_PEC, ClockTypes::Slow, PolyTypes::Mono, 100, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 100 CMB_PITCH_ENV_C
  { 119, Parameters::P_CMB_D, ClockTypes::Slow, PolyTypes::Mono, 80, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 101 CMB_DECAY
  { 121, Parameters::P_CMB_DKT, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 102 CMB_DECAY_KT
  { 305, Parameters::P_CMB_DG, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 103 CMB_DECAY_GATE
  { 123, Parameters::P_CMB_APT, ClockTypes::Slow, PolyTypes::Mono, 100, 9, 0, Signals::Invalid, SpreadTypes::Single, 0,
    14000 },  // 104 CMB_ALLPASS_TUNE
  { 125, Parameters::P_CMB_APKT, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 105 CMB_ALLPASS_KT
  { 126, Parameters::P_CMB_APEC, ClockTypes::Slow, PolyTypes::Mono, 100, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 106 CMB_ALLPASS_ENV_C
  { 127, Parameters::P_CMB_APR, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::CMB_APR, SpreadTypes::Spread,
    0, 16000 },  // 107 CMB_ALLPASS_RESON
  { 129, Parameters::P_CMB_LP, ClockTypes::Slow, PolyTypes::Mono, 100, 9, 40, Signals::Invalid, SpreadTypes::Single, 0,
    10000 },  // 108 CMB_HICUT
  { 131, Parameters::P_CMB_LPKT, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 109 CMB_HICUT_KT
  { 132, Parameters::P_CMB_LPEC, ClockTypes::Slow, PolyTypes::Mono, 80, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 110 CMB_HICUT_ENV_C
  { 133, Parameters::P_CMB_PM, ClockTypes::Slow, PolyTypes::Mono, 8000, 4, 0.9f, Signals::CMB_PM, SpreadTypes::Spread,
    1, 8000 },  // 111 CMB_PM
  { 135, Parameters::P_CMB_PMAB, ClockTypes::Slow, PolyTypes::Mono, 16000, 2, 1, Signals::CMB_PMAB, SpreadTypes::Spread,
    0, 16000 },  // 112 CMB_PM_AB

  // - - - STATE VARIABLE FILTER  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 136, Parameters::P_SVF_AB, ClockTypes::Fast, PolyTypes::Mono, 16000, 2, 1, Signals::SVF_AB, SpreadTypes::Spread, 0,
    16000 },  // 113 SVF_AB
  { 138, Parameters::P_SVF_CMB, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::SVF_CMIX, SpreadTypes::Spread,
    1, 8000 },  // 114 SVF_COMB_MIX
  { 140, Parameters::P_SVF_CUT, ClockTypes::Slow, PolyTypes::Mono, 100, 9, 20, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 115 SVF_CUTOFF
  { 142, Parameters::P_SVF_CKT, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 116 SVF_CUTOFF_KT
  { 143, Parameters::P_SVF_CEC, ClockTypes::Slow, PolyTypes::Mono, 80, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 117 SVF_CUTOFF_ENV_C
  { 148, Parameters::P_SVF_SPR, ClockTypes::Slow, PolyTypes::Mono, 100, 1, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 6000 },  // 118 SVF_SPREAD
  { 153, Parameters::P_SVF_FM, ClockTypes::Slow, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 119 SVF_FM
  { 144, Parameters::P_SVF_RES, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 120 SVF_RESONANCE
  { 146, Parameters::P_SVF_RKT, ClockTypes::Slow, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 121 SVF_RESONANCE_KT
  { 147, Parameters::P_SVF_REC, ClockTypes::Slow, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 122 SVF_RESONANCE_ENV_C
  { 150, Parameters::P_SVF_LBH, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 123 SVF_LBH
  { 152, Parameters::P_SVF_PAR, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 124 SVF_PARALLEL
  { 155, Parameters::P_SVF_FMAB, ClockTypes::Slow, PolyTypes::Mono, 16000, 2, 1, Signals::SVF_FMAB, SpreadTypes::Single,
    0, 16000 },  // 125 SVF_FM_AB

  // - - - FEEDBACK MIXER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 156, Parameters::P_FBM_CMB, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::FBM_CMB, SpreadTypes::Spread, 1,
    8000 },  // 126 FBM_COMB_MIX
  { 158, Parameters::P_FBM_SVF, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::FBM_SVF, SpreadTypes::Spread, 1,
    8000 },  // 127 FBM_SVF_MIX
  { 160, Parameters::P_FBM_FX, ClockTypes::Fast, PolyTypes::Mono, 8000, 3, 0, Signals::FBM_FX, SpreadTypes::Spread, 1,
    8000 },  // 128 FBM_EFFECTS_MIX
  { 162, Parameters::P_FBM_REV, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::FBM_REV, SpreadTypes::Single,
    0, 16000 },  // 129 FBM_REVERB_MIX
  { 164, Parameters::P_FBM_DRV, ClockTypes::Fast, PolyTypes::Mono, 200, 11, 2.5f, Signals::FBM_DRV, SpreadTypes::Spread,
    0, 10000 },  // 130 FBM_DRIVE
  { 166, Parameters::P_FBM_FLD, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::FBM_FLD, SpreadTypes::Spread,
    0, 16000 },  // 131 FBM_FOLD
  { 167, Parameters::P_FBM_ASM, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::FBM_ASM, SpreadTypes::Spread,
    0, 16000 },  // 132 FBM_ASYM
  { 168, Parameters::P_FBM_LKT, ClockTypes::Slow, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Spread, 1,
    8000 },  // 133 FBM_LEVEL_KT
  { 299, Parameters::P_FBM_LVL, ClockTypes::Fast, PolyTypes::Mono, 16000, 4, 4, Signals::Invalid, SpreadTypes::Spread,
    0, 16000 },  // 134 FBM_LEVEL

  // - - - OUTPUT MIXER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 169, Parameters::P_OM_AL, ClockTypes::Fast, PolyTypes::Mono, 4000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 135 OUT_A_LEVEL
  { 171, Parameters::P_OM_AP, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 8000 },  // 136 OUT_A_PAN
  { 172, Parameters::P_OM_BL, ClockTypes::Fast, PolyTypes::Mono, 4000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 137 OUT_B_LEVEL
  { 174, Parameters::P_OM_BP, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 8000 },  // 138 OUT_B_PAN
  { 175, Parameters::P_OM_CL, ClockTypes::Fast, PolyTypes::Mono, 4000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 139 OUT_COMB_LEVEL
  { 177, Parameters::P_OM_CP, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 8000 },  // 140 OUT_COMB_PAN
  { 178, Parameters::P_OM_SL, ClockTypes::Fast, PolyTypes::Mono, 4000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 141 OUT_SVF_LEVEL
  { 180, Parameters::P_OM_SP, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 8000 },  // 142 OUT_SVF_PAN
  { 181, Parameters::P_OM_DRV, ClockTypes::Fast, PolyTypes::Mono, 200, 11, 0.25f, Signals::OUT_DRV, SpreadTypes::Spread,
    0, 10000 },  // 143 OUT_DRIVE
  { 183, Parameters::P_OM_FLD, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::OUT_FLD, SpreadTypes::Spread, 0,
    16000 },  // 144 OUT_FOLD (later maybe fast type)
  { 184, Parameters::P_OM_ASM, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::OUT_ASM, SpreadTypes::Spread, 0,
    16000 },  // 145 OUT_ASYMETRY (later maybe fast type)
  { 185, Parameters::P_OM_LVL, ClockTypes::Fast, PolyTypes::Mono, 16000, 4, 2.56f, Signals::OUT_LVL,
    SpreadTypes::Single, 0, 16000 },  // 146 OUT_LEVEL
  { 187, Parameters::P_OM_KP, ClockTypes::Fast, PolyTypes::Mono, 960000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 147 OUT_KEY_PAN

  // - - - CABINET  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 188, Parameters::P_CAB_DRV, ClockTypes::Fast, PolyTypes::Mono, 200, 12, 0, Signals::CAB_DRV, SpreadTypes::Single, 0,
    10000 },  // 148 CABINET_DRIVE
  { 190, Parameters::P_CAB_FLD, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::CAB_FLD, SpreadTypes::Single,
    0, 16000 },  // 149 CABINET_FOLD (later maybe fast type)
  { 191, Parameters::P_CAB_ASM, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::CAB_ASM, SpreadTypes::Single,
    0, 16000 },  // 150 CABINET_ASYMETRY (later maybe fast type)
  { 192, Parameters::P_CAB_TILT, ClockTypes::Fast, PolyTypes::Mono, 80, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 151 CABINET_TILT (implement fast and slow handling)
  { 194, Parameters::P_CAB_LPF, ClockTypes::Slow, PolyTypes::Mono, 200, 9, 60, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 152 CABINET_HI_CUT
  { 196, Parameters::P_CAB_HPF, ClockTypes::Slow, PolyTypes::Mono, 200, 9, 20, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 153 CABINET_LOW_CUT
  { 197, Parameters::P_CAB_LVL, ClockTypes::Fast, PolyTypes::Mono, 200, 12, -50, Signals::Invalid, SpreadTypes::Single,
    0, 10000 },  // 154 CABINET_CAB_LEVEL
  { 199, Parameters::P_CAB_MIX, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 155 CABINET_MIX

  // - - - GAP FILTER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 201, Parameters::P_GAP_CNT, ClockTypes::Slow, PolyTypes::Mono, 100, 0, 24, Signals::Invalid, SpreadTypes::Single, 0,
    9600 },  // 156 GAP_FILTER_CENTER
  { 203, Parameters::P_GAP_STE, ClockTypes::Slow, PolyTypes::Mono, 200, 1, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 7200 },  // 157 GAP_FILTER_STEREO
  { 204, Parameters::P_GAP_GAP, ClockTypes::Slow, PolyTypes::Mono, 100, 1, 0.5f, Signals::Invalid, SpreadTypes::Single,
    0, 9600 },  // 158 GAP_FILTER_GAP
  { 206, Parameters::P_GAP_RES, ClockTypes::Slow, PolyTypes::Mono, 16000, 1, 0.9f, Signals::GAP_RES,
    SpreadTypes::Single, 0, 16000 },  // 159 GAP_FILTER_RESONANCE
  { 207, Parameters::P_GAP_BAL, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 160 GAP_FILTER_BALANCE
  { 209, Parameters::P_GAP_MIX, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 161 GAP_FILTER_MIX

  // - - - FLANGER  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 211, Parameters::P_FLA_TMOD, ClockTypes::Fast, PolyTypes::Mono, 8000, 4, 1, Signals::FLA_TMOD, SpreadTypes::Single,
    1, 8000 },  // 162 FLANGER_TIME_MOD
#if test_flanger_phs == 0
  { 213, Parameters::P_FLA_PHS, ClockTypes::Slow, PolyTypes::Mono, 80, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 0, 14400 },  // 163 FLANGER_PHASE
#elif test_flanger_phs == 1
  { 213, Parameters::P_FLA_PHS, ClockTypes::Fast, PolyTypes::Mono, 80, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    14400 },  // 163 FLANGER_PHASE
#elif test_flanger_phs == 2
  { 213, Parameters::P_FLA_PHS, ClockTypes::Audio, PolyTypes::Mono, 80, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 0, 14400 },  // 163 FLANGER_PHASE
#endif
  { 214, Parameters::P_FLA_RTE, ClockTypes::Audio, PolyTypes::Mono, 16000, 4, 10, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 164 FLANGER_RATE (audio)
  { 216, Parameters::P_FLA_TIME, ClockTypes::Slow, PolyTypes::Mono, 12500, 4, 50, Signals::Invalid, SpreadTypes::Single,
    0, 12500 },  // 165 FLANGER_TIME
  { 218, Parameters::P_FLA_STE, ClockTypes::Slow, PolyTypes::Mono, 160, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 166 FLANGER_STEREO
  { 219, Parameters::P_FLA_FB, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0.5f, Signals::Invalid, SpreadTypes::Single,
    1, 8000 },  // 167 FLANGER_FEEDBACK
  { 221, Parameters::P_FLA_CFB, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 168 FLANGER_CROSS_FEEDBACK
  { 222, Parameters::P_FLA_LPF, ClockTypes::Slow, PolyTypes::Mono, 200, 9, 60, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 169 FLANGER_HI_CUT
  { 223, Parameters::P_FLA_MIX, ClockTypes::Fast, PolyTypes::Mono, 8000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    8000 },  // 170 FLANGER_MIX
#if test_flanger_env == 0
  { 307, Parameters::P_FLA_ENV, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 0, 16000 },  // 171 FLANGER_ENVELOPE
#elif test_flanger_env == 1
  { 307, Parameters::P_FLA_ENV, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 0, 16000 },  // 171 FLANGER_ENVELOPE
#elif test_flanger_env == 2
  { 307, Parameters::P_FLA_ENV, ClockTypes::Audio, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 171 FLANGER_ENVELOPE
#endif
  { 308, Parameters::P_FLA_APM, ClockTypes::Slow, PolyTypes::Mono, 8000, 1, 70, Signals::Invalid, SpreadTypes::Single,
    1, 8000 },  // 172 FLANGER_ALLPASS_MOD
  { 310, Parameters::P_FLA_APT, ClockTypes::Slow, PolyTypes::Mono, 100, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    14000 },  // 173 FLANGER_ALLPASS_TUNE

  // - - - ECHO - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 225, Parameters::P_DLY_TIME, ClockTypes::Slow, PolyTypes::Mono, 16000, 4, 2, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 174 ECHO_DELAY_TIME
  { 227, Parameters::P_DLY_STE, ClockTypes::Slow, PolyTypes::Mono, 200, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    6600 },  // 175 ECHO_STEREO
  { 229, Parameters::P_DLY_FB, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 176 ECHO_FEEDBACK
  { 231, Parameters::P_DLY_CFB, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 177 ECHO_CROSS_FEEDBACK
  { 232, Parameters::P_DLY_LPF, ClockTypes::Slow, PolyTypes::Mono, 200, 9, 60, Signals::Invalid, SpreadTypes::Single, 0,
    16000 },  // 178 ECHO_HI_CUT
  { 233, Parameters::P_DLY_MIX, ClockTypes::Fast, PolyTypes::Mono, 16000, 4, 1, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 179 ECHO_MIX
  { 334, Parameters::P_DLY_SND, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::DLY_SND, SpreadTypes::Single,
    0, 16000 },  // 180 ECHO_SEND

// - - - REVERB - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
#if test_reverbParams == 0
  /* should the parameters run with fast clock? (except chorus) - see pe_defines_config.h */
  { 235, Parameters::P_REV_SIZE, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 0, 16000 },  // 181 REVERB_SIZE
  { 237, Parameters::P_REV_PRE, ClockTypes::Fast, PolyTypes::Mono, 16000, 5, 0,
    Signals::Invalid, SpreadTypes::Single, 0, 16000 },  // 182 REVERB_PRE_DELAY
  { 238, Parameters::P_REV_COL, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 0, 16000 },  // 183 REVERB_COLOR
  { 240, Parameters::P_REV_CHO, ClockTypes::Slow, PolyTypes::Mono, 16000, 4, 1,
    Signals::REV_CHO, SpreadTypes::Single, 0, 16000 },  // 184 REVERB_CHORUS
#elif test_reverbParams == 1
  /* should the parameters run with slow clock? - see pe_defines_config.h */
  { 235, Parameters::P_REV_SIZE, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 181 REVERB_SIZE
  { 237, Parameters::P_REV_PRE, ClockTypes::Slow, PolyTypes::Mono, 16000, 5, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 182 REVERB_PRE_DELAY
  { 238, Parameters::P_REV_COL, ClockTypes::Slow, PolyTypes::Mono, 16000, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 183 REVERB_COLOR
  { 240, Parameters::P_REV_CHO, ClockTypes::Slow, PolyTypes::Mono, 16000, 4, 1, Signals::REV_CHO, SpreadTypes::Single,
    0, 16000 },  // 184 REVERB_CHORUS
#endif
  { 241, Parameters::P_REV_MIX, ClockTypes::Fast, PolyTypes::Mono, 16000, 4, 1, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 185 REVERB_MIX
  { 336, Parameters::P_REV_SND, ClockTypes::Fast, PolyTypes::Mono, 16000, 0, 0, Signals::REV_SND, SpreadTypes::Single,
    0, 16000 },  // 186 REVERB_SEND

  // - - - MASTER - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 247, Parameters::P_MA_V, ClockTypes::Fast, PolyTypes::Mono, 16000, 4, 4, Signals::MST_VOL, SpreadTypes::Single, 0,
    16000 },  // 187 MASTER_VOLUME
  { 248, Parameters::P_MA_T, ClockTypes::Slow, PolyTypes::Mono, 100, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    4800 },  // 188 MASTER_TUNE
  { 338, Parameters::P_MA_SH, ClockTypes::Sync, PolyTypes::Mono, 1, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    48 },  // 189 NOTE_SHIFT

  // - - - UNISON - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
  { 249, Parameters::P_UN_V, ClockTypes::Sync, PolyTypes::Mono, 1, 0, -1, Signals::Invalid, SpreadTypes::Single, 0,
    11 },  // 190 UNISON_VOICES
  { 250, Parameters::P_UN_DET, ClockTypes::Slow, PolyTypes::Mono, 1000, 0, 0, Signals::Invalid, SpreadTypes::Single, 0,
    12000 },  // 191 UNISON_DETUNE
  { 252, Parameters::P_UN_PHS, ClockTypes::Audio, PolyTypes::Mono, 14400, 0, 0, Signals::Invalid, SpreadTypes::Single,
    0, 14400 },  // 192 UNISON_PHASE
  { 253, Parameters::P_UN_PAN, ClockTypes::Fast, PolyTypes::Mono, 16000, 1, 0.5f, Signals::Invalid, SpreadTypes::Single,
    0, 16000 },  // 193 UNISON_PAN

// - - - POLY KEY EVENT - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//       ID      CLOCK   POLY    RANGE   SCALE   (ARG)   POST    SPREAD  POL     FACTOR
#if test_milestone == 150
  { 400, Parameters::P_KEY_PH, ClockTypes::Sync, PolyTypes::Poly, 14400, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 1, 7200 },  // 194 KEY_UNISON_PHASE
  { 406, Parameters::P_KEY_NP, ClockTypes::Sync, PolyTypes::Poly, 1000, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 1, 1000 },  // 195 KEY_NOTE_PITCH
  { 407, Parameters::P_KEY_VP, ClockTypes::Sync, PolyTypes::Poly, 8000, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 1, 8000 },  // 196 KEY_VOICE_PAN
  { 409, Parameters::P_KEY_VS, ClockTypes::Sync, PolyTypes::Poly, 1, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 0, 1 }  // 197 KEY_VOICE_STEAL
#elif test_milestone == 155
  { 416, Parameters::P_KEY_BP, ClockTypes::Sync, PolyTypes::Poly, 1000, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 1, 1000 },  // 194 KEY_BASE_PITCH
  { 417, Parameters::P_KEY_IDX, ClockTypes::Sync, PolyTypes::Poly, 1, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 0, 1 },  // 195 KEY_UNISON_IDX
  { 418, Parameters::P_KEY_STL, ClockTypes::Sync, PolyTypes::Poly, 1, 0, 0,
    Signals::Invalid, SpreadTypes::Single, 0, 1 }  // 196 KEY_VOICE_STEAL
#elif test_milestone == 156
  { 416, Parameters::P_KEY_BP, ClockTypes::Sync, PolyTypes::Poly, 1000, 0, 0, Signals::Invalid, SpreadTypes::Single, 1,
    1000 }  // 194 KEY_BASE_PITCH
#endif

};

/* Utility Parameters */

struct UtilityDefinition
{
  uint32_t range;
  uint32_t scale;
  float scaleArg;
};

const UtilityDefinition utility_definition[sig_number_of_utilities] = {

  //       RANGE   SCALE   (ARG)
  { 4096, 0, 0 },   // velocity definition (for now: tcd range 4096)
  { 100, 0, 400 },  // pitch reference (A3)
  { 1, 0, 0 },      // Test Tone Frequency (400 ... 1000 Hz)
  { 1, 0, 0 },      // Test Tone Ampltude (-60 ... 0 dB)
  { 1, 0, 0 }       // Test Tone State (0, 1)
};
