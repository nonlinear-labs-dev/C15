#include "nl_midi_proxy.h"
#include "usb/nl_usba_midi.h"
#include "usb/nl_usbb_midi.h"
#include "drv/nl_dbg.h"

static void ReceiveA(uint8_t *buff, uint32_t len)
{
  if (!USBB_MIDI_IsConfigured() || !USBB_MIDI_Send(buff, len, 0))
  {
    DBG_Led_Warning_TimedOn(10);
  }
}

static void ReceiveB(uint8_t *buff, uint32_t len)
{
  if (!USBA_MIDI_IsConfigured() || !USBA_MIDI_Send(buff, len, 0))
  {
    DBG_Led_Warning_TimedOn(10);
  }
}

void MIDI_PROXY_Init(void)
{
  USBA_MIDI_Config(ReceiveA);
  USBB_MIDI_Config(ReceiveB);
}
