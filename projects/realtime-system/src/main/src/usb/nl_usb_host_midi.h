/******************************************************************************/
/** @file		nl_usb_host_midi.h
    @date		2015-11-15
    @brief    	Driver for the MIDI USB host port on LPC43xx
    @author		Nemanja Nikodijevic 2015-11-15
*******************************************************************************/
#ifndef _NL_USB_HOST_MIDI_H_
#define _NL_USB_HOST_MIDI_H_

#include <stdint.h>
#include "midi/nl_midi.h"

uint8_t USBH_MIDI_Parse_Config(uint8_t* desc, uint16_t len);
void    USBH_MIDI_Prepare_Endpoints(void);
void    USBH_MIDI_Remove_Device(void);
void    USBH_MIDI_QH_Process(void);

void USBH_MIDI_Set_Callback(MIDI_Message_Callback msg_cb);

#endif
