/******************************************************************************/
/** @file		nl_usb_midi.h
    @date		2014-12-11
    @brief    	Definitions for the USB-MIDI driver
    @example
    @ingroup  	nl_drv_modules
    @author		Nemanja Nikodijevic 2014-12-11
*******************************************************************************/
#pragma once

#include "nl_usbd.h"

/** USB MIDI driver configuration block
 * @{
 */
/** MIDI buffer size */
#define USB_MIDI_BUFFER_SIZE 1024
/** @} */

/* Definition for Midi Receive Callback function */
typedef void (*MidiRcvCallback)(uint8_t* buff, uint32_t len);

/* USB MIDI functions */
void USBA_MIDI_Init(void);
void USBA_MIDI_DeInit(void);
void USBA_MIDI_Config(MidiRcvCallback midircv);
void USBA_MIDI_Poll(void);

uint32_t USBA_MIDI_IsConfigured(void);
uint32_t USBA_MIDI_Send(uint8_t const* const buff, uint32_t const cnt, uint8_t const imm);
uint32_t USBA_MIDI_SendDelayed(uint8_t* buff, uint32_t cnt);
uint32_t USBA_MIDI_CheckBuffer(void);
uint32_t USBA_MIDI_BytesToSend(void);
void     USBA_MIDI_DropMessages(uint8_t drop);
