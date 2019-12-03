/******************************************************************************/
/** @file		nl_heartbeat.h
    @date		2019-11-18
    @brief    	Definitions for the HeartBeat mechanisms
    @example
    @ingroup  	nl_drv_modules
    @author		KSTR
*******************************************************************************/

#ifndef NL_HEARTBEAT_H_
#define NL_HEARTBEAT_H_

#include <stdint.h>

// # of milliseconds between calls to handler.
#define HBT_PROCESS_TIMESLICE (10)

void HBT_MidiReceive(uint8_t* buff, uint32_t len);
void HBT_Process(void);

#endif /* nl_heartbeat.h */
