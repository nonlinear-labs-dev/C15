/******************************************************************************/
/** @file		nl_heartbeat.c
    @date		2019-11-18
    @brief    	The HeartBeat mechanisms
    @example
    @ingroup  	nl_drv_modules
    @author		KSTR/hhoegelo
*******************************************************************************/

#include "heartbeat/nl_heartbeat.h"
#include "spibb/nl_bb_msg.h"
#include "sup/nl_sup.h"
#include "drv/nl_dbg.h"
#include "tcd/nl_tcd_msg.h"

static uint64_t audio_engine_heartbeat = 0;  // accumulator for collect proccess
static uint64_t heartbeat              = 0;  // value to transmit
static uint8_t  heartbeat_update       = 0;  // flag for pending transmit
static uint8_t  step                   = 0;  // collect process step chain

static uint8_t  traffic_update = 0;  // flag for incoming traffic
static uint64_t lpc_heartbeat  = 0;  // our own value
static int      any_traffic    = 0;

/******************************************************************************/
/** @brief		process incoming Midi data, interrupt(!!) callback for "USB_MIDI_Config()"
    @param[in]	buff	Pointer to data buffer
    @param[in]	len		Amount of bytes in buffer
*******************************************************************************/
void HBT_MidiReceive(uint8_t *buff, uint32_t len)
{
  traffic_update = 1;

  if (!any_traffic)
    MSG_DropMidiMessages(0);
  any_traffic = 1;

  // An USB midi frame is always 4 bytes
  // The first byte is the header which identifies the message type
  for (; len >= 4; len -= 4, buff += 4)  // repeat until no commands left
  {
    if (buff[0] != 0x0A)  // not an Aftertouch message ?
      continue;           //  then ignore
    switch (step)
    {
      // wait for lowest 14 bits
      case 0:
        if (buff[1] == 0xA0)  // marker found, Aftertouch channel 0
        {
          audio_engine_heartbeat = ((uint64_t) buff[3] << (7 * 0)) | ((uint64_t) buff[2] << (7 * 1));
          step                   = 1;
        }
        break;
      // wait for middle 14 bits
      case 1:
        if (buff[1] == 0xA1)  // marker found, Aftertouch channel 1
        {
          audio_engine_heartbeat |= ((uint64_t) buff[3] << (7 * 2)) | ((uint64_t) buff[2] << (7 * 3));
          step = 2;
        }
        else
          step = 0;  // reset step chain if command sequence is interrupted
        break;
      // wait for highest 14 bits and set new heartbeat value
      case 2:
        if ((buff[1] == 0xA2) && !heartbeat_update)  // marker found (AT channel 2) and no pending transmits
        {
          audio_engine_heartbeat |= ((uint64_t) buff[3] << (7 * 4)) | ((uint64_t) buff[2] << (7 * 5));
          heartbeat = audio_engine_heartbeat + lpc_heartbeat;
          lpc_heartbeat++;
          heartbeat_update = 1;
        }
        step = 0;  // reset step chain
        break;
    }  // switch
  }    // for
}

/******************************************************************************/
/** @brief		periodically called every 10ms, to process pending updates
*******************************************************************************/
void HBT_Process(void)
{
  if (traffic_update)
  {
    SUP_MidiTrafficDetected();
    MSG_SendActiveSensing();  // acknowledge any(!) incoming traffic with an "ActiveSensing" MIDI command
    traffic_update = 0;
  }

  if (heartbeat_update)
  {
    BB_MSG_WriteMessage(LPC_BB_MSG_TYPE_HEARTBEAT, 4, (uint16_t *) &heartbeat);
    BB_MSG_SendTheBuffer();
    heartbeat_update = 0;
  }
}

void HBT_ResetCounter(void)
{
  lpc_heartbeat = 0;
}

// EOF
