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
#include "espi/dev/nl_espi_dev_adc.h"

#define M0_SYSTICK_IN_NS      2500  // 2.5us
#define M0_SYSTICK_MULTIPLIER 50    // 2.5s*50 = 125us --> triggers Timer Interrupt of M4

#define M0_DEBUG 0

#define ESPI_MODE_ADC      LPC_SSP0, ESPI_CPOL_0 | ESPI_CPHA_0
#define ESPI_MODE_ATT_DOUT LPC_SSP0, ESPI_CPOL_0 | ESPI_CPHA_0
#define ESPI_MODE_DIN      LPC_SSP0, ESPI_CPOL_1 | ESPI_CPHA_1

static volatile uint8_t scheduler = 0;
static volatile uint8_t keybed    = 0;

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
  static uint8_t  state    = 0;
  static uint32_t hbLedCnt = 0;
  int32_t         val;
  switch (state)
  {
    case 0:  // switch mode: 13.6 µs
             // now, all adc channel & detect values have been read ==> sync read index to write index
      IPC_AdcUpdateReadIndex();
      // Starting a new round of adc channel & detect value read-ins, advance ipc write index first
      IPC_AdcBufferWriteNext();

      SPI_DMA_SwitchMode(ESPI_MODE_ADC);
      NL_GPDMA_Poll();

      // do heartbeat LED here, enough time
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

    case 1:  // pedal 1 : 57 µs
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

    case 2:  // pedal 2 : 57 µs
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

    case 3:  // pedal 3 : 57 µs
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

    case 4:  // pedal 4 : 57 µs
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

    case 5:  // detect pedals: 32.5 µs
      SPI_DMA_SwitchMode(ESPI_MODE_DIN);
      NL_GPDMA_Poll();

      ESPI_DEV_Pedals_Detect_EspiPull();
      NL_GPDMA_Poll();

      uint8_t detect = ESPI_DEV_Pedals_Detect_GetValue();
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL4_DETECT, ((detect & 0b00010000) >> 4) ? 4095 : 0);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL3_DETECT, ((detect & 0b00100000) >> 5) ? 4095 : 0);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL2_DETECT, ((detect & 0b01000000) >> 6) ? 4095 : 0);
      IPC_WriteAdcBuffer(IPC_ADC_PEDAL1_DETECT, ((detect & 0b10000000) >> 7) ? 4095 : 0);

      break;

    case 6:  // set pull resistors: best case: 17.3 µs - worst case: 36 µs
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

    case 7:  // pitchbender: 42 µs
      SPI_DMA_SwitchMode(ESPI_MODE_ADC);
      NL_GPDMA_Poll();

      ESPI_DEV_Pitchbender_EspiPull();
      NL_GPDMA_Poll();

      IPC_WriteAdcBuffer(IPC_ADC_PITCHBENDER, ESPI_DEV_Pitchbender_GetValue());
      break;

    case 8:  // aftertouch: 29 µs
      ESPI_DEV_Aftertouch_EspiPull();
      NL_GPDMA_Poll();

      IPC_WriteAdcBuffer(IPC_ADC_AFTERTOUCH, ESPI_DEV_Aftertouch_GetValue());
      break;

    case 9:  // 2 ribbons: 57 µs
      ESPI_DEV_Ribbons_EspiPull_Upper();
      NL_GPDMA_Poll();
      IPC_WriteAdcBuffer(IPC_ADC_RIBBON1, ESPI_DEV_Ribbons_GetValue(UPPER_RIBBON));

      ESPI_DEV_Ribbons_EspiPull_Lower();
      NL_GPDMA_Poll();
      IPC_WriteAdcBuffer(IPC_ADC_RIBBON2, ESPI_DEV_Ribbons_GetValue(LOWER_RIBBON));

      break;

    default:
      break;
  }

  state++;
  if (state == 10)
    state = 0;
}

/******************************************************************************/
int main(void)
{
  EMPHASE_V5_M0_Init();

  Emphase_IPC_Init();

  NL_GPDMA_Init(0b00000011);  // inverse to the mask in the M4_Main

  // 20MHz clock freq... works up to ~50MHz in V7.1 hardware (AT not checked, though).
  // With 20MHz, max M0 cycle time seems to be just under 50us even worst case, so
  // overruns (when > 60us) is unlikely, the scheduling always stays in sync, no missed
  // time slices
  ESPI_Init(2000000);

  ESPI_DEV_Pedals_Init();
  ESPI_DEV_Aftertouch_Init();
  ESPI_DEV_Pitchbender_Init();
  ESPI_DEV_Ribbons_Init();

  RIT_Init_IntervalInNs(M0_SYSTICK_IN_NS);

  uint16_t ticks;
  while (1)
  {
    if (scheduler)
    {
      ticks = IPC_GetTimer();
      Scheduler();
      IPC_SetSchedulerTime((uint16_t)(IPC_GetTimer() - ticks + 1));
      scheduler = 0;
    }
    if (keybed)
    {
      ticks = IPC_GetTimer();
      KBS_Process();  // keybed scanner: 51.6 µs best case - 53.7 µs worst case
      IPC_SetKBSTime((uint16_t)(IPC_GetTimer() - ticks + 1));
      keybed = 0;
    }
  }

  return 0;
}

/******************************************************************************/
static inline void SendInterruptToM4(void)
{
  __DSB();
  __SEV();
}

/******************************************************************************/
static volatile uint8_t sysTickMultiplier = 0;
void                    M0_RIT_OR_WWDT_IRQHandler(void)
{
  RIT_ClearInt();
  IPC_IncTimer();

  sysTickMultiplier++;

  // We wake up M4 at the beginning of the even state and hope it has the critical
  // section in POLY_Process (where Emphase_IPC_M4_KeyBuffer_ReadBuffer() is called)
  // completed before we enter our critical section in Emphase_IPC_M0_KeyBuffer_WriteKeyEvent()
  if (sysTickMultiplier == M0_SYSTICK_MULTIPLIER / 2 /* 62.5us */)
  {  // Wake up M4 which soon starts POLY_Process, and do non-critical stuff in the scheduler
    SendInterruptToM4();
    if (!scheduler)
      scheduler = 1;
    // else  // overrun, not likely to happen
    // DBG_Led_Warning_On();
  }

  if (sysTickMultiplier == M0_SYSTICK_MULTIPLIER /* 125us */)
  {  // Process the keybed, M4 will have completed POLY_Process() by this point in time
    sysTickMultiplier = 0;
    if (!keybed)
      keybed = 1;
    // else  // overrun, not likely to happen
    // DBG_Led_Warning_On();
  }
}
