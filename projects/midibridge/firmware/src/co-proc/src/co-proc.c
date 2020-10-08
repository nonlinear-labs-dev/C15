/******************************************************************************/
/**	@file 	co-proc.c
	@date	2020-05-27 KSTR
  	@author	2015-01-31 DTZ

  	@note	!!!!!! USE "optimized most", -O3, for compiling !!!!!!

*******************************************************************************/

#pragma GCC diagnostic ignored "-Wmain"

#include <stdint.h>
#include "cmsis/LPC43xx.h"
#include "ipc/emphase_ipc.h"
#include "sys/delays.h"
#include "drv/nl_rit.h"
#include "drv/nl_cgu.h"
#include "drv/nl_gpdma.h"
#include "drv/nl_kbs.h"
#include "sys/nl_version.h"
#include "espi/nl_espi_core.h"
#include "espi/dev/nl_espi_dev_aftertouch.h"
#include "espi/dev/nl_espi_dev_pedals.h"
#include "espi/dev/nl_espi_dev_pitchbender.h"
#include "espi/dev/nl_espi_dev_ribbons.h"
#include "espi/dev/nl_espi_dev_adc.h"
#include "io/pins.h"
#include "version.h"

#define ESPI_MODE_ADC      LPC_SSP0, ESPI_CPOL_0 | ESPI_CPHA_0
#define ESPI_MODE_ATT_DOUT LPC_SSP0, ESPI_CPOL_0 | ESPI_CPHA_0
#define ESPI_MODE_DIN      LPC_SSP0, ESPI_CPOL_1 | ESPI_CPHA_1

static inline uint32_t M4TicksToUS(uint32_t const ticks)
{
  return M4_PERIOD_US * ticks;
}

static uint32_t rand32 = 0x12345678;

static inline uint32_t xorshift_u32(void)
{  // 32 bit xor-shift generator, period 2^32-1, non-zero seed required, range 1...2^32-1
   // source : https://www.jstatsoft.org/index.php/jss/article/view/v008i14/xorshift.pdf
  rand32 ^= rand32 << 13;
  rand32 ^= rand32 >> 17;
  rand32 ^= rand32 << 5;
  return rand32;
}

// ------------------------ clock jitter for ESPI
static int16_t espiCLKDIV;
static void    jitterESPIclk(void)
{
  int16_t offset = (int16_t)((int16_t)(xorshift_u32() & 15) - 8);
  if ((espiCLKDIV + offset) > 1 && (espiCLKDIV + offset) < 255)
    ESPI_SetCRDIV((uint16_t)(espiCLKDIV + offset));
}

// ---------------- clock jitter for timer IRQ
static inline void jitterIRQ(void)
{
  RIT_SetCompVal((NL_LPC_CLK / M0_FREQ_HZ) - 32 + (xorshift_u32() & 63));  // offset clock by 2^6(+-32) clock periods
}

/******************************************************************************/
/** @note	Espi device functions do NOT switch mode themselves!
 	 	 	espi bus speed: 2.0 MHz -> 0.500 µs              Bytes    t_µs
                                                   POL/PHA | multi | t_µs_sum
--------------------------------------------------------------------------------
P1D1   ADC-2CH     ribbon_board         MCP3202   R/W  0/1   3   2   12  24
P1D2   ADC-1CH     pitch_bender_board   MCP3201   R    0/1   3   1   12  12
--------------------------------------------------------------------------------
P0D1   1CH-ADC     aftertouch_board     MCP3201   R    0/1   3   1   12  12
--------------------------------------------------------------------------------
P2D2   ATTENUATOR  pedal_audio_board    LM1972    W    0/0   2   2    8  16 (unused)
--------------------------------------------------------------------------------
P3D1   ADC-1CH     volume_poti_board    MCP3201   R    0/1   3   1   12  12 (unused)
--------------------------------------------------------------------------------
P4D1   ADC-8CH     pedal_audio_board    MCP3208   R/W  0/1   3   4   12  48
P4D2   DETECT      pedal_audio_board    HC165     R    1/1   1   1    4   4
P4D3   SET_PULL_R  pedal_audio_board    HC595     W    0/0   1   1    4   4
--------------------------------------------------------------------------------
                                                                 13      132
*******************************************************************************/
static uint32_t compensatingTicks = 0;

static inline void adjustedWait(void)
{
  for (uint32_t i = 0; i < compensatingTicks; i++)
    DELAY_TEN_CLK_CYCLES;  // something like 70ns on average, depending on IRQ load
}

static void ProcessADCs(void)
{
  // This process is free-running, not controlled by a timer IRQ, but with a feedback
  // loop to keep cycle time close to the required value.
  // Because of the adaptive action of the feedback loop, and because the process
  // is interrupted by the keybed-scanner many times at arbitrary points, the used
  // hardware lines will have jitter which is a good thing for EMC.
  // To increase that jitter (to spread RF frequency components), the SPI DMA clock
  // is slightly varied for each SPI burst access.

  static uint32_t totalTicks = 0;
  static uint32_t hbLedCnt   = 0;
  static uint8_t  first      = 1;
  int32_t         val;
  uint32_t        savedTicks;

  if (first)
  {
    first      = 0;
    totalTicks = s.ticker;
  }
  else
  {
    // Feedback loop to adjust the cycle time to collect all the ADC data
    // 16 times per 12.5ms ADC cycle, 781us.
    // The feedback is slow, only about +-700ns of adjustment per run of this process routine.
    // This is because the ADC readout from M4 side is with 16-fold averaging
    // which assumes we have 16 values collected during the 12.5ms.
    // The feedback loop is protected from trying to make the cycle time shorter
    // than physically possible, determined by interrupt load.
    // The actual waiting time is distributed across the whole function in 10 places
    // to increase jitter (good for EMC)
    IPC_SetADCTime(savedTicks = (s.ticker - totalTicks));
    totalTicks = s.ticker;

    if (!savedTicks)  // M4 seems to have stalled
      compensatingTicks = 0;
    else
    {
      if (M4TicksToUS(savedTicks) < 12500 / 16 - 7)  // cycle was faster than 16 rounds per 12.5ms (the 7 is a fudge factor)?
        compensatingTicks++;
      else
      {
        if (compensatingTicks)  // cycle time can be shortened ?
          compensatingTicks--;
      }
    }
  }

  // now, all adc channel & detect values have been read ==> sync read index to last write index
  IPC_AdcUpdateReadIndex();
  // Starting a new round of adc channel & detect value read-ins, advance ipc write index first
  IPC_AdcBufferWriteNext();

  jitterESPIclk();
  adjustedWait();
  // switch mode: 13.6 µs (NOTE: Timing not up to date, might be about 20% faster now)
  SPI_DMA_SwitchMode(ESPI_MODE_ADC);
  NL_GPDMA_Poll();
  // do heartbeat LED here, enough time
  hbLedCnt++;
  switch (hbLedCnt)
  {
    case 1:
      ledM0heartbeat = 1;
      break;
    case 300:
      ledM0heartbeat = 0;
      break;
    case 600:
      hbLedCnt = 0;
      break;
    default:
      break;
  }

  jitterESPIclk();
  adjustedWait();
  // pedal 1 : 57 µs (NOTE: Timing not up to date, might be about 20% faster now)
  ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_1_ADC_RING);
  NL_GPDMA_Poll();
  val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_1_ADC_RING);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL1_RING, val);

  ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_1_ADC_TIP);
  NL_GPDMA_Poll();
  val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_1_ADC_TIP);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL1_TIP, val);

  jitterESPIclk();
  adjustedWait();
  // pedal 2 : 57 µs (NOTE: Timing not up to date, might be about 20% faster now)
  ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_2_ADC_RING);
  NL_GPDMA_Poll();
  val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_2_ADC_RING);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL2_RING, val);

  ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_2_ADC_TIP);
  NL_GPDMA_Poll();
  val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_2_ADC_TIP);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL2_TIP, val);

  jitterESPIclk();
  adjustedWait();
  // pedal 3 : 57 µs (NOTE: Timing not up to date, might be about 20% faster now)
  ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_3_ADC_RING);
  NL_GPDMA_Poll();
  val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_3_ADC_RING);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL3_RING, val);

  ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_3_ADC_TIP);
  NL_GPDMA_Poll();
  val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_3_ADC_TIP);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL3_TIP, val);

  jitterESPIclk();
  adjustedWait();
  // pedal 4 : 57 µs (NOTE: Timing not up to date, might be about 20% faster now)
  ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_4_ADC_RING);
  NL_GPDMA_Poll();
  val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_4_ADC_RING);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL4_RING, val);

  ESPI_DEV_Pedals_EspiPullChannel_Blocking(ESPI_PEDAL_4_ADC_TIP);
  NL_GPDMA_Poll();
  val = ESPI_DEV_Pedals_GetValue(ESPI_PEDAL_4_ADC_TIP);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL4_TIP, val);

  jitterESPIclk();
  adjustedWait();
  // detect pedals: 32.5 µs (NOTE: Timing not up to date, might be about 20% faster now)
  SPI_DMA_SwitchMode(ESPI_MODE_DIN);
  NL_GPDMA_Poll();

  ESPI_DEV_Pedals_Detect_EspiPull();
  NL_GPDMA_Poll();

  uint8_t detect = ESPI_DEV_Pedals_Detect_GetValue();
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL4_DETECT, ((detect & 0b00010000) >> 4) ? 4095 : 0);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL3_DETECT, ((detect & 0b00100000) >> 5) ? 4095 : 0);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL2_DETECT, ((detect & 0b01000000) >> 6) ? 4095 : 0);
  IPC_WriteAdcBuffer(IPC_ADC_PEDAL1_DETECT, ((detect & 0b10000000) >> 7) ? 4095 : 0);

  jitterESPIclk();
  adjustedWait();
  // set pull resistors: best case: 17.3 µs - worst case: 36 µs (NOTE: Timing not up to date, might be about 20% faster now)
  SPI_DMA_SwitchMode(ESPI_MODE_ATT_DOUT);
  NL_GPDMA_Poll();

  uint32_t config = IPC_ReadPedalAdcConfig();
  ESPI_DEV_Pedals_SetPedalState(1, (uint8_t)((config >> 0) & 0xFF));
  ESPI_DEV_Pedals_SetPedalState(2, (uint8_t)((config >> 8) & 0xFF));
  ESPI_DEV_Pedals_SetPedalState(3, (uint8_t)((config >> 16) & 0xFF));
  ESPI_DEV_Pedals_SetPedalState(4, (uint8_t)((config >> 24) & 0xFF));

  ESPI_DEV_Pedals_PullResistors_EspiSendIfChanged();

  NL_GPDMA_Poll();

  jitterESPIclk();
  adjustedWait();
  // pitchbender: 42 µs (NOTE: Timing not up to date, might be about 20% faster now)
  SPI_DMA_SwitchMode(ESPI_MODE_ADC);
  NL_GPDMA_Poll();

  ESPI_DEV_Pitchbender_EspiPull();
  NL_GPDMA_Poll();

  IPC_WriteAdcBuffer(IPC_ADC_PITCHBENDER, ESPI_DEV_Pitchbender_GetValue());

  jitterESPIclk();
  adjustedWait();
  // aftertouch: 29 µs (NOTE: Timing not up to date, might be about 20% faster now)
  ESPI_DEV_Aftertouch_EspiPull();
  NL_GPDMA_Poll();

  IPC_WriteAdcBuffer(IPC_ADC_AFTERTOUCH, ESPI_DEV_Aftertouch_GetValue());

  jitterESPIclk();
  adjustedWait();
  // 2 ribbons: 57 µs (NOTE: Timing not up to date, might be about 20% faster now)
  ESPI_DEV_Ribbons_EspiPull_Upper();
  NL_GPDMA_Poll();
  IPC_WriteAdcBuffer(IPC_ADC_RIBBON1, ESPI_DEV_Ribbons_GetValue(UPPER_RIBBON));

  ESPI_DEV_Ribbons_EspiPull_Lower();
  NL_GPDMA_Poll();
  IPC_WriteAdcBuffer(IPC_ADC_RIBBON2, ESPI_DEV_Ribbons_GetValue(LOWER_RIBBON));
}

/******************************************************************************/
volatile char dummy;

void dummyFunction(const char* string)
{
  while (*string)
  {
    dummy = *string++;
  }
}

void main(void)
{
  // referencing the version string so compiler won't optimize it away
  dummyFunction(VERSION_STRING);
  dummyFunction(GetC15Version());
  dummyFunction(GetC15Build());

  Emphase_IPC_Init();
  KBS_Init();
  NL_GPDMA_Init(0b00000011);  // inverse to the mask in the M4_Main

  // Increased from 1.6 to 2MHz clock freq... works up to ~5MHz in V7.1 hardware.
  // With 2MHz, max M0 ADC scanning cycle time is reduced by about 15%
  ESPI_Init(2000000);
  espiCLKDIV = (int16_t) ESPI_GetCRDIV();

  ESPI_DEV_Pedals_Init();
  ESPI_DEV_Aftertouch_Init();
  ESPI_DEV_Pitchbender_Init();
  ESPI_DEV_Ribbons_Init();

  RIT_Init_IntervalInHz(M0_FREQ_HZ);

  while (1)
    ProcessADCs();
}

/******************************************************************************/

// __attribute__((section(".ramfunc")))
void M0_RIT_OR_WWDT_IRQHandler(void)
{
  // Clear interrupt flag early, to allow for short, single cycle IRQ overruns,
  // that is if, the keybed scanner occasionally takes longer than one time slot
  // (but no more than two), the interrupt routine is invoked again as soon as
  // it completed the first pass. By this, no IRQ is lost, albeit the ticker
  // used in KBS_Process for key timing will have a little bit of jitter.
  // Only when eeprom memory access etc slows down the bus there is a slight
  // chance of a burst of overruns where the M0 main process hardly gets any
  //  execution time if the IRQ execution time is close to IRQ repeat rate already.
  RIT_ClearInt();
  jitterIRQ();
  (*KBS_Process)();
  s.RitCrtlReg |= RIT_GetCtrlReg();  // note that this profiling increases IRQ time itself (Heisenberg ;-)
}
