/******************************************************************************/
/** @file		nl_sup.h
    @date		2019-10-20
    @brief    	Communication to Supervisor uC, for audio muting
    @author		KSTR

    Incoming Midi events over USB are interpreted as "audio engine alive".
    Any such event triggers a monoflop with 120ms timeout. To keep the
    monoflop high the incoming events should occur at least every 100ms.

    Whenever this monoflop changes state, the new state is signaled to
    the Supervisor.
    This signal is a 100ms square wave with varying duty cycle.
    - 30ms high, 70ms low : LOW  (MUTE request)
    - 70ms high, 30ms low : HIGH (UNMUTE request)

    The state signaled to the Supervisor may be overridden by :
    - a hardware test jumper that forces unmuting, highest priority
    - software functions, lower priority

    The Supervisor sends back its actual state of the muting relays with
    the same signaling mechanism.

    The current state of the muting is accessible via a function :
    - actual mute state as reported by the Supervisor, and if it is valid at all
    - any overrides and their state
    The return value can be used to signal the state to the BB

*******************************************************************************/

#include "sup/nl_sup.h"
#include "io/pins.h"
#include "spibb/nl_bb_msg.h"

static uint16_t unmute_status_bits     = 0;       // bit field
static uint16_t old_unmute_status_bits = 0xAAAA;  // bit field
static uint8_t  override               = 0;
static uint8_t  override_state         = 0;

// signal transmitter variables
static uint8_t  requested_unmute_state     = 0;  // ==0 means muted
static uint8_t  old_requested_unmute_state = 0xAA;
static uint16_t midi_timeout               = 0;
static uint8_t  step                       = 0;

// signal detector variables
static uint8_t returned_mstate = 0xAA;
static uint8_t old_returned_mstate;
static uint8_t high_time, low_time;
static uint8_t signal_timeout;
static uint8_t active_cntr;
static uint8_t inactive_cntr;
static uint8_t unknown_cntr;

#define PERIOD          (10)
#define MUTED           (3)
#define UNMUTED         (7)
#define ACTIVE          (UNMUTED)
#define INACTIVE        (MUTED)
#define ENOUGH_PATTERNS (3)

static uint8_t transition     = MUTED;
static uint8_t new_transition = MUTED;

// return requested/actual muting status bit field
uint16_t SUP_GetUnmuteStatusBits(void)
{
  return unmute_status_bits;
}

static inline void Set_Signalling_GPIO_Pin(uint8_t new_state)
{
  pinSup_OUTb = (new_state != 0);
}

static inline uint8_t Get_Status_GPIO_Pin(void)
{
  return pinSup_INb;
}

void SUP_Init(void)
{
  Set_Signalling_GPIO_Pin(0);

  midi_timeout               = 0;
  override                   = 0;
  override_state             = 0;
  requested_unmute_state     = 0;
  old_requested_unmute_state = 0xAA;
  step                       = 0;
  transition                 = MUTED;
  new_transition             = MUTED;

  high_time = low_time   = 0;
  signal_timeout         = 10;
  returned_mstate        = 0xAA;
  active_cntr            = 0;
  inactive_cntr          = 0;
  unknown_cntr           = 0;
  unmute_status_bits     = 0;
  old_unmute_status_bits = 0xAAAA;
}

// retrigger timeout monoflop
void SUP_MidiTrafficDetected(void)
{
  // set # of timeslices without traffic to raise "audio engine offline"
  midi_timeout = 1 + (TRAFFIC_TIMEOUT / SUP_PROCESS_TIMESLICE);
}

static inline void Enable_Override_Muting(uint8_t on_off)
{
  override = (on_off != 0);
}

static inline void Override_Muting(uint8_t new_unmute_state)  // effective only when enabled
{
  override_state = (new_unmute_state != 0);
}

// function called by BB_MSG_ReceiveCallback() dispatcher
void SUP_SetMuteOverride(uint32_t mode)
{
  Override_Muting(mode & SUP_UNMUTE_STATUS_SOFTWARE_VALUE);
  Enable_Override_Muting(mode & SUP_UNMUTE_STATUS_SOFTWARE_OVERRIDE);
}

// determine requested mute state and signal it to the Supervisor
// set "request" bits in status bit field
static void Transmitter(void)
{
  // determine the new mute state and its status bits (except the readback bits)
  if (midi_timeout)
    midi_timeout--;

  uint16_t bits = (unmute_status_bits & (SUP_UNMUTE_STATUS_HARDWARE_IS_VALID | SUP_UNMUTE_STATUS_HARDWARE_VALUE));

#if 0  // IO pin doesn't seem to work reliably on all boards
  if (NL_GPIO_Get(pins->lpc_unmute_jumper) == 0)  // hardware jumper reads low (jumper set) ?
  {
    requested_unmute_state = 1;  // 	-> force unmute
    bits |= (SUP_UNMUTE_STATUS_JUMPER_OVERRIDE | SUP_UNMUTE_STATUS_JUMPER_VALUE);
  }
  else
#endif
  if (override)  // software override ?
  {
    requested_unmute_state = override_state;  //  -> use override state
    bits |= SUP_UNMUTE_STATUS_SOFTWARE_OVERRIDE;
    bits |= (requested_unmute_state ? SUP_UNMUTE_STATUS_SOFTWARE_VALUE : 0);
  }
  else
  {                                                // normal operation ?
    requested_unmute_state = (midi_timeout != 0);  //  -> unmute if midi traffic did not time out
    bits |= SUP_UNMUTE_STATUS_MIDI_DERIVED;
    bits |= (requested_unmute_state ? SUP_UNMUTE_STATUS_MIDI_DERIVED_VALUE : 0);
  }
  bits |= SUP_UNMUTE_STATUS_IS_VALID;
  unmute_status_bits = bits;

  // process changes of currently requested unmute state
  if (requested_unmute_state != old_requested_unmute_state)
  {
    old_requested_unmute_state = requested_unmute_state;
    if (requested_unmute_state)
    {
      ledAudioOk     = 1;
      new_transition = UNMUTED;
    }
    else
    {
      ledAudioOk     = 0;
      new_transition = MUTED;
    }
  }

  // set signal pin according to current position in pattern cycle
  Set_Signalling_GPIO_Pin(step < transition);

  // cycle through the pattern and apply changes to pattern (but wait for one completed cycle first)
  if (++step >= PERIOD)
  {
    step       = 0;
    transition = new_transition;
  }
}

// detect actual mute state as returned by Supervisor
// set returned "actual" state bits in status bit field
static void Detector(void)
{
  old_returned_mstate = returned_mstate;
  returned_mstate     = Get_Status_GPIO_Pin();

  if (returned_mstate && (high_time < 127))
    high_time++;
  if (!returned_mstate && (low_time < 127))
    low_time++;

  if (returned_mstate != old_returned_mstate)  // pin transitioned
  {
    signal_timeout = PERIOD + 2;
    if (returned_mstate)  // pin went high, so one cycle is over
    {
      if ((high_time == ACTIVE - 1 || high_time == ACTIVE || high_time == ACTIVE + 1)
          && (low_time == INACTIVE - 1 || low_time == INACTIVE || low_time == INACTIVE + 1))
      {  // "active" pattern found
        inactive_cntr = 0;
        unknown_cntr  = 0;
        if (active_cntr < ENOUGH_PATTERNS)
          active_cntr++;
        else if (active_cntr >= ENOUGH_PATTERNS)  // enough "active" patterns ?
        {
          unmute_status_bits |= SUP_UNMUTE_STATUS_HARDWARE_IS_VALID;
          unmute_status_bits |= SUP_UNMUTE_STATUS_HARDWARE_VALUE;
        }
      }
      else if ((high_time == INACTIVE - 1 || high_time == INACTIVE || high_time == INACTIVE + 1)
               && (low_time == ACTIVE - 1 || low_time == ACTIVE || low_time == ACTIVE + 1))
      {  // "inactive" pattern found
        active_cntr  = 0;
        unknown_cntr = 0;
        if (inactive_cntr < ENOUGH_PATTERNS)
          inactive_cntr++;
        else if (inactive_cntr >= ENOUGH_PATTERNS)  // enough "inactive" patterns ?
        {
          unmute_status_bits |= SUP_UNMUTE_STATUS_HARDWARE_IS_VALID;
          unmute_status_bits &= ~SUP_UNMUTE_STATUS_HARDWARE_VALUE;
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
    }  // pin went high, so one cycle is over
  }    // pin transitioned

  if (signal_timeout)
    signal_timeout--;
  if (signal_timeout == 0)
  {
    signal_timeout = PERIOD + 2;
    if (unknown_cntr < ENOUGH_PATTERNS)
      unknown_cntr++;
  }
  if (unknown_cntr >= ENOUGH_PATTERNS)
  {
    active_cntr   = 0;
    inactive_cntr = 0;
    unmute_status_bits &= ~SUP_UNMUTE_STATUS_HARDWARE_IS_VALID;
  }
}

// process to be called constantly, every 10ms (resp., in "SUP_PROCCESS_TIMESLICE" intervals)
void SUP_Process(void)
{
#ifndef __SUP_DEFAULT_UNMUTE__
  Transmitter();

  Detector();

  if (unmute_status_bits != old_unmute_status_bits)
  {  // send any changed status to BB
    BB_MSG_WriteMessage1Arg(PLAYCONTROLLER_BB_MSG_TYPE_MUTESTATUS, unmute_status_bits);
    BB_MSG_SendTheBuffer();
    old_unmute_status_bits = unmute_status_bits;
  }
#endif
}

// EOF
