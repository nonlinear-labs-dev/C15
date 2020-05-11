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
#include "drv/nl_cgu.h"
#include "spibb/nl_bb_msg.h"

//------- module local variables

static uint32_t keyEvent[32];  // array for new events read from the ring buffer for keybed events

static uint32_t allVelTables[VEL_CURVE_COUNT][65] = {};  // converts time difference (timeInUs) to velocities
                                                         // element  0: shortest timeInUs   (2500 us or lower) -> 16383 = max. velocity
                                                         // element 64: longest timeInUs (526788 us or higher) -> 0     = min. velocity
static uint32_t *velTable = allVelTables[VEL_CURVE_NORMAL];

enum KeyLog_T
{
  LOG_OFF = 0,
  LOG_ON,
};

static enum KeyLog_T logKeys = LOG_OFF;

void POLY_KeyLogging(uint16_t const on)
{
  logKeys = (on != 0) ? LOG_ON : LOG_OFF;
}

/*******************************************************************************
@brief  	Generate_VelTable - generates the elements of velTable[]
@param[in]	curve - selects one of the five verlocity curves
			table index: 0 ... 64 (shortest ... longest key-switch time)
			table values: 16383 ... 0
*******************************************************************************/
static void Generate_VelTable(uint32_t const curve)
{
  if (curve >= VEL_CURVE_COUNT)
    return;

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
    allVelTables[curve][i] = (uint32_t)((vel_max + vel_max / (b * i_max)) / (1.0 + b * i) - vel_max / (b * i_max) + 0.5);
  }
}

//================= Initialisation:
void POLY_Init(void)
{
  for (int i = 0; i < VEL_CURVE_COUNT; i++)
    Generate_VelTable(i);
  velTable = allVelTables[VEL_CURVE_NORMAL];
}

/******************************************************************************
	@brief		Select a Velocity table
*******************************************************************************/
void POLY_Select_VelTable(uint32_t const curve)
{
  if (curve >= VEL_CURVE_COUNT)
    return;
  velTable = allVelTables[curve];
}

/******************************************************************************
	@brief		ProcessKeyEvent : convert key event to MIDI event.
	            Also emits the key message back to BBB, and log the full
	            key event if requested
*******************************************************************************/
static void ProcessKeyEvent(uint32_t const keyEvent, enum KeyLog_T const logFlag)
{
  uint32_t key = (keyEvent & IPC_KEYBUFFER_KEYMASK) + 36;
  MSG_KeyPosition(key);

  uint32_t time = M0_PERIOD_62_5NS * (keyEvent >> IPC_KEYBUFFER_TIME_SHIFT);  // us

  //--- Calc On velocity
  uint32_t vel;

  if (time <= 2500)  // clipping the low end: zero at a times <= 2.5 ms
    vel = velTable[0];
  else if (time >= ((1 << 19) + 2500))  // clipping at a maximum of 524 ms (2^19 us)
    vel = velTable[64];
  else
  {
    // first adjust the input to zero for a time of 2500us,
    // then lower 13 bits (0...8191) used for interpolation, upper 6 bits (0...63) used as index in the table
    uint32_t fract = (time - 2500) & 0x1FFF;
    uint32_t index = (time - 2500) >> 13;
    vel            = (velTable[index] * (8192 - fract) + velTable[index + 1] * fract) >> 13;  // ((0...16393) * 8192) / 8192
  }
  if (!(keyEvent & IPC_KEYBUFFER_NOTEON))  //--- releasing a key
  {
    MSG_KeyUp(vel);
    time = -time;  // negate time for key logging
  }
  else  //--- pressing a key
  {
    MSG_KeyDown(vel);
    ADC_WORK_WriteHWValueForUI(HW_SOURCE_ID_LAST_KEY, key);
  }
  if (logFlag)
  {
    uint16_t buffer[3];
    buffer[0] = key;
    buffer[1] = time & 0xFFFF;
    buffer[2] = time >> 16;
    BB_MSG_WriteMessage(LPC_BB_MSG_TYPE_KEY_EMUL, 3, buffer);
  }
}

/******************************************************************************
	@brief		POLY_ForceKey : emulate a key down/up event via software
*******************************************************************************/
void POLY_ForceKey(uint16_t const midiKeyNumber, uint16_t const timeLow, uint16_t const timeHigh)
{
  uint32_t forcedKeyEvent;
  int      time;

  forcedKeyEvent = midiKeyNumber - 36;
  time           = (int) (((uint32_t) timeHigh << 16) + (uint32_t) timeLow);
  if (time < 0)
    time = -time;
  else
    forcedKeyEvent |= IPC_KEYBUFFER_NOTEON;
  forcedKeyEvent |= (time / M0_PERIOD_62_5NS) << IPC_KEYBUFFER_TIME_SHIFT;
  ProcessKeyEvent(forcedKeyEvent, LOG_OFF);  // emulated key, never log it
  MSG_SendMidiBuffer();                      // note this call also takes care of sending other pending MIDI data like ActiveSensing
}

/******************************************************************************
	@brief		POLY_Process: reading new key events from the ring buffer
*******************************************************************************/
void POLY_Process(void)
{
  uint32_t i;
  uint32_t numKeyEvents = Emphase_IPC_M4_KeyBuffer_ReadBuffer(keyEvent, 32);  // reads the latest key up/down events from M0 ring buffer

  for (i = 0; i < numKeyEvents; i++)
    ProcessKeyEvent(keyEvent[i], logKeys);  // hardware key event, so send key log if requested
  MSG_SendMidiBuffer();                     // note this call also takes care of sending other pending MIDI data like ActiveSensing
}
