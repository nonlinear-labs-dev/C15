// system and input power processing
// KSTR 2019-06-07

#include "pwr_monitor.h"

#include "bit_manipulation.h"
#include "pin_manipulation.h"
#include "hardware.h"

PwrMonitor_t            PwrMonitor;  // public handler
static uint8_t          armed = 0;   // true once "power good" is detected, arm power fail sensing
static volatile uint8_t system_was_up_pattern __attribute__((section(".noinit")));

void PwrMonitor_t::Init(void)
{
  system_was_up = 0;
  armed         = 0;
  PinPud(SYS_ADC_19V);  // switch threshold to high initially
}

uint8_t PwrMonitor_t::PowerIsGood(void)
{
  if (!BitGet(ACSR, ACO))  // above threshold ?
  {
    PinPue(SYS_ADC_19V);  // switch threshold to low now
    Wait(2);              // give pullup switch some time to settle
    armed = 1;            // and arm low detector
    ArmComparatorIRQ();   // and arm the IRQ which instantly handles audio muting
    return 1;
  }
  return 0;
}

uint8_t PwrMonitor_t::PowerFail(void)
{
  return armed && BitGet(ACSR, ACO);  // below threshold
}

void PwrMonitor_t::SystemOn(void)
{
  if (power_failed)  // power fail event?
    return;          // then don't switch on anymore
  SwitchMainSupply(1);
}

void PwrMonitor_t::SystemOff(void)
{
  SwitchMainSupply(0);
}

// end of file
