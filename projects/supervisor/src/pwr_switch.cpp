// power switch/button processing
// KSTR 2019-06-07

#include "hardware.h"
#include "pwr_switch.h"
#include "pwr_monitor.h"
#include "globals.h"

/*
	The detector works as an time integrator. Whenever the switch/button is pressed it ramps up an
	accumulator and when it is released it ramps down. This allows for significant switch bounce
	and aging contacts, notably with a pushbutton. 
	As long as a "pressed" state is present more often than a "released" state on average,
	the integrator will finally ramp up to the required PRESS_CNT which triggers an event.
	Therefore, the minimum press time to trigger is the value given below.
	If, say 2 out of 3 pressed states are missed because of an aging contact, the time until
	trigger will increase threefold.
*/

#define DEBOUNCE_CNT (500 / PSW_TIMESLICE)  // 500ms

static uint8_t accu;
static int8_t  state, old_state;
static uint8_t reached_active;

PwrSwitch_t PwrSwitch;

void PSW_Process(void)
{
  if (GetPowerSwitch())
  {  // currently pressed
    if (accu < DEBOUNCE_CNT)
      accu++;
  }
  else
  {  // currently released
    if (accu > 0)
      accu--;
  }

  switch (accu)
  {
    case DEBOUNCE_CNT:
      state = +1;
      break;  // accu topps out   ==> switch/button state := "active"
    case 0:
      state = -1;
      break;  // accu bottoms out ==> switch/button state := "inactive"
    default:
      state = 0;  // accu not settled ==> switch/button state := "bouncing, invalid"
  }

  if (state != old_state)  // state change ?
  {
    old_state = state;
    if (state == +1)  // switch/button state reached "active" ==> flag it
      reached_active = 1;
  }
}

void PwrSwitch_t::Init(void)
{
  state = -99;               // invalid
  accu  = DEBOUNCE_CNT / 2;  // set accumulator to a center to have some de-bouncing of the first reading after init
}

void PwrSwitch_t::Arm(void)
{
  if (!config.power_switch_is_latching)
    reached_active = 0;  // reset "active" event flag
}

uint8_t PwrSwitch_t::On_Event(void)
{
  if (config.power_switch_is_latching)
    return (state == +1);  // signal true when current state has settled to "active"
  else
  {
    if (!reached_active)
      return 0;
    // ON event happened
    reached_active = 0;  // clear
    return 1;
  }
}

uint8_t PwrSwitch_t::Off_Event(void)
{
  if (config.power_switch_is_latching)
    return (state == -1);  // signal true when current state has settled to "inactive"
  else                     // pushbutton toggles ON/OFF events with every press
    return (On_Event());
}

int8_t PwrSwitch_t::GetState(void)
{
  return (state);
}

// end of file
