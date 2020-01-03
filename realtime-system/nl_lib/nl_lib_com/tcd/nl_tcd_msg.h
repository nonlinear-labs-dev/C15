/******************************************************************************/
/** @file		nl_tcd_msg.h
    @date		2013-04-23
    @version	0.02
    @author		Stephan Schmitt [2012-06-15]
    @brief		<tbd>
*******************************************************************************/

#ifndef NL_TCD_MSG_H_
#define NL_TCD_MSG_H_

#include "stdint.h"

/******************************************************************************
*	defines
******************************************************************************/

#define CURVE_TYPE_LIN         0
#define CURVE_TYPE_EXP         1
#define CURVE_TYPE_CLIPPED_EXP 2
#define CURVE_TYPE_POLYNOM     3

#define SEGMENT_PRIO_APPEND_OPEN   0
#define SEGMENT_PRIO_APPEND_CLOSED 128
#define SEGMENT_PRIO_INSERT_OPEN   256
#define SEGMENT_PRIO_INSERT_CLOSED 384

/******************************************************************************
*	public functions
******************************************************************************/

void MSG_CheckUSB(void);
void MSG_SendMidiBuffer(void);

void MSG_SelectParameter(uint32_t p);
void MSG_SelectMultipleParameters(uint32_t p_last);

void MSG_SetTime(uint32_t t);

void MSG_SetDestination(uint32_t d);
void MSG_SetDestinationSigned(int32_t d);

void MSG_KeyVoice(uint32_t steal, uint32_t voice);
void MSG_KeyDown(uint32_t vel);
void MSG_KeyUp(uint32_t vel);

void MSG_Reset(uint32_t mode);

void MSG_EnablePreload(void);         // enables the Preload mode: the transition will start when the Apply message arrives
void MSG_ApplyPreloadedValues(void);  // applies preloaded D values and starts the transition(s)

#endif /* NL_TCD_MSG_H_ */
