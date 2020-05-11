/******************************************************************************/
/** @file		nl_watchdog.c
    @date		2020-05-10
    @version	0
    @author		KSTR
    @brief		watchdog
*******************************************************************************/
#include "sys/nl_watchdog.h"
#include "cmsis/LPC43xx.h"

static int watchDogEnabled = 0;

void SYS_WatchDogClear(void)
{
  if (!watchDogEnabled)
    return;
  __disable_irq();
  LPC_WWDT->FEED = 0xAA;  // the required feed value sequence ...
  LPC_WWDT->FEED = 0x55;  // ... to reload the watchdog counter
  __enable_irq();
}

void SYS_WatchDogInit(uint32_t timeoutInMs)
{
  // Timeout = T_WDCLK*4 * TC = 1/3Mhz * TC --> TC = Timeout(s) * 3MHz = Timeout(ms) * 3kHz
  __disable_irq();
  LPC_WWDT->TC    = 3000ul * timeoutInMs;
  LPC_WWDT->MOD   = WWDT_MOD_WDEN_Msk | WWDT_MOD_WDRESET_Msk;
  watchDogEnabled = 1;
  __enable_irq();
  SYS_WatchDogClear();
}

void SYS_Reset(void)
{
  if (!watchDogEnabled)
    return;
  __disable_irq();
  LPC_WWDT->FEED = 0x00;  // invalid feed sequence ...
  LPC_WWDT->FEED = 0x00;  // ... causes instant reset
  __enable_irq();
}
