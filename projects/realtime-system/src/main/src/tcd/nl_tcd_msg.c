/******************************************************************************/
/** @file		nl_tcd_msg.c
    @date		2013-04-23
    @version	0.02
    @author		Stephan Schmitt [2012-06-15]
    @brief		<tbd>
    @ingroup	nl_tcd_modules
*******************************************************************************/

#include "nl_tcd_msg.h"
#include "sys/nl_status.h"
#include "usb/nl_usb_midi.h"
#include "drv/nl_dbg.h"
#include "tcd/nl_tcd_adc_work.h"

#include "shared/lpc-defs.h"

/******************************************************************************/
/*	modul local defines														  */
/******************************************************************************/

#define BUFFER_SIZE 512

/******************************************************************************/
/*	modul local variables													  */
/******************************************************************************/

static uint8_t  buff[2][BUFFER_SIZE] = {};  // Two buffers for MIDI sending, alternately used
static uint32_t buf                  = 0;   // Counts the used bytes of the bulk buffer

static uint32_t writeBuffer = 0;

static uint8_t midiUSBConfigured = 0;
static uint8_t dropMessages      = 0;

void MSG_DropMidiMessages(uint8_t drop)
{
  dropMessages = drop;
}

/******************************************************************************/
/** @brief		A scheduler task function for regular checks of the USB
 * 				connection to the ePC.
*******************************************************************************/
void MSG_CheckUSB(void)  // every 200 ms
{
  if (USB_MIDI_IsConfigured())
  {
    if (midiUSBConfigured == 0)
    {
      // DBG_Led_Usb_Off();
      USB_MIDI_DropMessages(0);
    }

    midiUSBConfigured = 1;
  }
  else
  {
    if (midiUSBConfigured == 1)
    {
      // DBG_Led_Usb_On();
      USB_MIDI_DropMessages(1);
    }

    midiUSBConfigured = 0;
  }
}

/******************************************************************************/
/**	@brief  SendMidiBuffer - sends the USB bulk
*******************************************************************************/
void MSG_SendMidiBuffer(void)
{

  if (dropMessages)
  {
    buf = 0;  // discard all messages
    return;
  }
  if (buf)  // Anything to send
  {
#if 0
		if (USB_MIDI_Send(buff[writeBuffer], buf, 1) )	// Sending of the current writeBuffer and checking for success
#endif

    if ((USB_MIDI_BytesToSend() == 0)                   // Last transfer has finished
        && (USB_MIDI_Send(buff[writeBuffer], buf, 0)))  // Sending of the current writeBuffer and checking for success
    {
      writeBuffer = (writeBuffer + 1) & 0x1;  // Use the other buffer for next writing
    }
    else  // USB failure or "traffic jam"
    {
      DBG_Led_Error_TimedOn(10);
      if (NL_systemStatus.TCD_usbJams < 0xFFFF)
        NL_systemStatus.TCD_usbJams++;
    }

    buf = 0;  // this discard the messages in case of error, no re-send attempts
  }
}

/*****************************************************************************
*	@brief  MSG_KeyPosition
*	Sends the index of the selected key before sending the On/Off velocity
*   @param  key: 36...96 (C3 = 60)
******************************************************************************/
void MSG_KeyPosition(uint32_t key)
{
  if ((key < 36) || (key > 96))
  {
    return;  /// assertion
  }

  buff[writeBuffer][buf++] = AE_TCD_WRAPPER;
  buff[writeBuffer][buf++] = AE_TCD_KEY_POS;
  buff[writeBuffer][buf++] = key >> 7;    // first 7 bits
  buff[writeBuffer][buf++] = key & 0x7F;  // second 7 bits

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
}

/*****************************************************************************
*	@brief  MSG_KeyDown
*	Sends the Down (On) velocity and starts a note
*   @param  vel: velocity value 0...16383
******************************************************************************/
void MSG_KeyDown(uint32_t vel)
{
  buff[writeBuffer][buf++] = AE_TCD_WRAPPER;
  buff[writeBuffer][buf++] = AE_TCD_KEY_DOWN;
  buff[writeBuffer][buf++] = vel >> 7;    // first 7 bits
  buff[writeBuffer][buf++] = vel & 0x7F;  // second 7 bits

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
}

/*****************************************************************************
*	@brief  MSG_KeyUp
*	Sends the Up (Off) velocity and stops a note
*   @param  vel: velocity value 0...16383
******************************************************************************/
void MSG_KeyUp(uint32_t vel)
{
  buff[writeBuffer][buf++] = AE_TCD_WRAPPER;
  buff[writeBuffer][buf++] = AE_TCD_KEY_UP;
  buff[writeBuffer][buf++] = vel >> 7;    // first 7 bits
  buff[writeBuffer][buf++] = vel & 0x7F;  // second 7 bits

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
}

/*****************************************************************************
*   @brief	MSG_HWSourceUpdate
*   Sends the new position, when a hardware source has been moved
*   @param  source: 0...11  identifies the hardware source
*   @param  position: 0...16000
******************************************************************************/
void MSG_HWSourceUpdate(uint32_t source, uint32_t position)
{
  if ((source >= NUM_HW_REAL_SOURCES) || (position > 16000))
  {
    return;  /// assertion
  }

  buff[writeBuffer][buf++] = AE_TCD_WRAPPER;
  buff[writeBuffer][buf++] = AE_TCD_HW_POS | source;
  buff[writeBuffer][buf++] = position >> 7;    // first 7 bits
  buff[writeBuffer][buf++] = position & 0x7F;  // second 7 bits

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
}

/*****************************************************************************
*   @brief	MSG_SendAEDeveloperCmd
*   Sends a special command to audio engine
*   @param  cmd : command, 14bit (1:testtone OFF; 2:testtone ON; 3:default sound)
******************************************************************************/
void MSG_SendAEDevelopperCmd(uint32_t cmd)
{
  buff[writeBuffer][buf++] = AE_TCD_WRAPPER;
  buff[writeBuffer][buf++] = AE_TCD_DEVELOPPER_CMD;
  buff[writeBuffer][buf++] = cmd >> 7;    // first 7 bits
  buff[writeBuffer][buf++] = cmd & 0x7F;  // second 7 bits

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
}

/*****************************************************************************
*   @brief	MSG_SendActiveSensing
*   Sends an "Active Sensing" MIDI command to AE, to indicate LPC is up and running
******************************************************************************/
void MSG_SendActiveSensing(void)
{
#ifdef __IMPLEMENT_ACTIVE_SENSING
  buff[writeBuffer][buf++] = 0x0F;  // cable 0, packet type "single byte"
  buff[writeBuffer][buf++] = 0xFE;  // MIDI real-time command "active sensing"
  buff[writeBuffer][buf++] = 0x00;
  buff[writeBuffer][buf++] = 0x00;

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
#endif
}
