/******************************************************************************/
/** @file		nl_tcd_msg.c
    @date		2013-04-23
    @version	0.02
    @author		Stephan Schmitt [2012-06-15]
    @brief		<tbd>
    @ingroup	nl_tcd_modules
*******************************************************************************/

#include "nl_tcd_msg.h"

#include "sys/nl_coos.h"

#include "usb/nl_usb_midi.h"
#include "drv/nl_dbg.h"

#include "nl_tcd_valloc.h"

/******************************************************************************/
/*	modul local defines														  */
/******************************************************************************/

#define BUFFER_SIZE 512

/******************************************************************************/
/*	modul local variables													  */
/******************************************************************************/

static int32_t oldParameter   = -1;
static int32_t multipleParams = 0;

static uint8_t  buff[2][BUFFER_SIZE] = {};  // Two buffers for MIDI sending, alternately used
static uint32_t buf                  = 0;   // Counts the used bytes of the bulk buffer

static uint32_t writeBuffer = 0;

static uint8_t midiUSBConfigured = 0;

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
  if (buf)  // Anything to send
  {
#if 0
		if (USB_MIDI_Send(buff[writeBuffer], buf, 1) )	// Sending of the current writeBuffer and checking for success
#endif

    if ((USB_MIDI_BytesToSend() == 0)                   // Last transfer has finished
        && (USB_MIDI_Send(buff[writeBuffer], buf, 0)))  // Sending of the current writeBuffer and checking for success
    {
      writeBuffer = !writeBuffer;  // Use the other buffer for next writing	/// better use [0] and [1], or +1  modulo_2
    }
    else  // USB failure or "traffic jam"
    {
      DBG_Led_Error_On();  // Turn on ERROR LED for USB send problem
      COOS_Task_Add(DBG_Led_Error_Off, 1000 * 8, 0);
    }

    buf = 0;  /// Achtung - damit gibt es beim Scheitern keinen zweiten Sendeversuch !!! Wir beobachten die Error-LED
  }
}

/*****************************************************************************
*	@brief  MSG_SelectParameter
*   @param  p: Id (14 bits) of a single parameter or the first of multiple
*   parameters
******************************************************************************/

void MSG_SelectParameter(uint32_t p)
{
  if ((multipleParams == 1) || (p != oldParameter))  // avoid re-selecting the same single parameter
  {
    if (p > 0x3FFE)
    {
      p = 0x3FFE;  // clip to 14 bits (0x3FFF = All)
    }

    buff[writeBuffer][buf++] = 0x08;      // MIDI status 8
    buff[writeBuffer][buf++] = 0x81;      // MIDI status 8, MIDI channel 1  (P)
    buff[writeBuffer][buf++] = p >> 7;    // first 7 bits
    buff[writeBuffer][buf++] = p & 0x7F;  // second 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }

    oldParameter = p;

    multipleParams = 0;
  }
}

/*****************************************************************************
*	@brief  MSG_SelectMultipleParameters
*   @param  p_last: Id (14 bits) of the last of multiple parameters
******************************************************************************/

void MSG_SelectMultipleParameters(uint32_t p_last)
{
  if (p_last != oldParameter)  // no group of one parameter or redundant selection of last voice
  {
    if (p_last > 0x3FFE)
    {
      p_last = 0x3FFE;  // clip to 14 bits (0x3FFF = All)
    }

    buff[writeBuffer][buf++] = 0x09;           // MIDI status 9
    buff[writeBuffer][buf++] = 0x91;           // MIDI status 9, MIDI channel 1  (PM)
    buff[writeBuffer][buf++] = p_last >> 7;    // first 7 bits
    buff[writeBuffer][buf++] = p_last & 0x7F;  // second 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }

    multipleParams = 1;
  }
}

/*****************************************************************************
*	@brief  MSG_SetTime
*   @param  t: linear time (14 or 28 bits) as number of samples
******************************************************************************/

void MSG_SetTime(uint32_t t)
{
  if (t > 0x3FFF)  // needs 28-bit format
  {
    if (t > 0xFFFFFFF)
    {
      t = 0xFFFFFFF;  // clip to 28 bits
    }

    buff[writeBuffer][buf++] = 0x0A;              // MIDI status A
    buff[writeBuffer][buf++] = 0xA2;              // MIDI status A, MIDI channel 2  (TU)
    buff[writeBuffer][buf++] = (t >> 21);         // first 7 bits
    buff[writeBuffer][buf++] = (t >> 14) & 0x7F;  // second 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }

    buff[writeBuffer][buf++] = 0x0B;             // MIDI status B
    buff[writeBuffer][buf++] = 0xB2;             // MIDI status B, MIDI channel 2  (TL)
    buff[writeBuffer][buf++] = (t >> 7) & 0x7F;  // third 7 bits
    buff[writeBuffer][buf++] = t & 0x7F;         // fourth 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }
  }
  else  // 14-bit format is enough
  {
    buff[writeBuffer][buf++] = 0x08;      // MIDI status 8
    buff[writeBuffer][buf++] = 0x82;      // MIDI status 8, MIDI channel 2  (T)
    buff[writeBuffer][buf++] = t >> 7;    // first 7 bits
    buff[writeBuffer][buf++] = t & 0x7F;  // second 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }
  }
}

/*****************************************************************************
*	@brief  MSG_SetDestinationSigned - used for signed values, which will be
*	packed into (1 + 13) bit (DS) or (1 + 27) bit (DU, DL) messages
*   @param  d: destination value (may be negative)
******************************************************************************/

void MSG_SetDestinationSigned(int32_t d)
{
  uint32_t sign = 0x00;

  if (d < 0)
  {
    d    = -d;    // absolute value
    sign = 0x40;  // first of seven bits
  }

  if (d > 0x1FFF)  // absolute value does not fit into 13 bits - we use 27 bits
  {
    if (d > 0x7FFFFFF)
    {
      d = 0x7FFFFFF;  // clip to 27-bit range
    }

    buff[writeBuffer][buf++] = 0x0A;              // MIDI status A
    buff[writeBuffer][buf++] = 0xA5;              // MIDI status A, MIDI channel 5  (DU)
    buff[writeBuffer][buf++] = (d >> 21) | sign;  // first 7 bits (sign and upper 6 bits of absolute value)
    buff[writeBuffer][buf++] = (d >> 14) & 0x7F;  // second 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }

    buff[writeBuffer][buf++] = 0x0B;             // MIDI status B
    buff[writeBuffer][buf++] = 0xB5;             // MIDI status B, MIDI channel 5  (DL)
    buff[writeBuffer][buf++] = (d >> 7) & 0x7F;  // third 7 bits
    buff[writeBuffer][buf++] = d & 0x7F;         // fourth 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }
  }
  else  // (1+13)-bit format is enough
  {
    buff[writeBuffer][buf++] = 0x09;             // MIDI status 9
    buff[writeBuffer][buf++] = 0x95;             // MIDI status 9, MIDI channel 5  (DS)
    buff[writeBuffer][buf++] = (d >> 7) | sign;  // first 7 bits (sign and upper 6 bits of absolute value)
    buff[writeBuffer][buf++] = d & 0x7F;         // second 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }
  }
}

/*****************************************************************************
*	@brief  MSG_SetDestination
*	Used for unsigned values. They are packed into 14-bit (D) or 27-bit (DU, DL) messages
*   @param  d: destination value (positive values only)
******************************************************************************/

void MSG_SetDestination(uint32_t d)
{
  if (d > 0x3FFF)  // value does not fit into 14 bits - we use 27 bits
  {
    if (d > 0x7FFFFFF)
    {
      d = 0x7FFFFFF;  // clip to 27-bit range (setting the sign bit to zero)
    }

    buff[writeBuffer][buf++] = 0x0A;              // MIDI status A
    buff[writeBuffer][buf++] = 0xA5;              // MIDI status A, MIDI channel 5  (DU)
    buff[writeBuffer][buf++] = (d >> 21);         // first 7 bits (sign is zero)
    buff[writeBuffer][buf++] = (d >> 14) & 0x7F;  // second 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }

    buff[writeBuffer][buf++] = 0x0B;             // MIDI status B
    buff[writeBuffer][buf++] = 0xB5;             // MIDI status B, MIDI channel 5  (DL)
    buff[writeBuffer][buf++] = (d >> 7) & 0x7F;  // third 7 bits
    buff[writeBuffer][buf++] = d & 0x7F;         // fourth 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }
  }
  else  // 14-bit format is enough
  {
    buff[writeBuffer][buf++] = 0x08;      // MIDI status 8
    buff[writeBuffer][buf++] = 0x85;      // MIDI status 8, MIDI channel 5  (D)
    buff[writeBuffer][buf++] = d >> 7;    // first 7 bits
    buff[writeBuffer][buf++] = d & 0x7F;  // second 7 bits

    if (buf == BUFFER_SIZE)
    {
      MSG_SendMidiBuffer();
    }
  }
}

/*****************************************************************************
*	@brief  MSG_KeyVoice
*	Used to transfer the index of the allocated voice
*   @param  steal: (0: no, 1: yes), voice: index (0...19, 13 bits available)
******************************************************************************/

void MSG_KeyVoice(uint32_t steal, uint32_t voice)
{
  uint32_t keyVoice = (steal ? 1 : 0) + (voice << 1);

  buff[writeBuffer][buf++] = 0x0B;             // MIDI status B
  buff[writeBuffer][buf++] = 0xB7;             // MIDI status B, MIDI channel 7  (KV)
  buff[writeBuffer][buf++] = keyVoice >> 7;    // first 7 bits
  buff[writeBuffer][buf++] = keyVoice & 0x7F;  // second 7 bits

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
}

/*****************************************************************************
*	@brief  MSG_KeyDown
*	Used to start a note
*   @param  v: velocity value (0...4095, 14 bits would be possible)
******************************************************************************/

void MSG_KeyDown(uint32_t vel)
{
  buff[writeBuffer][buf++] = 0x09;        // MIDI status 9
  buff[writeBuffer][buf++] = 0x97;        // MIDI status 9, MIDI channel 7  (KD)
  buff[writeBuffer][buf++] = vel >> 7;    // first 7 bits
  buff[writeBuffer][buf++] = vel & 0x7F;  // second 7 bits

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
}

/*****************************************************************************
*	@brief  MSG_KeyUp
*	Used to start the release phase of a note
*   @param  v: velocity value (0...4095, 14 bits would be possible)
******************************************************************************/

void MSG_KeyUp(uint32_t vel)
{
  buff[writeBuffer][buf++] = 0x08;        // MIDI status 8
  buff[writeBuffer][buf++] = 0x87;        // MIDI status 8, MIDI channel 7  (KU)
  buff[writeBuffer][buf++] = vel >> 7;    // first 7 bits
  buff[writeBuffer][buf++] = vel & 0x7F;  // second 7 bits

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
}

/*****************************************************************************
*	@brief  PreloadMode - For internal use with MSG_ commands below
*   @param  m: (14 bits) Id of the selected list (upper 7 bits)
*   			0 - no list
*   			1 - list of parameters (for preset loading)
*   			2 - list of Key Down, Key Up parameters, separated by D (paramters) and KD, KU (voices)
*              and selected mode (lower 7 bits)
*   			0 - disable
*   			1 - enable
*   			2 - apply
******************************************************************************/

void PreloadMode(uint32_t m)
{
  buff[writeBuffer][buf++] = 0x0A;      // MIDI status A
  buff[writeBuffer][buf++] = 0xAF;      // MIDI status A, MIDI channel F  (PL)
  buff[writeBuffer][buf++] = m >> 7;    // first 7 bits
  buff[writeBuffer][buf++] = m & 0x7F;  // second 7 bits

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
}

/*****************************************************************************
* @brief	MSG_EnablePreload - enables the Preload mode: the transition will
* start when the Apply message arrives
******************************************************************************/

void MSG_EnablePreload(void)
{
  PreloadMode(1);
}

/*****************************************************************************
* @brief	MSG_ApplyPreloadedValues - the rendering cells are set to the
* preloaded D values and the transitions are started
******************************************************************************/

void MSG_ApplyPreloadedValues(void)
{
  PreloadMode(2);
}

/*****************************************************************************
* @brief	MSG_Reset - triggers different type of audio resets
*           mode = 0: flush,  mode = 1: env stop,  mode = 2: dsp reset (panic)
******************************************************************************/

void MSG_Reset(uint32_t mode)
{
  buff[writeBuffer][buf++] = 0x0A;         // MIDI status A
  buff[writeBuffer][buf++] = 0xA7;         // MIDI status A, MIDI channel 7  (RST)
  buff[writeBuffer][buf++] = mode >> 7;    // first 7 bits
  buff[writeBuffer][buf++] = mode & 0x7F;  // second 7 bits

  if (buf == BUFFER_SIZE)
  {
    MSG_SendMidiBuffer();
  }
}
