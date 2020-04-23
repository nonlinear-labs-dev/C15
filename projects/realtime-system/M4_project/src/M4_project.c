/******************************************************************************/
/** @file	Emphase_M4_Main.c
    @date	2016-03-09 SSC
		@changes 2020-03-31 KSTR
*******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "sys/nl_coos.h"
#include "sys/nl_ticker.h"
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

static volatile uint16_t waitForFirstSysTick = 1;
static volatile uint32_t nsTickerHigh        = 0;

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
  DBG_Led_Error_Off();
  DBG_Led_Cpu_Off();
  DBG_Led_Warning_Off();
  DBG_Led_Audio_Off();
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
  COOS_Task_Add(POLY_Process,             40, 1);     // every 125 us, reading and applying keybed events
  COOS_Task_Add(NL_GPDMA_Poll,            10, 1);     // every 125 us, for all the DMA transfers (SPI devices)
  COOS_Task_Add(USB_MIDI_Poll,            20, 1);     // every 125 us, same time grid as in USB 2.0
  COOS_Task_Add(SPI_BB_Polling,           30, 1);     // every 125 us, checking the buffer with messages from the BBB

  // slower stuff
  //   to avoid potential overrun by any of these falling into the same time slot when their time-slices are integer multiples,
  //   the start offsets are fine-stepped in increments, the max increment being less that the shortest time-slice
  COOS_Task_Add(ADC_WORK_Process1,        50+1, 100);       // every 12.5 ms, reading ADC values and applying changes
  COOS_Task_Add(ADC_WORK_Process2,        60+2, 100);       // every 12.5 ms, reading ADC values and applying changes
  COOS_Task_Add(ADC_WORK_Process3,        70+3, 100);       // every 12.5 ms, reading ADC values and applying changes
  COOS_Task_Add(ADC_WORK_Process4,        80+4, 100);       // every 12.5 ms, reading ADC values and applying changes
  COOS_Task_Add(ADC_WORK_SendBBMessages,  90+5, 100);       // every 12.5 ms, sending the results of the ADC processing to the BBB
  COOS_Task_Add(DBG_Process,              100+6, 100 * 8);  // every 100 ms, processes error and warning LEDs
  COOS_Task_Add(SUP_Process,              110+7, SUP_PROCESS_TIMESLICE * 8);  // supervisor communication every 10ms
  COOS_Task_Add(HBT_Process,              120+8, HBT_PROCESS_TIMESLICE * 8); // heartbeat communication every 10ms
  COOS_Task_Add(NL_EEPROM_Process,        130+9, 10);                         // EEPROM write every 1.25ms

  // single run stuff
  COOS_Task_Add(ADC_WORK_Init2,           140, 0);     // preparing the ADC processing (will be executed after the M0 has been initialized)
  // clang-format on

  /* M0 init, also starting our time-slice interrupt  */
  CPU_M0_Init();
  NVIC_SetPriority(M0CORE_IRQn, M0APP_IRQ_PRIORITY);
  NVIC_EnableIRQ(M0CORE_IRQn);
}

/******************************************************************************/
int main(void)
{
  Init();

  while (waitForFirstSysTick)
    ;

  while (1)
  {
    COOS_Dispatch();
  }

  return 0;
}

/*************************************************************************/ /**
* @brief	ticker interrupt
* this will be triggered every 125us from M0 core, to sync it with M0's data
* aquisition
******************************************************************************/
void M0CORE_IRQHandler(void)
{
  LPC_CREG->M0TXEVENT = 0;
  SYS_ticker++;
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
