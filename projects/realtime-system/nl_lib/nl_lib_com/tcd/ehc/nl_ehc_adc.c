/******************************************************************************/
/** @file		nl_ehc_adc.c
    @date		2020-01-10
    @version	0
    @author		KSTR
    @brief		ADC access for external controllers (pedals etc) processing
    @ingroup	nl_tcd_modules
*******************************************************************************/

#include "nl_ehc_adc.h"
#include "tcd/nl_tcd_adc_work.h"
#include "ipc/emphase_ipc.h"
#include "espi/dev/nl_espi_dev_pedals.h"
#include <stdlib.h>

// =============
// ============= local constants and types
// =============

// ============= ADC channels with sync'd buffers
#define SBUF_MOD          (SBUF_SIZE - 1)  // value for predecrement and masking
#define DEFAULT_ADC_VALUE (2047)           // "default" value, initial dummy input

// IIR lowpass coefficients
#define B0 (6554)
#define B1 (13107)
#define B2 (6554)
#define A1 (0)
#define A2 (6554)

// main working variable
EHC_AdcBuffer_T EHC_adc[ADC_CHANNELS] = {
  // order must be exactly like this !!
  { IPC_ADC_PEDAL1_TIP, IPC_ADC_PEDAL1_DETECT },
  { IPC_ADC_PEDAL1_RING, IPC_ADC_PEDAL1_DETECT },
  { IPC_ADC_PEDAL2_TIP, IPC_ADC_PEDAL2_DETECT },
  { IPC_ADC_PEDAL2_RING, IPC_ADC_PEDAL2_DETECT },
  { IPC_ADC_PEDAL3_TIP, IPC_ADC_PEDAL3_DETECT },
  { IPC_ADC_PEDAL3_RING, IPC_ADC_PEDAL3_DETECT },
  { IPC_ADC_PEDAL4_TIP, IPC_ADC_PEDAL4_DETECT },
  { IPC_ADC_PEDAL4_RING, IPC_ADC_PEDAL4_DETECT },
};

static uint16_t sbuf_index;                            // index of current front element for all sample buffers
static int      sampleBuffersInvalid = SBUF_SIZE * 2;  // flag until buffers are full, after init

/*************************************************************************/ /**
* @brief	InitSampleBuffers and ADCs
******************************************************************************/
void EHC_initSampleBuffers(void)
{
  sampleBuffersInvalid = SBUF_SIZE * 2;
  sbuf_index           = 0;
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    for (int k = 0; k < SBUF_SIZE; k++)
    {
      EHC_adc[i].values[k]          = DEFAULT_ADC_VALUE * AVG_DIV;
      EHC_adc[i].filtered_values[k] = DEFAULT_ADC_VALUE * AVG_DIV;
    }
    EHC_adc[i].flags.pullup_10k  = 0;
    EHC_adc[i].flags.pullup_5V   = 0;
    EHC_adc[i].flags.useIIR      = 0;
    EHC_adc[i].flags.useStats    = 0;
    EHC_adc[i].current           = DEFAULT_ADC_VALUE * AVG_DIV;
    EHC_adc[i].filtered_current  = DEFAULT_ADC_VALUE * AVG_DIV;
    EHC_adc[i].detect            = 0xFF;
    EHC_adc[i].flags.initialized = 1;
  }
}

/*************************************************************************/ /**
* @brief	multiplication for IIR
******************************************************************************/
static inline int multQ15(const int a, const int b)
{
  return (a * b) >> 15;
}

/*************************************************************************/ /**
* @brief	fill sample buffers and do some pre-processing / denoising
******************************************************************************/
void EHC_fillSampleBuffers(void)
{
  sbuf_index = (sbuf_index + SBUF_MOD) & SBUF_MOD;  // pre-decrement ...modulo buffer size

  // read data from current conversion
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    EHC_adc[i].current = EHC_adc[i].values[sbuf_index] = IPC_ReadAdcBufferSum(EHC_adc[i].ipcAdcID);
    EHC_adc[i].detect                                  = IPC_ReadAdcBufferAveraged(EHC_adc[i].ipcDetectID);
  }

  // set PULLUP bits for next conversion
  uint32_t cfg = 0;
  for (int i = 0; i < ADC_CHANNELS; i += 2)
  {
    if (EHC_adc[i].flags.pullup_10k)
      cfg |= (PEDAL_TIP_TO_PULLUP << 8 * (i / 2));
    if (EHC_adc[i + 1].flags.pullup_10k)
      cfg |= (PEDAL_RING_TO_PULLUP << 8 * (i / 2));
  }
  IPC_WritePedalAdcConfig(cfg);

  // IIR lowpass filtering
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    if (EHC_adc[i].flags.useIIR)
    {
      EHC_adc[i].filtered_current = EHC_adc[i].filtered_values[sbuf_index] =       // y[k] =
          multQ15(B0, EHC_adc[i].values[sbuf_index])                               // + B0*x[k]
          + multQ15(B1, EHC_adc[i].values[(sbuf_index + 1) & SBUF_MOD])            // + B1*x[k-1]
          + multQ15(B2, EHC_adc[i].values[(sbuf_index + 2) & SBUF_MOD])            // + B2*x[k-2]
          + multQ15(A1, EHC_adc[i].filtered_values[(sbuf_index + 1) & SBUF_MOD])   // + A1*y[k-1]
          + multQ15(A2, EHC_adc[i].filtered_values[(sbuf_index + 2) & SBUF_MOD]);  // + A2*y[k-2]
    }
    else
      EHC_adc[i].filtered_current = EHC_adc[i].current;
  }

  if (sampleBuffersInvalid)
    --sampleBuffersInvalid;
}

int EHC_sampleBuffersValid(void)
{
  return sampleBuffersInvalid == 0;
}
/*************************************************************************/ /**
* @brief	Get Statistical Data
* @param	return != 0 : success
******************************************************************************/
int EHC_getADCStats(const EHC_AdcBuffer_T *this, int bufferDepth, uint16_t *pMin, uint16_t *pMax, uint16_t *pAvg)
{
  if (!this->flags.initialized || !this->flags.useStats)
    return 0;

  if (bufferDepth < 1)
    bufferDepth = 1;
  if (bufferDepth > SBUF_SIZE)
    bufferDepth = SBUF_SIZE;

  const register uint16_t *buffer = this->flags.useIIR ? this->filtered_values : this->values;
  const register uint16_t  index  = sbuf_index;

  register int      avg = 0;
  register uint16_t max = 0;
  register uint16_t min = 4095 * AVG_DIV;
  for (register int i = bufferDepth - 1; i >= 0; i--)
  {
    register uint16_t sample = buffer[(index + i) & SBUF_MOD];
    avg += sample;
    if (sample > max)
      max = sample;
    if (sample < min)
      min = sample;
  }
  if (pAvg)
    *pAvg = avg / bufferDepth;
  if (pMin)
    *pMin = min;
  if (pMax)
    *pMax = max;
  return bufferDepth;
}

// EOF
