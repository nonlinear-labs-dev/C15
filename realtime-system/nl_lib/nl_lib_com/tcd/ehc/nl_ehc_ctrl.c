/******************************************************************************/
/** @file		nl_ehc_ctrl.c
    @date		2020-01-10
    @version	0
    @author		KSTR
    @brief		handlers for external controllers (pedals etc) processing
    @ingroup	nl_tcd_modules
*******************************************************************************/

#include "nl_ehc_ctrl.h"
#include "nl_ehc_adc.h"
#include "tcd/nl_tcd_adc_work.h"
#include "spibb/nl_bb_msg.h"
#include "tcd/nl_tcd_msg.h"
#include "drv/nl_dbg.h"
#include <stdlib.h>

// =============
// ============= local constants and types
// =============

// ============= Pot channels
#define POT_SCALE_FACTOR  (10000)  // don't change this unless you know what you do
#define RHEO_SCALE_FACTOR (4500)   // don't change this unless you know what you do
// autoranging
#define AR_NOISE_LIMIT     (30)   // 100 -> 1% (assuming SCALE_FACTOR is 10000 !!)
#define AR_SPAN            (500)  // 500 -> 5% (assuming SCALE_FACTOR is 10000 !!)
#define AR_SPAN_RHEO       (200)  // 200 -> 2.0 (max/min minimum factor)
#define AR_UPPER_DEAD_ZONE (4)    // 4 -> 4%
#define AR_LOWER_DEAD_ZONE (4)    // 4 -> 4%

// settling
#define CBUF_SIZE (8)  // 2^N !!! Floating Average is used based on this size
#define CBUF_MOD  (CBUF_SIZE - 1)

// Offset: settling minimum in LSBs.
// Stable/low-creepage pedals are OK with 3..10, bad pedals (Moog) may need 20..50
// The higher the value, the more there is a step transition on a change, after settling
#warning "ToDo : make this parametrizable. Like 3 10dB steps 'Pedal Auto-Stabilizing' : weak(4), normal(13), strong(40)"
// This mainly important when the pedal is shared with a ribbon, noise (from vibration, creep) shall not overwrite
// the ribbon value(but even when the pedal is the only modulator, too much noise is not ideal).
// Therefore, the PG should temporarily increase the stabilizing level by one when pedal and ribbon / other pedal run shared.
#define SETTLING_OFFSET (4)
// gain factor for how many LSBs to add at fullscale (this is because the ADC has more noise at larger values
#define SETTLING_GAIN (8)

typedef struct
{
  ControllerType_T type;
  int              HwSourceId;  // Id for AE and UI "changed value event" messages
  AdcBuffer_T *    wiper;       // pot (or rheostat) wiper
  AdcBuffer_T *    top;         // pot top (not used for rheostat)
  struct
  {
    // output
    unsigned initialized : 1;
    unsigned isSettled : 1;
    // input
    unsigned denoiseWhenSettled : 1;  // 1 --> denoise output when settled long and close enough
  } flags;
  // value buffer
  uint16_t values[CBUF_SIZE];
  uint16_t val_index;
  uint16_t valueBufInvalid;
  // output buffer
  uint16_t out[CBUF_SIZE];
  uint16_t out_index;
  uint16_t outBufInvalid;

  // autoranging
  uint16_t min;
  uint16_t max;
  // settling
  uint16_t settledValue;
  // final HW-source output
  uint16_t final;
  uint16_t lastFinal;
} Controller_T;

Controller_T ctrl[4];  // main working variable

/*************************************************************************/ /**
* @brief	"changed" event : send value to AudioEngine and UI
******************************************************************************/
void SendControllerData(uint32_t hwSourceId, uint32_t value)
{
  MSG_HWSourceUpdate(hwSourceId, value);
  ADC_WORK_WriteHWValueForBB(hwSourceId, value);
}

/*************************************************************************/ /**
* @brief	Init a controller
******************************************************************************/
void initController(Controller_T *this, int HwSourceId, AdcBuffer_T *wiper, AdcBuffer_T *top, ControllerType_T type)
{
  this->type                     = type;
  this->HwSourceId               = HwSourceId;
  this->min                      = 32767;
  this->max                      = 0;
  this->wiper                    = wiper;
  this->top                      = NULL;
  this->flags.denoiseWhenSettled = 0;
  this->val_index                = 0;
  this->out_index                = 0;

  switch (type)
  {
    case POT:
      this->top                      = top;
      this->top->flags.pullup_10k    = 1;  // apply pullup to top (input pin)
      this->top->flags.useIIR        = 1;  // low pass filter the raw input
      this->top->flags.useStats      = 1;  // enable min/max/avg statistics
      this->wiper->flags.pullup_10k  = 0;  // readout wiper without pullup;
      this->wiper->flags.useIIR      = 1;  // low pass filter the raw input
      this->wiper->flags.useStats    = 1;  // enable min/max/avg statistics
      this->flags.denoiseWhenSettled = 1;
      break;
    case RHEOSTAT:
      this->wiper->flags.pullup_10k  = 1;  // readout wiper with pullup;
      this->wiper->flags.useIIR      = 1;  // low pass filter the raw input
      this->wiper->flags.useStats    = 1;  // enable min/max/avg statistics
      this->flags.denoiseWhenSettled = 1;
      break;
    case SWITCH:
      this->wiper->flags.pullup_10k = 1;  // readout wiper with pullup;
      this->wiper->flags.useIIR     = 0;  // do NOT low pass filter the raw input
      this->wiper->flags.useStats   = 0;  // DISable min/max/avg statistics
      break;
    case CV:
      this->wiper->flags.pullup_10k = 0;  // readout wiper without pullup;
      this->wiper->flags.useIIR     = 1;  // low pass filter the raw input
      this->wiper->flags.useStats   = 0;  // DISable min/max/avg statistics
      break;
    case UNKNOWN:
    default:
      this->flags.initialized = 0;
      return;
  }

  this->valueBufInvalid = SBUF_SIZE;  // number of runs until value buffer is filled
  this->outBufInvalid   = SBUF_SIZE;  // number of runs until value buffer is filled

  this->final             = 0;
  this->lastFinal         = 0xFFFF;
  this->flags.initialized = 1;
}

/*************************************************************************/ /**
* @brief	Get Statistical Data of a Ring Buffer
* @param	return != 0 : success
******************************************************************************/
int GetBufferStats(uint16_t *buf, uint16_t pos, int bufferDepth, uint16_t *pMin, uint16_t *pMax, uint16_t *pAvg)
{
  if (buf == NULL)
    return 0;

  if (bufferDepth < 1)
    bufferDepth = 1;
  if (bufferDepth > SBUF_SIZE)
    bufferDepth = SBUF_SIZE;

  int      avg = 0;
  uint16_t max = 0;
  uint16_t min = 65535;
  for (int i = 0; i < bufferDepth; i++)
  {
    uint16_t sample = buf[(pos + i) & CBUF_MOD];
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

/*************************************************************************/ /**
* @brief	readout a pot or rheostat, with optional denoising
******************************************************************************/
void readoutController(Controller_T *this)
{
  if (!this->flags.initialized || (this->type != POT && this->type != RHEOSTAT))
    return;

  // get potentiometric value
  int value;
  this->val_index = (this->val_index + CBUF_MOD) & CBUF_MOD;
  if (this->type == POT)
  {
    value = this->values[this->val_index] = POT_SCALE_FACTOR * this->wiper->filtered_current / this->top->filtered_current;
  }
  else  // rheostat
  {
    value = this->wiper->filtered_current;
    if (value > 4000)
      value = 4000;
    value = RHEO_SCALE_FACTOR * value / (4096 - value);
    if (value > 60000)  // limit to uint16 range
      value = 60000;
    this->values[this->val_index] = value;
  }

  // wiper buffer filled ?
  if (this->valueBufInvalid)
  {
    this->valueBufInvalid--;
    return;
  }

  // basic autorange
  if (value > this->max)
    GetBufferStats(this->values, this->val_index, 8, NULL, NULL, &this->max);
  if (value < this->min)
    GetBufferStats(this->values, this->val_index, 8, NULL, NULL, &this->min);

  // back off autorange limits
  int min = this->min + AR_LOWER_DEAD_ZONE * (this->max - this->min) / 100;  // remove lower dead-zone
  int max = this->max - AR_UPPER_DEAD_ZONE * (this->max - this->min) / 100;  // remove upper dead-zone

  if (this->type == POT)
  {
    if (max - min <= AR_SPAN)  // not enough autorange span ?
      return;
  }
  else
  {
    if ((max - min <= 0) || (100 * (int) max / (int) min < AR_SPAN_RHEO))  // not enough autorange span (max/min < 2.0)?
      return;
  }

  // scale output value via autorange
  uint16_t out;
  if (value < min)
    out = 0;
  else if (value > max)
    out = 16000;  // 100%
  else
    out = 16000 * (int) (value - min) / (int) (max - min);  // scale current value with the currently detected span.

  this->out_index            = (this->out_index + CBUF_MOD) & CBUF_MOD;
  this->out[this->out_index] = out;
  // output buffer filled ?
  if (this->outBufInvalid)
  {
    this->outBufInvalid--;
    return;
  }

  if (this->flags.denoiseWhenSettled)
  {
    uint16_t min;
    uint16_t max;
    uint16_t avg;
    int      settled = 0;
    // determine min, max and average wiper sample values
    if (GetADCStats(this->wiper, SBUF_SIZE, &min, &max, &avg))
      settled = (max - min) < SETTLING_OFFSET + SETTLING_GAIN * avg / 4096;  // noise scales with voltage for this ADC chip
    if (settled)                                                             // wiper has settled ?
    {
      if (!this->flags.isSettled)  // was not settled before ?
      {
        this->flags.isSettled = 1;
        GetBufferStats(this->out, this->out_index, 8, NULL, NULL, &this->settledValue);  // freeze current output;
      }
      else
      {
        if (abs(out - this->settledValue) > 240)                                           // out value drifted away more than 240/16000 = 1.5% ?
          GetBufferStats(this->out, this->out_index, 8, NULL, NULL, &this->settledValue);  // freeze current output;
      }
      if ((out != 0) && (out != 16000))  // use settled output only when not saturated on the edges
        out = this->settledValue;
    }
    else  // not settled
      this->flags.isSettled = 0;
  }

  this->final = out;
  if (out != this->lastFinal)
  {
    this->lastFinal = out;
    SendControllerData(this->HwSourceId, out);
  }
}

/*************************************************************************/ /**
* @brief	process all pots or rheostats
******************************************************************************/
void ProcessPots(void)
{
  readoutController(&ctrl[0]);
  readoutController(&ctrl[1]);
  readoutController(&ctrl[2]);
  readoutController(&ctrl[3]);
}

/*************************************************************************/ /**
* @brief	init everything
******************************************************************************/
void NL_EHC_InitControllers(void)
{
  // ???? temp init
  initController(&ctrl[0], HW_SOURCE_ID_PEDAL_1, &adc[0], &adc[1], POT);
  initController(&ctrl[1], HW_SOURCE_ID_PEDAL_2, &adc[2], &adc[3], POT);
  initController(&ctrl[2], HW_SOURCE_ID_PEDAL_3, &adc[4], &adc[5], POT);
  initController(&ctrl[3], HW_SOURCE_ID_PEDAL_4, &adc[6], &adc[7], POT);
}

/*************************************************************************/ /**
* @brief	init everything
******************************************************************************/
void NL_EHC_DeInitControllers(void)
{
}

/*************************************************************************/ /**
* @brief	init everything
* main repetitive process called from ADC_Work_Process
******************************************************************************/
void NL_EHC_ProcessControllers(void)
{
  ProcessPots();

  // temp ????? LED is OFF while any pot is still settling or still below autoranging span
  if (ctrl[0].flags.isSettled && ctrl[1].flags.isSettled && ctrl[2].flags.isSettled && ctrl[3].flags.isSettled)
    DBG_Led_Error_Off();
  else
    DBG_Led_Error_On();

// temp ????? send raw data
#if 0
  {
    uint16_t data[13];
    data[0] = adc[0].detect << 0
        | adc[2].detect << 1
        | adc[4].detect << 2
        | adc[6].detect << 3;
    data[1]  = adc[0].filtered_current;
    data[2]  = adc[1].filtered_current;
    data[3]  = ctrl[0].final;
    data[4]  = 0;
    data[5]  = 0;
    data[6]  = 0;
    data[7]  = adc[6].filtered_current;
    data[8]  = adc[7].filtered_current;
    data[9]  = 0;
    data[10] = 0;
    data[11] = 0;
    data[12] = 0;
    BB_MSG_WriteMessage(BB_MSG_TYPE_SENSORS_RAW, 13, data);
    BB_MSG_SendTheBuffer();
  }
#endif
}

// EOF
