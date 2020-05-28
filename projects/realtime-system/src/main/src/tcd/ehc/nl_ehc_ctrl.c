/******************************************************************************/
/** @file		nl_ehc_ctrl.c
    @date		2020-01-10
    @version	0
    @author		KSTR
    @brief		handlers for external controllers (pedals etc) processing
    @ingroup	nl_tcd_modules
*******************************************************************************/

#include <stddef.h>
#include "nl_ehc_ctrl.h"
#include "nl_ehc_adc.h"
#include "tcd/nl_tcd_adc_work.h"
#include "spibb/nl_bb_msg.h"
#include "drv/nl_dbg.h"
#include "sys/nl_eeprom.h"
#include "shared/lpc-defs.h"
#include "shared/lpc-converters.h"

// =============
// ============= local constants and types
// =============

static inline uint16_t diff(uint16_t const a, uint16_t const b)
{
  if (a >= b)
    return a - b;
  else
    return b - a;
}

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

#define WAIT_TIME_AFTER_PLUGIN   (120)      // timeout after a plug-in event, in 12.5ms units (120 == 1500 milliseconds)
#define EEPROM_UPDATE_CHECK_TIME (80 * 10)  // 10secs time interval to check controller state against saved state

#define NUMBER_OF_CONTROLLERS (8)  // 4 jacks, each with tip and ring ADC channels

// ============= Pot channels
#define POT_SCALE_FACTOR   (50000ul)  // don't change this unless you know what you do
#define REFERENCE_RESISTOR (10000ul)  // don't change this unless you know what you do
#define REFERENCE_DIVIDER  (2)        // don't change this unless you know what you do
#define RHEO_SCALE_FACTOR  (REFERENCE_RESISTOR / REFERENCE_DIVIDER)
#define RHEO_MIN_RES       (300)
// autoranging
#define AR_SPAN            (5 * POT_SCALE_FACTOR / 100)  //  5%
#define AR_SPAN_RHEO       (115)                         // 115 -> 1.15 (max/min minimum factor, must be greater than 1.0, of course)
#define AR_UPPER_DEAD_ZONE (5)                           // 5 -> 5%, electrical dead zone
#define AR_LOWER_DEAD_ZONE (4)                           // 4 -> 4%, electrical dead zone

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

typedef struct __attribute__((packed))
{
  const uint16_t MAX_DRIFT;
  const uint16_t DRIFT_INDUCED_RAMPING_TIME;          // ramping time for drift-induced change
  const uint16_t DRIFT_INDUCED_RAMPING_TIME_REDUCED;  // // ramping time for drift-induced change when already ramping otherwise
  const uint16_t NORMAL_RAMPING_TIME;
  const uint16_t SHORT_RAMPING_TIME;
  const uint16_t SHOCK_CHANGE_THRESHOLD;  // amount of parameter change to trigger shock change, in 16k units
  // when going out of settling
  const uint16_t SETTLING_OFFSET;  // # of ADC lsb's which are allowed to vary near zero
  const uint16_t SETTLING_GAIN;    // #  of ADC lsb's which are allowed to vary near full scale
  // when going into settling
  const uint16_t SETTLING_OFFSET_REDUCED;  // # of ADC lsb's which are allowed to vary near zero
  const uint16_t SETTLING_GAIN_REDUCED;    // #  of ADC lsb's which are allowed to vary near full scale
} ControllerParameterSet_T;

#define PARAMETER_SETS        (4)  // number of different parameter sets
#define DEFAULT_PARAMETER_SET (1)  // index of default set

const ControllerParameterSet_T CTRL_PARAMS[PARAMETER_SETS] = {
  {
      // set 0 : high sensitivity to noise & drifts
      .MAX_DRIFT                          = (160 * 1 / 10),      // 0.1%
      .DRIFT_INDUCED_RAMPING_TIME         = (10),                // 10*12.5ms = 125ms, the larger MAX_DRIFT the longer this should be set
      .DRIFT_INDUCED_RAMPING_TIME_REDUCED = (10 / 2),            //
      .NORMAL_RAMPING_TIME                = (6),                 // 6*12.5ms = 75ms
      .SHORT_RAMPING_TIME                 = (3),                 // 3*12.5ms  = 38ms
      .SHOCK_CHANGE_THRESHOLD             = (1600),              // 1600/16000 : > 10% max change is considered a shock change
      .SETTLING_OFFSET                    = (5 * AVG_DIV),       //
      .SETTLING_GAIN                      = (10 * AVG_DIV),      //
      .SETTLING_OFFSET_REDUCED            = (5 * AVG_DIV / 2),   //
      .SETTLING_GAIN_REDUCED              = (10 * AVG_DIV / 2),  //
  },
  {
      // set 1 : normal sensitivity to noise & drifts
      .MAX_DRIFT                          = (160 * 3 / 10),      // 0.3%
      .DRIFT_INDUCED_RAMPING_TIME         = (20),                // 20*12.5ms = 250ms, the larger MAX_DRIFT the longer this should be set
      .DRIFT_INDUCED_RAMPING_TIME_REDUCED = (20 / 2),            //
      .NORMAL_RAMPING_TIME                = (10),                // 10*12.5ms = 100ms
      .SHORT_RAMPING_TIME                 = (3),                 // 3*12.5ms  = 38ms
      .SHOCK_CHANGE_THRESHOLD             = (2000),              // 2000/16000 : > 12.5% max change is considered a shock change
      .SETTLING_OFFSET                    = (11 * AVG_DIV),      //
      .SETTLING_GAIN                      = (22 * AVG_DIV),      //
      .SETTLING_OFFSET_REDUCED            = (11 * AVG_DIV / 3),  //
      .SETTLING_GAIN_REDUCED              = (22 * AVG_DIV / 3),  //
  },
  {
      // set 2 : medium-low sensitivity to noise & drifts
      .MAX_DRIFT                          = (160 * 10 / 10),     // 1%
      .DRIFT_INDUCED_RAMPING_TIME         = (40),                // 40*12.5ms = 500ms, the larger MAX_DRIFT the longer this should be set
      .DRIFT_INDUCED_RAMPING_TIME_REDUCED = (40 / 2),            //
      .NORMAL_RAMPING_TIME                = (10),                // 10*12.5ms = 100ms
      .SHORT_RAMPING_TIME                 = (3),                 // 3*12.5ms  = 38ms
      .SHOCK_CHANGE_THRESHOLD             = (3000),              // 3000/16000 : > 19% max change is considered a shock change
      .SETTLING_OFFSET                    = (25 * AVG_DIV),      //
      .SETTLING_GAIN                      = (50 * AVG_DIV),      //
      .SETTLING_OFFSET_REDUCED            = (25 * AVG_DIV / 2),  //
      .SETTLING_GAIN_REDUCED              = (50 * AVG_DIV / 2),  //
  },
  {
      // set 3 : lowest sensitivity to noise & drifts
      .MAX_DRIFT                          = (160 * 15 / 10),     // 1.5%
      .DRIFT_INDUCED_RAMPING_TIME         = (40),                // 50*12.5ms = 625ms, the larger MAX_DRIFT the longer this should be set
      .DRIFT_INDUCED_RAMPING_TIME_REDUCED = (40 / 2),            //
      .NORMAL_RAMPING_TIME                = (10),                // 10*12.5ms = 100ms
      .SHORT_RAMPING_TIME                 = (3),                 // 3*12.5ms  = 38ms
      .SHOCK_CHANGE_THRESHOLD             = (3000),              // 3000/16000 : > 19% max change is considered a shock change
      .SETTLING_OFFSET                    = (35 * AVG_DIV),      //
      .SETTLING_GAIN                      = (70 * AVG_DIV),      //
      .SETTLING_OFFSET_REDUCED            = (35 * AVG_DIV / 2),  //
      .SETTLING_GAIN_REDUCED              = (70 * AVG_DIV / 2),  //
  },
};

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

  uint16_t intermediate;
  // (auto-)ranging
  uint16_t dead_zones;   // high byte: upper dead zone in %, low byte: lower dead zone in %
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

typedef struct __attribute__((packed))
{
  EHC_ControllerConfig_T config;
  // EHC_ControllerStatus_T status;
  // (auto-)ranging
  uint16_t min;          // values used to reflect ...
  uint16_t max;          // ... physical ranges.
  uint16_t used_min;     // values used for the actual ranging, ...
  uint16_t used_max;     // ... even with auto-ranging off.
  uint16_t range_scale;  // scale (100%) value for the actual ranging
  uint16_t dead_zones;   // dead zones for auto-ranging
} ControllerSave_T;

// =============
// ============= local variables
// =============

// main working variable, array of 8 controllers
Controller_T ctrl[NUMBER_OF_CONTROLLERS];
// EEPROM save/restore buffer
ControllerSave_T NL_EEPROM_ALIGN ctrlSaveData[NUMBER_OF_CONTROLLERS];

static int      requestGetEHCdata         = 0;  // flag for pending send of EHC data
static int      enableEHC                 = 1;  // master enable flag for all runtime processing (except init() etc)
static uint16_t eepromHandle              = 0;  // EEPROM access handle
static uint16_t forceEepromUpdate         = 0;  // internal: force update EEPROM if contents were initially invalid
static uint16_t forceEepromUpdateExternal = 0;  // external: force update EEPROM if contents were initially invalid

/*************************************************************************/ /**
* @brief	"changed" event : send value to AudioEngine and UI
******************************************************************************/
static void sendControllerData(const EHC_ControllerConfig_T config, const uint32_t value)
{
  if (config.hwId == 15)  // catch de-activated controller, just in case
    return;
  if (!config.silent)
    ADC_WORK_WriteHWValueForAE(config.hwId, value);
  ADC_WORK_WriteHWValueForUI(config.hwId, value);
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
  {  // for rheostats/switches we use a nominal range of 5% to 90%
     // wrt a resistance of 10k
    this->used_min = 1 * RHEO_SCALE_FACTOR / 20;
    this->used_max = 18 * RHEO_SCALE_FACTOR / 20;
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
  {  // hardware source ID #15 is special and will delete the controller and clear its data
    this->config        = EHC_uint16ToConfig(0);
    this->config.hwId   = config.hwId;
    this->config.silent = config.silent;
    this->config.ctrlId = config.ctrlId;

    EHC_ControllerStatus_T tmp = this->status;
    this->status               = EHC_uint16ToStatus(0);
    this->status.isSaved       = tmp.isSaved;
    this->status.isRestored    = tmp.isRestored;

    this->final        = 8000;
    this->lastFinal    = 0xFFFF;
    this->intermediate = 0;
    this->dead_zones   = (AR_UPPER_DEAD_ZONE << 8) + AR_LOWER_DEAD_ZONE;
    this->used_min     = 65535;
    this->used_max     = 0;
    this->min          = 65535;
    this->max          = 0;
    this->range_scale  = 0;
    this->wait         = 0;
    this->step         = 0;
    // clear pullups of associated ADCs, making the input High-Z
    if (this->wiper)
      this->wiper->flags.pullup_10k = 0;
    if (this->top)
      this->top->flags.pullup_10k = 0;
    this->wiper = NULL;
    this->top   = NULL;
    return;
  }
  if (config.hwId >= NUM_HW_REAL_SOURCES)
    return;

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
  uint8_t                thatId      = this->config.ctrlId ^ 0b001;
  Controller_T *         that        = &ctrl[thatId];
  EHC_ControllerConfig_T clearConfig = EHC_uint16ToConfig(0xF800 | (thatId << 8));
  if (this->config.is3wire)  // controller using both ADCs ?
  {                          // check if there is something on the secondary channel and disable it
    if (that->status.initialized)
    {  // remove controller from processing
      initController(clearConfig, 0);
    }  // subsequent init will take care of pullups
  }
  else  // 2-wire now, so check if there is a 3-wire pot on this jack and clear/disable
  {
    if (this->status.initialized && old.is3wire)
    {                                   // found an initialized 3-wire at this very controller ID
      this->top->flags.pullup_10k = 0;  // make the former top end input High-Z
    }

    if (that->status.initialized && that->config.is3wire)
    {                                   // found an initialized 3-wire at the adjacent controller ID
      that->top->flags.pullup_10k = 0;  // make the top end input High-Z
      initController(clearConfig, 0);   // and remove controller from processing
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

  this->wiper->flags.useIIR     = this->config.continuous;  // low pass filter the raw input only for continuous
  this->wiper->flags.useStats   = this->config.continuous;  // enable min/max/avg statistics for continuous only
  this->wiper->flags.pullup_10k = this->config.pullup;

  if (this->config.is3wire)
  {                                                           // have a 3-wire pot, then initialize top ADC also
    this->top->flags.pullup_10k   = 1;                        // apply pullup to top (input pin)
    this->top->flags.useIIR       = this->config.continuous;  // low pass filter the raw input only for continuous
    this->top->flags.useStats     = 0;                        // disable min/max/avg statistics
    this->wiper->flags.pullup_10k = 0;                        // and force wiper readout without pullup;
  }

  this->final              = 8000;
  this->lastFinal          = 0xFFFF;
  this->status.initialized = 1;
  this->status.isReset     = 1;
  this->status.pluggedIn   = 0;
  this->intermediate       = 0;
  this->dead_zones         = (AR_UPPER_DEAD_ZONE << 8) + AR_LOWER_DEAD_ZONE;
  InitAutoRange(this);
}

static void initControllerFromSavedState(const ControllerSave_T *ctrlData)
{
  initController(ctrlData->config, 1);
  Controller_T *this = &ctrl[ctrlData->config.ctrlId];
  // this->status            = ctrlData->saveData.status;
  this->status.isRestored = 1;
  this->status.isSaved    = 0;
  this->min               = ctrlData->min;
  this->max               = ctrlData->max;
  this->used_min          = ctrlData->used_min;
  this->used_max          = ctrlData->used_max;
  this->range_scale       = ctrlData->range_scale;
  this->dead_zones        = ctrlData->dead_zones;
  // DBG_Led_Warning_TimedOn(10);  // ???
}

static void saveControllerState(Controller_T *const this, ControllerSave_T *ctrlData)
{
  ctrlData->config = this->config;
  // ctrlData->saveData.status = this->status;
  this->status.isSaved = 1;
  // this->status.isRestored        = 0;
  ctrlData->min         = this->min;
  ctrlData->max         = this->max;
  ctrlData->used_min    = this->used_min;
  ctrlData->used_max    = this->used_max;
  ctrlData->range_scale = this->range_scale;
  ctrlData->dead_zones  = this->dead_zones;
  // DBG_Led_Warning_TimedOn(1);  // ???
}

static int savedControllerStateIsDifferent(Controller_T *const this, ControllerSave_T *ctrlData)
{
  return !((EHC_configToUint16(ctrlData->config) == EHC_configToUint16(this->config))
           // && (ctrlData->min == this->min)
           // && (ctrlData->max == this->max)
           && (ctrlData->used_min == this->used_min)
           && (ctrlData->used_max == this->used_max)
           && (ctrlData->range_scale == this->range_scale)
           && (ctrlData->dead_zones == this->dead_zones));
}

/*************************************************************************/ /**
* @brief	Reset an initialized controller
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
  this->lastFinal            = 0xFFFF;
  this->status.initialized   = 1;
  this->status.isReset       = 1;
  this->status.pluggedIn     = 0;
  this->step                 = 0;
  this->intermediate         = 0;
  if (this->config.doAutoRanging)
    InitAutoRange(this);
  else
    this->status.isAutoRanged = 1;
  clearValueBuffer(&this->rawBuffer, SBUF_SIZE + VALBUF_SIZE);
  clearValueBuffer(&this->outBuffer, VALBUF_SIZE);
}

void resetControllerById(const uint16_t id, const uint16_t wait_time)
{
  if (id >= NUMBER_OF_CONTROLLERS)
    return;
  Controller_T *this = &ctrl[id];
  resetController(this, wait_time);
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
      if (value > 3800 * AVG_DIV)
        value = 3800 * AVG_DIV;  // limit input to equiv. 130.943kOhms (ADC value 3800)
      // value in 0.5kOhm units,  can't be larger than 65467
      // 4090 is the open-circuit max ADC output
      int tmp = REFERENCE_RESISTOR * value;
      tmp /= (4090 * AVG_DIV - value);
      tmp -= 100;  // subtract the built-in 100R break-out resistor
      tmp /= REFERENCE_DIVIDER;
      if (tmp < 0)
        tmp = 0;
      if (tmp > 65535)
        tmp = 65535;
      value = tmp;
    }
  }
  this->intermediate = value;
  return value;
}

// --------------- do autoranging, return != 0 if successful
static int doAutoRange(Controller_T *const this, const int value, int *const out)
{
  // basic autorange
  if (this->config.doAutoRanging)
  {  // adapt range to input span only when requested
    uint16_t avgVal = getAvgFromValueBuffer(&this->rawBuffer);
    if (avgVal > this->max)
      this->max = avgVal;
    if (avgVal < this->min)
      this->min = avgVal;

    // back off autorange limits
    this->used_min = this->min + (this->dead_zones & 0xFF) * (this->max - this->min) / 100;  // remove lower dead-zone
    this->used_max = this->max - (this->dead_zones >> 8) * (this->max - this->min) / 100;    // remove upper dead-zone

    if (this->config.is3wire)
    {
      if (this->used_max <= this->used_min + AR_SPAN)  // not enough autorange span ?
        return 0;
    }
    else
    {
      int min = this->used_min ? this->used_min : 1;  // avoid min being zero
      if ((this->used_max <= this->used_min)
          || (REFERENCE_DIVIDER * (int) (this->used_max - this->used_min) < RHEO_MIN_RES)
          || (100 * (int) this->used_max / min < AR_SPAN_RHEO))  // not enough autorange span (max/min < limit)?
        return 0;
    }
    // a valid auto-range span of values has now been detected
  }
  if (this->used_min >= this->used_max)
    return 0;

  this->status.isAutoRanged = 1;

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
  {
    this->status.isSettled = 0;
    return value;
  }

  uint16_t min;
  uint16_t max;
  uint16_t sum;
  int      settled = 0;
  // determine min, max and average wiper sample values
  EHC_getADCStats(this->wiper, &min, &max, &sum);
  // noise scales with voltage for this ADC chip
  if (this->status.isSettled)
    settled = (int) (max - min) < CTRL_PARAMS[this->paramSet].SETTLING_OFFSET + CTRL_PARAMS[this->paramSet].SETTLING_GAIN * (int) sum / ADC_MAX_SCALED;
  else
    settled = (int) (max - min) < CTRL_PARAMS[this->paramSet].SETTLING_OFFSET_REDUCED + CTRL_PARAMS[this->paramSet].SETTLING_GAIN_REDUCED * (int) sum / ADC_MAX_SCALED;
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
      if (saturated || (diff(avg, this->settledValue) > CTRL_PARAMS[this->paramSet].MAX_DRIFT))
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
  if (diff(new, value) > CTRL_PARAMS[this->paramSet].SHOCK_CHANGE_THRESHOLD)  // fast "shock" change present ?
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
  this->final = (this->config.polarityInvert) ? 16000 - value : value;
  if (this->final != this->lastFinal)
  {
    this->lastFinal = this->final;
    sendControllerData(this->config, this->final);
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

// set ranging min and max, deadzones
void setRangeMin(uint8_t which, uint16_t min)
{
  if (which >= NUMBER_OF_CONTROLLERS)
    return;
  ctrl[which].used_min = min;
}

void setRangeMax(uint8_t which, uint16_t max)
{
  if (which >= NUMBER_OF_CONTROLLERS)
    return;
  ctrl[which].used_max = max;
}

void setDeadZones(uint8_t which, uint16_t ranges)
{
  if (which >= NUMBER_OF_CONTROLLERS)
    return;
  uint8_t low  = ranges & 0xFF;
  uint8_t high = ranges >> 8;
  if (low > 20)
    low = 20;
  if (high > 20)
    high = 20;
  ctrl[which].dead_zones = ((uint16_t) high << 8) + low;
}

// ----------------------------
void forceOutput(uint8_t which)
{
  if (which >= NUMBER_OF_CONTROLLERS)
    return;
  if (ctrl[which].status.initialized && ctrl[which].status.pluggedIn
      && ctrl[which].status.isAutoRanged && ctrl[which].status.outputIsValid)
  {
    ctrl[which].lastFinal = 0xFFFF;
  }
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
    case LPC_EHC_COMMAND_SET_CONTROL_REGISTER:  // config control register
      initController(EHC_uint16ToConfig(data), 0);
      break;
    case LPC_EHC_COMMAND_SET_RANGE_MIN:  // set ranging min
      setRangeMin(cmd & 0xFF, data);
      break;
    case LPC_EHC_COMMAND_SET_RANGE_MAX:  // set ranging max
      setRangeMax(cmd & 0xFF, data);
      break;
    case LPC_EHC_COMMAND_SET_DEAD_ZONES:  // set dead zones for auto-ranging
      setDeadZones(cmd & 0xFF, data);
      break;
    case LPC_EHC_COMMAND_RESET_DELETE:  // reset or full delete controller
      if (data == 0)                    // reset
        resetControllerById(cmd & 0xFF, 0);
      else  // full delete
      {
        if ((cmd & 0xFF) >= NUMBER_OF_CONTROLLERS)
          return;
        initController(EHC_uint16ToConfig(0xF800 | (cmd & 0xFF) << 8), 1);
      }
      break;
    case LPC_EHC_COMMAND_FORCE_OUTPUT:
      forceOutput(cmd & 0xFF);
      break;
  }
}
/*************************************************************************/ /**
* @brief	 Send Config, Status, Last value, Min/Max/Scale etc to BB (all 8 controllers)
******************************************************************************/
void NL_EHC_SendEHCdata(void)
{
#define EHC_DATA_MSG_SIZE (NUMBER_OF_CONTROLLERS * 9)
  uint16_t  data[EHC_DATA_MSG_SIZE];
  uint16_t *p = data;
  for (int i = 0; i < NUMBER_OF_CONTROLLERS; i++)
  {
    *p++ = EHC_configToUint16(ctrl[i].config);
    *p++ = EHC_statusToUint16(ctrl[i].status);
    *p++ = ctrl[i].lastFinal;
    *p++ = ctrl[i].intermediate;
    *p++ = ctrl[i].used_min;
    *p++ = ctrl[i].used_max;
    *p++ = ctrl[i].range_scale;
    *p++ = ((int) EHC_adc[i].filtered_current + AVG_DIV / 2) / AVG_DIV;  // rounded scaling from 0..16*4095 to 0..4095
    *p++ = ctrl[i].dead_zones;
  }
  BB_MSG_WriteMessage(LPC_BB_MSG_TYPE_EHC_DATA, EHC_DATA_MSG_SIZE, data);
  BB_MSG_WriteMessage2Arg(LPC_BB_MSG_TYPE_NOTIFICATION, LPC_NOTIFICATION_ID_EHC_DATA, 1);
  BB_MSG_SendTheBuffer();
  for (int i = 0; i < NUMBER_OF_CONTROLLERS; i++)
    ctrl[i].status.isSaved = 0;
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
* The controllers are process from controller #7 down to #0, and update their
* output accordingly. When several controllers share the same HWSID and want to
* update it, the last controller dominates. This establishes a priority
* with the device connected to jack #1 being highest, jack #4 being lowest.
* Within a jack, the tip contact has priority over the ring contact.
******************************************************************************/
void NL_EHC_ProcessControllers1(void)
{
  // any processing only after buffers are fully filled initially, also gives some initial power-up settling
  if (!EHC_sampleBuffersValid())
    return;

  ResetSettlingDisplay();
  for (int i = NUMBER_OF_CONTROLLERS - 1; i >= NUMBER_OF_CONTROLLERS / 2; i--)
  {  // process controllers #7 down to #4
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
}

void NL_EHC_ProcessControllers2(void)
{
  // any processing only after buffers are fully filled initially, also gives some initial power-up settling
  if (!EHC_sampleBuffersValid())
    return;

  for (int i = (NUMBER_OF_CONTROLLERS / 2) - 1; i >= 0; i--)
  {  // process controllers #3 down to #0
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
}

void NL_EHC_ProcessControllers3(void)
{
  static uint32_t checkTimer = EEPROM_UPDATE_CHECK_TIME;

  if (requestGetEHCdata)
  {
    requestGetEHCdata = 0;
    NL_EHC_SendEHCdata();
  }

  if (!--checkTimer || forceEepromUpdateExternal)
  {
    checkTimer = EEPROM_UPDATE_CHECK_TIME;
    forceEepromUpdate |= forceEepromUpdateExternal;
    //#warning "test code only!"
    //    forceEepromUpdate = 1;
    int writeNeeded = forceEepromUpdate;
    for (int i = 0; i < NUMBER_OF_CONTROLLERS; i++)
    {
      if (forceEepromUpdate || savedControllerStateIsDifferent(&ctrl[i], &ctrlSaveData[i]))
      {
        writeNeeded = 1;
        saveControllerState(&ctrl[i], &ctrlSaveData[i]);
      }
    }
    if (writeNeeded)
      forceEepromUpdate = (NL_EEPROM_StartWriteBlock(eepromHandle, &ctrlSaveData[0]) == 0);
    // forceEepromUpdate is cleared now only when StartWrite was successful
    if (forceEepromUpdateExternal)
    {
      forceEepromUpdateExternal = 0;
      BB_MSG_WriteMessage2Arg(LPC_BB_MSG_TYPE_NOTIFICATION, LPC_NOTIFICATION_ID_EHC_EEPROMSAVE, 0 == forceEepromUpdate);
      BB_MSG_SendTheBuffer();
    }
  }

  EHC_fillSampleBuffers();
}

/*************************************************************************/ /**
* @brief	init everything
******************************************************************************/
void NL_EHC_InitControllers(void)
{
  requestGetEHCdata = 0;
  enableEHC         = 1;
  EHC_initSampleBuffers();

  eepromHandle = NL_EEPROM_RegisterBlock(sizeof ctrlSaveData, EEPROM_BLOCK_ALIGN_TO_PAGE);
  if (NL_EEPROM_ReadBlock(eepromHandle, &ctrlSaveData[0], EEPROM_READ_BOTH))
  {  // data in EERPOM is valid (including a restore from a backup when main data was corrupt)
    for (int i = 0; i < NUMBER_OF_CONTROLLERS; i++)
      initControllerFromSavedState(&ctrlSaveData[i]);
    forceEepromUpdate = NL_EEPROM_ReadBlockHadToUseBackup();  // force new write when backup had to be used
  }
  else
  {  // else clear all controllers
    for (int i = 0; i < NUMBER_OF_CONTROLLERS; i++)
      initController(EHC_uint16ToConfig(0xF800 | (i << 8)), 1);
    forceEepromUpdate = 1;
  }
}

void NL_EHC_ForceEepromUpdate(void)
{
  forceEepromUpdateExternal = 1;
}

// EOF
