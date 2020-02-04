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

/******************************************************************************
*	public functions
******************************************************************************/

void MSG_CheckUSB(void);
void MSG_SendMidiBuffer(void);
void MSG_DropMidiMessages(uint8_t drop);

void MSG_KeyPosition(uint32_t key);
void MSG_KeyDown(uint32_t vel);
void MSG_KeyUp(uint32_t vel);

void MSG_HWSourceUpdate(uint32_t source, uint32_t position);

#endif /* NL_TCD_MSG_H_ */
