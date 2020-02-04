/*
 * nl_bb_msg.h
 *
 *  Created on: 21.01.2015
 *      Author: ssc
 *	last changed: 2020-01-03  KSTR
 *	- SW_VERSION  203
 * */

#ifndef NL_DRV_NL_BB_MSG_H_
#define NL_DRV_NL_BB_MSG_H_

#include "stdint.h"

//===========================

// ==== V 205 ====
// fix for max. Velocity und max. HWSource, added keybed press sent to BBBB
// Velocity-Auflösung 14 Bit statt 12 Bit
// #define SW_VERSION 205

// ==== V 206 ====
// complete revised "pedals" (aka external controllers) proccessing
#define SW_VERSION 206

//===========================

// #define BB_MSG_TYPE_PRESET_DIRECT 0x0100  // direction: input; arguments(uint16): N, Nx data
// #define BB_MSG_TYPE_MORPH_SET_A   0x0200  // not used
// #define BB_MSG_TYPE_MORPH_SET_B   0x0300  // not used
#define BB_MSG_TYPE_PARAMETER    0x0400  // direction: output; arguments(uint16): 2, 1x parameter ID , 1x data
#define BB_MSG_TYPE_EDIT_CONTROL 0x0500  // not used
// #define BB_MSG_TYPE_MORPH_POS     0x0600  // not used
#define BB_MSG_TYPE_SETTING      0x0700  // direction: input;  argument (uint16): 2, 1x SETTING_ID_*, 1x data
#define BB_MSG_TYPE_NOTIFICATION 0x0800  // direction: output; arguments(uint16): 2, 1x type, 1x value
#define BB_MSG_TYPE_ASSERTION    0x0900  // direction: output; arguments(uint16): n (string)
#define BB_MSG_TYPE_REQUEST      0x0A00  // direction: input;  argument (uint16): 1, 1x REQUEST_ID_*
#define BB_MSG_TYPE_HEARTBEAT    0x0B00  // direction: output; arguments(uint16): 4, 4x uint16 (==uint64)
#define BB_MSG_TYPE_SENSORS_RAW  0x0E00  // direction: output; arguments(uint16): 13, sensor raw data (see nl_tcd_adc_work.c)
// TODO implement the following new types in BBBB/Playground also:
#define BB_MSG_TYPE_MUTESTATUS 0x0C00  // direction: output; argument (uint16): 1, 1x bit pattern
#define BB_MSG_TYPE_RIBBON_CAL 0x0D00  // direction: input; arguments(uint16): 134, 134x data [2x (33x 34x)]

//----- Setting Ids:

#define SETTING_ID_PLAY_MODE_UPPER_RIBBON_BEHAVIOUR 0  // ==> BIT 0 set if (returnMode == RETURN), ...
#define SETTING_ID_PLAY_MODE_LOWER_RIBBON_BEHAVIOUR 1  // ... BIT 1 set if (touchBehaviour == RELATIVE)

// #define SETTING_ID_NOTE_SHIFT 2  // ==> tTcdRange (-48, 48)

#define SETTING_ID_BASE_UNIT_UI_MODE 3  // ==> PLAY = 0, PARAMETER_EDIT = 1

#define SETTING_ID_EDIT_MODE_RIBBON_BEHAVIOUR 4  // ==> RELATIVE = 0, ABSOLUTE = 1

#define SETTING_ID_PEDAL_1_MODE 5  // ==> STAY = 0
#define SETTING_ID_PEDAL_2_MODE 6  // ... RETURN_TO_ZERO = 1
#define SETTING_ID_PEDAL_3_MODE 7  // ... RETURN_TO_CENTER = 2,
#define SETTING_ID_PEDAL_4_MODE 8

#define SETTING_ID_UPPER_RIBBON_REL_FACTOR 9   // ==> tTcdRange(256, 2560)
#define SETTING_ID_LOWER_RIBBON_REL_FACTOR 10  // ==> tTcdRange(256, 2560)

#define SETTING_ID_VELOCITY_CURVE 11  // ==> VERY_SOFT = 0, SOFT = 1, NORMAL = 2, HARD = 3, VERY_HARD = 4

// #define SETTING_ID_TRANSITION_TIME 12  // ==> tTcdRange(0, 16000)

#define SETTING_ID_PEDAL_1_TYPE 26  // ==> PotTipActive = 0
#define SETTING_ID_PEDAL_2_TYPE 27  // ... PotRingActive = 1
#define SETTING_ID_PEDAL_3_TYPE 28  // ... SwitchClosing = 2
#define SETTING_ID_PEDAL_4_TYPE 29  // ... SwitchOpening = 3

#define SETTING_ID_AFTERTOUCH_CURVE 30  // SOFT = 0, NORMAL = 1, HARD = 2
#define SETTING_ID_BENDER_CURVE     31  // SOFT = 0, NORMAL = 1, HARD = 2
// #define SETTING_ID_PITCHBEND_ON_PRESSED_KEYS 32  // OFF = 0, ON = 1
// #define SETTING_ID_EDIT_SMOOTHING_TIME       33  // ==> tTcdRange(0, 16000)
// #define SETTING_ID_PRESET_GLITCH_SUPPRESSION 34  // OFF = 0, ON = 1
// #define BENDER_RAMP_BYPASS                   35  // OFF = 0, ON = 1
#define SETTING_ID_SOFTWARE_MUTE_OVERRIDE 0xFF01  // Software Mute Override
#define SETTING_ID_SEND_RAW_SENSOR_DATA   0xFF02  // direction: input; arguments(uint16): 1, flag (!= 0)

//----- Request Ids:

#define REQUEST_ID_SW_VERSION    0x0000
#define REQUEST_ID_UNMUTE_STATUS 0x0001

//----- Notification Ids:

#define NOTIFICATION_ID_SW_VERSION    0x0000
#define NOTIFICATION_ID_UNMUTE_STATUS 0x0001

void BB_MSG_WriteMessage_DBG(uint16_t type, uint16_t length, uint16_t *data);
void BB_MSG_WriteMessage1Arg_DBG(uint16_t type, uint16_t arg);

int32_t BB_MSG_WriteMessage(uint16_t type, uint16_t length, uint16_t *data);
int32_t BB_MSG_WriteMessage2Arg(uint16_t type, uint16_t arg0, uint16_t arg1);
int32_t BB_MSG_WriteMessage1Arg(uint16_t type, uint16_t arg);
int32_t BB_MSG_WriteMessageNoArg(uint16_t type);

int32_t BB_MSG_SendTheBuffer(void);

void BB_MSG_ReceiveCallback(uint16_t type, uint16_t length, uint16_t *data);

#endif /* NL_DRV_NL_BB_MSG_H_ */
