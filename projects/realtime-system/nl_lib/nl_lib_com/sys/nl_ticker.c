/******************************************************************************/
/** @file		nl_ticker.c
    @date		2020-01-27
    @version	0
    @author		KSTR
    @brief		system ticker global variable
*******************************************************************************/
#include "sys/nl_ticker.h"
#include "cmsis/LPC43xx.h"

volatile uint32_t SYS_ticker = 0;

void SYS_WatchDogClear(void)
{
  __disable_irq();
  LPC_WWDT->FEED = 0xAA;  // the required feed value sequence ...
  LPC_WWDT->FEED = 0x55;  // ... to reload the watchdog counter
  __enable_irq();
}

void SYS_WatchDogInit(uint32_t timeoutInMs)
{
  // Timeout = T_WDCLK*4 * TC = 1/3Mhz * TC --> TC = Timeout(s) * 3MHz = Timeout(ms) * 3kHz
  LPC_WWDT->TC  = 3000ul * timeoutInMs;
  LPC_WWDT->MOD = WWDT_MOD_WDEN_Msk | WWDT_MOD_WDRESET_Msk;
  SYS_WatchDogClear();
}

void SYS_Reset(void)
{
  __disable_irq();
  LPC_WWDT->FEED = 0x00;  // invalid feed sequence ...
  LPC_WWDT->FEED = 0x00;  // ... causes instant reset
  __enable_irq();
}
