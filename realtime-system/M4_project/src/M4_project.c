/******************************************************************************/
/** @file	Emphase_M4_Main.c
    @date	2016-03-09 SSC
		@changes 2020-01-03 KSTR
*******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "sys/nl_coos.h"
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

volatile uint8_t waitForFirstSysTick = 1;

#if 0
void BbbCallback(uint16_t type, uint16_t length, uint16_t* data)
{
	uint32_t param = *data;
	uint32_t value1 = *(data + 1);

	uint32_t value2;

	if (length == 3)
	{
		value2 = *(data + 2);
	}

	if (type == BB_MSG_TYPE_PARAMETER)
	{
		if (length == 3)
		{
			PARAM_Set2(param, value1, value2);
		}
		else
		{
			PARAM_Set(param, value1);
		}
	}
	else if (type == BB_MSG_TYPE_SETTING)
	{
		if (param == 0)							//  Ribbon 1 Mode
		{
			ADC_WORK_SetRibbon1AsEditControl(value1);			// 0: Play Control, 1: Edit Control
		}
	}
}
#endif

void ToggleGpios(void)
{
  static uint8_t tgl = 0;

  if (tgl == 0)
  {
    SPI_BB_TestGpios(0);
    tgl = 1;
  }
  else
  {
    SPI_BB_TestGpios(1);
    tgl = 0;
  }
}

void Task_TestBbbLpc(void)
{
  BB_MSG_WriteMessage2Arg(BB_MSG_TYPE_PARAMETER, 284, 10);

  if (BB_MSG_SendTheBuffer() < 0)
  {
    DBG_Led_Error_On();
    COOS_Task_Add(DBG_Led_Error_Off, 1600, 0);
  }
}

void Init(void)
{
  /* board */
  EMPHASE_V5_M4_Init();

  /* supervisor */
  SUP_Init();

  /* system */
  Emphase_IPC_M4_Init();
  NL_GPDMA_Init(0b11111100);

  /* debug */
  DBG_Init();
  DBG_Led_Error_Off();
  DBG_Led_Cpu_Off();
  DBG_Led_Warning_Off();
  DBG_Led_Audio_Off();
#if 0
    DBG_Pod_0_Off();
    DBG_Pod_1_Off();
    DBG_Pod_2_Off();
    DBG_Pod_3_Off();
    DBG_Pod_4_Off();
    DBG_Pod_5_Off();
    DBG_Pod_6_Off();
    DBG_Pod_7_Off();
#endif

  /* USB */
  USB_MIDI_Init();
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

  /* scheduler */
  COOS_Init();

  COOS_Task_Add(NL_GPDMA_Poll, 10, 1);  // every 125 us, for all the DMA transfers (SPI devices)
  COOS_Task_Add(USB_MIDI_Poll, 15, 1);  // every 125 us, same time grid as in USB 2.0

	COOS_Task_Add(POLY_Process, 20, 1);  // every 125 us, reading and applying keybed events

  COOS_Task_Add(SPI_BB_Polling, 30, 1);  // every 125 us, checking the buffer with messages from the BBB and driving the LPC-BB "heartbeat"

  COOS_Task_Add(ADC_WORK_Init, 50, 0);              // preparing the ADC processing (will be executed after the M0 has been initialized)
  COOS_Task_Add(ADC_WORK_Process, 60, 100);         // every 12.5 ms, reading ADC values and applying changes
  COOS_Task_Add(ADC_WORK_SendBBMessages, 85, 800);  // every 100 ms, sending the results of the ADC processing to the BBB

  COOS_Task_Add(MSG_CheckUSB, 70, 1600);  // every 200 ms, checking if the USB connection to the ePC or the ePC is still working
  COOS_Task_Add(DBG_Process, 80, 4800);   // every 600 ms
  COOS_Task_Add(SUP_Process, 90, SUP_PROCESS_TIMESLICE * 8);
  COOS_Task_Add(HBT_Process, 100, HBT_PROCESS_TIMESLICE * 8);

  /* M0 */
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

void M0CORE_IRQHandler(void)
{
  LPC_CREG->M0TXEVENT = 0;

  waitForFirstSysTick = 0;
  COOS_Update();
}
