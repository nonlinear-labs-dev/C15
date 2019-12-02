// system and input power processing
// KSTR 2019-06-07

#ifndef __PWR_MONITOR_H__
#define __PWR_MONITOR_H__

#include <stdint.h>

struct PwrMonitor_t
{
  uint8_t system_was_up;
  void    Init(void);
  uint8_t PowerIsGood(void);  // true when incoming 19V are above upper threshold (~15.5V)
                              // note this will be blocking for 2ms when power is detected good!
  uint8_t PowerFail(void);    // true when incoming 19V are below lower threshold (~14.5V)
  void    SystemOn(void);     // turn on 19V line to system
  void    SystemOff(void);    // turn off 19V line to system
};

extern PwrMonitor_t PwrMonitor;  // public handler

#endif

// end of file
