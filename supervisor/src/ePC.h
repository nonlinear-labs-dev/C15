// ePC (Intel NUC) processing
// KSTR 2019-06-07

#ifndef __ePC_H__
#define __ePC_H__

#include <stdint.h>
#include "globals.h"

#define EPC_TURN_OFF_TIMEOUT (8000 / ePC_TIMESLICE)

struct ePC_t
{
  void    Init(void);
  void    SwitchOn(void);   // start startup sequence
  void    SwitchOff(void);  // start shutdown sequence
  uint8_t IsOn(void);
  uint8_t IsOff(void);
  uint8_t StateChange(void);
};

void ePC_Process(void);  // step-chain task

extern ePC_t ePC;  // public handler

#endif

// end of file
