// Beagle Bone Black processing
// KSTR 2019-06-07

#ifndef __BBB_H__
#define __BBB_H__

#include <stdint.h>
#include "globals.h"

struct BBB_t
{
  void    Init(void);
  void    SwitchOn(void);   // currently a dummy, not used, as BBB starts with power applied
  void    SwitchOff(void);  // start shutdown sequence
  uint8_t IsOn(void);
  uint8_t IsOff(void);
  uint8_t StateChange(void);
  uint8_t GetHeartbeatLED(void);
  uint8_t HeartbeatTimedOut(void);
  void    ArmHeartbeat(void);
};

void BBB_Process(void);  // step-chain task

extern BBB_t BBB;  // public handler

#endif

// end of file