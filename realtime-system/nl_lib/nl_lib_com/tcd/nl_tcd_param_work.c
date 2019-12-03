/*
 * nl_tcd_param_work.c
 *
 *  Created on: 30.01.2015
 *      Author: ssc
 */

#include "tcd/nl_tcd_param_work.h"
#include "tcd/nl_tcd_poly.h"
#include "tcd/nl_tcd_msg.h"
#include "tcd/nl_tcd_adc_work.h"
#include "tcd/nl_tcd_expon.h"

//------------------------ Storage for the values and limits of all parameters (including play and macro controls and PC amounts)

static int32_t paramValue[NUM_UI_PARAMS];      // basic params, play controls, pc amounts, macro controls, mc source_and_amounts (raw),
static int32_t modulatedValue[NUM_UI_PARAMS];  // modulation results, up-scaled by maxMCAmount[paramId] (40000 for MCs) and not clipped

static int16_t maxMCAmount[NUM_UI_PARAMS];  // maximum of the MC Amount (8000 if the Amt is in %, smaller if the Amt is in dB or semitones)

static int16_t minValue[NUM_UI_PARAMS];  // lower limit of the parameter
static int16_t maxValue[NUM_UI_PARAMS];  // upper limit of the parameter

//------------------------ Storage for the MC amounts and linked list

static int16_t mcAmount[NUM_UI_PARAMS];  // decoded mc amounts

static uint16_t assignedMCTargets[NUM_MCS];         // length of the list of assigned targets
static uint16_t mcTarget[NUM_MCS][NUM_MC_TARGETS];  // Ids of the target parameters of an MC

static uint32_t transitionTime;
static uint32_t updateTransitionTime;
static uint32_t smoothingTime;
static uint32_t updateSmoothingTime;
static uint32_t usingTransitionTime;
static uint32_t usingGlitchSuppression;

//============================== Init

void PARAM_WORK_Init(void)
{
  uint32_t mcId;
  uint32_t i;

  //-------- Initializing assignedMCTargets[] and mcTarget[][])

  for (mcId = 0; mcId < NUM_MCS; mcId++)
  {
    assignedMCTargets[mcId] = 0;

    for (i = 0; i < NUM_MC_TARGETS; i++)
    {
      mcTarget[mcId][i] = 0;
    }
  }

  //-------- Initializing paramValue[], mcAmount[], modulatedValue[], minValue[], maxValue[]

  for (i = 0; i < NUM_UI_PARAMS; i++)
  {
    paramValue[i]     = 0;
    mcAmount[i]       = 0;
    modulatedValue[i] = 0;

    switch (i)
    {
      case PARAM_ID_ENV_A_RELEASE_TIME:
      case PARAM_ID_ENV_B_RELEASE_TIME:
      case PARAM_ID_ENV_C_RELEASE_TIME:
        minValue[i] = 0;
        maxValue[i] = 16160;  // greater than 16000 because 16160 is "inf"
        break;

      case PARAM_ID_OSC_A_PITCH:
      case PARAM_ID_OSC_B_PITCH:
      case PARAM_ID_ECHO_TIME:
        minValue[i] = 0;
        maxValue[i] = 15000;
        break;

      case PARAM_ID_COMB_AP_TUNE:     /// neu
      case PARAM_ID_FLANGER_AP_TUNE:  /// war vorher 0...12000
        minValue[i] = 0;
        maxValue[i] = 14000;
        break;

      case PARAM_ID_COMB_PITCH:
      case PARAM_ID_SVF_CUTOFF:
      case PARAM_ID_UNISON_DETUNE:
        minValue[i] = 0;
        maxValue[i] = 12000;
        break;

      case PARAM_ID_SHAPER_A_DRIVE:
      case PARAM_ID_SHAPER_B_DRIVE:
      case PARAM_ID_COMB_HI_CUT:
      case PARAM_ID_FB_MIXER_DRIVE:
      case PARAM_ID_OUT_MIXER_DRIVE:
      case PARAM_ID_CABINET_DRIVE:
      case PARAM_ID_CABINET_LEVEL:
        minValue[i] = 0;
        maxValue[i] = 10000;
        break;

      case PARAM_ID_GAP_FILT_CENTER:
      case PARAM_ID_GAP_FILT_GAP:
        minValue[i] = 0;
        maxValue[i] = 9600;
        break;

      case PARAM_ID_ENV_C_BREAKPOINT_LEVEL:
      case PARAM_ID_ENV_C_SUSTAIN_LEVEL:
      case PARAM_ID_OSC_A_PM_SELF:
      case PARAM_ID_OSC_A_PM_B:
      case PARAM_ID_OSC_A_PM_FB:
      case PARAM_ID_SHAPER_A_MIX:
      case PARAM_ID_OSC_B_PM_SELF:
      case PARAM_ID_OSC_B_PM_A:
      case PARAM_ID_OSC_B_PM_FB:
      case PARAM_ID_SHAPER_B_MIX:
      case PARAM_ID_COMB_DECAY:
      case PARAM_ID_COMB_PM:
      case PARAM_ID_SVF_COMB_MIX:
      case PARAM_ID_SVF_FM:
      case PARAM_ID_FB_MIXER_COMB:
      case PARAM_ID_FB_MIXER_SVF:
      case PARAM_ID_FB_MIXER_EFFECTS:
      case PARAM_ID_OUT_MIXER_A:
      case PARAM_ID_OUT_MIXER_B:
      case PARAM_ID_OUT_MIXER_COMB:
      case PARAM_ID_OUT_MIXER_SVF:
      case PARAM_ID_CABINET_TILT:
      case PARAM_ID_GAP_FILT_BALANCE:
      case PARAM_ID_GAP_FILT_MIX:
      case PARAM_ID_FLANGER_T_MOD:   /// war vorher unipolar
      case PARAM_ID_FLANGER_AP_MOD:  /// neu
      case PARAM_ID_FLANGER_STEREO:  /// war vorher nicht hier
      case PARAM_ID_FLANGER_FB:
      case PARAM_ID_FLANGER_CROSS_FB:  /// war vorher unipolar - 07.06.2017
      case PARAM_ID_FLANGER_MIX:
        minValue[i] = -8000;
        maxValue[i] = 8000;
        break;

      case PARAM_ID_ENV_A_GAIN:
      case PARAM_ID_ENV_B_GAIN:
      case PARAM_ID_OSC_A_PHASE:
      case PARAM_ID_OSC_B_PHASE:
        minValue[i] = -7200;
        maxValue[i] = 7200;
        break;

      case PARAM_ID_ECHO_STEREO:
        minValue[i] = -6600;
        maxValue[i] = 6600;
        break;

      case PARAM_ID_SVF_SPREAD:
        minValue[i] = -6000;
        maxValue[i] = 6000;
        break;

      default:
        minValue[i] = 0;
        maxValue[i] = 16000;
    }

    switch (i)
    {
      case PARAM_ID_ENV_C_BREAKPOINT_LEVEL:
      case PARAM_ID_ENV_C_SUSTAIN_LEVEL:
      case PARAM_ID_OSC_A_PM_SELF:
      case PARAM_ID_OSC_A_PM_B:
      case PARAM_ID_OSC_A_PM_FB:
      case PARAM_ID_SHAPER_A_MIX:
      case PARAM_ID_OSC_B_PM_SELF:
      case PARAM_ID_OSC_B_PM_A:
      case PARAM_ID_OSC_B_PM_FB:
      case PARAM_ID_SHAPER_B_MIX:
      case PARAM_ID_COMB_PM:
      case PARAM_ID_SVF_COMB_MIX:
      case PARAM_ID_SVF_FM:
      case PARAM_ID_FB_MIXER_COMB:
      case PARAM_ID_FB_MIXER_SVF:
      case PARAM_ID_FB_MIXER_EFFECTS:
      case PARAM_ID_OUT_MIXER_A:
      case PARAM_ID_OUT_MIXER_B:
      case PARAM_ID_OUT_MIXER_COMB:
      case PARAM_ID_OUT_MIXER_SVF:
      case PARAM_ID_CABINET_TILT:
      case PARAM_ID_GAP_FILT_BALANCE:
      case PARAM_ID_GAP_FILT_MIX:
      case PARAM_ID_FLANGER_T_MOD:
      case PARAM_ID_FLANGER_AP_MOD:
      case PARAM_ID_FLANGER_FB:
      case PARAM_ID_FLANGER_MIX:
        maxMCAmount[i] = 2000;  // für bipolare Parameter mit +/-100 %
        break;

      case PARAM_ID_ENV_A_GAIN:
      case PARAM_ID_ENV_B_GAIN:
        maxMCAmount[i] = 480;
        break;

      case PARAM_ID_OSC_A_PITCH:
      case PARAM_ID_OSC_B_PITCH:
        maxMCAmount[i] = 7500;
        break;

      case PARAM_ID_SHAPER_A_DRIVE:  // umgestellt von 25 auf 50 dB 12.06.2017
      case PARAM_ID_SHAPER_B_DRIVE:
      case PARAM_ID_FB_MIXER_DRIVE:
      case PARAM_ID_OUT_MIXER_DRIVE:
      case PARAM_ID_CABINET_DRIVE:
      case PARAM_ID_CABINET_LEVEL:
        maxMCAmount[i] = 500;
        break;

      case PARAM_ID_COMB_PITCH:
      case PARAM_ID_UNISON_DETUNE:
        maxMCAmount[i] = 6000;
        break;

      case PARAM_ID_COMB_AP_TUNE:
        maxMCAmount[i] = 1400;
        break;

      case PARAM_ID_SVF_CUTOFF:
        maxMCAmount[i] = 1200;
        break;

      case PARAM_ID_SVF_SPREAD:
        maxMCAmount[i] = 600;
        break;

      case PARAM_ID_CABINET_HI_CUT:
        maxMCAmount[i] = 800;
        break;

      case PARAM_ID_GAP_FILT_CENTER:
      case PARAM_ID_GAP_FILT_GAP:
        maxMCAmount[i] = 960;
        break;

      default:
        maxMCAmount[i] = 1000;
    }
  }

  paramValue[PARAM_ID_PITCHBEND] = 8000;

  transitionTime         = 100;  /// default: 30 ms - Fehler in Exp-Curve ???
  updateTransitionTime   = 0;
  smoothingTime          = 800;  // default: 10 ms
  updateSmoothingTime    = 0;
  usingTransitionTime    = 0;
  usingGlitchSuppression = 1;
}

//===================== Elementary Parameters

static void ProcessBasicParam(uint32_t paramId, int32_t paramVal)
{
  switch (paramId)
  {
    case PARAM_ID_ENV_A_ATTACK_CURVE:  // bipolar parameters directly sent to the TCD renderer
    case PARAM_ID_ENV_A_LEVELS_KEY_TRK:
    case PARAM_ID_ENV_A_GAIN:
    case PARAM_ID_ENV_B_ATTACK_CURVE:
    case PARAM_ID_ENV_B_LEVELS_KEY_TRK:
    case PARAM_ID_ENV_B_GAIN:
    case PARAM_ID_ENV_C_ATTACK_CURVE:
    case PARAM_ID_ENV_C_BREAKPOINT_LEVEL:
    case PARAM_ID_ENV_C_SUSTAIN_LEVEL:
    case PARAM_ID_ENV_C_LEVELS_KEY_TRK:
    case PARAM_ID_OSC_A_PHASE:
    case PARAM_ID_OSC_A_PITCH_ENV:
    case PARAM_ID_OSC_A_FLUCT_ENV:
    case PARAM_ID_OSC_A_PM_SELF:
    case PARAM_ID_OSC_A_PM_SELF_SHAPER:
    case PARAM_ID_OSC_A_PM_B:
    case PARAM_ID_OSC_A_PM_B_SHAPER:
    case PARAM_ID_OSC_A_PM_FB:
    case PARAM_ID_SHAPER_A_MIX:
    case PARAM_ID_OSC_B_PHASE:
    case PARAM_ID_OSC_B_PITCH_ENV:
    case PARAM_ID_OSC_B_FLUCT_ENV:
    case PARAM_ID_OSC_B_PM_SELF:
    case PARAM_ID_OSC_B_PM_SELF_SHAPER:
    case PARAM_ID_OSC_B_PM_A:
    case PARAM_ID_OSC_B_PM_A_SHAPER:
    case PARAM_ID_OSC_B_PM_FB:
    case PARAM_ID_SHAPER_B_MIX:
    case PARAM_ID_COMB_PITCH_ENV:
    case PARAM_ID_COMB_DECAY:
    case PARAM_ID_COMB_AP_ENV:
    case PARAM_ID_COMB_HI_CUT_ENV:  // war lange vergessen
    case PARAM_ID_COMB_PM:
    case PARAM_ID_SVF_COMB_MIX:
    case PARAM_ID_SVF_CUTOFF_ENV:
    case PARAM_ID_SVF_RESON_KEY_TRK:
    case PARAM_ID_SVF_RESON_ENV:
    case PARAM_ID_SVF_SPREAD:
    case PARAM_ID_SVF_PARALLEL:
    case PARAM_ID_SVF_FM:
    case PARAM_ID_FB_MIXER_COMB:
    case PARAM_ID_FB_MIXER_SVF:
    case PARAM_ID_FB_MIXER_EFFECTS:
    case PARAM_ID_FB_MIXER_LVL_KT:
    case PARAM_ID_OUT_MIXER_A:
    case PARAM_ID_OUT_MIXER_A_PAN:
    case PARAM_ID_OUT_MIXER_B:
    case PARAM_ID_OUT_MIXER_B_PAN:
    case PARAM_ID_OUT_MIXER_COMB:
    case PARAM_ID_OUT_MIXER_COMB_PAN:
    case PARAM_ID_OUT_MIXER_SVF:
    case PARAM_ID_OUT_MIXER_SVF_PAN:
    case PARAM_ID_CABINET_TILT:
    case PARAM_ID_GAP_FILT_STEREO:
    case PARAM_ID_GAP_FILT_BALANCE:
    case PARAM_ID_GAP_FILT_MIX:
    case PARAM_ID_FLANGER_T_MOD:
    case PARAM_ID_FLANGER_STEREO:
    case PARAM_ID_FLANGER_AP_MOD:
    case PARAM_ID_FLANGER_FB:
    case PARAM_ID_FLANGER_CROSS_FB:  // neuerdings bipolar
    case PARAM_ID_FLANGER_MIX:
    case PARAM_ID_ECHO_STEREO:
    case PARAM_ID_MASTER_TUNE:
      MSG_SelectParameter(paramId);
      MSG_SetDestinationSigned(paramVal);
      break;

    case PARAM_ID_UNISON_VOICES:  // number of Unison voices, applied in POLY and VALLOC and sent to the TCD renderer
      POLY_SetUnisonVoices(paramVal);
      MSG_SelectParameter(paramId);
      MSG_SetDestination(paramVal);
      break;

    default:  // unipolar parameters directly sent to the TCD renderer
      MSG_SelectParameter(paramId);
      MSG_SetDestination(paramVal);
  }

  paramValue[paramId] = paramVal;
}

static void ProcessBasicParamDirectly(uint32_t paramId, uint32_t paramVal)
{
  int32_t val;

  if (paramVal & 0x8000)
  {
    val = -(paramVal & 0x7FFF);
  }
  else
  {
    val = paramVal;
  }

  modulatedValue[paramId] = val * maxMCAmount[paramId];  // up-scaled for integer calculation of weighted increments of the MC modulation

  ProcessBasicParam(paramId, val);
}

//===================== Modulation Parameters

//------------------------ Play Control Amounts

static void SetPCAmount(uint32_t paramId, uint32_t paramVal)
{
  if (paramVal & 0x8000)
  {
    paramValue[paramId] = -(paramVal & 0x7FFF);
  }
  else
  {
    paramValue[paramId] = paramVal;
  }

  if ((paramId == 275) || (paramId == 276) || (paramId == 277) || (paramId == 278))
  {
    paramValue[paramId] *= 2;  // In the UI the pitchbend range is bipolar (-1...1). Therefore the amounts are 1/2 of the others.
  }                            // But here the ADC range is 0...16000, like the others.
}

//=========================== Macro Control Sources and Amounts

static void AddMCTarget(uint16_t amtId, uint32_t mcId)
{
  if (mcId >= NUM_MCS)  // invalid Id for the MC
  {
    return;  /// assertion
  }
  else
  {
    uint16_t length = assignedMCTargets[mcId];

    if (length >= NUM_MC_TARGETS)  // trying to assign too many targets
    {
      return;  /// assertion
    }
    else
    {
      mcTarget[mcId][length]  = amtId;       // appending the Id of the target at the end of the list
      assignedMCTargets[mcId] = length + 1;  // the length of the list is increased
    }
  }
}

static void RemoveMCTarget(uint16_t amtId, uint32_t mcId)
{
  if (mcId >= NUM_MCS)  // invalid Id for the MC
  {
    return;  /// assertion
  }
  else
  {
    uint16_t length = assignedMCTargets[mcId];
    uint32_t i;

    for (i = 0; i < length; i++)
    {
      if (mcTarget[mcId][i] == amtId)  // finding the target that should be removed
      {
        uint32_t j;

        for (j = i; j < (length - 1); j++)
        {
          mcTarget[mcId][j] = mcTarget[mcId][j + 1];  // filling the gap - the rest of the targets are moved down
        }

        assignedMCTargets[mcId] = length - 1;  // the length of the list is decreased

        break;
      }
    }
  }
}

//------------------------------- SetMCAmount

static void SetMCAmount(uint32_t amtId, uint32_t srcAndAmt)
{
  int32_t amt;

  if (0x2000 & srcAndAmt)  // bit 13 is the sign (1: negative)
  {
    amt = -(0x1FFF & srcAndAmt);  // bits 12...0 hold the absolute value
  }
  else
  {
    amt = 0x1FFF & srcAndAmt;  // bits 12...0
  }

  uint32_t srcId    = (0xC000 & srcAndAmt) >> 14;  // bits 15 and 14
  uint32_t oldSrcId = (0xC000 & paramValue[amtId]) >> 14;

  if (mcAmount[amtId] != 0)  // amounts of zero have already been removed from list
  {
    if ((amt == 0) || (srcId != oldSrcId))
    {
      RemoveMCTarget(amtId, oldSrcId);
    }
  }

  if (amt != 0)
  {
    if ((mcAmount[amtId] == 0) || (srcId != oldSrcId))
    {
      AddMCTarget(amtId, srcId);
    }
  }

  mcAmount[amtId]   = amt;
  paramValue[amtId] = srcAndAmt;
}

//=========================== Macro Control Activity

//--------------------------- ApplyMCToParam

static void ApplyMCToParam(uint32_t amtId, int32_t mcInc)
{
  int32_t  paramVal;
  uint32_t paramId;

  paramId = amtId - 1;  // assuming that the Id of the destination is the one before the Id of its MC amount

  int64_t min = minValue[paramId];
  int64_t max = maxValue[paramId];

  int64_t weightedInc = mcInc * mcAmount[amtId];  // up-scaled by the range of mcAmount

  modulatedValue[paramId] += (weightedInc * (max - min)) / 3200;  // down-scaled by the the MC Range (3200)
  paramVal = modulatedValue[paramId] / maxMCAmount[paramId];      // down-scaled by the range of mcAmount

  if (paramVal > max)
    paramVal = max;
  else if (paramVal < min)
    paramVal = min;

  ProcessBasicParam(paramId, paramVal);
}

//--------------------------- ProcessMacroControl

static void ProcessMacroControl(uint32_t mcId, uint32_t mcVal)  // mcVal: 0 ... 3200
{
  uint16_t  length;
  uint16_t* target;

  switch (mcId)
  {
    case PARAM_ID_MACRO_CONTROL_A:
      length = assignedMCTargets[0];
      target = mcTarget[0];
      break;
    case PARAM_ID_MACRO_CONTROL_B:
      length = assignedMCTargets[1];
      target = mcTarget[1];
      break;
    case PARAM_ID_MACRO_CONTROL_C:
      length = assignedMCTargets[2];
      target = mcTarget[2];
      break;
    case PARAM_ID_MACRO_CONTROL_D:
      length = assignedMCTargets[3];
      target = mcTarget[3];
      break;
    default:
      return;
  }

  int32_t inc;

  inc              = mcVal - paramValue[mcId];
  paramValue[mcId] = mcVal;

  uint32_t i;

  for (i = 0; i < length; i++)
  {
    ApplyMCToParam(target[i], inc);
  }
}

static void ProcessMacroControlDirectly(uint32_t mcId, uint32_t mcVal)  // mcVal: 0 ... 3200
{
  modulatedValue[mcId] = mcVal * 40000;  // up-scaled by 5 * 8000 for integer calculation of weighted increments of the play controls (0 ... 16000) * (0 ... 8000)
                                         // (pcVal is 5 times larger than mcVal: 16000/3200)
  ProcessMacroControl(mcId, mcVal);
}

//------------------------ ProcessPlayControl

static void ProcessPlayControl(uint32_t pcId, uint32_t pcVal, uint32_t behaviour)  // pcVal is for all in the range 0 ... 16000 (Pitchbend center at 8000)
{
  int32_t inc;
  int32_t mcVal;

  inc              = pcVal - paramValue[pcId];
  paramValue[pcId] = pcVal;

  if (paramValue[pcId + 1])  // the PC amounts are located at the next 4 Ids after the play control
  {
    if (behaviour == NON_RETURN)
    {
      modulatedValue[PARAM_ID_MACRO_CONTROL_A] = pcVal * 8000;                                      // up-scaled by the range of the PC Amount (-8000 ... 8000)
      mcVal                                    = modulatedValue[PARAM_ID_MACRO_CONTROL_A] / 40000;  // down-scaled by 5 * 8000 (pcVal is 5 times larger than mcVal)
    }
    else
    {
      modulatedValue[PARAM_ID_MACRO_CONTROL_A] += inc * paramValue[pcId + 1];  // up-scaled by the range of the PC Amount (-8000 ... 8000)
      mcVal = modulatedValue[PARAM_ID_MACRO_CONTROL_A] / 40000;                // down-scaled by 5 * 8000 (pcVal is 5 times larger than mcVal)

      if (mcVal > 3200)
        mcVal = 3200;
      else if (mcVal < 0)
        mcVal = 0;
    }

    ProcessMacroControl(PARAM_ID_MACRO_CONTROL_A, mcVal);
  }

  if (paramValue[pcId + 2])
  {
    if (behaviour == NON_RETURN)
    {
      modulatedValue[PARAM_ID_MACRO_CONTROL_B] = pcVal * 8000;                                      // up-scaled by the range of the PC Amount (-8000 ... 8000)
      mcVal                                    = modulatedValue[PARAM_ID_MACRO_CONTROL_B] / 40000;  // down-scaled by 5 * 8000 (pcVal is 5 times larger than mcVal)
    }
    else
    {
      modulatedValue[PARAM_ID_MACRO_CONTROL_B] += inc * paramValue[pcId + 2];  // up-scaled by the range of the PC Amount (-8000 ... 8000)
      mcVal = modulatedValue[PARAM_ID_MACRO_CONTROL_B] / 40000;                // down-scaled by 5 * 8000 (pcVal is 5 times larger than mcVal)

      if (mcVal > 3200)
        mcVal = 3200;
      else if (mcVal < 0)
        mcVal = 0;
    }

    ProcessMacroControl(PARAM_ID_MACRO_CONTROL_B, mcVal);
  }

  if (paramValue[pcId + 3])
  {
    if (behaviour == NON_RETURN)
    {
      modulatedValue[PARAM_ID_MACRO_CONTROL_C] = pcVal * 8000;                                      // up-scaled by the range of the PC Amount (-8000 ... 8000)
      mcVal                                    = modulatedValue[PARAM_ID_MACRO_CONTROL_C] / 40000;  // down-scaled by 5 * 8000 (pcVal is 5 times larger than mcVal)
    }
    else
    {
      modulatedValue[PARAM_ID_MACRO_CONTROL_C] += inc * paramValue[pcId + 3];  // up-scaled by the range of the PC Amount (8000 ... 8000)
      mcVal = modulatedValue[PARAM_ID_MACRO_CONTROL_C] / 40000;                // down-scaled by 5 * 8000 (pcVal is 5 times larger than mcVal)

      if (mcVal > 3200)
        mcVal = 3200;
      else if (mcVal < 0)
        mcVal = 0;
    }

    ProcessMacroControl(PARAM_ID_MACRO_CONTROL_C, mcVal);
  }

  if (paramValue[pcId + 4])
  {
    if (behaviour == NON_RETURN)
    {
      modulatedValue[PARAM_ID_MACRO_CONTROL_D] = pcVal * 8000;                                      // up-scaled by the range of the PC Amount (-8000 ... 8000)
      mcVal                                    = modulatedValue[PARAM_ID_MACRO_CONTROL_D] / 40000;  // down-scaled by 5 * 8000 (pcVal is 5 times larger than mcVal)
    }
    else
    {
      modulatedValue[PARAM_ID_MACRO_CONTROL_D] += inc * paramValue[pcId + 4];  // up-scaled by the range of the PC Amount (-8000 ... 8000)
      mcVal = modulatedValue[PARAM_ID_MACRO_CONTROL_D] / 40000;                // down-scaled by 5 * 8000 (pcVal is 5 times larger than mcVal)

      if (mcVal > 3200)
        mcVal = 3200;
      else if (mcVal < 0)
        mcVal = 0;
    }

    ProcessMacroControl(PARAM_ID_MACRO_CONTROL_D, mcVal);
  }
}

//------------------------ SetAllTimes

void SetAllTimes(uint32_t time)  // Common smoothing or transition time for all audio parameters
{
  MSG_SelectParameter(0);
  MSG_SelectMultipleParameters(310);
  MSG_SetTime(time);
}

//======================= Entry points for parameter and preset messages

void PARAM_Set(uint32_t paramId, uint32_t paramVal)
{
  if (paramId < NUM_UI_PARAMS)
  {
    ADC_WORK_Suspend();

    if ((usingTransitionTime == 1) || (updateSmoothingTime == 1))
    {
      SetAllTimes(smoothingTime);  // switching from transition smoothing to edit smoothing

      usingTransitionTime = 0;
      updateSmoothingTime = 0;
    }

    switch (paramId)
    {
      case PARAM_ID_PEDAL_1:
        ProcessPlayControl(PARAM_ID_PEDAL_1, paramVal, ADC_WORK_GetPedal1Behaviour());
        break;
      case PARAM_ID_PEDAL_2:
        ProcessPlayControl(PARAM_ID_PEDAL_2, paramVal, ADC_WORK_GetPedal2Behaviour());
        break;
      case PARAM_ID_PEDAL_3:
        ProcessPlayControl(PARAM_ID_PEDAL_3, paramVal, ADC_WORK_GetPedal3Behaviour());
        break;
      case PARAM_ID_PEDAL_4:
        ProcessPlayControl(PARAM_ID_PEDAL_4, paramVal, ADC_WORK_GetPedal4Behaviour());
        break;
      case PARAM_ID_PITCHBEND:
        ProcessPlayControl(PARAM_ID_PITCHBEND, paramVal, RETURN_TO_CENTER);
        break;
      case PARAM_ID_AFTERTOUCH:
        ProcessPlayControl(PARAM_ID_AFTERTOUCH, paramVal, RETURN_TO_ZERO);
        break;
      case PARAM_ID_RIBBON_1:
        ProcessPlayControl(PARAM_ID_RIBBON_1, paramVal, ADC_WORK_GetRibbon1Behaviour());
        break;
      case PARAM_ID_RIBBON_2:
        ProcessPlayControl(PARAM_ID_RIBBON_2, paramVal, ADC_WORK_GetRibbon2Behaviour());
        break;

      case PARAM_ID_MACRO_CONTROL_A:
      case PARAM_ID_MACRO_CONTROL_B:
      case PARAM_ID_MACRO_CONTROL_C:
      case PARAM_ID_MACRO_CONTROL_D:
        ProcessMacroControlDirectly(paramId, paramVal);
        break;

      case PARAM_ID_PEDAL_1_TO_MC_A:
      case PARAM_ID_PEDAL_1_TO_MC_B:
      case PARAM_ID_PEDAL_1_TO_MC_C:
      case PARAM_ID_PEDAL_1_TO_MC_D:
      case PARAM_ID_PEDAL_2_TO_MC_A:
      case PARAM_ID_PEDAL_2_TO_MC_B:
      case PARAM_ID_PEDAL_2_TO_MC_C:
      case PARAM_ID_PEDAL_2_TO_MC_D:
      case PARAM_ID_PEDAL_3_TO_MC_A:
      case PARAM_ID_PEDAL_3_TO_MC_B:
      case PARAM_ID_PEDAL_3_TO_MC_C:
      case PARAM_ID_PEDAL_3_TO_MC_D:
      case PARAM_ID_PEDAL_4_TO_MC_A:
      case PARAM_ID_PEDAL_4_TO_MC_B:
      case PARAM_ID_PEDAL_4_TO_MC_C:
      case PARAM_ID_PEDAL_4_TO_MC_D:
      case PARAM_ID_PITCHBEND_TO_MC_A:
      case PARAM_ID_PITCHBEND_TO_MC_B:
      case PARAM_ID_PITCHBEND_TO_MC_C:
      case PARAM_ID_PITCHBEND_TO_MC_D:
      case PARAM_ID_AFTERTOUCH_TO_MC_A:
      case PARAM_ID_AFTERTOUCH_TO_MC_B:
      case PARAM_ID_AFTERTOUCH_TO_MC_C:
      case PARAM_ID_AFTERTOUCH_TO_MC_D:
      case PARAM_ID_RIBBON_1_TO_MC_A:
      case PARAM_ID_RIBBON_1_TO_MC_B:
      case PARAM_ID_RIBBON_1_TO_MC_C:
      case PARAM_ID_RIBBON_1_TO_MC_D:
      case PARAM_ID_RIBBON_2_TO_MC_A:
      case PARAM_ID_RIBBON_2_TO_MC_B:
      case PARAM_ID_RIBBON_2_TO_MC_C:
      case PARAM_ID_RIBBON_2_TO_MC_D:
        SetPCAmount(paramId, paramVal);
        break;

      case PARAM_ID_SCALE_BASE_KEY:
        POLY_SetScaleBase(paramVal);  // 0: C, 1: C# ... 11: H
        break;

      case PARAM_ID_SCALE_OFFSET_1:
      case PARAM_ID_SCALE_OFFSET_2:
      case PARAM_ID_SCALE_OFFSET_3:
      case PARAM_ID_SCALE_OFFSET_4:
      case PARAM_ID_SCALE_OFFSET_5:
      case PARAM_ID_SCALE_OFFSET_6:
      case PARAM_ID_SCALE_OFFSET_7:
      case PARAM_ID_SCALE_OFFSET_8:
      case PARAM_ID_SCALE_OFFSET_9:
      case PARAM_ID_SCALE_OFFSET_10:
      case PARAM_ID_SCALE_OFFSET_11:
        POLY_SetScaleOffset(paramId, paramVal);  // -8000: -800 Cent ... 8000: +800 Cent
        break;

      default:
        ProcessBasicParamDirectly(paramId, paramVal);
    }

    ADC_WORK_Resume();
  }
}

void PARAM_Set2(uint32_t paramId, uint32_t paramVal1, uint32_t paramVal2)
{
  if (paramId < NUM_UI_PARAMS)
  {
    ADC_WORK_Suspend();

    if ((usingTransitionTime == 1) || (updateSmoothingTime == 1))
    {
      SetAllTimes(smoothingTime);  // switching from transition smoothing to edit smoothing

      usingTransitionTime = 0;
      updateSmoothingTime = 0;
    }

    if (paramVal1 != paramValue[paramId])
    {
      ProcessBasicParamDirectly(paramId, paramVal1);
    }

    if (paramVal2 != paramValue[paramId + 1])
    {
      SetMCAmount(paramId + 1, paramVal2);  // relying in the rule that the MC amount is located directly after the parameter to which it belongs
    }

    ADC_WORK_Resume();
  }
}

void PARAM_ApplyPreset(uint16_t numParams, uint16_t* data)
{
  if (numParams > NUM_UI_PARAMS)
  {
    numParams = NUM_UI_PARAMS;  /// Vielleicht ein Fall für eine Assertion ???
  }

  ADC_WORK_Suspend();

  if ((usingTransitionTime == 0) || (updateTransitionTime == 1))
  {
    SetAllTimes(transitionTime);  // switching from edit smoothing to transition smoothing

    usingTransitionTime  = 1;
    updateTransitionTime = 0;
  }

  if (usingGlitchSuppression)
  {
    MSG_Reset(0);  // flushing the delays of the audio engine to avoid glitches
  }

  MSG_EnablePreload();

  uint32_t paramId;

  for (paramId = 0; paramId < numParams; paramId++)
  {
    switch (paramId)
    {
      case PARAM_ID_PEDAL_1:  // the play controls
      case PARAM_ID_PEDAL_2:
      case PARAM_ID_PEDAL_3:
      case PARAM_ID_PEDAL_4:
      case PARAM_ID_PITCHBEND:
      case PARAM_ID_AFTERTOUCH:
      case PARAM_ID_RIBBON_1:
      case PARAM_ID_RIBBON_2:
        break;  // we ignore them, because they can conflict with the state of the hardware sources

      case PARAM_ID_MACRO_CONTROL_A:  // the macro controls
      case PARAM_ID_MACRO_CONTROL_B:
      case PARAM_ID_MACRO_CONTROL_C:
      case PARAM_ID_MACRO_CONTROL_D:
        modulatedValue[paramId] = data[paramId] * 40000;  // up-scaled for integer calculation of weighted increments of the play controls
        paramValue[paramId]     = data[paramId];          // only setting the variables, no processing!
        break;

      case PARAM_ID_PEDAL_1_TO_MC_A:  // the play control amounts
      case PARAM_ID_PEDAL_1_TO_MC_B:
      case PARAM_ID_PEDAL_1_TO_MC_C:
      case PARAM_ID_PEDAL_1_TO_MC_D:
      case PARAM_ID_PEDAL_2_TO_MC_A:
      case PARAM_ID_PEDAL_2_TO_MC_B:
      case PARAM_ID_PEDAL_2_TO_MC_C:
      case PARAM_ID_PEDAL_2_TO_MC_D:
      case PARAM_ID_PEDAL_3_TO_MC_A:
      case PARAM_ID_PEDAL_3_TO_MC_B:
      case PARAM_ID_PEDAL_3_TO_MC_C:
      case PARAM_ID_PEDAL_3_TO_MC_D:
      case PARAM_ID_PEDAL_4_TO_MC_A:
      case PARAM_ID_PEDAL_4_TO_MC_B:
      case PARAM_ID_PEDAL_4_TO_MC_C:
      case PARAM_ID_PEDAL_4_TO_MC_D:
      case PARAM_ID_PITCHBEND_TO_MC_A:
      case PARAM_ID_PITCHBEND_TO_MC_B:
      case PARAM_ID_PITCHBEND_TO_MC_C:
      case PARAM_ID_PITCHBEND_TO_MC_D:
      case PARAM_ID_AFTERTOUCH_TO_MC_A:
      case PARAM_ID_AFTERTOUCH_TO_MC_B:
      case PARAM_ID_AFTERTOUCH_TO_MC_C:
      case PARAM_ID_AFTERTOUCH_TO_MC_D:
      case PARAM_ID_RIBBON_1_TO_MC_A:
      case PARAM_ID_RIBBON_1_TO_MC_B:
      case PARAM_ID_RIBBON_1_TO_MC_C:
      case PARAM_ID_RIBBON_1_TO_MC_D:
      case PARAM_ID_RIBBON_2_TO_MC_A:
      case PARAM_ID_RIBBON_2_TO_MC_B:
      case PARAM_ID_RIBBON_2_TO_MC_C:
      case PARAM_ID_RIBBON_2_TO_MC_D:
        SetPCAmount(paramId, data[paramId]);
        break;

      case PARAM_ID_ENV_A_ATTACK_MC:  // the macro control amounts
      case PARAM_ID_ENV_A_DECAY_1_MC:
      case PARAM_ID_ENV_A_BREAKPOINT_MC:
      case PARAM_ID_ENV_A_DECAY_2_MC:
      case PARAM_ID_ENV_A_SUSTAIN_MC:
      case PARAM_ID_ENV_A_RELEASE_MC:
      case PARAM_ID_ENV_A_GAIN_MC:
      case PARAM_ID_ENV_B_ATTACK_MC:
      case PARAM_ID_ENV_B_DECAY_1_MC:
      case PARAM_ID_ENV_B_BREAKPOINT_MC:
      case PARAM_ID_ENV_B_DECAY_2_MC:
      case PARAM_ID_ENV_B_SUSTAIN_MC:
      case PARAM_ID_ENV_B_RELEASE_MC:
      case PARAM_ID_ENV_B_GAIN_MC:
      case PARAM_ID_ENV_C_ATTACK_MC:
      case PARAM_ID_ENV_C_DECAY_1_MC:
      case PARAM_ID_ENV_C_BREAKPOINT_MC:
      case PARAM_ID_ENV_C_DECAY_2_MC:
      case PARAM_ID_ENV_C_SUSTAIN_MC:
      case PARAM_ID_ENV_C_RELEASE_MC:
      case PARAM_ID_OSC_A_PITCH_MC:
      case PARAM_ID_OSC_A_FLUCT_MC:
      case PARAM_ID_OSC_A_PM_SELF_MC:
      case PARAM_ID_OSC_A_PM_B_MC:
      case PARAM_ID_OSC_A_PM_FB_MC:
      case PARAM_ID_SHAPER_A_DRIVE_MC:
      case PARAM_ID_SHAPER_A_MIX_MC:
      case PARAM_ID_SHAPER_A_FB_MIX_MC:
      case PARAM_ID_SHAPER_A_RING_MOD_MC:
      case PARAM_ID_OSC_B_PITCH_MC:
      case PARAM_ID_OSC_B_FLUCT_MC:
      case PARAM_ID_OSC_B_PM_SELF_MC:
      case PARAM_ID_OSC_B_PM_A_MC:
      case PARAM_ID_OSC_B_PM_FB_MC:
      case PARAM_ID_SHAPER_B_DRIVE_MC:
      case PARAM_ID_SHAPER_B_MIX_MC:
      case PARAM_ID_SHAPER_B_FB_MIX_MC:
      case PARAM_ID_SHAPER_B_RING_MOD_MC:
      case PARAM_ID_COMB_A_B_MC:
      case PARAM_ID_COMB_PITCH_MC:
      case PARAM_ID_COMB_DECAY_MC:
      case PARAM_ID_COMB_DECAY_GATE_MC:
      case PARAM_ID_COMB_AP_TUNE_MC:
      case PARAM_ID_COMB_AP_RESON_MC:
      case PARAM_ID_COMB_HI_CUT_MC:
      case PARAM_ID_COMB_PM_MC:
      case PARAM_ID_SVF_A_B_MC:
      case PARAM_ID_SVF_COMB_MIX_MC:
      case PARAM_ID_SVF_CUTOFF_MC:
      case PARAM_ID_SVF_RESON_MC:
      case PARAM_ID_SVF_SPREAD_MC:
      case PARAM_ID_SVF_L_B_H_MC:
      case PARAM_ID_SVF_FM_MC:
      case PARAM_ID_FB_MIXER_COMB_MC:
      case PARAM_ID_FB_MIXER_SVF_MC:
      case PARAM_ID_FB_MIXER_EFFECTS_MC:
      case PARAM_ID_FB_MIXER_REVERB_MC:
      case PARAM_ID_FB_MIXER_DRIVE_MC:
      case PARAM_ID_FB_MIXER_LEVEL_MC:
      case PARAM_ID_OUT_MIXER_A_MC:
      case PARAM_ID_OUT_MIXER_B_MC:
      case PARAM_ID_OUT_MIXER_COMB_MC:
      case PARAM_ID_OUT_MIXER_SVF_MC:
      case PARAM_ID_OUT_MIXER_DRIVE_MC:
      case PARAM_ID_OUT_MIXER_LEVEL_MC:
      case PARAM_ID_CABINET_DRIVE_MC:
      case PARAM_ID_CABINET_TILT_MC:
      case PARAM_ID_CABINET_HI_CUT_MC:
      case PARAM_ID_CABINET_LEVEL_MC:
      case PARAM_ID_CABINET_MIX_MC:
      case PARAM_ID_GAP_FILT_CENTER_MC:
      case PARAM_ID_GAP_FILT_GAP_MC:
      case PARAM_ID_GAP_FILT_BALANCE_MC:
      case PARAM_ID_GAP_FILT_MIX_MC:
      case PARAM_ID_FLANGER_T_MOD_MC:
      case PARAM_ID_FLANGER_RATE_MC:
      case PARAM_ID_FLANGER_TIME_MC:
      case PARAM_ID_FLANGER_AP_MOD_MC:
      case PARAM_ID_FLANGER_AP_TUNE_MC:
      case PARAM_ID_FLANGER_FB_MC:
      case PARAM_ID_FLANGER_MIX_MC:
      case PARAM_ID_ECHO_TIME_MC:
      case PARAM_ID_ECHO_STEREO_MC:
      case PARAM_ID_ECHO_FB_MC:
      case PARAM_ID_ECHO_MIX_MC:
      case PARAM_ID_REVERB_SIZE_MC:
      case PARAM_ID_REVERB_HI_CUT_MC:
      case PARAM_ID_REVERB_MIX_MC:
      case PARAM_ID_UNISON_DETUNE_MC:
        if (data[paramId] != paramValue[paramId])
        {
          SetMCAmount(paramId, data[paramId]);
        }
        break;

      case PARAM_ID_SCALE_BASE_KEY:
        POLY_SetScaleBase(data[paramId]);  // 0: C, 1: C# ... 11: H
        break;

      case PARAM_ID_SCALE_OFFSET_1:
      case PARAM_ID_SCALE_OFFSET_2:
      case PARAM_ID_SCALE_OFFSET_3:
      case PARAM_ID_SCALE_OFFSET_4:
      case PARAM_ID_SCALE_OFFSET_5:
      case PARAM_ID_SCALE_OFFSET_6:
      case PARAM_ID_SCALE_OFFSET_7:
      case PARAM_ID_SCALE_OFFSET_8:
      case PARAM_ID_SCALE_OFFSET_9:
      case PARAM_ID_SCALE_OFFSET_10:
      case PARAM_ID_SCALE_OFFSET_11:
        POLY_SetScaleOffset(paramId, data[paramId]);  // -8000: -800 Cent ... 8000: +800 Cent		/// Implizites Casting auf int16_t! Besser unser sign+abs verwenden?
        break;

      default:  // all basic parameters
        ProcessBasicParamDirectly(paramId, data[paramId]);
    }
  }

  ADC_WORK_Resume();

  MSG_ApplyPreloadedValues();  /// war vorher kurz vor dem Fade-In
}

//======================= Entry points for setting messages

void PARAM_SetTransitionTime(uint32_t time)
{
  time = EXPON_Time(time);  // exponentiell: 0...16000 => 0...16000 ms

  if (time != transitionTime)
  {
    updateTransitionTime = 1;  // makes sure that it will be applied with the next preset recall

    transitionTime = time;
  }
}

void PARAM_SetEditSmoothingTime(uint32_t time)
{
  time = (9830 * time) >> 14;  // (9600 * time) / 16000  //  linear: 0...16000 => 0...200 ms in Samples

  if (time != smoothingTime)
  {
    updateSmoothingTime = 1;  // makes sure that it will be applied with the next parameter editing

    smoothingTime = time;
  }
}

void PARAM_SetGlitchSuppression(uint32_t mode)
{
  usingGlitchSuppression = mode;
}

void PARAM_SetNoteShift(uint32_t shift)  // kommt als Setting (uint16 with sign bit) vom BB
{
  int32_t noteShift;

  if (shift & 0x8000)
  {
    noteShift = -(shift & 0x7FFF);
  }
  else
  {
    noteShift = shift;
  }

  MSG_SelectParameter(PARAM_ID_NOTE_SHIFT);
  MSG_SetDestinationSigned(noteShift);
}
