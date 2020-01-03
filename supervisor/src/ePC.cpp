// ePC (Intel NUC) processing
// KSTR 2019-06-07

#include "ePC.h"

#include "bit_manipulation.h"
#include "pin_manipulation.h"
#include "hardware.h"

ePC_t ePC;

static int16_t step = 0;
static int8_t  old_state, state;

void ePC_t::Init(void)
{
#ifndef __NO_EPC__
  PinClr(EPC_OPT_Pwr);  // clear button just in case
  step      = 0;        // clear step-chain
  state     = 0xFE;
  old_state = 0xFF;
#endif
}

void ePC_t::SwitchOn(void)
{
#ifndef __NO_EPC__
  if (!IsOn())
    step = 10;  // start startup step-chain when ePC is currently OFF
#endif
}

void ePC_t::SwitchOff(void)
{
#ifndef __NO_EPC__
  if (IsOn())
    step = 20;  // start shutdown step-chain when ePC is currently ON
#endif
}

uint8_t ePC_t::IsOn(void)
{  // true IF power is nominally applied AND signal is set. Note: ePC might take some time to signal this
#ifndef __NO_EPC__
  return PinGet(SYS_IOPT_19V_FET) && !PinGet(EPC_IPT_nSystemRunning);
#else
  return 1;
#endif
}

uint8_t ePC_t::IsOff(void)
{
#ifndef __NO_EPC__
  if (!IsOn())  // off state reached?
  {
    if (step >= 20)  // running shutdown chain?
      Init();        // then clear button and any button-toggling.
    return 1;
  }
  else
    return 0;
#else
  return 1;
#endif
}

uint8_t ePC_t::StateChange()
{
#ifndef __NO_EPC__
  state = ePC.IsOn();
  if (state != old_state)
  {
    old_state = state;
    return 1;
  };
  return 0;
#else
  return 0;
#endif
}

void ePC_Process(void)
{
#ifndef __NO_EPC__
  if (step == 0)
    return;

  switch (step)
  {
    // "startup" step-chain
    case 10:
      PinSet(EPC_OPT_Pwr);
      step++;  // wait 2 time-slice before releasing button
      return;
    case 11:
      step++;
      return;
    case 12:
      PinClr(EPC_OPT_Pwr);  // clear button
      step = 0;             // stop chain
      return;
  }

  if (step >= 20)  // "shutdown" step-chain
  {
    if (!ePC.IsOn())
    {
      PinClr(EPC_OPT_Pwr);
      step = 0;
      return;
    }
    switch (step)
    {
      case 20:
        PinSet(EPC_OPT_Pwr);
        step++;
        return;
      case 21:
        step++;
        return;
      case 22:
        PinClr(EPC_OPT_Pwr);
        step++;
        return;
      default:
        if (step++ >= 23 + EPC_TURN_OFF_TIMEOUT)  // turn-off timeout elapsed ?
          step = 20;                              // yes, try pushing button again
        return;
    }
  }
#endif
}

// end of file
