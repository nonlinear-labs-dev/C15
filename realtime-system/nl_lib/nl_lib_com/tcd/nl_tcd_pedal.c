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
#include <stdlib.h>

// =============
// ============= local constants and types
// =============

// ============= ADC channels with sync'd buffers
#define ADC_CHANNELS            (8)                       // fixed !
#define SAMPLE_BUFFER_SIZE      (16)                      // must be 2^N !!
#define SAMPLE_BUFFER_SIZE_MASK (SAMPLE_BUFFER_SIZE - 1)  // value for predecrement and masking
#define ILLEGAL_ADC_VALUE       (11111)                   // "illegal" value way out of range of 0...4095

#define ADC_PIN_TO_5V     (0b10000000)  // not used atm
#define ADC_PIN_TO_PULLUP (0b01000000)  // tie input via 10kOhm to +5V

typedef struct
{
  uint16_t ipcAdcID;      // ADC channel value access ID
  uint16_t ipcDetectID;   // Plug detect access ID
  uint16_t ipcControlID;  // channel pullup control access ID
  uint8_t  config;        // channel 5v/pullup config (bits 7/6)
  // raw data
  uint16_t values[SAMPLE_BUFFER_SIZE];             // sample buffer, a reverse ring buffer.
  uint8_t  detect_and_config[SAMPLE_BUFFER_SIZE];  // buffer for the cable detect(bit 0) and 5v/pullup config (bits 7/6)
  // averaged and min/max data
  uint16_t min;
  uint16_t max;
  uint16_t sum3;          // last three sample avg (sum, actually, scale during use)
  uint16_t min3, max3;    // last three sample min/max
  uint16_t sum7;          // last seven sample avg (sum, actually, scale during use)
  uint16_t min7, max7;    // last seven sample min/max
  uint16_t sum16;         // last sixteen sample avg (sum, actually, scale during use)
  uint16_t min16, max16;  // last sixteen sample min/max
  // pre-processed data
  uint16_t current;
  uint16_t last;
} AdcBuffer_T;

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

static uint16_t front;                      // index of current front element for all buffers
static int      sampleBuffersInvalid = 16;  // flag until buffers are full, after init

/*************************************************************************/ /**
* @brief	InitSampleBuffers and ADCs
******************************************************************************/
void InitSampleBuffers(void)
{
  sampleBuffersInvalid = 100;
  front                = 0;
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    for (int k = 0; k < SAMPLE_BUFFER_SIZE; k++)
    {
      adc[i].values[k]            = ILLEGAL_ADC_VALUE;
      adc[i].detect_and_config[k] = 0xFF;
    }
    adc[i].config = ADC_PIN_TO_PULLUP;
    adc[i].min = 4095;
    adc[i].max = 0;
  }
}

/*************************************************************************/ /**
* @brief	fill sample buffers and do some pre-processing / denoising
******************************************************************************/
void FillSampleBuffers(void)
{
  front = (front + SAMPLE_BUFFER_SIZE_MASK) & SAMPLE_BUFFER_SIZE_MASK;  // pre-decrement ...modulo buffer size

  // read data from current conversion
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
    adc[i].values[front]            = Emphase_IPC_PlayBuffer_Read(adc[i].ipcAdcID);
    adc[i].detect_and_config[front] = Emphase_IPC_PlayBuffer_Read(adc[i].ipcDetectID) | adc[i].config;
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

  // additional processing only after buffers are fully filled, also gives some initial power-up settling
  if (sampleBuffersInvalid)
  {
    --sampleBuffersInvalid;
    return;
  }
  // sample statistics
  for (int i = 0; i < ADC_CHANNELS; i++)
  {
#if 0
    uint32_t sum = 0;
    uint16_t min = 4095;
    uint16_t max = 0;
    uint16_t sample;
    for (uint16_t k = 0; k < SAMPLE_BUFFER_SIZE; k++)
    {
      sample = adc[i].values[(front + k) & SAMPLE_BUFFER_SIZE_MASK];
      sum += sample;
      if (sample > max)
        max = sample;
      if (sample < min)
        min = sample;
      if (k == 3 - 1)  // last 3
      {
        adc[i].sum3 = sum;
        adc[i].min3 = min;
        adc[i].max3 = max;
      }
      if (k == 7 - 1)  // last 7
      {
        adc[i].sum7 = sum;
        adc[i].min7 = min;
        adc[i].max7 = max;
      }
      if (k == 16 - 1)  // last 16
      {
        adc[i].sum16 = sum;
        adc[i].min16 = min;
        adc[i].max16 = max;
      }
    }
    // find de-noised value if possible
    adc[i].current = 9999;
    if ((adc[i].max16 - adc[i].min16) < 7)  // last 16 samples range across less than 5 (+-2) samples ?
      adc[i].current = adc[i].sum16 / 16;
    else if ((adc[i].max7 - adc[i].min7) < 15)  // last 7 samples range across less than 15 (+-7) samples ?
      adc[i].current = adc[i].sum7 / 7;
    else if ((adc[i].max3 - adc[i].min3) < 45)  // last 3 samples range across less than 45 (+-22) samples ?
      adc[i].current = adc[i].sum3 / 3;
    else
      adc[i].current = adc[i].values[front];  // otherwise use raw value
#endif
   adc[i].current = adc[i].values[front];
   if (adc[i].current > adc[i].max)
	   adc[i].max = adc[i].current;
   if (adc[i].current < adc[i].min)
	   adc[i].min = adc[i].current;
  }  // all ADC channels

  // temp ????? send raw data
  {
    uint16_t data[13];
    data[0] = (adc[0].detect_and_config[front] & 1) << 0
        | (adc[2].detect_and_config[front] & 1) << 1
        | (adc[4].detect_and_config[front] & 1) << 2
        | (adc[6].detect_and_config[front] & 1) << 3;
    data[1]  = adc[0].current;
    data[2]  = adc[1].current;
    data[3]  = adc[2].current;
    data[4]  = adc[3].current;
    data[5]  = adc[4].current;
    data[6]  = adc[5].current;
    data[7]  = adc[6].current;
    data[8]  = adc[7].current;
    data[9]  = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_PITCHBENDER_ADC);
    data[10] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_AFTERTOUCH_ADC);
    data[11] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_RIBBON_1_ADC);
    data[12] = Emphase_IPC_PlayBuffer_Read(EMPHASE_IPC_RIBBON_2_ADC);
    BB_MSG_WriteMessage(BB_MSG_TYPE_SENSORS_RAW, 13, data);
    BB_MSG_SendTheBuffer();
  }
}

//
//
//
//

#define PULLUP_DEFAULT_STATE (PEDAL_TIP_TO_PULLUP | PEDAL_RING_TO_PULLUP)
#define UNKNOWN              (0)
#define TIP_ACTIVE           (1)
#define RING_ACTIVE          (2)

#define VAL_BUF_SIZE (10)  // how many values are kept for noise judgment, slew rate etc, 10 gives 125ms autorange settling delay

#define SCALE_FACTOR (10000)  // don't change this unless you know what you do
// autoranging
#define AR_NOISE_LIMIT     (30)   // 100 -> 1% (assuming SCALE_FACTOR is 10000 !!)
#define AR_SPAN            (500)  // 500 -> 5% (assuming SCALE_FACTOR is 10000 !!)
#define AR_UPPER_DEAD_ZONE (3)    // 3 -> 3%
#define AR_LOWER_DEAD_ZONE (3)    // 3 -> 3%

// ============= main data structure "Controller_T"
typedef struct
{
  // control
  int initialized;   // flag
  int number;        // 0...N-1
  int HwSourceId;    // Id for AE and UI "changed value event" messages
  int ipcControlId;  // Id for Pullup Control for that TRS
  int ipcIdAdc[2];   // Id for "tip" and "ring" ADC channel
  int isRingActive;  // flag for "ring" is wiper
                     // specials
  int potType;       // 0:unknown; 1:tip-active; 2:ring-active

  // adc data
  // output/save data
  int valueBuffer[VAL_BUF_SIZE];  // 0...10000
  int lastValue;                  // last output (0..16000)
  int maximum;
  int minimum;
} Controller_T;

void Constructor(Controller_T *this, int number);
void Destructor(Controller_T *this);
void resetBounds(Controller_T *this);

void SendControllerData(uint32_t hwSourceId, uint32_t value);

// =============
// ============= local variables and constants
// =============
Controller_T controller[4];

// ============= Controller_T member functions

// Initialize object
void Constructor(Controller_T *this, int number)
{
  if (!this)
    return;
  this->initialized = 0;

  switch (number)
  {
    case 0:
      this->HwSourceId   = HW_SOURCE_ID_PEDAL_1;
      this->ipcControlId = EMPHASE_IPC_PEDAL_1_STATE;
      this->ipcIdAdc[0]  = EMPHASE_IPC_PEDAL_1_ADC_TIP;
      this->ipcIdAdc[1]  = EMPHASE_IPC_PEDAL_1_ADC_RING;
      break;
    case 1:
      this->HwSourceId   = HW_SOURCE_ID_PEDAL_2;
      this->ipcControlId = EMPHASE_IPC_PEDAL_2_STATE;
      this->ipcIdAdc[0]  = EMPHASE_IPC_PEDAL_2_ADC_TIP;
      this->ipcIdAdc[1]  = EMPHASE_IPC_PEDAL_2_ADC_RING;
      break;
    case 2:
      this->HwSourceId   = HW_SOURCE_ID_PEDAL_3;
      this->ipcControlId = EMPHASE_IPC_PEDAL_3_STATE;
      this->ipcIdAdc[0]  = EMPHASE_IPC_PEDAL_3_ADC_TIP;
      this->ipcIdAdc[1]  = EMPHASE_IPC_PEDAL_3_ADC_RING;
      break;
    case 3:
      this->HwSourceId   = HW_SOURCE_ID_PEDAL_4;
      this->ipcControlId = EMPHASE_IPC_PEDAL_4_STATE;
      this->ipcIdAdc[0]  = EMPHASE_IPC_PEDAL_4_ADC_TIP;
      this->ipcIdAdc[1]  = EMPHASE_IPC_PEDAL_4_ADC_RING;
      break;
    default:  // invalid number
      return;
  }
  this->number = number;

  this->lastValue = -1;  // set a value that is illegal (negative), forces update on first run
  for (int i = 0; i < VAL_BUF_SIZE; i++)
    this->valueBuffer[i] = -20000;  // put extreme values into buffer so that it never settles when those are still present on startup
  resetBounds(this);

  // ??? temp !!!
  if (number == 0 || number == 1)
  {
    this->potType      = TIP_ACTIVE;
    this->isRingActive = 0;
    Emphase_IPC_PlayBuffer_Write(this->ipcControlId, PEDAL_RING_TO_PULLUP);
  }
  else
  {
    this->potType      = RING_ACTIVE;
    this->isRingActive = 1;
    Emphase_IPC_PlayBuffer_Write(this->ipcControlId, PEDAL_TIP_TO_PULLUP);
  }

  this->initialized = 1;
}

// De-Initialize object
void Destructor(Controller_T *this)
{
  if (!this)
    return;
  if (this->initialized)
  {
    Emphase_IPC_PlayBuffer_Write(this->ipcControlId, PULLUP_DEFAULT_STATE);
  }
  this->initialized = 0;
}

// ============= Reset Bounds
void resetBounds(Controller_T *this)
{
  this->maximum = INT32_MIN;
  this->minimum = INT32_MAX;
}

// ============= push new value into buffer
void addInBuffer(Controller_T *this, int value)
{
  for (int i = VAL_BUF_SIZE - 1; i > 0; i--)
    this->valueBuffer[i] = this->valueBuffer[i - 1];
  this->valueBuffer[0] = value;
}

int isSettled(Controller_T *const this, int *average)
{
  int     min = INT32_MAX;
  int     max = INT32_MIN;
  int64_t sum = 0;
  for (int i = 0; i < VAL_BUF_SIZE; i++)
  {
    sum += this->valueBuffer[i];
    if (this->valueBuffer[i] < min)
      min = this->valueBuffer[i];
    if (this->valueBuffer[i] > max)
      max = this->valueBuffer[i];
  }
  if (average)
    *average = sum / VAL_BUF_SIZE;  // return averaged value if settled / low noise
  return (max - min) < AR_NOISE_LIMIT;
}

// ============= read out a 3 wire pot
void potIsStable_doReadout(Controller_T *this)
{
  if (!this || !this->initialized || this->potType == 0)
    return;

  // calc fraction of wiper vs total, scale factor 10'000 --> 2 decimal digits after % decimal point
  int current = SCALE_FACTOR * (int) Emphase_IPC_PlayBuffer_Read(this->ipcIdAdc[this->isRingActive])
      / (int) Emphase_IPC_PlayBuffer_Read(this->ipcIdAdc[1 - this->isRingActive]);
  addInBuffer(this, current);

  int average;
  if (isSettled(this, &average))
  {
    if (current < this->minimum)
      this->minimum = average;
    if (current > this->maximum)
      this->maximum = average;
  }

  //  int out = 8000;            // will set all pedals to 50% until autoranging successful ????
  int out = this->lastValue;  // will wait until autoranging successful, then set either 0% or 100% accordingly
  if (this->maximum > this->minimum)
  {
    int min = this->minimum + AR_LOWER_DEAD_ZONE * (this->maximum - this->minimum) / 100;  // remove lower dead-zone
    int max = this->maximum - AR_UPPER_DEAD_ZONE * (this->maximum - this->minimum) / 100;  // remove upper dead-zone

    if (max - min > AR_SPAN)  // enough span ?
    {
      if (current < min)
        out = 0;
      else if (current > max)
        out = 16000;  // 100%
      else
        out = 16000 * (current - min) / (max - min);  // scale current value with the currently detected span.
    }
  }  // maximum > minimum
  if (out != this->lastValue)
  {
    this->lastValue = out;
    SendControllerData(this->HwSourceId, out);
  }
}

// =============
// ============= local functions
// =============

// ============= "changed" event : send value to AudioEngine and UI
void SendControllerData(uint32_t hwSourceId, uint32_t value)
{
  MSG_HWSourceUpdate(hwSourceId, value);
  ADC_WORK_WriteHWValueForBB(hwSourceId, value);
}

// =============
// ============= global functions
// =============

void NL_TCD_PEDALS_Init(void)
{
  InitSampleBuffers();
  // ???? temp
  adc[0].config = 0;                  // TIP 1
  adc[1].config = ADC_PIN_TO_PULLUP;  // RING 1
  adc[2].config = 0;                  // TIP 2
  adc[3].config = ADC_PIN_TO_PULLUP;  // RING 2
  adc[4].config = 0;                  // TIP 3
  adc[5].config = ADC_PIN_TO_PULLUP;  // RING 3
  adc[6].config = 0;                  // TIP 4
  adc[7].config = ADC_PIN_TO_PULLUP;  // RING 4
}

void NL_TCD_PEDALS_DeInit(void)
{
}

// ============= main repetitive process called from ADC_Work_Process
void NL_TCD_PEDALS_Process(void)
{
  FillSampleBuffers();
}

// EOF
