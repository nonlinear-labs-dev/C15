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
#define ILLEGAL_ADC_VALUE (11111)          // "illegal" value way out of range of 0...4095

// IIR lowpass coefficients
#define B0 (6554)
#define B1 (13107)
#define B2 (6554)
#define A1 (0)
#define A2 (6554)

// main working variable
AdcBuffer_T adc[ADC_CHANNELS] = {
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

static uint16_t sbuf_index;                 // index of current front element for all sample buffers
static int      sampleBuffersInvalid = 40;  // flag until buffers are full, after init

/*************************************************************************/ /**
* @brief	InitSampleBuffers and ADCs
******************************************************************************/
void initSampleBuffers(void)
{
  sampleBuffersInvalid = 40;
  sbuf_index           = 0;
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    for (int k = 0; k < SBUF_SIZE; k++)
    {
      adc[i].values[k]          = ILLEGAL_ADC_VALUE;
      adc[i].filtered_values[k] = ILLEGAL_ADC_VALUE;
    }
    adc[i].flags.pullup_10k  = 1;  // force pullup on every pin, initially
    adc[i].flags.pullup_5V   = 0;
    adc[i].flags.useIIR      = 0;
    adc[i].flags.useStats    = 0;
    adc[i].current           = ILLEGAL_ADC_VALUE;
    adc[i].filtered_current  = ILLEGAL_ADC_VALUE;
    adc[i].detect            = 0xFF;
    adc[i].flags.initialized = 1;
  }
}

/*************************************************************************/ /**
* @brief	multiplication for IIR
******************************************************************************/
static inline int multQ15(int a, int b)
{
  return (a * b) >> 15;
}

/*************************************************************************/ /**
* @brief	fill sample buffers and do some pre-processing / denoising
******************************************************************************/
int FillSampleBuffers(void)
{
  sbuf_index = (sbuf_index + SBUF_MOD) & SBUF_MOD;  // pre-decrement ...modulo buffer size

  // read data from current conversion
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    adc[i].current = adc[i].values[sbuf_index] = IPC_ReadAdcBufferAveraged(adc[i].ipcAdcID);
    adc[i].detect                              = IPC_ReadAdcBufferAveraged(adc[i].ipcDetectID);
  }

  // set PULLUP bits for next conversion
  uint32_t cfg = 0;
  for (int i = 0; i < ADC_CHANNELS; i += 2)
  {
    if (adc[i].flags.pullup_10k)
      cfg |= (PEDAL_TIP_TO_PULLUP << 8 * (i / 2));
    if (adc[i + 1].flags.pullup_10k)
      cfg |= (PEDAL_RING_TO_PULLUP << 8 * (i / 2));
  }
  IPC_WritePedalAdcConfig(cfg);

  // IIR lowpass filtering
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    if (adc[i].flags.useIIR)
    {
      adc[i].filtered_current = adc[i].filtered_values[sbuf_index] =           // y[k] =
          multQ15(B0, adc[i].values[sbuf_index])                               // + B0*x[k]
          + multQ15(B1, adc[i].values[(sbuf_index + 1) & SBUF_MOD])            // + B1*x[k-1]
          + multQ15(B2, adc[i].values[(sbuf_index + 2) & SBUF_MOD])            // + B2*x[k-2]
          + multQ15(A1, adc[i].filtered_values[(sbuf_index + 1) & SBUF_MOD])   // + A1*y[k-1]
          + multQ15(A2, adc[i].filtered_values[(sbuf_index + 2) & SBUF_MOD]);  // + A2*y[k-2]
    }
    else
      adc[i].filtered_current = adc[i].current;
  }

  if (sampleBuffersInvalid)
    --sampleBuffersInvalid;
  return !sampleBuffersInvalid;
}

/*************************************************************************/ /**
* @brief	Get Statistical Data
* @param	return != 0 : success
******************************************************************************/
int GetADCStats(AdcBuffer_T *this, int bufferDepth, uint16_t *pMin, uint16_t *pMax, uint16_t *pAvg)
{
  if (!this->flags.initialized)
    return 0;

  if (!this->flags.useStats)
    return this->flags.useIIR ? this->filtered_current : this->current;

  if (bufferDepth < 1)
    bufferDepth = 1;
  if (bufferDepth > SBUF_SIZE)
    bufferDepth = SBUF_SIZE;

  uint16_t *buffer = this->flags.useIIR ? this->filtered_values : this->values;

  int      avg = 0;
  uint16_t max = 0;
  uint16_t min = 4095;
  for (int i = 0; i < bufferDepth; i++)
  {
    uint16_t sample = buffer[(sbuf_index + i) & SBUF_MOD];
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
