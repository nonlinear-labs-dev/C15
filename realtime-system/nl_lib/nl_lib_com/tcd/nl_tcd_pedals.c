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
#include "nl_tcd_msg.h"
#include "ipc/emphase_ipc.h"
#include "espi/dev/nl_espi_dev_pedals.h"
#include <stdlib.h>

// =============
// ============= local constants and types
// =============

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
  Constructor(&controller[0], 0);
  Constructor(&controller[1], 1);
  Constructor(&controller[2], 2);
  Constructor(&controller[3], 3);
}

void NL_TCD_PEDALS_DeInit(void)
{
  Destructor(&controller[0]);
  Destructor(&controller[1]);
  Destructor(&controller[2]);
  Destructor(&controller[3]);
}

// ============= main repetitive process called from ADC_Work_Process
void NL_TCD_PEDALS_Process(void)
{
  for (int i = 0; i < 4; i++)
    potIsStable_doReadout(&controller[i]);
}

// EOF
