/******************************************************************************/
/** @file		nl_dbg.c
    @date		2013-04-23
    @version	0.03
    @author		Daniel Tzschentke [2012-03-08]
    @brief		This module is a debug interface and provides access to LEDs, 
				buttons and the logic-analyzer IOs
    @ingroup	nl_drv_modules
*******************************************************************************/

#include "io/pins.h"
#include "globals.h"

typedef struct
{
  uint16_t           timer;
  uint16_t           flicker;
  volatile uint32_t *led;
} LED_t;

static LED_t leds[6] = {
  { 0, 0, pLED_REDA },
  { 0, 0, pLED_REDB },
  { 0, 0, pLED_YELLOWA },
  { 0, 0, pLED_YELLOWB },
  { 0, 0, pLED_GREENA },
  { 0, 0, pLED_GREENB },
};

#define ON  0
#define OFF 1

void DBG_Led(uint8_t const ledId, uint8_t const on)
{
  if (ledId >= 6)
    return;
  *leds[ledId].led = on ? ON : OFF;
}

void DBG_Led_TimedOn(uint8_t const ledId, int16_t time)
{
  if (time == 0)
    return;
  if (ledId >= 6)
    return;
  leds[ledId].flicker = leds[ledId].timer != 0 && time > 0;
  if (time < 0)
    time = -time;
  if (time >= leds[ledId].timer)
    leds[ledId].timer = time;
  *leds[ledId].led = !leds[ledId].flicker ? ON : OFF;
}

/******************************************************************************/
/** @brief    	Handling the M4 LEDs every 500ms
*******************************************************************************/
void DBG_Process(void)
{
  for (int id = 0; id < 6; id++)
  {
    if (leds[id].flicker)
    {
      if (!--leds[id].flicker)
        *leds[id].led = ON;
    }
    else if (leds[id].timer)
    {
      if (!--leds[id].timer)
        *leds[id].led = OFF;
    }
  }
}
