// LED command processing
// KSTR 2019-06-07

#ifndef __LED_H__
#define __LED_H__

#include <stdint.h>

struct Led_t
{
  void On(void);
  void Off(void);
  void Blink_VeryFast(void);
  void Blink_Fast(void);
  void Blink_Medium(void);
  void Blink_Slow(void);
  void Blink_Standby(void);
  void On_IndicateGlitch1(void);
  void On_IndicateGlitch2(void);
  void PowerCyclingOverride(uint8_t on);
  void Show_Hardware_and_Firmware_ID(void);  // overrides LED output for the time this function takes
};

void LED_Process(void);  // step-chain task

extern Led_t Led;  // public handler

#endif

// end of file
