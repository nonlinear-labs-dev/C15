/******************************************************************************/
/** @file		nl_dbg.h
    @date		2013-03-08
    @version	0.01
    @author		Daniel Tzschentke [2012--]
    @brief		This module is a debug interface
 	 	 	 	- LEDs
 	 	 	 	- buttons
 	 	 	 	- logic-analyzer IOs
    @ingroup	e2_mainboard
*******************************************************************************/
#pragma once

#include <stdint.h>

#define REDA    0
#define REDB    1
#define YELLOWA 2
#define YELLOWB 3
#define GREENA  4
#define GREENB  5

void DBG_Process(void);
void DBG_Led_TimedOn(uint8_t const ledId, int16_t time);
