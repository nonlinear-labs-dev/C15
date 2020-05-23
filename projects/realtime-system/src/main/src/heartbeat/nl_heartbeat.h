/******************************************************************************/
/** @file		nl_heartbeat.h
    @date		2019-11-18
    @brief    	Definitions for the HeartBeat mechanisms
    @example
    @ingroup  	nl_drv_modules
    @author		KSTR
*******************************************************************************/
#pragma once

#include <stdint.h>

// # of milliseconds between calls to handler.
#define HBT_PROCESS_TIMESLICE (10)

void HBT_MidiReceive(uint8_t* buff, uint32_t len);
void HBT_Process(void);
void HBT_ResetCounter(void);
