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

#define LED_REDA    0
#define LED_REDB    1
#define LED_YELLOWA 2
#define LED_YELLOWB 3
#define LED_GREENA  4
#define LED_GREENB  5
#define LED_DUMMY   6  // will be discarded as out of range

// LED meanings, EV board, left to right
// persistent/latching
#define LED_CONNECTED     LED_GREENA
#define LED_TRAFFIC_STALL LED_YELLOWA
#define LED_DATA_LOSS     LED_REDA
// momentary flashers
#define LED_MIDI_TRAFFIC     LED_GREENB
#define LED_GENERAL_ACTIVITY LED_YELLOWB
#define LED_ERROR            LED_REDB

void DBG_Process(void);
void DBG_Led(uint8_t const ledId, uint8_t const on);
void DBG_Led_TimedOn(uint8_t const ledId, int16_t time);
