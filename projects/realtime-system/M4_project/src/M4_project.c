/******************************************************************************/
/** @file	Emphase_M4_Main.c
    @date	2016-03-09 SSC
	@changes 2020-05-11 KSTR
*******************************************************************************/

#include <stdint.h>

#include "sys/nl_coos.h"
#include "sys/nl_watchdog.h"
#include "sys/delays.h"
#include "boards/emphase_v5.h"

#include "drv/nl_gpio.h"
#include "drv/nl_dbg.h"
#include "drv/nl_gpdma.h"
#include "drv/nl_kbs.h"

#include "usb/nl_usb_midi.h"
#include "cpu/nl_cpu.h"
#include "ipc/emphase_ipc.h"

#include "spibb/nl_spi_bb.h"
#include "spibb/nl_bb_msg.h"

#include "tcd/nl_tcd_adc_work.h"
#include "tcd/nl_tcd_poly.h"
#include "tcd/nl_tcd_msg.h"
#include "sup/nl_sup.h"
#include "heartbeat/nl_heartbeat.h"
#include "sys/nl_eeprom.h"
#include "sys/crc.h"

#define DBG_CLOCK_MONITOR (0)

#define WATCHDOG_TIMEOUT_MS (100ul)  // timeout in ms

static volatile uint16_t waitForFirstSysTick = 1;

void M4SysTick_Init(void);

// --- all 125us processes combined in one single tasks
void FastProcesses(void)
{
  SYS_WatchDogClear();  // every 125 us, clear Watchdog
  NL_GPDMA_Poll();      // every 125 us, for all the DMA transfers (SPI devices), may do callbacks ?
  SPI_BB_Polling();     // every 125 us, checking the buffer with messages from the BBB, may do callbacks ?
}

void Init(void)
{
  /* board */
  EMPHASE_V5_M4_Init();

  /* supervisor */
  SUP_Init();

  /* system */
  Emphase_IPC_Init();
  NL_GPDMA_Init(0b11111100);

  /* debug */
  DBG_Init();
  DBG_Led_Error_On();
  DBG_Led_Cpu_On();
  DBG_Led_Warning_On();
  DBG_Led_Audio_On();
  DBG_GPIO3_1_Off();
  DBG_GPIO3_2_Off();
  DBG_GPIO3_3_Off();
  DBG_GPIO3_4_Off();

  /* EEPROM */
  NL_EEPROM_Init();

  /* crc */
  crcInit();

  /* USB */
  USB_MIDI_Init();
  MSG_DropMidiMessages(1);
  USB_MIDI_Config(HBT_MidiReceive);

  volatile uint32_t timeOut = 0x0FFFFF;

  do
  {
    USB_MIDI_Poll();
    timeOut--;
  } while ((!USB_MIDI_IsConfigured()) && (timeOut > 0));

  //	if (USB_MIDI_IsConfigured() == TRUE)
  //		DBG_Led_Usb_Off();

  /* lpc bbb communication */
  SPI_BB_Init(BB_MSG_ReceiveCallback);

  /* velocity table */
  POLY_Init();

  /* ADC processing */
  ADC_WORK_Init1();

  /* scheduler */
  COOS_Init();

  // clang-format off
  // fast and simultaneous processes
  COOS_Task_Add(FastProcesses,            0, 1);      // every 125 us, combined routines

  // slower stuff
  //   we are using a fixed time granularity here and make sure the tasks are started interleaved in micro-steps
  //   within that granularity. The repetitive calls are made with multiples of that granularity.
  //   By this, no more than one of the following task will ever be called at a time.
#define  TS (10)  // 1.25 ms time slice
  COOS_Task_Add(ADC_WORK_Process1,        0*TS+0,   10*TS);       // every 12.5 ms, reading ADC values and applying changes
  COOS_Task_Add(ADC_WORK_Process2,        1*TS+1,   10*TS);       // every 12.5 ms, reading ADC values and applying changes
  COOS_Task_Add(ADC_WORK_Process3,        2*TS+2,   10*TS);       // every 12.5 ms, reading ADC values and applying changes
  COOS_Task_Add(ADC_WORK_Process4,        3*TS+3,   10*TS);       // every 12.5 ms, reading ADC values and applying changes
  COOS_Task_Add(ADC_WORK_SendUIMessages,  4*TS+4,   10*TS);       // every 12.5 ms, sending the results of the ADC processing to the BBB
  COOS_Task_Add(DBG_Process,              5*TS+5,   80*TS);       // every 100 ms, processes error and warning LEDs
  COOS_Task_Add(SUP_Process,              6*TS+6,   8 *TS);       // supervisor communication every 10ms
  COOS_Task_Add(HBT_Process,              80*TS+7,  8 *TS);       // heartbeat communication every 10ms, start after 100ms
  COOS_Task_Add(NL_EEPROM_Process,        8*TS+8,      TS);       // EEPROM write every 1.25ms

  // single run stuff
  COOS_Task_Add(ADC_WORK_Init2,           9*TS+9, 0); // preparing the ADC processing (will be executed after the M0 has been initialized)
  // clang-format on

  /* M0 init  */
  CPU_M0_Init();

  /* M4 sysTick */
  M4SysTick_Init();

  /* watchdog */
  SYS_WatchDogInit(WATCHDOG_TIMEOUT_MS);
}

/******************************************************************************/
int main(void)
{
  Init();

  while (waitForFirstSysTick)
    ;

  DBG_Led_Error_Off();
  DBG_Led_Cpu_Off();
  DBG_Led_Warning_Off();
  DBG_Led_Audio_Off();

  while (1)
  {                   // Since M0 handles key events in 16us turnaround time, we want to ...
    POLY_Process();   // ... read and process keybed events as fast as possible.
    USB_MIDI_Poll();  // Send/receive MIDI data, may do callbacks, also from within interrupt ?
    COOS_Dispatch();  // Standard dispatching of the slower stuff
  }

  return 0;
}

/*************************************************************************/ /**
* @brief	ticker interrupt routines using the standard M4 system ticker
*           IRQ will be triggered every 125us, our basic scheduler time-slice
******************************************************************************/

#define SYST_CSR   (uint32_t *) (0xE000E010)  // SysTick Control & Status Reg
#define SYST_RVR   (uint32_t *) (0xE000E014)  // SysTick Reload Value
#define SYST_CVR   (uint32_t *) (0xE000E018)  // SysTick Counter Value
#define SYST_CALIB (uint32_t *) (0xE000E01C)  // SysTick Calibration

void M4SysTick_Init(void)
{
  *SYST_RVR = (NL_LPC_CLK / M4_FREQ_HZ) - 1;
  *SYST_CVR = 0;
  *SYST_CSR = 0b111;  // processor clock | IRQ enabled | counter enabled
}

void SysTick_Handler(void)
{
  static uint16_t cntr = 25;  // 25 * 5us = 125us time slice

  s.ticker++;
  if (!--cntr)
  {
    cntr = 25;
#if DBG_CLOCK_MONITOR
    DBG_GPIO3_1_On();
#endif
    waitForFirstSysTick = 0;
    SPI_BB_ToggleHeartbeat();  // driving the LPC-BB "heartbeat" from IRQ for high precision.
    COOS_Update();
#if DBG_CLOCK_MONITOR
    DBG_GPIO3_1_Off();
#endif
  }
}
