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
#include "sys/nl_status.h"
#include "globals.h"

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
 *              Also checks for missed keybed events and displays them as
 *              a 200ms warning LED blink every 500ms. Of course also set
 *              during playing when keys actually are pressed.
*******************************************************************************/
void DBG_Process(void)
{
  static int HB_Timer = 5;  // 500ms on/off times
  if (!--HB_Timer)
  {
#if LPC_KEYBED_DIAG
    if (NL_STAT_CheckMissedKeybedEvents())
      DBG_Led_Warning_TimedOn(2);
#endif
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
