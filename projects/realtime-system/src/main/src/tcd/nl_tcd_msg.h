/******************************************************************************/
/** @file		nl_tcd_msg.h
    @date		2013-04-23
    @version	0.02
    @author		Stephan Schmitt [2012-06-15]
    @brief		<tbd>
*******************************************************************************/
#pragma once

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
void MSG_SendAEDevelopperCmd(uint32_t cmd);
void MSG_SendActiveSensing(void);
