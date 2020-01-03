// LPC processing (muting communication)
// KSTR 2019-11-04

#include "LPC.h"

#include "bit_manipulation.h"
#include "pin_manipulation.h"
#include "hardware.h"
#include "coos.h"
#include "audio.h"

static int8_t   LPC_Monitor_ID;
static uint8_t  mstate = 0xAA;
static uint8_t  old_mstate;
static uint8_t  high_time, low_time;
static uint8_t  signal_timeout;
static uint8_t  had_a_high_transition;
static uint8_t  active_cntr;
static uint8_t  inactive_cntr;
static uint8_t  unknown_cntr;
static uint16_t default_timeout;

#define PERIOD          (10)
#define INACTIVE        (3)
#define ACTIVE          (7)
#define ENOUGH_PATTERNS (3)

static uint8_t unmute_state     = 0;  // ==0 means muted
static uint8_t old_unmute_state = 0xAA;
static uint8_t step             = 0;

static uint8_t transition     = INACTIVE;
static uint8_t new_transition = INACTIVE;

void LPC_Start_MonitorAudioEngineStatus(int32_t timeout)
{
  SetUnmuteSignalPin(0);
  unmute_state     = 0;  // ==0 means muted
  old_unmute_state = 0xAA;
  step             = 0;
  transition       = INACTIVE;
  new_transition   = INACTIVE;

  high_time = low_time  = 0;
  signal_timeout        = 10;
  mstate                = 0xAA;
  had_a_high_transition = 0;
  active_cntr           = 0;
  inactive_cntr         = 0;
  unknown_cntr          = 0;
  if (timeout > 0)
    default_timeout = timeout / MONENGINE_TIMESLICE;  // default timeout to unmute
  else
    default_timeout = 0;
  LPC_Monitor_ID = coos_add_task(LPC_MonitorAudioEngineStatus_Process, 3, -MONENGINE_TIMESLICE);
}

void LPC_Stop_MonitorAudioEngineStatus(void)
{
  coos_delete_task(LPC_Monitor_ID);
  SetUnmuteSignalPin(0);
}

void LPC_MonitorAudioEngineStatus_Process(void)
{
  // part 1 : Transmitter
  unmute_state = BitGet(config.status, STAT_UNMUTED);
  if (unmute_state != old_unmute_state)
  {
    old_unmute_state = unmute_state;
    if (unmute_state)
      new_transition = ACTIVE;
    else
      new_transition = INACTIVE;
  }
  SetUnmuteSignalPin(step < transition);
  if (++step >= PERIOD)
  {
    step       = 0;
    transition = new_transition;
  }

  // part 2 : Detector
  if (default_timeout)
    default_timeout--;

  old_mstate = mstate;
  mstate     = GetAudioEngineSignalPin();
  // LED_A(mstate);
  if (mstate && (high_time < 127))
    high_time++;
  if (!mstate && (low_time < 127))
    low_time++;

  if (mstate != old_mstate)  // pin transitioned
  {
    signal_timeout = PERIOD + 2;
    if (mstate)  // pin went high, so one cycle is over
    {
      if ((high_time == ACTIVE - 1 || high_time == ACTIVE || high_time == ACTIVE + 1)
          && (low_time == INACTIVE - 1 || low_time == INACTIVE || low_time == INACTIVE + 1))
      {  // "active" pattern found
        inactive_cntr = 0;
        unknown_cntr  = 0;
        if (active_cntr < ENOUGH_PATTERNS)
          active_cntr++;
        else if (active_cntr == ENOUGH_PATTERNS)  // enough "active" patterns ?
        {
          Audio.UnMute();
          active_cntr++;  // block further unmutes
        }
      }
      else if ((high_time == INACTIVE - 1 || high_time == INACTIVE || high_time == INACTIVE + 1)
               && (low_time == ACTIVE - 1 || low_time == ACTIVE || low_time == ACTIVE + 1))
      {  // "inactive" pattern found
        active_cntr  = 0;
        unknown_cntr = 0;
        if (inactive_cntr < ENOUGH_PATTERNS)
          inactive_cntr++;
        else if (inactive_cntr == ENOUGH_PATTERNS)  // enough "inactive" patterns ?
        {
          Audio.Mute();
          inactive_cntr++;  // block further mutes
        }
      }
      else  // illegal pattern found
      {
        active_cntr   = 0;
        inactive_cntr = 0;
        if (unknown_cntr < ENOUGH_PATTERNS)
          unknown_cntr++;
      }

      high_time = 0;
      low_time  = 0;
    }
  }

  if (signal_timeout)
    signal_timeout--;
  if (signal_timeout == 0)
  {
    signal_timeout = PERIOD + 2;
    if (unknown_cntr < ENOUGH_PATTERNS)
      unknown_cntr++;
  }

  if (default_timeout == 0)  // timed out?
  {
    if (unknown_cntr >= ENOUGH_PATTERNS)
    {
      Audio.UnMute();
      unknown_cntr++;
      active_cntr   = 0;
      inactive_cntr = 0;
    }
  }
}

// end of file
