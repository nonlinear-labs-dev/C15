/******************************************************************************/
/**	@file 	M0_IPC_Test.c
	@date	2016-03-09 SSC
  	@author	2015-01-31 DTZ

	@brief	see the M4_IPC_Test.c file for the documentation

  	@note	!!!!!! USE optimized most -O3 for compiling !!!!!!

*******************************************************************************/
#include <stdint.h>
#include "cmsis/LPC43xx.h"

#include "sys/delays.h"

#include "boards/emphase_v5.h"

#include "ipc/emphase_ipc.h"

#include "drv/nl_rit.h"
#include "drv/nl_dbg.h"
#include "drv/nl_gpdma.h"
#include "drv/nl_kbs.h"

#include "espi/nl_espi_core.h"

#include "espi/dev/nl_espi_dev_aftertouch.h"
#include "espi/dev/nl_espi_dev_pedals.h"
#include "espi/dev/nl_espi_dev_pitchbender.h"
#include "espi/dev/nl_espi_dev_ribbons.h"

#define M0_SYSTICK_IN_NS      62500  // 62.5us
#define M0_SYSTICK_MULTIPLIER 2      // 62.5us*2 = 125us --> triggers Timer Interrupt of M4

#define PENDING_INTERRUPT 1
#define SCHED_FINISHED    0

#define M0_DEBUG 0

#define ESPI_MODE_ADC      LPC_SSP0, ESPI_CPOL_0 | ESPI_CPHA_0
#define ESPI_MODE_ATT_DOUT LPC_SSP0, ESPI_CPOL_0 | ESPI_CPHA_0
#define ESPI_MODE_DIN      LPC_SSP0, ESPI_CPOL_1 | ESPI_CPHA_1
// ID's for the low-level fetch routines
// DO NOT CHANGE !!
#define ESPI_PEDAL_1_ADC_RING 7
#define ESPI_PEDAL_1_ADC_TIP  6
#define ESPI_PEDAL_2_ADC_RING 5
#define ESPI_PEDAL_2_ADC_TIP  4
#define ESPI_PEDAL_3_ADC_RING 3
#define ESPI_PEDAL_3_ADC_TIP  2
#define ESPI_PEDAL_4_ADC_RING 1
#define ESPI_PEDAL_4_ADC_TIP  0
#define ESPI_PITCHBENDER_ADC 8
#define ESPI_AFTERTOUCH_ADC  9
#define ESPI_RIBBON_1_ADC    10
#define ESPI_RIBBON_2_ADC    11


static volatile uint8_t stateFlag = 0;

void SendInterruptToM4(void);

/******************************************************************************/
/** @note	Espi device functions do NOT switch mode themselves!
 	 	 	espi bus speed: 1.6 MHz -> 0.625 µs                    Bytes    t_µs
                                                     POL/PHA | multi | t_µs_sum
--------------------------------------------------------------------------------
P1D1   ADC-2CH     ribbon_board         MCP3202   R/W  0/1   3   2   15  30
P1D2   ADC-1CH     pitch_bender_board   MCP3201   R    0/1   3   1   15  15
--------------------------------------------------------------------------------
P0D1   1CH-ADC     aftertouch_board     MCP3201   R    0/1   3   1   15  15
--------------------------------------------------------------------------------
P2D2   ATTENUATOR  pedal_audio_board    LM1972    W    0/0   2   2   10  20
--------------------------------------------------------------------------------
P3D1   ADC-1CH     volume_poti_board    MCP3201   R    0/1   3   1   15  15
--------------------------------------------------------------------------------
P4D1   ADC-8CH     pedal_audio_board    MCP3208   R/W  0/1   3   4   15  60
P4D2   DETECT      pedal_audio_board    HC165     R    1/1   1   1    5   5
P4D3   SET_PULL_R  pedal_audio_board    HC595     W    0/0   1   1    5   5
--------------------------------------------------------------------------------
                                                                13      165
*******************************************************************************/

void Scheduler(void)
{
  static uint8_t state = 0;

  int32_t val;

  switch (state)
  {
    case 1:  // keybed scanner: 51.6 µs best case - 53.7 µs worst case
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15:
    case 17:
    case 19:
    case 21:
    case 23:
    {
      KBS_Process();
      break;
    }

    case 0:  // switch mode: 13.6 µs
    {
      SPI_DMA_SwitchMode(ESPI_MODE_ADC);
      NL_GPDMA_Poll();

      // do heartbeat LED here, enough time
      static uint32_t hbLedCnt = 0;
      hbLedCnt++;
      switch (hbLedCnt)
      {
        case 1:
          DBG_Led_Cpu_On();
          break;
        case 201:
          DBG_Led_Cpu_Off();
          break;
        case 400:
          hbLedCnt = 0;
          break;
        default:
          break;
      }

      break;
    }

    case 2:  // pedal 1 : 57 µs
    {
       // Starting a new round of adc channel & detect value read-ins, advance ipc write index first
    	IPC_AdcBufferWriteNext();

#if M0_DEBUG
      DBG_GPIO3_2_On();
#endif
      ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_1_ADC_RING);
      NL_GPDMA_Poll();
      val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_1_ADC_RING);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL1_RING, val);

      ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_1_ADC_TIP);
      NL_GPDMA_Poll();
      val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_1_ADC_TIP);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL1_TIP, val);
#if M0_DEBUG
      DBG_GPIO3_2_Off();
#endif
      break;
    }

    case 4:  // pedal 2 : 57 µs
    {
#if M0_DEBUG
      DBG_GPIO3_2_On();
#endif
      ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_2_ADC_RING);
      NL_GPDMA_Poll();
      val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_2_ADC_RING);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL2_RING, val);

      ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_2_ADC_TIP);
      NL_GPDMA_Poll();
      val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_2_ADC_TIP);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL2_TIP, val);
#if M0_DEBUG
      DBG_GPIO3_2_Off();
#endif
      break;
    }

    case 6:  // pedal 3 : 57 µs
    {
#if M0_DEBUG
      DBG_GPIO3_2_On();
#endif
      ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_3_ADC_RING);
      NL_GPDMA_Poll();
      val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_3_ADC_RING);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL3_RING, val);

      ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_3_ADC_TIP);
      NL_GPDMA_Poll();
      val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_3_ADC_TIP);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL3_TIP, val);
#if M0_DEBUG
      DBG_GPIO3_2_Off();
#endif
      break;
    }

    case 8:  // pedal 4 : 57 µs
    {
#if M0_DEBUG
      DBG_GPIO3_2_On();
#endif
      ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_4_ADC_RING);
      NL_GPDMA_Poll();
      val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_4_ADC_RING);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL4_RING, val);

      ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_4_ADC_TIP);
      NL_GPDMA_Poll();
      val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_4_ADC_TIP);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL4_TIP, val);
#if M0_DEBUG
      DBG_GPIO3_2_Off();
#endif
      break;
    }

    case 10:  // detect pedals: 32.5 µs
    {
      SPI_DMA_SwitchMode(ESPI_MODE_DIN);
      NL_GPDMA_Poll();

      ESPI_DEV_Pedals_Detect_EspiPull();
      NL_GPDMA_Poll();

      uint8_t detect = ESPI_DEV_Pedals_Detect_GetValue();
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL4_DETECT, ((detect & 0b00010000) >> 4) << 12);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL3_DETECT, ((detect & 0b00100000) >> 5) << 12);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL2_DETECT, ((detect & 0b01000000) >> 6) << 12);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL1_DETECT, ((detect & 0b10000000) >> 7) << 12);

      break;
    }

    case 12:  // set pull resistors: best case: 17.3 µs - worst case: 36 µs
    {
      SPI_DMA_SwitchMode(ESPI_MODE_ATT_DOUT);
      NL_GPDMA_Poll();

      uint32_t config = IPC_ReadPedalAdcConfig();
      ESPI_DEV_Pedals_SetPedalState(1, (uint8_t)((config >> 0) & 0xFF));
      ESPI_DEV_Pedals_SetPedalState(2, (uint8_t)((config >> 8) & 0xFF));
      ESPI_DEV_Pedals_SetPedalState(3, (uint8_t)((config >> 16) & 0xFF));
      ESPI_DEV_Pedals_SetPedalState(4, (uint8_t)((config >> 24) & 0xFF));

      ESPI_DEV_Pedals_PullResistors_EspiSendIfChanged();

      NL_GPDMA_Poll();
      break;
    }

    case 14:  // pitchbender: 42 µs
    {
      SPI_DMA_SwitchMode(ESPI_MODE_ADC);
      NL_GPDMA_Poll();

      ESPI_DEV_Pitchbender_EspiPull();
      NL_GPDMA_Poll();

      IPC_WriteAdcBuffer(IPC_ADC_PITCHBENDER, ESPI_DEV_Pitchbender_GetValue());
      break;
    }

    case 16:  // aftertouch: 29 µs
    {
      ESPI_DEV_Aftertouch_EspiPull();
      NL_GPDMA_Poll();

      IPC_WriteAdcBuffer(IPC_ADC_AFTERTOUCH, ESPI_DEV_Aftertouch_GetValue());
      break;
    }

    case 18:  // 2 ribbons: 57 µs
    {
      ESPI_DEV_Ribbons_EspiPull_Upper();
      NL_GPDMA_Poll();
      IPC_WriteAdcBuffer(IPC_ADC_RIBBON1, ESPI_DEV_Ribbons_GetValue(UPPER_RIBBON));

      ESPI_DEV_Ribbons_EspiPull_Lower();
      NL_GPDMA_Poll();
      IPC_WriteAdcBuffer(IPC_ADC_RIBBON2, ESPI_DEV_Ribbons_GetValue(LOWER_RIBBON));

      // now, all adc channel & detect values have been read ==> sync read index to write index
      IPC_AdcUpdateReadIndex();
      break;
    }

    default:
      break;
  }

  state++;

  if (state == 20)
    state = 0;

  stateFlag = SCHED_FINISHED;
}

/******************************************************************************/

int main(void)
{
  EMPHASE_V5_M0_Init();

  Emphase_IPC_Init();

  NL_GPDMA_Init(0b00000011);  /// inverse to the mask in the M4_Main

  ESPI_Init(1600000);

  ESPI_DEV_Pedals_Init();
  ESPI_DEV_Aftertouch_Init();
  ESPI_DEV_Pitchbender_Init();
  ESPI_DEV_Ribbons_Init();

  RIT_Init_IntervalInNs(M0_SYSTICK_IN_NS);

  while (1)
  {
    if (stateFlag == PENDING_INTERRUPT)
    {
      Scheduler();
    }
  }

  return 0;
}

/******************************************************************************/
void M0_RIT_OR_WWDT_IRQHandler(void)
{
  RIT_ClearInt();

  static uint8_t sysTickMultiplier = 0;
  sysTickMultiplier++;

  if (sysTickMultiplier == M0_SYSTICK_MULTIPLIER)
  {
    SendInterruptToM4();
  }

  if (sysTickMultiplier == 2)
    sysTickMultiplier = 0;

  if (stateFlag == SCHED_FINISHED)
  {
    stateFlag = PENDING_INTERRUPT;
  }
  else
  {
    DBG_Led_Warning_On();
  }
}

/******************************************************************************/
void SendInterruptToM4(void)
{
  __DSB();
  __SEV();
}
