/******************************************************************************/
/** @file	Emphase_M4_Main.c
    @date	2016-03-09 SSC
	@changes 2020-05-11 KSTR
*******************************************************************************/

#pragma GCC diagnostic ignored "-Wmain"

#include <stdint.h>

#include "cr_start_m0.h"
#include "sys/nl_coos.h"
#include "sys/nl_watchdog.h"
//#include "sys/delays.h"
#include "CPU_clock.h"
#include "usb/nl_usba_midi.h"
#include "usb/nl_usbb_midi.h"
#include "drv/nl_dbg.h"
#include "drv/nl_cgu.h"
#include "io/pins.h"
#include "midi/nl_midi_proxy.h"
#include "sys/nl_version.h"
#include "version.h"

#define DBG_CLOCK_MONITOR (0)

#define WATCHDOG_TIMEOUT_MS (100ul)  // timeout in ms

static volatile uint16_t waitForFirstSysTick = 1;

void M4SysTick_Init(void);

// --- all 125us processes combined in one single tasks
void FastProcesses(void)
{
  SYS_WatchDogClear();  // every 125 us, clear Watchdog
}

volatile char dummy;

void dummyFunction(const char *string)
{
  while (*string)
  {
    dummy = *string++;
  }
}

void Init(void)
{
  // referencing the version string so compiler won't optimize it away
  dummyFunction(VERSION_STRING);
  dummyFunction(GetC15Version());
  dummyFunction(GetC15Build());

  /* CPU clock */
  CPU_ConfigureClocks();

  /* I/O pins */
  PINS_Init();

  /* USB */
  USBA_MIDI_Init();
  USBB_MIDI_Init();

  /* MIDI Proxy */
  MIDI_PROXY_Init();

  /* scheduler */
  COOS_Init();

  COOS_Task_Add(SYS_WatchDogClear, 0, 1);  // every 125 us

#define TS (10)                                     // 1.25 ms time slice
  COOS_Task_Add(DBG_Process, 1 * TS + 0, 40 * TS);  // every 50 ms, processes error and warning LEDs

  /* M0 init  */
  // cr_start_m0(SLAVE_M0APP, &__core_m0app_START__);

  /* M4 sysTick */
  M4SysTick_Init();

  /* watchdog */
  SYS_WatchDogInit(WATCHDOG_TIMEOUT_MS);
  //#warning "watchdog is off!"
}

/******************************************************************************/
void main(void)
{
  Init();

  while (waitForFirstSysTick)
    ;

  ledAudioOk = 0;
  ledWarning = 0;
  ledError   = 0;

  while (1)
  {                    // Since M0 handles key events in ~20us turnaround time, we want to ...
    USBA_MIDI_Poll();  // Send/receive MIDI data, may do callbacks, also from within interrupt ?
    USBB_MIDI_Poll();  // Send/receive MIDI data, may do callbacks, also from within interrupt ?
    COOS_Dispatch();   // Standard dispatching of the slower stuff
  }
}

/*************************************************************************/ /**
* @brief	ticker interrupt routines using the standard M4 system ticker
*           IRQ will be triggered every 125us, our basic scheduler time-slice
******************************************************************************/
void M4SysTick_Init(void)
{
#define SYST_CSR   (uint32_t *) (0xE000E010)  // SysTick Control & Status Reg
#define SYST_RVR   (uint32_t *) (0xE000E014)  // SysTick Reload Value
#define SYST_CVR   (uint32_t *) (0xE000E018)  // SysTick Counter Value
#define SYST_CALIB (uint32_t *) (0xE000E01C)  // SysTick Calibration
  *SYST_RVR = (NL_LPC_CLK / M4_FREQ_HZ) - 1;
  *SYST_CVR = 0;
  *SYST_CSR = 0b111;  // processor clock | IRQ enabled | counter enabled
}

void SysTick_Handler(void)
{
  static uint16_t cntr = 25;  // 25 * 5us = 125us time slice
  if (!--cntr)
  {
    cntr                = 25;
    waitForFirstSysTick = 0;
    COOS_Update();
  }
}
