// Audio mute/unmute processing
// KSTR 2019-06-07

#include "audio.h"

#include "bit_manipulation.h"
#include "pin_manipulation.h"
#include "hardware.h"

Audio_t Audio;  // public handler

static int8_t step = 0;  // step-chain variable

void Audio_t::Init(void)
{
  step            = 0;
  requested_state = 0;
}

void Audio_t::_Mute(void)
{
  PinClr(SYS_OPT_Relays_UNMUTE);
  PinSet(SYS_IOPT_Relays_MUTE);
  BitClr(config.status, STAT_UNMUTED);
  LED_B(0);
  if (!config.mute_is_monostable)
    step = 1;  // start step-chain to release coil after some time when we have bi-stable/latching relays
}

void Audio_t::_UnMute(void)
{
  if (power_failed)  // power fail event?
    return;          // then don't unmute
  PinClr(SYS_IOPT_Relays_MUTE);
  PinSet(SYS_OPT_Relays_UNMUTE);
  BitSet(config.status, STAT_UNMUTED);
  LED_B(1);
  if (!config.mute_is_monostable)
    step = 1;  // start step-chain to release coil after some time when we have bi-stable/latching relays
}

void Audio_t::Mute(uint8_t forced)
{
  requested_state = 0;
  if (forced)
    BitClr(config.status, STAT_MUTING_OVERRIDE_ENABLE);
  if (!BitGet(config.status, STAT_MUTING_OVERRIDE_ENABLE))
    _Mute();
}

void Audio_t::UnMute(void)
{
  requested_state = 1;
  if (!BitGet(config.status, STAT_MUTING_OVERRIDE_ENABLE))
    _UnMute();
}

void Audio_Process(void)
{
  uint8_t wanted;
  if (BitGet(config.status, STAT_MUTING_OVERRIDE_ENABLE))
    wanted = BitGet(config.status, STAT_MUTING_OVERRIDE_VALUE);
  else
    wanted = Audio.requested_state;
  if (wanted != BitGet(config.status, STAT_UNMUTED))
  {
    if (wanted)
      Audio._UnMute();
    else
      Audio._Mute();
  }

  if (step == 0)
    return;
  if (step == 1)
    step = 2;  // let another time-slice elapse first
  else
  {  // de-energize both coils
    PinClr(SYS_IOPT_Relays_MUTE);
    PinClr(SYS_OPT_Relays_UNMUTE);
    step = 0;  // stop chain
  }
}

// end of file
