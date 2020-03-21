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

#define SHOW_SETTLING (0)

#if SHOW_SETTLING
static int global_unsettling = 0;
static int global_ramping    = 0;
#endif

static void ResetSettlingDisplay(void)
{
#if SHOW_SETTLING
  global_unsettling = 0;
  global_ramping    = 0;
#endif
}

static void UpdateSettlingDisplay(int s, int r)
{
#if SHOW_SETTLING
  global_unsettling |= (s == 0);
  global_ramping |= (r != 0);
#endif
}

static void ShowSettlingDisplay(void)
{
#if SHOW_SETTLING
  if (global_unsettling)
    DBG_Led_Error_TimedOn(-2);
  if (global_ramping)
    DBG_Led_Warning_TimedOn(-2);
#endif
}

#define WAIT_TIME_AFTER_PLUGIN (120)  // timeout after a plug-in event, in 12.5ms units (120 == 1500 milliseconds)

#define NUMBER_OF_CONTROLLERS (8)  // 4 jacks, each with tip and ring ADC channels

// ============= Pot channels
#define POT_SCALE_FACTOR  (20000)  // don't change this unless you know what you do
#define RHEO_SCALE_FACTOR (4500)   // don't change this unless you know what you do
// autoranging
#define AR_SPAN            (1000)  // 1000/20000 -> 5%
#define AR_SPAN_RHEO       (115)   // 115 -> 1.15 (max/min minimum factor, must be greater than 1.0, of course)
#define AR_UPPER_DEAD_ZONE (5)     // 5 -> 5%, electrical dead zone
#define AR_LOWER_DEAD_ZONE (4)     // 4 -> 4%, electrical dead zone

// settling
#define VALBUF_SIZE (8)  // 2^N !!! Floating Average is used based on this size
#define VALBUF_MOD  (VALBUF_SIZE - 1)

// ============= Switch channels
#define SWITCH_INITIAL_RAW_THRESHOLD (1400)  // raw ADC value that determinis inital switch state (> means "high")
// the next 3 values are relative to 0...16000 output range
#define SWITCH_DEADRANGE_LOW      (6000)   // only values below this are accepted as "switch closed", 6000/16000 = 37.5%
#define SWITCH_DEADRANGE_HIGH     (10000)  // only values below this are accepted as "switch open",  10000/16000 = 62.5%
#define SWITCH_TRIGGER_HYSTERESIS (1600)   // hysteresis for stabilizing triggers, 16000/16000 = 10%

#define ADC_MAX_SCALED (4096 * AVG_DIV)

typedef struct
{
  const int MAX_DRIFT;
  const int DRIFT_INDUCED_RAMPING_TIME;
  const int DRIFT_INDUCED_RAMPING_TIME_REDUCED;
  const int NORMAL_RAMPING_TIME;
  const int SHORT_RAMPING_TIME;
  const int SHOCK_CHANGE_THRESHOLD;
  const int SETTLING_OFFSET;
  const int SETTLING_GAIN;
  const int SETTLING_OFFSET_REDUCED;
  const int SETTLING_GAIN_REDUCED;
} ControllerParameterSet_T;

#define PARAMETER_SETS        (4)  // number of different parameter sets
#define DEFAULT_PARAMETER_SET (1)

const ControllerParameterSet_T CTRL_PARAMS[PARAMETER_SETS] = {
  {
      // set 0 : high sensitivity to noise & drifts
      /* MAX_DRIFT                          */ (160 * 1 / 10),      // 0.1%
      /* DRIFT_INDUCED_RAMPING_TIME         */ (10),                // 10*12.5ms = 125ms, the larger MAX_DRIFT the longer this should be set
      /* DRIFT_INDUCED_RAMPING_TIME_REDUCED */ (10 / 2),            //
      /* NORMAL_RAMPING_TIME                */ (6),                 // 6*12.5ms = 75ms
      /* SHORT_RAMPING_TIME                 */ (3),                 // 3*12.5ms  = 38ms
      /* SHOCK_CHANGE_THRESHOLD             */ (1600),              // 1600/16000 : > 10% max change is considered a shock change
      /* SETTLING_OFFSET                    */ (5 * AVG_DIV),       //
      /* SETTLING_GAIN                      */ (10 * AVG_DIV),      //
      /* SETTLING_OFFSET_REDUCED            */ (5 * AVG_DIV / 2),   //
      /* SETTLING_GAIN_REDUCED              */ (10 * AVG_DIV / 2),  //
  },
  {
      // set 1 : normal sensitivity to noise & drifts
      /* MAX_DRIFT                          */ (160 * 3 / 10),      // 0.3%
      /* DRIFT_INDUCED_RAMPING_TIME         */ (20),                // 20*12.5ms = 250ms, the larger MAX_DRIFT the longer this should be set
      /* DRIFT_INDUCED_RAMPING_TIME_REDUCED */ (20 / 2),            //
      /* NORMAL_RAMPING_TIME                */ (10),                // 10*12.5ms = 100ms
      /* SHORT_RAMPING_TIME                 */ (3),                 // 3*12.5ms  = 38ms
      /* SHOCK_CHANGE_THRESHOLD             */ (2000),              // 2000/16000 : > 12.5% max change is considered a shock change
      /* SETTLING_OFFSET                    */ (11 * AVG_DIV),      //
      /* SETTLING_GAIN                      */ (22 * AVG_DIV),      //
      /* SETTLING_OFFSET_REDUCED            */ (11 * AVG_DIV / 3),  //
      /* SETTLING_GAIN_REDUCED              */ (22 * AVG_DIV / 3),  //
  },
  {
      // set 2 : medium-low sensitivity to noise & drifts
      /* MAX_DRIFT                          */ (160 * 10 / 10),     // 1%
      /* DRIFT_INDUCED_RAMPING_TIME         */ (40),                // 40*12.5ms = 500ms, the larger MAX_DRIFT the longer this should be set
      /* DRIFT_INDUCED_RAMPING_TIME_REDUCED */ (40 / 2),            //
      /* NORMAL_RAMPING_TIME                */ (10),                // 10*12.5ms = 100ms
      /* SHORT_RAMPING_TIME                 */ (3),                 // 3*12.5ms  = 38ms
      /* SHOCK_CHANGE_THRESHOLD             */ (3000),              // 3000/16000 : > 19% max change is considered a shock change
      /* SETTLING_OFFSET                    */ (25 * AVG_DIV),      //
      /* SETTLING_GAIN                      */ (50 * AVG_DIV),      //
      /* SETTLING_OFFSET_REDUCED            */ (25 * AVG_DIV / 2),  //
      /* SETTLING_GAIN_REDUCED              */ (50 * AVG_DIV / 2),  //
  },
  {
      // set 3 : lowest sensitivity to noise & drifts
      /* MAX_DRIFT                          */ (160 * 15 / 10),     // 1.5%
      /* DRIFT_INDUCED_RAMPING_TIME         */ (40),                // 50*12.5ms = 625ms, the larger MAX_DRIFT the longer this should be set
      /* DRIFT_INDUCED_RAMPING_TIME_REDUCED */ (40 / 2),            //
      /* NORMAL_RAMPING_TIME                */ (10),                // 10*12.5ms = 100ms
      /* SHORT_RAMPING_TIME                 */ (3),                 // 3*12.5ms  = 38ms
      /* SHOCK_CHANGE_THRESHOLD             */ (3000),              // 3000/16000 : > 19% max change is considered a shock change
      /* SETTLING_OFFSET                    */ (35 * AVG_DIV),      //
      /* SETTLING_GAIN                      */ (70 * AVG_DIV),      //
      /* SETTLING_OFFSET_REDUCED            */ (35 * AVG_DIV / 2),  //
      /* SETTLING_GAIN_REDUCED              */ (70 * AVG_DIV / 2),  //
  },
};

// ----------
typedef struct
{
  unsigned ctrlId : 3;            // controller number 0...7, aka input (main) ADC channel 0...7, 0/1=J1T/R, 2/3=J2T/R, etc,
  unsigned hwId : 4;              // hardware ID used for messages to AE and PG
  unsigned silent : 1;            // disable messaging to AudioEngine
  unsigned is3wire : 1;           // controller connection type, 0=2wire(rheo/sw/cv), 1=3wire(pot)
  unsigned pullup : 1;            // controller input sensing, 0=unloaded(pot/CV), 1=with pullup(rheo/sw)
  unsigned continuous : 1;        // controller output type, 0=continuous(all), 1=bi-stable(all)
  unsigned polarityInvert : 1;    // invert, or don't, the final output(all)
  unsigned autoHoldStrength : 3;  // controller auto-hold 0..7, 0(off)...4=autoHold-Strength for pot/rheo
  unsigned doAutoRanging : 1;     // enable auto-ranging, or assume static (but adjustable) thresholds/levels
} EHC_ControllerConfig_T;

uint16_t configToUint16(const EHC_ControllerConfig_T c)
{
  uint16_t ret = 0;
  ret |= c.autoHoldStrength << 0;
  ret |= c.continuous << 3;
  ret |= c.doAutoRanging << 4;
  ret |= c.polarityInvert << 5;
  ret |= c.pullup << 6;
  ret |= c.is3wire << 7;
  ret |= c.ctrlId << 8;
  ret |= c.silent << 11;
  ret |= c.hwId << 12;
  return ret;
}
EHC_ControllerConfig_T uint16ToConfig(const uint16_t c)
{
  EHC_ControllerConfig_T ret;
  ret.autoHoldStrength = (c & 0b0000000000000111) >> 0;
  ret.continuous       = (c & 0b0000000000001000) >> 3;
  ret.doAutoRanging    = (c & 0b0000000000010000) >> 4;
  ret.polarityInvert   = (c & 0b0000000000100000) >> 5;
  ret.pullup           = (c & 0b0000000001000000) >> 6;
  ret.is3wire          = (c & 0b0000000010000000) >> 7;
  ret.ctrlId           = (c & 0b0000111100000000) >> 8;
  ret.silent           = (c & 0b0001000000000000) >> 11;
  ret.hwId             = (c & 0b1111000000000000) >> 12;
  return ret;
}

typedef struct
{
  unsigned initialized : 1;    // controller is initialized (has valid setup)
  unsigned pluggedIn : 1;      // controller is plugged in
  unsigned isReset : 1;        // controller is freshly reset
  unsigned outputIsValid : 1;  // controller final output value has been set
  unsigned isAutoRanged : 1;   // controller has finished auto-ranging (always=1 when disabled)
  unsigned isSettled : 1;      // controller output is within 'stable' bounds and step-freezing (not valid for bi-stable)
  unsigned isRamping : 1;      // controller currently does a ramp to the actual value (pot/rheo) (not valid for bi-stable)
} EHC_ControllerStatus_T;

uint16_t statusToUint16(const EHC_ControllerStatus_T s)
{
  uint16_t ret = 0;
  ret |= s.initialized << 0;
  ret |= s.pluggedIn << 1;
  ret |= s.isReset << 2;
  ret |= s.outputIsValid << 3;
  ret |= s.isAutoRanged << 4;
  ret |= s.isSettled << 5;
  ret |= s.isRamping << 6;
  return ret;
}
// ----------

// ---------------- begin Value Buffer defs
typedef struct
{
  uint16_t values[VALBUF_SIZE];
  uint16_t index;
  uint16_t invalidCntr;
  int      sum;
} ValueBuffer_T;

static void clearValueBuffer(ValueBuffer_T *const this, const uint16_t invalidCount)
{
  this->index = 0;
  this->sum   = 0;
  if (invalidCount < VALBUF_SIZE)
    this->invalidCntr = VALBUF_SIZE;
  else
    this->invalidCntr = invalidCount;
  for (int i = 0; i < VALBUF_SIZE; i++)
    this->values[i] = 0;
}

static uint16_t addInValueBuffer(ValueBuffer_T *const this, const uint16_t value)
{
  this->index = (this->index + 1) & VALBUF_MOD;
  if (this->invalidCntr)
    this->invalidCntr--;
  this->sum += (int) value - (int) this->values[this->index];
  this->values[this->index] = value;
  return value;
}

static uint16_t getAvgFromValueBuffer(const ValueBuffer_T *const this)
{
  return this->sum / VALBUF_SIZE;
}

static int isValueBufferFilled(ValueBuffer_T *const this)
{
  return (this->invalidCntr == 0);
}
// ---------------- end Value Buffer defs

typedef struct
{
  EHC_AdcBuffer_T *      wiper;  // pot (or rheostat) wiper
  EHC_AdcBuffer_T *      top;    // pot top (not used for rheostat)
  EHC_ControllerConfig_T config;
  EHC_ControllerStatus_T status;

  ValueBuffer_T rawBuffer;  // intermediate value buffer
  ValueBuffer_T outBuffer;  // output buffer

  // (auto-)ranging
  uint16_t min;          // values used to reflect ...
  uint16_t max;          // ... physical ranges.
  uint16_t used_min;     // values used for the actual ranging, ...
  uint16_t used_max;     // ... even with auto-ranging off.
  uint16_t range_scale;  // scale (100%) value for the actual ranging
  // settling
  uint16_t settledValue;
  // final HW-source output
  uint16_t final;
  uint16_t lastFinal;

  // control
  uint16_t paramSet;
  uint16_t wait;
  uint16_t step;

  // auto-hold and ramping
  uint16_t rampStart;
  uint16_t rampTime;
  uint16_t rampStep;
} Controller_T;

// =============
// ============= local variables
// =============

// main working variable, array of 8 controllers
Controller_T ctrl[NUMBER_OF_CONTROLLERS];

static int requestGetEHCdata = 0;  // flag for pending send of EHC data

static int enableEHC = 1;  // master enable flag for all runtime processing (except init() etc)

/*************************************************************************/ /**
* @brief	"changed" event : send value to AudioEngine and UI
******************************************************************************/
static void sendControllerData(const EHC_ControllerConfig_T config, const uint32_t value)
{
  if (config.hwId == 15)  // catch de-activated controller, just in case
    return;
  if (!config.silent)
    MSG_HWSourceUpdate(config.hwId, value);
  ADC_WORK_WriteHWValueForBB(config.hwId, value);
}

// --------------- init autoranging, that is, set reasonable default for fixed ranges
void InitAutoRange(Controller_T *const this)
{

  this->range_scale = AVG_DIV * 4095;  // assume CV first
  if (this->config.is3wire)
    this->range_scale = POT_SCALE_FACTOR;  // 3-wire means potentiometric readout
  else if (this->config.pullup)
    this->range_scale = RHEO_SCALE_FACTOR;  // 2-wire and pullup means rheostatic readout

  if (this->config.doAutoRanging)
  {  // set min and max so as to catch range
    this->used_min = this->min = 65535;
    this->used_max = this->max = 0;
    this->status.isAutoRanged  = 0;
    return;
  }
  this->status.isAutoRanged = 1;
  if (this->config.is3wire)
  {  // for 3-wire pot we use a range from 5% to 95% full scale
    this->used_min = 1 * POT_SCALE_FACTOR / 20;
    this->used_max = 19 * POT_SCALE_FACTOR / 20;
    return;
  }
  if (this->config.pullup)
  {  // for rheostats/switches we use a nominal range of 0...10kOhms
    this->used_min = 0;
    this->used_max = RHEO_SCALE_FACTOR;
    return;
  }
  // for direct CV we will use the full ADC range
  this->used_min = 0;
  this->used_max = AVG_DIV * 4095;
}

/*************************************************************************/ /**
* @brief	Init a controller
******************************************************************************/
static void initController(const EHC_ControllerConfig_T config, const int forced)
{
  Controller_T *this = &ctrl[config.ctrlId];
  if (config.hwId == 15)
  {  // hardware source ID #15 is special and will de-activate the controller
    this->status.initialized = 0;
    if (config.silent)
    {  // clear all data fields
      this->config               = uint16ToConfig(0);
      this->final                = 8000;
      this->lastFinal            = ~this->final;
      this->used_max             = 0;
      this->used_min             = 65535;
      this->range_scale          = 0;
      this->status.isAutoRanged  = 0;
      this->status.isRamping     = 0;
      this->status.isReset       = 0;
      this->status.isSettled     = 0;
      this->status.outputIsValid = 0;
      this->status.pluggedIn     = 0;
    }
    return;
  }

  if (config.continuous && config.autoHoldStrength > 0)  // set paramset only for a valid index 1...4
    this->paramSet = config.autoHoldStrength - 1;
  if (this->paramSet >= PARAMETER_SETS)
    this->paramSet = PARAMETER_SETS - 1;  // map autohold-strengths 1...4 to paramsets 0...3

  // determine adc channels in use
  EHC_AdcBuffer_T *wiper = &EHC_adc[config.ctrlId];
  EHC_AdcBuffer_T *top   = NULL;
  if (config.is3wire)
    top = &EHC_adc[config.ctrlId ^ 0b001];  // select the adjacent adc, eg 4 becomes 5 and vice versa

  // check if we are re-initializing simple things and don't need a full init
  EHC_ControllerConfig_T old = this->config;
  this->config               = config;
  if (!forced && this->status.initialized
      && this->wiper == wiper
      && this->top == top
      && this->config.is3wire == old.is3wire
      && this->config.pullup == old.pullup
      // we don't check auto-ranging, auto-hold strength, continuous, polarity
      // silent and hwId, as those can be changed on the fly
  )
  {
    if (this->config.doAutoRanging != old.doAutoRanging)
      InitAutoRange(this);  // re-init auto-ranging only when changed
    return;
  }

  // check for conflicting controllers and disable them
  Controller_T *that;
  if (this->config.is3wire)  // controller using both ADCs ?
  {                          // check if there is something on the secondary channel and disable it
    that = &ctrl[this->config.ctrlId ^ 0b001];
    if (that->status.initialized)
    {
      that->status.initialized = 0;  // remove controller from processing
    }                                // subsequent init will take care of pullups
  }
  else  // 2-wire now, so check if there is a 3-wire pot on this jack and clear/disable
  {
    if (this->status.initialized && old.is3wire)
    {                                   // found an initialized 3-wire at this very controller ID
      this->top->flags.pullup_10k = 0;  // make the former top end input High-Z
    }

    that = &ctrl[this->config.ctrlId ^ 0b001];
    if (that->status.initialized && that->config.is3wire)
    {                                   // found an initialized 3-wire at the adjacent controller ID
      that->top->flags.pullup_10k = 0;  // make the top end input High-Z
      that->status.initialized    = 0;  // and remove controller from processing
    }
  }

  // full init
  this->wiper = wiper;
  this->top   = top;
  this->wait  = 0;
  this->step  = 0;
  clearValueBuffer(&this->rawBuffer, SBUF_SIZE + VALBUF_SIZE);
  clearValueBuffer(&this->outBuffer, VALBUF_SIZE);
  this->status.isAutoRanged  = 0;
  this->status.isRamping     = 0;
  this->status.isSettled     = 0;
  this->status.outputIsValid = 0;
  this->status.pluggedIn     = 0;

  this->wiper->flags.useIIR     = 0;                        // don't low pass filter the raw input
  this->wiper->flags.useStats   = this->config.continuous;  // enable min/max/avg statistics for continuous only
  this->wiper->flags.pullup_10k = this->config.pullup;

  if (this->config.is3wire)
  {                                     // have a 3-wire pot, then initialize top ADC also
    this->top->flags.pullup_10k   = 1;  // apply pullup to top (input pin)
    this->top->flags.useIIR       = 0;  // don't low pass filter the raw input
    this->top->flags.useStats     = 0;  // disable min/max/avg statistics
    this->wiper->flags.pullup_10k = 0;  // and force wiper readout without pullup;
  }

  this->final              = 8000;
  this->lastFinal          = ~this->final;
  this->status.initialized = 1;
  this->status.isReset     = 1;
  this->status.pluggedIn   = 0;
  InitAutoRange(this);
}

/*************************************************************************/ /**
* @brief	Reset a controller
******************************************************************************/
static void resetController(Controller_T *const this, const uint16_t wait_time)
{
  if (!this || !this->status.initialized)
    return;
  if (this->status.isReset)
    return;
  this->wait                 = wait_time;
  this->status.isAutoRanged  = 0;
  this->status.isRamping     = 0;
  this->status.isSettled     = 0;
  this->status.outputIsValid = 0;
  this->final                = 8000;
  this->lastFinal            = ~this->final;
  this->status.initialized   = 1;
  this->status.isReset       = 1;
  this->status.pluggedIn     = 0;
  this->step                 = 0;
  InitAutoRange(this);
  clearValueBuffer(&this->rawBuffer, SBUF_SIZE + VALBUF_SIZE);
  clearValueBuffer(&this->outBuffer, VALBUF_SIZE);
}

/*************************************************************************/ /**
* @brief	Clear / Disable a controller
******************************************************************************/
static void clearController(Controller_T *const this)
{
  this->status.initialized = 0;
}

// --------------- get potentiometer or variable resistor value
static int getIntermediateValue(Controller_T *const this)
{
  int value;

  if (this->config.is3wire)
  {  // potentiometric ratio wiper/top
    int top   = this->top->filtered_current;
    int wiper = this->wiper->filtered_current;
    if (top < 10 * AVG_DIV)
      top = 10 * AVG_DIV;
    value = POT_SCALE_FACTOR * wiper / top;
  }
  else
  {
    value = this->wiper->filtered_current;  // == output in case for CV
    if (this->config.pullup)                // only rheostat or switch have pullup
    {                                       // get calculate absolute resistance value
      if (value > 4000 * AVG_DIV)
        value = 4000 * AVG_DIV;  // avoid excessive values after division
      value = RHEO_SCALE_FACTOR * value / (ADC_MAX_SCALED - value);
      if (value > 60000)  // limit to uint16 range, 100k pot shall be still within
        value = 60000;
    }
  }
  return value;
}

// --------------- do autoranging, return != 0 if successful
static int doAutoRange(Controller_T *const this, const int value, int *const out)
{
  // basic autorange
  if (this->config.doAutoRanging)
  {  // adapt range to input span only when requested
    if (value > this->max)
      this->max = getAvgFromValueBuffer(&this->rawBuffer);
    if (value < this->min)
      this->min = getAvgFromValueBuffer(&this->rawBuffer);

    // back off autorange limits
    this->used_min = this->min + AR_LOWER_DEAD_ZONE * (this->max - this->min) / 100;  // remove lower dead-zone
    this->used_max = this->max - AR_UPPER_DEAD_ZONE * (this->max - this->min) / 100;  // remove upper dead-zone

    if (this->config.is3wire)
    {
      if (this->used_max <= this->used_min + AR_SPAN)  // not enough autorange span ?
        return 0;
    }
    else
    {
      if ((this->used_max <= this->used_min) || (100 * (int) this->used_max / (int) this->used_min < AR_SPAN_RHEO))  // not enough autorange span (max/min < limit)?
        return 0;
    }
    this->status.isAutoRanged = 1;  // a valid auto-range span of values has been detected
  }
  if (this->used_min >= this->used_max)
    return 0;

  // scale output value via autorange
  if (value < this->used_min)
    *out = 0;
  else if (value > this->used_max)
    *out = 16000;  // 100%
  else
    *out = 16000 * (int) (value - this->used_min) / (int) (this->used_max - this->used_min);  // scale current value with the currently detected span.
  // clip values, and that's why we used signed ints
  if (*out < 0)
    *out = 0;
  if (*out > 16000)
    *out = 16000;
  return 1;
}

// --------------- initialize ramping to new value
void initRamping(Controller_T *const this, const uint16_t start, const uint16_t time)
{
  this->rampStart        = start;
  this->rampTime         = time;
  this->rampStep         = 0;
  this->status.isRamping = 1;
}

// --------------- ramp to new value
uint16_t doRamping(Controller_T *const this, const uint16_t value, const int update)
{  // successively ramp the value in this->rampStart to zero
   // think of it as a DC offset that is gradually removed from whatever current value
  if (!this->status.isRamping)
    return value;

  if (this->rampStep > this->rampTime)  // ramping finished ?
  {
    this->status.isRamping = 0;
    return value;
  }
  int tmp = 1024 - 1024 * this->rampStep / this->rampTime;
  if (update)
    this->rampStep++;
  //  return this->rampStart + (value - this->rampStart) * tmp / 1024;
  return value + (this->rampStart - value) * tmp / 1024;
}

// --------------- implement auto-hold
static int doAutoHold(Controller_T *const this, int value)
{
  if (this->config.autoHoldStrength == 0)
    return value;

  uint16_t min;
  uint16_t max;
  uint16_t sum;
  int      settled = 0;
  // determine min, max and average wiper sample values
  if (EHC_getADCStats(this->wiper, SBUF_SIZE, &min, &max, &sum))
  {
    // noise scales with voltage for this ADC chip
    if (this->status.isSettled)
      settled = (int) (max - min) < CTRL_PARAMS[this->paramSet].SETTLING_OFFSET + CTRL_PARAMS[this->paramSet].SETTLING_GAIN * (int) sum / ADC_MAX_SCALED;
    else
      settled = (int) (max - min) < CTRL_PARAMS[this->paramSet].SETTLING_OFFSET_REDUCED + CTRL_PARAMS[this->paramSet].SETTLING_GAIN_REDUCED * (int) sum / ADC_MAX_SCALED;
  }
  // note this is a dynamic rate-of-change settling, that is the span of values in the buffer is smaller than some limit,
  // whereas the absolute values are irrelevant. This means very slowly changing values always are considered settled

  if (settled)  // wiper has settled ?
  {
    int avg       = getAvgFromValueBuffer(&this->outBuffer);
    int saturated = ((avg == 0) || (avg == 16000));
    if (!this->status.isSettled)  // was not settled before ?
    {
      this->status.isSettled = 1;
      // freeze current averaged output as "settled" value;
      this->settledValue = avg;
      value              = doRamping(this, value, 0);
      initRamping(this, value, CTRL_PARAMS[this->paramSet].NORMAL_RAMPING_TIME);
    }
    else  // already settled
    {
      if (saturated || (abs(avg - this->settledValue) > CTRL_PARAMS[this->paramSet].MAX_DRIFT))
      {  // value drifted away too far, or reached end points?
        int alreadyRamping = this->status.isRamping;
        this->settledValue = doRamping(this, this->settledValue, 0);  // advance to next output candidate value
        if (this->settledValue != avg)
        {
          if (alreadyRamping)
            initRamping(this, this->settledValue, CTRL_PARAMS[this->paramSet].DRIFT_INDUCED_RAMPING_TIME_REDUCED);
          else
            initRamping(this, this->settledValue, CTRL_PARAMS[this->paramSet].DRIFT_INDUCED_RAMPING_TIME);
          this->settledValue = avg;  // update current averaged output as new "settled" value to avoid larger jumps
        }
      }
    }
    value = this->settledValue;
  }
  else  // wiper not settled, hence use raw input value
  {
    if (this->status.isSettled)                                                                // was settled before ?
    {                                                                                          // now ramp to new value
      this->settledValue = doRamping(this, this->settledValue, 0);                             // advance to next output candidate value
      initRamping(this, this->settledValue, CTRL_PARAMS[this->paramSet].NORMAL_RAMPING_TIME);  // and ramp from there
      // DBG_Led_Warning_TimedOn(3);
    }
    this->status.isSettled = 0;
  }
  int new = doRamping(this, value, 0);                                        // get next output candidate value
  if (abs(new - value) > CTRL_PARAMS[this->paramSet].SHOCK_CHANGE_THRESHOLD)  // fast "shock" change present ?
  {
    initRamping(this, (new + value) / 2, CTRL_PARAMS[this->paramSet].SHORT_RAMPING_TIME);  //   then init a short "shock" ramp from half-way there
    // DBG_Led_Error_TimedOn(3);
  }
  value = doRamping(this, value, 1);  // now perform the actual ramping

  UpdateSettlingDisplay(this->status.isSettled, this->status.isRamping);

  // value = this->outSum / CBUF_SIZE; // ???
  return value;
}

// --------------- update and output final value
void updateAndOutput(Controller_T *const this, const uint16_t value)
{
  this->final = value;
  if (this->final != this->lastFinal)
  {
    this->lastFinal = this->final;
    sendControllerData(this->config, (this->config.polarityInvert) ? 16000 - value : value);
    this->status.outputIsValid = 1;
  }
}

/*************************************************************************/ /**
* @brief	readout intermediate value, return != 0 when value is valid
******************************************************************************/
static int basicReadout(Controller_T *const this, int *ret)
{
  if (!this->status.initialized)
    return 0;
  this->status.isReset = 0;
  if (this->wait)
  {  // some settling time was requested, so delay any processing until then
    this->wait--;
    return 0;
  }
  *ret = getIntermediateValue(this);
  addInValueBuffer(&this->rawBuffer, *ret);
  // wiper buffer filled ?
  if (!isValueBufferFilled(&this->rawBuffer))
    return 0;
  return 1;
}

/*************************************************************************/ /**
* @brief	readout a continuous controller
******************************************************************************/
static void readoutContinuous(Controller_T *const this)
{
  int value = 0;
  if (!basicReadout(this, &value))
    return;

  int out = 0;
  if (!doAutoRange(this, value, &out))
    return;

  addInValueBuffer(&this->outBuffer, out);
  // output buffer filled ?
  if (!isValueBufferFilled(&this->outBuffer))
    return;

  out = doAutoHold(this, out);
  updateAndOutput(this, out);
}

/*************************************************************************/ /**
* @brief	readout a bi-stable controller
******************************************************************************/
static void readoutBiStable(Controller_T *const this)
{
  int value = 0;
  if (!basicReadout(this, &value))
    return;

  int out = 0;
  if (!doAutoRange(this, value, &out))
  {  // auto-ranging not finished, so force a best guess value from direct readout
    value = this->wiper->filtered_current;
    if (value > SWITCH_INITIAL_RAW_THRESHOLD * AVG_DIV)
      out = 16000;
    else
      out = 0;
    this->step = 0;  // force step chain to judge initial value
  }

  switch (this->step)
  {
    case 0:  // wait for initial clear edge
      if (out > SWITCH_DEADRANGE_HIGH)
      {
        out        = 16000;
        this->step = 20;  // now wait for positive hysteresis point
      }
      else if (out < SWITCH_DEADRANGE_LOW)
      {
        out        = 0;
        this->step = 40;  // now wait for negative hysteresis point
      }
      else
        return;
      break;

    case 10:  // wait for positive going trigger point
      if (out < SWITCH_DEADRANGE_LOW)
        return;
      this->step = 20;  // now wait for positive hysteresis point
      out        = 16000;
      break;  // and transmit edge

    case 20:  // wait for positive hysteresis point reached, or below trigger again (with some hysteresis)
      if ((out > SWITCH_DEADRANGE_LOW - SWITCH_TRIGGER_HYSTERESIS) && (out < SWITCH_DEADRANGE_HIGH))
        return;
      this->step = 30;  // now wait for negative trigger
      return;

    case 30:  // wait for negative going trigger point
      if (out > SWITCH_DEADRANGE_HIGH)
        return;
      this->step = 40;  // now wait for negative hysteresis point
      out        = 0;
      break;  // and transmit edge

    case 40:  // wait for negative hysteresis point reached, or above trigger again
      if ((out > SWITCH_DEADRANGE_LOW) && (out < SWITCH_DEADRANGE_HIGH + SWITCH_TRIGGER_HYSTERESIS))
        return;
      this->step = 10;  // now wait for positive trigger
      return;

    default:
      this->step = 0;
      return;
  }

  updateAndOutput(this, 16000 - out);  // initial output must be inverted as we've worked on the resistance
}

/*************************************************************************/ /**
* @brief	init everything
******************************************************************************/
void NL_EHC_InitControllers(void)
{
  requestGetEHCdata = 0;
  enableEHC         = 1;
  EHC_initSampleBuffers();
  clearController(&ctrl[0]);
  clearController(&ctrl[1]);
  clearController(&ctrl[2]);
  clearController(&ctrl[3]);
  clearController(&ctrl[4]);
  clearController(&ctrl[5]);
  clearController(&ctrl[6]);
  clearController(&ctrl[7]);

#if 0
  // debug ??? enable all 8 controllers as rheostats
  EHC_ControllerConfig_T config;

  config.autoHoldStrength = 2;
  config.continuous = 1;
  config.doAutoRanging = 1;
  config.is3wire = 0;
  config.polarityInvert = 0;
  config.pullup = 1;

  for (int i = 0; i <= NUMBER_OF_CONTROLLERS; i++)
  {
	config.ctrlId = i;
	switch (i)
	{
	case 0 :
	  config.hwId = 0;
	  break;
	case 1 :
	  config.hwId = 1;
	  break;
	case 2 :
	  config.hwId = 2;
	  break;
	case 3 :
	  config.hwId = 3;
	  break;
	case 4 :
	  config.hwId = 8;
	  break;
	case 5 :
	  config.hwId = 9;
	  break;
	case 6 :
	  config.hwId = 10;
	  break;
	case 7 :
	  config.hwId = 11;
	  break;
	}
	initController(config, 1);
  }
#endif
}

// set ranging min and max
void setRangeMin(uint8_t which, uint16_t min)
{
  if (which >= NUMBER_OF_CONTROLLERS)
    return;
  ctrl[which].used_min = min;
}
void setRangeMax(uint8_t which, uint16_t min)
{
  if (which >= NUMBER_OF_CONTROLLERS)
    return;
  ctrl[which].used_min = min;
}

/*************************************************************************/ /**
* @brief	 Configure External Hardware Controller
* @param[in] command
* @param[in] data
******************************************************************************/
void NL_EHC_SetEHCconfig(const uint16_t cmd, uint16_t data)
{
  switch (cmd & 0xFF00)
  {
    case 0x0100:  // config control register
      initController(uint16ToConfig(data), 0);
      break;
    case 0x0200:  // set ranging min
      setRangeMin(cmd & 0xFF, data);
      break;
    case 0x0300:  // set ranging max
      setRangeMax(cmd & 0xFF, data);
      break;
  }
}
/*************************************************************************/ /**
* @brief	 Send Config, Status, Last value, and Min/Max/Scale to BB (all 8 controllers)
******************************************************************************/
void NL_EHC_SendEHCdata(void)
{
#define EHC_DATA_MSG_SIZE (NUMBER_OF_CONTROLLERS * 6)
  uint16_t  data[EHC_DATA_MSG_SIZE];
  uint16_t *p = data;
  for (int i = 0; i < NUMBER_OF_CONTROLLERS; i++)
  {
    *p++ = configToUint16(ctrl[i].config);
    *p++ = statusToUint16(ctrl[i].status);
    *p++ = ctrl[i].lastFinal;
    *p++ = ctrl[i].used_min;
    *p++ = ctrl[i].used_max;
    *p++ = ctrl[i].range_scale;
  }
  BB_MSG_WriteMessage(BB_MSG_TYPE_EHC_DATA, EHC_DATA_MSG_SIZE, data);
  BB_MSG_WriteMessage2Arg(BB_MSG_TYPE_NOTIFICATION, NOTIFICATION_ID_EHC_DATA, 1);
  BB_MSG_SendTheBuffer();
#undef EHC_DATA_MSG_SIZE
}
void NL_EHC_RequestToSendEHCdata(void)
{
  requestGetEHCdata = 1;
}

void NL_EHC_Enable(uint16_t flag)
{
  enableEHC = (flag != 0);
}

/*************************************************************************/ /**
* @brief	 Set Pedal Type, Legacy Style
* @param[in] controller 0...3
* @param[in] type (0/1:pot tip/ring act.; 2/3:switch closing/opening on tip)
******************************************************************************/
void NL_EHC_SetLegacyPedalType(uint16_t const controller, uint16_t type)
{
  if (controller >= 4 || type >= 4)
    return;

  // de-init any controllers that are active on that jack
  EHC_ControllerConfig_T tmp;
  switch (type)
  {
    case 0:
      // pot, tip active
      tmp.ctrlId           = controller * 2;  // even adc's are tip
      tmp.hwId             = controller;
      tmp.is3wire          = 1;
      tmp.pullup           = 0;
      tmp.continuous       = 1;
      tmp.autoHoldStrength = 2;
      tmp.polarityInvert   = 0;
      tmp.doAutoRanging    = 1;
      tmp.silent           = 0;
      break;
    case 1:
      // pot, ring active
      tmp.ctrlId           = controller * 2 + 1;  // odd adc's are ring
      tmp.hwId             = controller;
      tmp.is3wire          = 1;
      tmp.pullup           = 0;
      tmp.continuous       = 1;
      tmp.autoHoldStrength = 2;
      tmp.polarityInvert   = 0;
      tmp.doAutoRanging    = 1;
      tmp.silent           = 0;
#if 0  // ??? debug
      // rheo, tip active
      tmp.ctrlId           = controller * 2;  // even adc's are tip
      tmp.hwId             = controller;
      tmp.is3wire          = 0;
      tmp.pullup           = 1;
      tmp.continuous       = 1;
      tmp.autoHoldStrength = 2;
      tmp.polarityInvert   = 0;
      tmp.doAutoRanging    = 1;
      tmp.silent           = 0;
#endif
      break;
    case 2:
      // switch, closing, on Tip
      tmp.ctrlId           = controller * 2;  // even adc's are tip
      tmp.hwId             = controller;
      tmp.is3wire          = 0;
      tmp.pullup           = 1;
      tmp.continuous       = 0;
      tmp.autoHoldStrength = 0;
      tmp.doAutoRanging    = 1;
      tmp.polarityInvert   = 0;
      tmp.silent           = 0;
      break;
    case 3:
      // switch, opening, on Tip
      tmp.ctrlId           = controller * 2;  // even adc's are tip
      tmp.hwId             = controller;
      tmp.is3wire          = 0;
      tmp.pullup           = 1;
      tmp.continuous       = 0;
      tmp.autoHoldStrength = 0;
      tmp.doAutoRanging    = 1;
      tmp.polarityInvert   = 1;
      tmp.silent           = 0;
      break;
    default:
      return;
  }
  initController(tmp, 0);
}

/*************************************************************************/ /**
* @brief	NL_EHC_ProcessControllers
* main repetitive process called from ADC_Work_Process every 12.5ms
******************************************************************************/
void NL_EHC_ProcessControllers(void)
{
  // any processing only after buffers are fully filled initially, also gives some initial power-up settling
  if (!EHC_fillSampleBuffers())
    return;

  ResetSettlingDisplay();
  for (int i = 0; i < NUMBER_OF_CONTROLLERS; i++)
  {
    if (!ctrl[i].status.initialized)
      continue;
    if (ctrl[i].wiper->detect >= 4095)  // low level input (M0) has been reading "detect" all the time ?
    {
      ctrl[i].status.pluggedIn = 1;
      if (enableEHC)
      {
        if (ctrl[i].config.continuous)
          readoutContinuous(&ctrl[i]);
        else
          readoutBiStable(&ctrl[i]);
      }
    }
    else
      resetController(&ctrl[i], WAIT_TIME_AFTER_PLUGIN);
  }
  ShowSettlingDisplay();

  if (requestGetEHCdata)
  {
    requestGetEHCdata = 0;
    NL_EHC_SendEHCdata();
  }

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
