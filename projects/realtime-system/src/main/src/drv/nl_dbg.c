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

static uint16_t errorTimer          = 0;
static uint16_t warningTimer        = 0;
static uint16_t errorTimerFlicker   = 0;
static uint16_t warningTimerFlicker = 0;

void DBG_Led_Error_TimedOn(int16_t time)
{
  if (time == 0)
    return;
  errorTimerFlicker = errorTimer != 0 && time > 0;
  if (time < 0)
    time = -time;
  if (time >= errorTimer)
    errorTimer = time;
  ledError = !errorTimerFlicker;
}

void DBG_Led_Warning_TimedOn(int16_t time)
{
  if (time == 0)
    return;
  warningTimerFlicker = warningTimer != 0 && time > 0;
  if (time < 0)
    time = -time;
  if (time >= warningTimer)
    warningTimer = time;
  ledWarning = !warningTimerFlicker;
}

/******************************************************************************/
/** @brief    	Handling the M4 LEDs (HeartBeat, Warning, Error), every 100ms
*******************************************************************************/
void DBG_Process(void)
{
  static int HB_Timer = 5;
  if (!--HB_Timer)
  {
    HB_Timer       = 5;
    ledM4heartbeat = ~ledM4heartbeat;
  }

  // Error LED
  if (errorTimerFlicker)
  {
    if (!--errorTimerFlicker)
      ledError = 1;
  }
  else if (errorTimer)
  {
    if (!--errorTimer)
      ledError = 0;
  }

  // Warning LED
  if (warningTimerFlicker)
  {
    if (!--warningTimerFlicker)
      ledWarning = 1;
  }
  else if (warningTimer)
  {
    if (!--warningTimer)
      ledWarning = 0;
  }
}
