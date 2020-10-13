#include "nl_midi_proxy.h"
#include "usb/nl_usba_midi.h"
#include "usb/nl_usbb_midi.h"
#include "drv/nl_dbg.h"
#include "io/pins.h"

static void ReceiveA(uint8_t *buff, uint32_t len)
{
  if (USBB_MIDI_IsConfigured())
  {
    if (USBB_MIDI_BytesToSend())
    {
      DBG_Led(LED_TRAFFIC_STALL, 1);
      DBG_Led(LED_DATA_LOSS, 1);
    }
    else
    {
      if (USBB_MIDI_Send(buff, len, 0))
        DBG_Led_TimedOn(LED_MIDI_TRAFFIC, -2);  // success
      else
      {
        DBG_Led(LED_DATA_LOSS, 1);
        DBG_Led_TimedOn(LED_ERROR, -10);  // send failure
      }
    }
    return;
  }
  DBG_Led(LED_DATA_LOSS, 1);
  DBG_Led_TimedOn(LED_ERROR, -10);  // send failure
}

static void ReceiveB(uint8_t *buff, uint32_t len)
{
  if (USBA_MIDI_IsConfigured())
  {
    if (USBA_MIDI_BytesToSend())
    {
      DBG_Led(LED_TRAFFIC_STALL, 1);
      DBG_Led(LED_DATA_LOSS, 1);
    }
    else
    {
      if (USBA_MIDI_Send(buff, len, 0))
        DBG_Led_TimedOn(LED_MIDI_TRAFFIC, -2);  // success
      else
      {
        DBG_Led(LED_DATA_LOSS, 1);
        DBG_Led_TimedOn(LED_ERROR, -10);  // send failure
      }
    }
    return;
  }
  DBG_Led(LED_DATA_LOSS, 1);
  DBG_Led_TimedOn(LED_ERROR, -10);  // send failure
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

void MIDI_PROXY_ProcessFast(void)
{
  if (!pinUSBA_VBUS)  // VBUS is off now
  {
    if (USBA_NotConnected == 0)  // was on before ?
    {                            // then turn USB off
      USBA_MIDI_DeInit();
      USBB_MIDI_DeInit();
    }
    USBA_NotConnected = 3;
  }

  if (!pinUSBB_VBUS)  // VBUS is off now
  {
    if (USBB_NotConnected == 0)  // was on before ?
    {                            // then turn USB off
      USBB_MIDI_DeInit();
      USBA_MIDI_DeInit();
    }
    USBB_NotConnected = 3;
  }
  DBG_Led(LED_CONNECTED, (!USBA_NotConnected && !USBB_NotConnected && USBA_MIDI_IsConfigured() && USBB_MIDI_IsConfigured()));
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
    USBA_MIDI_Init();
    USBB_MIDI_Init();
  }
}
