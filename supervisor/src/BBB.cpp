// Beagle Bone Black processing
// KSTR 2019-06-07

#include "BBB.h"

#include "bit_manipulation.h"
#include "pin_manipulation.h"
#include "hardware.h"
#include "coos.h"
#include "audio.h"

BBB_t BBB;  // public handler

static int8_t step = 0;  // step-chain variable
static int8_t old_state, state;
#define BBB_HEARTBEAT_TIMEOUT (1000 / BBB_TIMESLICE)  // >= one second with no change is considered a failure

void BBB_t::Init(void)
{
  PinClr(BBB_OPT_ShutDown);  // clear button just in case
  step      = 0;
  state     = 0xFE;
  old_state = 0xFF;
}

void BBB_t::SwitchOn(void)
{
  // currently a dummy, not used, as BBB starts with power applied
}

void BBB_t::SwitchOff(void)
{
  PinSet(BBB_OPT_ShutDown);  // set button
  step = 1;                  // start step-chain
}

uint8_t BBB_t::IsOn(void)
{  // true IF power is applied AND signal is set. Note: BBB might take some time to signal this
  return (PinGet(SYS_IOPT_19V_FET) && PinGet(BBB_IPT_SystemRunning));
}

uint8_t BBB_t::IsOff(void)
{
  if (!IsOn())  // off state reached ?
  {
    Init();
    return 1;
  }  // clear pin and stop shutdown step-chain fist, then return true
  else
    return 0;
}

uint8_t BBB_t::StateChange()
{
  state = BBB.IsOn();
  if (state != old_state)
  {
    old_state = state;
    return 1;
  }
  return 0;
}

uint8_t BBB_t::GetHeartbeatLED(void)
{
  if (config.hardware_id == 71)
    return PinGet(BBB_IPT_HeartBeat_7_1);
  else
    return PinGet(BBB_IPT_HeartBeat_7_0);
}

uint8_t BBB_t::HeartbeatTimedOut(void)
{
  return 0;
}

void BBB_t::ArmHeartbeat(void)
{
}

void BBB_Process(void)
{

  if (step == 0)
    return;
  if (step == 1)
    step = 2;  // let another time-slice elapse first
  else
    PinTgl(BBB_OPT_ShutDown);  // toggle button every time-slice
}

// end of file
