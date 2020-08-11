/******************************************************************************/
/** @file		nl_tcd_msg.c
    @date		2020-06-19 KSTR
    @version	1.0
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
#include "shared/globals.h"

/******************************************************************************/
/*	module local defines														  */
/******************************************************************************/

#define BUFFER_SIZE            (256)  // space for 256 / 4 = 64 TCD events per buffer
#define NUMBER_OF_BUFFERS      (8)    // number of buffers, must be 2^N !
#define NUMBER_OF_BUFFERS_MASK (NUMBER_OF_BUFFERS - 1)

/******************************************************************************/
/*	modul local variables													  */
/******************************************************************************/

// ringbuffer of buffers
static uint8_t  buff[NUMBER_OF_BUFFERS][BUFFER_SIZE];
static uint16_t bufHead      = 0;  // 0..(NUMBER_OF_BUFFERS-1)
static uint16_t bufTail      = 0;  // 0..(NUMBER_OF_BUFFERS-1)
static uint16_t bufHeadIndex = 0;  // used bytes in the current head (front) buffer

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
      USB_MIDI_DropMessages(0);
    midiUSBConfigured = 1;
  }
  else
  {
    if (midiUSBConfigured == 1)
      USB_MIDI_DropMessages(1);
    midiUSBConfigured = 0;
  }
}

// -----------------------------------------------
static void LogError(void)
{
  DBG_Led_Error_TimedOn(10);
  if (NL_systemStatus.TCD_usbJams < 0xFFFF)
    NL_systemStatus.TCD_usbJams++;
}

// -----------------------------------------------
static inline void FillBufferWithOneRawMIDIpacket(uint8_t const b1, uint8_t const b2, uint8_t const b3, uint8_t const b4)
{
  if (bufHeadIndex >= BUFFER_SIZE)
  {  // current head is full, switch to next one
    uint16_t tmpBufHead = (bufHead + 1) & NUMBER_OF_BUFFERS_MASK;
    if (tmpBufHead == bufTail)  // no space left ?
    {
      LogError();  // ring of buffers is full
      return;      // discard incoming data
    }
    bufHead      = tmpBufHead;
    bufHeadIndex = 0;
  }
  buff[bufHead][bufHeadIndex++] = b1;
  buff[bufHead][bufHeadIndex++] = b2;
  buff[bufHead][bufHeadIndex++] = b3;
  buff[bufHead][bufHeadIndex++] = b4;
}

// -----------------------------------------------
static inline void FillBufferWithTCDdata(uint8_t const b1, uint8_t const b2, uint16_t const w)
{
  FillBufferWithOneRawMIDIpacket(b1, b2, w >> 7, w & 0x7F);
}

// -----------------------------------------------
static inline uint8_t GetNext7bits(uint8_t *buffer, uint16_t *bitNo, uint16_t const totalBits)
{
  uint16_t startIndex = (*bitNo) / 8;
  uint16_t startBit   = (*bitNo) % 8;
  uint16_t stopBit    = (*bitNo+6) % 8;
  (*bitNo) += 7;

  if (startBit <= 1 )  // all bits in current byte
    return (buffer[startIndex] >> (1 - startBit)) & 0x7F;

  uint16_t out;
  if ((*bitNo) < totalBits)
     out = ((uint16_t)(buffer[startIndex]) << 8) | buffer[startIndex + 1];
  else
    out = (uint16_t)(buffer[startIndex]) << 8;
  out >>= (7 - stopBit);
  return out & 0x7F;
}

// -----------------------------------------------
#define CABLE_NUMBER (0 << 4)
#define SYSEX_START  (0x04)
#define SYSEX_LAST3  (0x07)
#define SYSEX_LAST2  (0x06)
#define SYSEX_LAST1  (0x05)
void PutRaw(uint8_t data, int8_t last)
{
  static uint8_t byteCntr = 0;
  static uint8_t midiPacket[3];

  midiPacket[byteCntr++] = data;
  if (last)  // last data byte
  {
    if (last > 0)
      switch (byteCntr)
      {
        case 1:  // one byte
          FillBufferWithOneRawMIDIpacket(CABLE_NUMBER | SYSEX_LAST1, midiPacket[0], 0, 0);
          break;
        case 2:  // two byte
          FillBufferWithOneRawMIDIpacket(CABLE_NUMBER | SYSEX_LAST2, midiPacket[0], midiPacket[1], 0);
          break;
        case 3:  // three byte
          FillBufferWithOneRawMIDIpacket(CABLE_NUMBER | SYSEX_LAST3, midiPacket[0], midiPacket[1], midiPacket[2]);
          break;
      }
    byteCntr = 0;
    return;
  }

  if (byteCntr >= 3)  // packet is full and it is not the last packet
  {
    FillBufferWithOneRawMIDIpacket(CABLE_NUMBER | SYSEX_START, midiPacket[0], midiPacket[1], midiPacket[2]);
    byteCntr = 0;  // next packet
  }
}

// -----------------------------------------------
void MSG_FillBufferWithSysExData(void *buffer, uint16_t len)
{
#define SYSEX_START_CMD (0xF0)
#define SYSEX_END_CMD   (0xF7)
  if (!len)
    return;
  uint16_t rawLen = (len * 8 + 6) / 7;  // raw amount of 7bit bytes
  PutRaw(SYSEX_START_CMD, 0);
  uint16_t BitNo = 0;
  while (rawLen--)
    PutRaw(GetNext7bits(buffer, &BitNo, 8 * len), 0);
  PutRaw(SYSEX_END_CMD, 1);
}

/******************************************************************************/
/**	@brief  SendMidiBuffer - as USB bulk
*******************************************************************************/
void MSG_Process(void)
{
  if (dropMessages)
  {  // discard all messages
    bufHead      = 0;
    bufTail      = 0;
    bufHeadIndex = 0;
    return;
  }

  if (bufHead == bufTail && bufHeadIndex == 0)
    return;  // nothing to send

  if (USB_MIDI_BytesToSend() != 0)
    return;  // last transfer still in progress

  if (bufHead != bufTail)
  {  // send stashed buffers first
    if (!USB_MIDI_Send(buff[bufTail], BUFFER_SIZE))
      LogError();  // send failed
    bufTail = (bufTail + 1) & NUMBER_OF_BUFFERS_MASK;
    return;
  }
  if (bufHeadIndex != 0)
  {  // send current buffer
    if (!USB_MIDI_Send(buff[bufHead], bufHeadIndex))
      LogError();  // send failed
    bufHead      = (bufHead + 1) & NUMBER_OF_BUFFERS_MASK;
    bufHeadIndex = 0;
    bufTail      = bufHead;
  }
}

#if LPC_KEYBED_DIAG
int16_t         TCD_keyOnOffCntr[128];
static uint16_t keyOnOffIndex;
#endif

/*****************************************************************************
*	@brief  MSG_KeyPosition
*	Sends the index of the selected key before sending the On/Off velocity
*   @param  key: 36...96 (C3 = 60)
******************************************************************************/
void MSG_KeyPosition(uint32_t key)
{
  if ((key < 36) || (key > 96))
    return;  /// assertion
  FillBufferWithTCDdata(AE_TCD_WRAPPER, AE_TCD_KEY_POS, key);
#if LPC_KEYBED_DIAG
  keyOnOffIndex = key;
#endif
}

/*****************************************************************************
*	@brief  MSG_KeyDown
*	Sends the Down (On) velocity and starts a note
*   @param  vel: velocity value 0...16383
******************************************************************************/
void MSG_KeyDown(uint32_t vel)
{
  FillBufferWithTCDdata(AE_TCD_WRAPPER, AE_TCD_KEY_DOWN, vel);
#if LPC_KEYBED_DIAG
  TCD_keyOnOffCntr[keyOnOffIndex]++;
#endif
}

/*****************************************************************************
*	@brief  MSG_KeyUp
*	Sends the Up (Off) velocity and stops a note
*   @param  vel: velocity value 0...16383
******************************************************************************/
void MSG_KeyUp(uint32_t vel)
{
  FillBufferWithTCDdata(AE_TCD_WRAPPER, AE_TCD_KEY_UP, vel);
#if LPC_KEYBED_DIAG
  TCD_keyOnOffCntr[keyOnOffIndex]--;
#endif
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
    return;  /// assertion
  FillBufferWithTCDdata(AE_TCD_WRAPPER, AE_TCD_HW_POS | source, position);
}

/*****************************************************************************
*   @brief	MSG_SendAEDeveloperCmd
*   Sends a special command to audio engine
*   @param  cmd : command, 14bit (1:testtone OFF; 2:testtone ON; 3:default sound)
******************************************************************************/
void MSG_SendAEDevelopperCmd(uint32_t cmd)
{
  FillBufferWithTCDdata(AE_TCD_WRAPPER, AE_TCD_DEVELOPPER_CMD, cmd);
}

/*****************************************************************************
*   @brief	MSG_SendActiveSensing
*   Sends an "Active Sensing" MIDI command to AE, to indicate LPC is up and running
******************************************************************************/
void MSG_SendActiveSensing(void)
{
#ifdef __IMPLEMENT_ACTIVE_SENSING
  FillBufferWithTCDdata(0x0F,  // cable 0, packet type "single byte"
                        0xFE,  // MIDI real-time command "active sensing"
                        0);
#endif
}
