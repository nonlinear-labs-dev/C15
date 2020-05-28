/******************************************************************************/
/** @file		nl_sup.h
    @date		2019-10-20
    @brief    		Communication to Supervisor uC
    @author		KSTR
*******************************************************************************/
#pragma once

#include <stdint.h>

#include "shared/lpc-defs.h"

// # of milliseconds between calls to handler.
// DO NOT CHANGE without need, and check back corresponding Supervisor Code!!
#define SUP_PROCESS_TIMESLICE (10)

// time in milliseconds without incoming midi traffic to raise "audio engine offline"
#define TRAFFIC_TIMEOUT (120)
void SUP_Init(void);
void SUP_Process(void);
void SUP_MidiTrafficDetected(void);

void     SUP_SetMuteOverride(uint32_t mode);
uint16_t SUP_GetUnmuteStatusBits(void);
// bit masks for these functions :
// (for SUP_SetMuteOverride() only *_SOFTWARE_* masks are used/allowed)
