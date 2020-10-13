#include "nl_midi_proxy.h"
#include "usb/nl_usba_midi.h"
#include "usb/nl_usbb_midi.h"
#include "usb/nl_usb_descmidi.h"
#include "drv/nl_dbg.h"
#include "io/pins.h"

#define BUFFER_SIZE            (1024)  // same size as low-level MIDI transfer buffers !!
#define NUMBER_OF_BUFFERS      (8)     // number of buffers, must be 2^N !
#define NUMBER_OF_BUFFERS_MASK (NUMBER_OF_BUFFERS - 1)

// ringbuffer of buffers
typedef struct
{
  uint8_t  buff[NUMBER_OF_BUFFERS][BUFFER_SIZE];
  uint16_t bufHead;       // 0..(NUMBER_OF_BUFFERS-1)
  uint16_t bufTail;       // 0..(NUMBER_OF_BUFFERS-1)
  uint16_t bufHeadIndex;  // used bytes in the current head (front) buffer
} ringbuffer_t;

static ringbuffer_t rb[2];

#define A (0)
#define B (1)

static void ringbufferReset(void)
{
  for (int i = 0; i <= 1; i++)
  {
    rb[i].bufHead      = 0;
    rb[i].bufTail      = 0;
    rb[i].bufHeadIndex = 0;
  }
}

static inline int fillBuffer(uint8_t const which, uint8_t *buff, uint32_t len)
{
  ringbuffer_t *const p                 = &rb[which];
  uint16_t const      savedBufHead      = p->bufHead;
  uint16_t const      savedBufHeadIndex = p->bufHeadIndex;
  while (len)
  {
    if (p->bufHeadIndex >= BUFFER_SIZE)
    {  // current head is full, switch to next one
      uint16_t tmpBufHead = (p->bufHead + 1) & NUMBER_OF_BUFFERS_MASK;
      if (tmpBufHead == p->bufTail)  // no space left ?
      {                              // discard data and signal failure
        p->bufHead      = savedBufHead;
        p->bufHeadIndex = savedBufHeadIndex;
        return 0;
      }
      p->bufHead      = tmpBufHead;
      p->bufHeadIndex = 0;
    }
    p->buff[p->bufHead][p->bufHeadIndex++] = *(buff++);
    len--;
  }
  return 1;
}

static void ReceiveA(uint8_t *buff, uint32_t len)
{
  if (!fillBuffer(B, buff, len))  // out of buffers ?
    DBG_Led(LED_DATA_LOSS, 1);
}

static void ReceiveB(uint8_t *buff, uint32_t len)
{
  if (!fillBuffer(A, buff, len))  // out of buffers ?
    DBG_Led(LED_DATA_LOSS, 1);
}

void MIDI_PROXY_Init(void)
{
  USBA_MIDI_Config(ReceiveA);
  USBB_MIDI_Config(ReceiveB);
}

// ------------------------------------------------------------------

#if USBA_PORT_FOR_MIDI == 0
#define pinUSBA_VBUS pinUSB0_VBUS
#else
#define pinUSBA_VBUS pinUSB1_VBUS
#endif

#if USBB_PORT_FOR_MIDI == 0
#define pinUSBB_VBUS pinUSB0_VBUS
#else
#define pinUSBB_VBUS pinUSB1_VBUS
#endif

static uint8_t USBA_NotConnected = 3;
static uint8_t USBB_NotConnected = 3;

static inline uint32_t bytesToSend(uint8_t const which)
{
  return (which == A) ? USBA_MIDI_BytesToSend() : USBB_MIDI_BytesToSend();
}

static inline uint32_t MIDI_Send(uint8_t const which, uint8_t const *const buff, uint32_t const cnt)
{
  return (which == A) ? USBA_MIDI_Send(buff, cnt, 0) : USBB_MIDI_Send(buff, cnt, 0);
}

static inline uint32_t MIDI_isConfigured(uint8_t const which)
{
  return (which == A) ? USBA_MIDI_IsConfigured() : USBB_MIDI_IsConfigured();
}

static inline void checkSends(uint8_t const which)
{
  ringbuffer_t *const p = &rb[which];
  if (p->bufHead == p->bufTail && p->bufHeadIndex == 0)
    return;  // nothing to send

  if (!MIDI_isConfigured(which) || bytesToSend(which))
  {  // not config'd or last transfer still in progress
    DBG_Led_TimedOn(LED_TRAFFIC_STALL, -2);
    return;
  }

  if (p->bufHead != p->bufTail)
  {  // send stashed buffers first
    if (!MIDI_Send(which, p->buff[p->bufTail], BUFFER_SIZE))
    {  // send failed
      DBG_Led(LED_DATA_LOSS, 1);
      DBG_Led_TimedOn(LED_ERROR, -10);  // send failure
    }
    else
      DBG_Led_TimedOn(LED_MIDI_TRAFFIC, -2);  // success
    p->bufTail = (p->bufTail + 1) & NUMBER_OF_BUFFERS_MASK;
    return;
  }
  if (p->bufHeadIndex != 0)
  {  // send current buffer
    if (!MIDI_Send(which, p->buff[p->bufHead], p->bufHeadIndex))
    {  // send failed
      DBG_Led(LED_DATA_LOSS, 1);
      DBG_Led_TimedOn(LED_ERROR, -10);  // send failure
    }
    DBG_Led_TimedOn(LED_MIDI_TRAFFIC, -2);  // success
    p->bufHead      = (p->bufHead + 1) & NUMBER_OF_BUFFERS_MASK;
    p->bufHeadIndex = 0;
    p->bufTail      = p->bufHead;
  }
}

void MIDI_PROXY_ProcessFast(void)
{
  if (!pinUSBA_VBUS)  // VBUS is off now
  {
    if (USBA_NotConnected == 0)  // was on before ?
    {                            // then turn USB off
      USBA_MIDI_DeInit();
      USBB_MIDI_DeInit();
      ringbufferReset();
    }
    USBA_NotConnected = 3;
  }
  if (!pinUSBB_VBUS)  // VBUS is off now
  {
    if (USBB_NotConnected == 0)  // was on before ?
    {                            // then turn USB off
      USBB_MIDI_DeInit();
      USBA_MIDI_DeInit();
      ringbufferReset();
    }
    USBB_NotConnected = 3;
  }
  DBG_Led(LED_CONNECTED, (!USBA_NotConnected && !USBB_NotConnected && USBA_MIDI_IsConfigured() && USBB_MIDI_IsConfigured()));

  checkSends(A);
  checkSends(B);
}

void MIDI_PROXY_Process(void)
{
  uint8_t armA = 0;
  uint8_t armB = 0;

  if (pinUSBA_VBUS)  // VBUS is on now
  {
    if (USBA_NotConnected)  // still running plug-in time-out ?
    {
      if (!--USBA_NotConnected)  // time-out elapsed ?
        armA = 1;                // arm turn-on trigger
    }
  }

  if (pinUSBB_VBUS)  // VBUS is on now
  {
    if (USBB_NotConnected)  // still running plug-in time-out ?
    {
      if (!--USBB_NotConnected)  // time-out elapsed ?
        armB = 1;                // arm turn-on trigger
    }
  }

  armA = armA && !USBB_NotConnected;
  armB = armB && !USBA_NotConnected;

  if (armA || armB)
  {
    USB_MIDI_setStringDescriptorHash(0);  // TODO : get some true(!) random number
    USBA_MIDI_Init();
    USBB_MIDI_Init();
  }
}
