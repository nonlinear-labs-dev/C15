// power switch/button processing
// KSTR 2019-06-07

#ifndef __PWR_SWITCH_H__
#define __PWR_SWITCH_H__

#include "globals.h"

struct PwrSwitch_t
{
  void    Init(void);
  void    Arm(void);
  uint8_t On_Event(void);   // true when a "turn system ON" request event happened
  uint8_t Off_Event(void);  // true when a "turn system FF" request event happened
  int8_t  GetState(void);   // get current debounced state of the hardware element
};

void PSW_Process(void);  // step-chain task

extern PwrSwitch_t PwrSwitch;  // public handler

#endif

// end of file
