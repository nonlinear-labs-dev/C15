/******************************************************************************/
/** @file		nl_midi.h
    @date		2015-09-28
    @brief    	MIDI framework
    @author		Nemanja Nikodijevic 2015-09-28
*******************************************************************************/
#ifndef _NL_MIDI_H_
#define _NL_MIDI_H_
#include "cmsis/LPC43xx.h"

#define MIDI_CHANNEL(x) ((x & 0x0F))

#define MIDI_MSG_TYPE(x) ((x & 0xF0))
#define MIDI_NOTE_OFF    0x80
#define MIDI_NOTE_ON     0x90
#define MIDI_AFTERTOUCH  0xA0
#define MIDI_CTRL_CHANGE 0xB0
#define MIDI_PITCH_BEND  0xE0

typedef struct MIDI_msg
{
  uint8_t m_status;
  uint8_t m_data0;
  uint8_t m_data1;
} MIDI_Message;

typedef void (*MIDI_Message_Callback)(MIDI_Message *);

void  MIDI_UART_Init(LPC_USARTn_Type *UARTx, MIDI_Message_Callback midi_cb);
float MIDI_Get_Note_Freq(uint8_t note);

#endif
