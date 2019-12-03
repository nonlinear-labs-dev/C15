// Audio mute/unmute processing
// KSTR 2019-06-07

#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdint.h>
#include "globals.h"

struct Audio_t
{
  void    Init(void);
  void    Mute(uint8_t forced = 0);
  void    UnMute(void);
  void    _Mute(void);    // has immediate effect, doesn't wait until step-chain handler is actually called
  void    _UnMute(void);  // has immediate effect, doesn't wait until step-chain handler is actually called
  uint8_t requested_state;
};

void Audio_Process(void);  // step-chain task

extern Audio_t Audio;  // public handler

#endif

// end of file
