/******************************************************************************/
/** @file		nl_tcd_pedals.h
    @date		2020-01-10
    @version	0
    @author		KSTR
    @brief		external controllers (pedals etc) processing
    @ingroup	nl_tcd_modules
*******************************************************************************/

#include "nl_tcd_pedals.h"
#include "nl_tcd_adc_work.h"
#include "spibb/nl_bb_msg.h"
#include "nl_tcd_msg.h"
#include "ipc/emphase_ipc.h"
#include "espi/dev/nl_espi_dev_pedals.h"
#include "drv/nl_dbg.h"
#include <stdlib.h>

// =============
// ============= local constants and types
// =============

// ============= ADC channels with sync'd buffers
#define ADC_CHANNELS      (8)              // fixed !
#define SBUF_SIZE         (4)              // must be 2^N and larger than 3 (to hold the IIR buffer)
#define SBUF_MOD          (SBUF_SIZE - 1)  // value for predecrement and masking
#define ILLEGAL_ADC_VALUE (11111)          // "illegal" value way out of range of 0...4095

#define ADC_PIN_TO_5V     (0b10000000)  // not used atm
#define ADC_PIN_TO_PULLUP (0b01000000)  // tie input via 10kOhm to +5V

// IIR lowpass coefficients
#define C1 (4786)  // 0.20657 * 2^15
#define C2 (9573)  // 0.41314 * 2^15
#define C3 (C1)    //
#define C4 (8562)  // 0.36953‬ * 2^15
#define C5 (4537)  // 0.19582‬ * 2^15

typedef struct
{
  uint16_t ipcAdcID;      // ADC channel value access ID
  uint16_t ipcDetectID;   // Plug detect access ID
  uint16_t ipcControlID;  // channel pullup control access ID
  uint8_t  config;        // channel 5v/pullup config (bits 7/6)
  // raw data
  int     values[SBUF_SIZE];             // raw values
  int     filtered_values[SBUF_SIZE];    // values after IIR filtering
  uint8_t detect_and_config[SBUF_SIZE];  // buffer for the cable detect(bit 0) and 5v/pullup config (bits 7/6)
  // pre-processed data
  uint16_t current;           // == filtered_values[sbuf_index]
  uint16_t filtered_current;  // == values[sbuf_index]
} AdcBuffer_T;

// main working variable
static AdcBuffer_T adc[ADC_CHANNELS] = {  // order must be exactly like this !!
  { EMPHASE_IPC_PEDAL_1_ADC_TIP, EMPHASE_IPC_PEDAL_1_DETECT, EMPHASE_IPC_PEDAL_1_STATE },
  { EMPHASE_IPC_PEDAL_1_ADC_RING, EMPHASE_IPC_PEDAL_1_DETECT, EMPHASE_IPC_PEDAL_1_STATE },
  { EMPHASE_IPC_PEDAL_2_ADC_TIP, EMPHASE_IPC_PEDAL_2_DETECT, EMPHASE_IPC_PEDAL_2_STATE },
  { EMPHASE_IPC_PEDAL_2_ADC_RING, EMPHASE_IPC_PEDAL_2_DETECT, EMPHASE_IPC_PEDAL_2_STATE },
  { EMPHASE_IPC_PEDAL_3_ADC_TIP, EMPHASE_IPC_PEDAL_3_DETECT, EMPHASE_IPC_PEDAL_3_STATE },
  { EMPHASE_IPC_PEDAL_3_ADC_RING, EMPHASE_IPC_PEDAL_3_DETECT, EMPHASE_IPC_PEDAL_3_STATE },
  { EMPHASE_IPC_PEDAL_4_ADC_TIP, EMPHASE_IPC_PEDAL_4_DETECT, EMPHASE_IPC_PEDAL_4_STATE },
  { EMPHASE_IPC_PEDAL_4_ADC_RING, EMPHASE_IPC_PEDAL_4_DETECT, EMPHASE_IPC_PEDAL_4_STATE }
};

static uint16_t sbuf_index;                 // index of current front element for all sample buffers
static int      sampleBuffersInvalid = 33;  // flag until buffers are full, after init

// ============= Pot channels
#define POT_SCALE_FACTOR (10000)  // don't change this unless you know what you do
// autoranging
#define AR_NOISE_LIMIT     (30)   // 100 -> 1% (assuming SCALE_FACTOR is 10000 !!)
#define AR_SPAN            (500)  // 500 -> 5% (assuming SCALE_FACTOR is 10000 !!)
#define AR_UPPER_DEAD_ZONE (3)    // 3 -> 3%
#define AR_LOWER_DEAD_ZONE (3)    // 3 -> 3%
// settling
#define PBUF_SIZE (64)  // 2^N !!!
#define PBUF_MOD  (PBUF_SIZE - 1)

typedef struct
{
  int          HwSourceId;  // Id for AE and UI "changed value event" messages
  AdcBuffer_T *wiper;       // pot (or rheostat) wiper
  AdcBuffer_T *top;         // pot top (not used for rheostat)
  struct
  {
    // output
    unsigned initialized : 1;
    unsigned isSettled : 1;
    // input
    unsigned isPot : 1;               // 0 --> 2-wire rheostat; 1 --> 3-wire pot
    unsigned denoiseWhenSettled : 1;  // 1 --> denoise output when settled long and close enough
  } flags;
  // wiper buffer
  uint16_t wiperBuf[PBUF_SIZE];
  uint16_t invalid;
  uint16_t pbufIndex;  // index of current front element
  // autoranging
  uint16_t min;
  uint16_t max;
  // settling
  uint16_t settledValue;
  // final HW-source output
  uint16_t final;
  uint16_t lastFinal;
} PotController_T;

PotController_T pot[4];  // main working variable

/*************************************************************************/ /**
* @brief	InitSampleBuffers and ADCs
******************************************************************************/
void initSampleBuffers(void)
{
  sampleBuffersInvalid = 33;
  sbuf_index           = 0;
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    for (int k = 0; k < SBUF_SIZE; k++)
    {
      adc[i].values[k]            = ILLEGAL_ADC_VALUE;
      adc[i].detect_and_config[k] = 0xFF;
    }
    adc[i].config = ADC_PIN_TO_PULLUP;  // force pullup on every pin, initially
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
void FillSampleBuffers(void)
{
  sbuf_index = (sbuf_index + SBUF_MOD) & SBUF_MOD;  // pre-decrement ...modulo buffer size

  // read data from current conversion
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    adc[i].current = adc[i].values[sbuf_index] = Emphase_IPC_PlayBuffer_Read(adc[i].ipcAdcID);
    adc[i].detect_and_config[sbuf_index]       = Emphase_IPC_PlayBuffer_Read(adc[i].ipcDetectID) | adc[i].config;
  }

  // set PULLUP bits for next conversion
  for (int i = 0; i < ADC_CHANNELS; i += 2)
  {
    uint16_t cfg = 0;
    if (adc[i].config & ADC_PIN_TO_PULLUP)
      cfg |= PEDAL_TIP_TO_PULLUP;
    if (adc[i + 1].config & ADC_PIN_TO_PULLUP)
      cfg |= PEDAL_RING_TO_PULLUP;
    Emphase_IPC_PlayBuffer_Write(adc[i].ipcControlID, cfg);
  }

  // IIR lowpass filtering
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    adc[i].filtered_current = adc[i].filtered_values[sbuf_index] =           // y[k] =
        multQ15(C1, adc[i].values[sbuf_index])                               // + C1*x[k]
        + multQ15(C2, adc[i].values[(sbuf_index + 1) & SBUF_MOD])            // + C2*x[k-1]
        + multQ15(C3, adc[i].values[(sbuf_index + 2) & SBUF_MOD])            // + C3*x[k-2]
        + multQ15(C4, adc[i].filtered_values[(sbuf_index + 1) & SBUF_MOD])   // + C4*y[k-1]
        + multQ15(C5, adc[i].filtered_values[(sbuf_index + 2) & SBUF_MOD]);  // + C5*y[k-2]
  }
}

/*************************************************************************/ /**
* @brief	"changed" event : send value to AudioEngine and UI
******************************************************************************/
void SendControllerData(uint32_t hwSourceId, uint32_t value)
{
  MSG_HWSourceUpdate(hwSourceId, value);
  ADC_WORK_WriteHWValueForBB(hwSourceId, value);
}

/*************************************************************************/ /**
* @brief	Init a pot's controller
******************************************************************************/
void initPot(PotController_T *this, int HwSourceId, AdcBuffer_T *wiper, AdcBuffer_T *top, int isPot, int denoise)
{
  this->HwSourceId  = HwSourceId;
  this->min         = 32767;
  this->max         = 0;
  this->flags.isPot = (isPot != 0);
  this->wiper       = wiper;
  if (isPot)
  {
    this->top           = top;
    this->top->config   = ADC_PIN_TO_PULLUP;  // apply pullup to top (input pin)
    this->wiper->config = 0;                  // readout wiper without pullup;
  }
  else
  {
    this->top           = NULL;
    this->wiper->config = ADC_PIN_TO_PULLUP;  // rheostat needs pullup on wiper
  }

  this->pbufIndex = 0;
  for (int i = 0; i < PBUF_SIZE; i++)
    this->wiperBuf[i] = 0;
  this->invalid = PBUF_SIZE;  // number of runs before buffer is filled

  this->final                    = 0;
  this->lastFinal                = 0xFFFF;
  this->flags.denoiseWhenSettled = (denoise != 0);
  this->flags.initialized        = 1;
}

/*************************************************************************/ /**
* @brief	readout a pot or rheostat, with optional denoising
******************************************************************************/
void readoutPot(PotController_T *this)
{
  if (!this->flags.initialized)
    return;

  // get potentiometric value
  int value = POT_SCALE_FACTOR * this->wiper->filtered_current / this->top->filtered_current;
  // and stuff it into the wiper buffer
  this->pbufIndex                 = (this->pbufIndex + PBUF_MOD) & PBUF_MOD;
  this->wiperBuf[this->pbufIndex] = this->wiper->filtered_current;

  // wiper buffer filled ?
  if (this->invalid)
  {
    this->invalid--;
    return;
  }

  // wiper settled ?
  int settled = 0;
  {
    uint16_t min = 65535;
    uint16_t max = 0;
    uint16_t x;
    uint32_t avg = 0;
    // determine min, max and average wiper sample values
    for (int i = 0; i < PBUF_SIZE; i++)
    {
      x = this->wiperBuf[i];
      avg += x;
      if (x > max)
        max = x;
      if (x < min)
        min = x;
    }
    avg /= PBUF_SIZE;

    settled = (max - min) < 5 + 15 * avg / 4096;  // noise scales with voltage for this ADC chip
  }

  // basic autorange
  if (value > this->max)
    this->max = value;
  if (value < this->min)
    this->min = value;

  // process autorange end zones
  int min = this->min + AR_LOWER_DEAD_ZONE * (this->max - this->min) / 100;  // remove lower dead-zone
  int max = this->max - AR_UPPER_DEAD_ZONE * (this->max - this->min) / 100;  // remove upper dead-zone
  if (max - min <= AR_SPAN)                                                  // not enough autorange span ?
    return;

  // scale output value via autorange
  uint16_t out;
  if (value < min)
    out = 0;
  else if (value > max)
    out = 16000;  // 100%
  else
    out = 16000 * (value - min) / (max - min);  // scale current value with the currently detected span.

  if (this->flags.denoiseWhenSettled)
  {
    if (settled)  // wiper has settled ?
    {
      if (!this->flags.isSettled)  // was not settled before ?
      {
        this->flags.isSettled = 1;
        this->settledValue    = out;  // freeze current output;
      }
      else
      {
        if (abs(out - this->settledValue) > 160)  // out value drifted away more than 160/16000 = 1% ?
          this->settledValue = out;               // freeze again current output
      }
      if ((out != 0) && (out != 16000))  // use settled output only when not on the edges
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
  readoutPot(&pot[0]);
  readoutPot(&pot[1]);
  readoutPot(&pot[2]);
  readoutPot(&pot[3]);
}

/*************************************************************************/ /**
* @brief	init everything
******************************************************************************/
void NL_TCD_PEDALS_Init(void)
{
  initSampleBuffers();

  // ???? temp init
  initPot(&pot[0], HW_SOURCE_ID_PEDAL_1, &adc[0], &adc[1], 1, 1);
  initPot(&pot[1], HW_SOURCE_ID_PEDAL_2, &adc[2], &adc[3], 1, 1);
  initPot(&pot[2], HW_SOURCE_ID_PEDAL_3, &adc[4], &adc[5], 1, 1);
  initPot(&pot[3], HW_SOURCE_ID_PEDAL_4, &adc[6], &adc[7], 1, 1);
}

/*************************************************************************/ /**
* @brief	init everything
******************************************************************************/
void NL_TCD_PEDALS_DeInit(void)
{
}

/*************************************************************************/ /**
* @brief	init everything
* main repetitive process called from ADC_Work_Process
******************************************************************************/
void NL_TCD_PEDALS_Process(void)
{
  FillSampleBuffers();

  // additional processing only after buffers are fully filled, also gives some initial power-up settling
  if (sampleBuffersInvalid)
  {
    --sampleBuffersInvalid;
    return;
  }

  ProcessPots();

  // temp ????? LED is OFF while any pot is still settling or still below autoranging span
  if (pot[0].flags.isSettled && pot[1].flags.isSettled && pot[2].flags.isSettled && pot[3].flags.isSettled)
    DBG_Led_Error_Off();
  else
    DBG_Led_Error_On();

// temp ????? send raw data
#if 1
  {
    uint16_t data[13];
    data[0] = (adc[0].detect_and_config[sbuf_index] & 1) << 0
        | (adc[2].detect_and_config[sbuf_index] & 1) << 1
        | (adc[4].detect_and_config[sbuf_index] & 1) << 2
        | (adc[6].detect_and_config[sbuf_index] & 1) << 3;
    data[1]  = adc[0].filtered_current;
    data[2]  = adc[1].filtered_current;
    data[3]  = pot[0].final;
    data[4]  = 0;
    data[5]  = 0;
    data[6]  = 0;
    data[7]  = adc[6].filtered_current;
    data[8]  = adc[7].filtered_current;
    data[9]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PITCHBENDER_ADC);
    data[10] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_AFTERTOUCH_ADC);
    data[11] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_RIBBON_1_ADC);
    data[12] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_RIBBON_2_ADC);
    BB_MSG_WriteMessage(BB_MSG_TYPE_SENSORS_RAW, 13, data);
    BB_MSG_SendTheBuffer();
  }
#endif
}

// EOF
