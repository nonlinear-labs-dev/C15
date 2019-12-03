/*
 * nl_tcd_poly.c
 *
 *  Created on: 27.01.2015
 *      Author: ssc
 */

#include "math.h"

#include "nl_tcd_poly.h"

#include "nl_tcd_param_work.h"
#include "nl_tcd_msg.h"
#include "nl_tcd_valloc.h"

static int32_t v_KeyPos[NUM_VOICES] = {};

static int32_t e_UnisonVoices;

static uint32_t e_ScaleBase;
static int16_t  e_ScaleOffset[12];

static uint32_t velTable[65] = {};  // converts time difference (timeInUs) to velocities
                                    // element  0: shortest timeInUs   (2500 us or lower) -> 4096 = max. velocity
                                    // element 64: longest timeInUs (526788 us or higher) -> 0    = min. velocity

/*******************************************************************************
@brief  	ENV_Generate_VelTable - generates the elements of velTable[]
@param[in]	curve - selects one of the five verlocity curves
			table index: 0 ... 64 (shortest ... longest key-switch time)
			table values: 4096 ... 0
*******************************************************************************/

void POLY_Generate_VelTable(uint32_t curve)
{
  float_t vel_max = 4096.0;  // the hyperbola goes from vel_max (at 0) to 0 (at i_max)

  float_t b = 0.5;

  switch (curve)  // b defines the curve shape
  {
    case VEL_CURVE_VERY_SOFT:
      b = 0.125;
      break;
    case VEL_CURVE_SOFT:
      b = 0.25;
      break;
    case VEL_CURVE_NORMAL:
      b = 0.5;
      break;
    case VEL_CURVE_HARD:
      b = 1.0;
      break;
    case VEL_CURVE_VERY_HARD:
      b = 2.0;
      break;
    default:
      /// Error
      break;
  }

  uint32_t i_max = 64;

  uint32_t i;

  for (i = 0; i <= i_max; i++)
  {
    velTable[i] = (uint32_t)((vel_max + vel_max / (b * i_max)) / (1.0 + b * i) - vel_max / (b * i_max) + 0.5);
  }
}

//================= Initialisation:

void POLY_Init(void)
{
  uint32_t v;
  uint32_t i;

  for (v = 0; v < NUM_VOICES; v++)
  {
    v_KeyPos[v] = 0;
  }

  e_UnisonVoices = 1;

  e_ScaleBase = 0;

  for (i = 0; i < 12; i++)
  {
    e_ScaleOffset[i] = 0;  // e_ScaleOffset[0] bleibt immer Null (base key)
  }

  POLY_Generate_VelTable(VEL_CURVE_NORMAL);
}

//================= Custom Scale interface:

void POLY_SetScaleBase(uint32_t baseKey)
{
  if (baseKey < 12)
  {
    e_ScaleBase = 12 - baseKey;
  }
  /// else error
}

void POLY_SetScaleOffset(uint32_t paramId, uint32_t offset)
{
  uint32_t step;

  step = 1 + paramId - PARAM_ID_SCALE_OFFSET_1;

  if (step < 12)
  {
    if (offset & 0x8000)
    {
      e_ScaleOffset[step] = -(offset & 0x7FFF);
    }
    else
    {
      e_ScaleOffset[step] = offset;
    }
  }  /// es wäre besser, auch klingende Stimmen verstimmen zu können, ähnlich wie Tune/Unison
     /// else error
}

//================= Keybed Events:

void POLY_KeyDown(uint32_t allocVoice, uint32_t key, uint32_t timeInUs)
{
  int32_t  pitch;
  uint32_t vel;

  //--- Calc pitch

  pitch = (key - 24) * 1000 + e_ScaleOffset[(key + e_ScaleBase) % 12];

  //--- Calc On velocity

  if (timeInUs <= 2500)  // clipping the low end: zero at a times <= 2.5 ms
  {
    vel = velTable[0];
  }
  else if (timeInUs >= (524288 + 2500))  // clipping at a maximum of 524 ms (2^19 us)
  {
    vel = velTable[64];
  }
  else
  {
    timeInUs -= 2500;  // shifting the curve to the left adjusting the input to zero for a time of 2.5 ms

    uint32_t fract = timeInUs & 0x1FFF;                                                       // lower 13 bits used for interpolation
    uint32_t index = timeInUs >> 13;                                                          // upper 6 bits (0...63) used as index in the table
    vel            = (velTable[index] * (8192 - fract) + velTable[index + 1] * fract) >> 13;  // (0...4096) * 8192 / 8192
  }

  MSG_KeyVoice(0, allocVoice * e_UnisonVoices);  // selects voice(s) and parameter 416 (Base Pitch)

  MSG_SetDestinationSigned(pitch);  // NotePitch

  MSG_KeyDown(vel);
}

void POLY_KeyUp(uint32_t allocVoice, uint32_t timeInUs)
{
  uint32_t vel;

  //--- Calc Off velocity

  if (timeInUs <= 2500)  // clipping the low end: zero at a times <= 2.5 ms
  {
    vel = velTable[0];
  }
  else if (timeInUs >= (524288 + 2500))  // clipping at a maximum of 524 ms (2^19 us)
  {
    vel = velTable[64];
  }
  else
  {
    timeInUs -= 2500;  // shifting the curve to the left adjusting the input to zero for a time of 2.5 ms

    uint32_t fract = timeInUs & 0x1FFF;                                                       // lower 13 bits used for interpolation
    uint32_t index = timeInUs >> 13;                                                          // upper 6 bits (0...63) used as index in the table
    vel            = (velTable[index] * (8192 - fract) + velTable[index + 1] * fract) >> 13;  // (0...4096) * 8192 / 8192
  }

  MSG_KeyVoice(0, allocVoice * e_UnisonVoices);  // selects voice(s)

  MSG_KeyUp(vel);
}

//================= Parameter-Events:

void POLY_SetUnisonVoices(int32_t value)
{
  if (value != e_UnisonVoices)
  {
    e_UnisonVoices = value;

    VALLOC_Init((uint32_t)(NUM_VOICES / e_UnisonVoices));  // for Unison only fraction of the number of voices will allocated

    MSG_Reset(1);  // reset the envelopes
  }
}
