/******************************************************************************/
/** @file		nl_usb_bb.h
    @date		2020-08-04
    @brief    	Control interface USB to/from PG (ePC) via MIDI
    @example
    @ingroup  	USB_BB
    @author		KSTR
*******************************************************************************/
#pragma once

#include <stdint.h>

#define USB_BB_BUFFER_SIZE 1024
#define USB_BB_BUFFER_NUM  2

typedef void (*MessageCallback)(uint16_t type, uint16_t length, uint16_t* data);

void     USB_BB_Init(MessageCallback msg_cb);
void     USB_BB_Reset(void);
uint32_t USB_BB_Send(uint8_t* buff, uint32_t len);
