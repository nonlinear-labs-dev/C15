/******************************************************************************/
/** @file		nl_usb_midi.h
    @date		2014-12-11
    @brief    	Definitions for the USB-MIDI driver
    @example
    @ingroup  	nl_drv_modules
    @author		Nemanja Nikodijevic 2014-12-11
*******************************************************************************/

#ifndef NL_USB_MIDI_H_
#define NL_USB_MIDI_H_

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
void USB_MIDI_Init(void);
void USB_MIDI_Config(MidiRcvCallback midircv);
void USB_MIDI_Poll(void);

uint32_t USB_MIDI_IsConfigured(void);
uint32_t USB_MIDI_Send(uint8_t* buff, uint32_t cnt, uint8_t imm);
uint32_t USB_MIDI_SendDelayed(uint8_t* buff, uint32_t cnt);
uint32_t USB_MIDI_CheckBuffer(void);
uint32_t USB_MIDI_BytesToSend(void);
void     USB_MIDI_DropMessages(uint8_t drop);
#endif /* NL_USB_MIDI_H_ */
