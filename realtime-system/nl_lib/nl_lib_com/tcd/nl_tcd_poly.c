/*
 * nl_tcd_poly.c
 *
 *  Created on: 27.01.2015
 *      Author: ssc
 *	Last Changed : 2020-01-10 KSTR (implement 14bit velocities)
 */

#include "math.h"
#include "nl_tcd_poly.h"
#include "nl_tcd_adc_work.h"
#include "nl_tcd_msg.h"
#include "ipc/emphase_ipc.h"

//------- modul local variables

static IPC_KEY_EVENT_T keyEvent[32];  // array for new events read from the ring buffer for keybed events

static uint32_t velTable[65] = {};  // converts time difference (timeInUs) to velocities
                                    // element  0: shortest timeInUs   (2500 us or lower) -> 4096 = max. velocity
                                    // element 64: longest timeInUs (526788 us or higher) -> 0    = min. velocity

/*******************************************************************************
@brief  	POLY_Generate_VelTable - generates the elements of velTable[]
@param[in]	curve - selects one of the five verlocity curves
			table index: 0 ... 64 (shortest ... longest key-switch time)
			table values: 4095 ... 0
*******************************************************************************/
void POLY_Generate_VelTable(uint32_t curve)
{
  float_t vel_max = 16383.0;  // the hyperbola goes from vel_max (at 0) to 0 (at i_max)
  float_t b       = 0.5;

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
  POLY_Generate_VelTable(VEL_CURVE_NORMAL);
}

/******************************************************************************
	@brief		POLY_Process: reading new key events from the ring buffer,
*******************************************************************************/
void POLY_Process(void)
{
  uint32_t i;
  uint32_t time;
  uint32_t vel;

  uint32_t numKeyEvents = Emphase_IPC_M4_KeyBuffer_ReadBuffer(keyEvent, 32);  // reads the latest key up/down events from the ring buffer shared with the M0

  for (i = 0; i < numKeyEvents; i++)
  {
    MSG_KeyPosition(keyEvent[i].key + 36);

    time = keyEvent[i].timeInUs;

    //--- Calc On velocity

    if (time <= 2500)  // clipping the low end: zero at a times <= 2.5 ms
    {
      vel = velTable[0];
    }
    else if (time >= (524288 + 2500))  // clipping at a maximum of 524 ms (2^19 us)
    {
      vel = velTable[64];
    }
    else
    {
      time -= 2500;  // shifting the curve to the left to adjust the input to zero for a time of 2.5 ms

      uint32_t fract = time & 0x1FFF;                                                           // lower 13 bits used for interpolation
      uint32_t index = time >> 13;                                                              // upper 6 bits (0...63) used as index in the table
      vel            = (velTable[index] * (8192 - fract) + velTable[index + 1] * fract) >> 13;  // (0...16393) * 8192 / 8192
    }
    if (keyEvent[i].direction == KEY_DIR_UP)  //--- releasing a key
    {
      MSG_KeyUp(vel);
    }
    else  //--- pressing a key
    {
      MSG_KeyDown(vel);
      ADC_WORK_WriteHWValueForBB(HW_SOURCE_ID_LAST_KEY, keyEvent[i].key + 36);
    }
  }

  MSG_SendMidiBuffer();
}
