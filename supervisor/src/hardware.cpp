// hardware defintions and support routines
// KSTR 2019-06-07

#include "hardware.h"
#include "bit_manipulation.h"
#include "pin_manipulation.h"
#include "globals.h"
#include "comm.h"

#include <avr/wdt.h>
#include <avr/interrupt.h>

volatile uint16_t ticker       = 0;  // system ticker
volatile uint8_t  power_failed = 0;  // flag to indicate a power-fail

#define LIVE_PATTERN (0x5A)
// reset-surviving flag for "system was in running state" (Running only)
volatile uint8_t was_in_running_state_RAM __attribute__((section(".noinit")));
// reset-surviving flag for "system was in active state"	(Booting or Running)
volatile uint8_t was_in_active_state_RAM __attribute__((section(".noinit")));

config_t config;  // master configuration variable, set be the Init routines

void config_t::SetRunState(run_state_t state)
{
  config.status &= ~((1 << STAT_SYSTEM_STATUS1) | (1 << STAT_SYSTEM_STATUS0));
  config.status |= ((int8_t)(state)) << STAT_SYSTEM_STATUS0;
}

run_state_t config_t::GetRunState(void)
{
  return (run_state_t)((config.status & ((1 << STAT_SYSTEM_STATUS1) | (1 << STAT_SYSTEM_STATUS0)))
                       >> STAT_SYSTEM_STATUS0);
}

//----------------------------------------------------------------------
void IOPortInit(void)
{
  // unused pins are inputs with pullups enabled.
  PinIpt(PORT_B5);
  PinPue(PORT_B5);
  PinIpt(PORT_B6);
  PinPue(PORT_B6);
  PinIpt(PORT_B7);
  PinPue(PORT_B7);
  PinIpt(PORT_C4);
  PinPue(PORT_C4);
  PinIpt(PORT_C5);
  PinPue(PORT_C5);
  //	PinIpt(PORT_D0); PinPue(PORT_D0);
  //	PinIpt(PORT_D1); PinPue(PORT_D1);
  PinIpt(PORT_D2);
  PinPue(PORT_D2);
  //	PinIpt(PORT_D5); PinPue(PORT_D5);
  PinIpt(PORT_D6);
  PinPue(PORT_D6);

  PinPud(SYS_IOPT_19V_FET);
  PinIpt(SYS_IOPT_19V_FET);  // switch to input for reading back a live reset event
  PinIpt(SYS_IPT_CFG0);
  PinPue(SYS_IPT_CFG0);  // activate PU on CFG0 Jumper input
  PinIpt(SYS_IPT_CFG1);
  PinPue(SYS_IPT_CFG1);  // activate PU on CFG1 Jumper input
  PinOpt(SYS_OPT_Relays_UNMUTE);
  PinClr(SYS_OPT_Relays_UNMUTE);  // unmute coil/relay OFF
  PinIpt(SYS_IOPT_Relays_MUTE);
  PinPud(SYS_IOPT_Relays_MUTE);  // switch to input for reading back external PullUp (HW7.1-ID)

  PinIpt(SYS_IPT_nPwrSwitch);
  PinPue(SYS_IPT_nPwrSwitch);  // activate PU on Power Switch input
  PinOpt(SYS_OPT_SystemLed);
  PinClr(SYS_OPT_SystemLed);  // clear LED

  PinOpt(SYS_OPT_ESPI_DISABLE);
  PinClr(SYS_OPT_ESPI_DISABLE);  // BBB ESPI override OFF
  PinOpt(SYS_OPT_ESPI_SAP);
  PinClr(SYS_OPT_ESPI_SAP);  // BBB ESPI SAP force low (defined voltage)
  PinOpt(SYS_OPT_ESPI_SCK);
  PinClr(SYS_OPT_ESPI_SCK);  // BBB ESPI SAP force low (defined voltage)

  PinIpt(EPC_IPT_nSystemRunning);
  PinPud(EPC_IPT_nSystemRunning);  // disable PU (not needed)
  PinOpt(EPC_OPT_Pwr);
  PinClr(EPC_OPT_Pwr);  // ePC power button released

  PinIpt(BBB_IPT_SystemRunning);
  PinPue(BBB_IPT_SystemRunning);  // enable PU (pin might be floating)
  PinPud(BBB_OPT_ShutDown);
  PinOpt(BBB_OPT_ShutDown);
  PinClr(BBB_OPT_ShutDown);  // BBB power button released

  PinOpt(SYS_OPT_5V_FET);
  PinSet(SYS_OPT_5V_FET);  // 5V FET ON (default)
  PinOpt(SYS_OPT_3V_FET);
  PinSet(SYS_OPT_3V_FET);  // 3V FET ON (default)

  PinIpt(LPC_MUTE_REQ);
  PinPue(LPC_MUTE_REQ);  // LPC Mute Request
  PinOpt(LPC_UNMUTE_ACK);
  PinClr(LPC_UNMUTE_ACK);

  PinOpt(SYS_OPT_LED_A);
  PinClr(SYS_OPT_LED_A);
  PinOpt(SYS_OPT_LED_B);
  PinClr(SYS_OPT_LED_B);

  PinIpt(BBB_SER_RXD);
  PinPud(BBB_SER_RXD);
  PinIpt(BBB_SER_TXD);
  PinPud(BBB_SER_TXD);
}

void TimerInit(void)
{
  // set Timer Mode to CTC (Clear Timer on Compare Match)
  BitSet(TCCR0, WGM01);
  BitClr(TCCR0, WGM00);

  // 2MHz
  // prescaler ( 8 -> 010b ) and start the timer
  BitClr(TCCR0, CS00);
  BitSet(TCCR0, CS01);
  BitClr(TCCR0, CS02);
  OCR0 = 249;            // Set the value that you want to count to minus one
  BitSet(TIMSK, OCIE0);  // Set the ISR COMPA vector
}

void ComparatorInit(void)
{

  PinIpt(SYS_ADC_19V);  // set as input to access Pullup control
  PinPud(SYS_ADC_19V);  // disable Pullup
  // Analog Comparator Setup :
  BitClr(ADCSRA, ADEN);  // ADC OFF
  BitSet(SFIOR, ACME);   // Comp -IN uses ADC Muxer
  BitSet(ADMUX, MUX0);   // Select ...
  BitSet(ADMUX, MUX1);   // ... channel 7 (PortA, 7) ...
  BitSet(ADMUX, MUX2);   // ... as Comp Input.
  BitSet(ACSR, ACBG);    // Select 1.24V Bandgap for Comp +IN
}

void ArmComparatorIRQ(void)
{
  BitClr(ACSR, ACIE);   // Disable Comparator Interrupts
  BitClr(ACSR, ACIS0);  // Trigger IRQ ...
  BitSet(ACSR, ACIS1);  // ... on falling edge
  BitSet(ACSR, ACIE);   // Enable Comparator Interrupts
}

uint8_t GetPowerSwitch(void)
{
  return !PinGet(SYS_IPT_nPwrSwitch);
}

void PowerLED(uint8_t on)
{
#if !DEBUG_LED
  if (on != 0)
    PinSet(SYS_OPT_SystemLed);
  else
    PinClr(SYS_OPT_SystemLed);
#endif
}

void PowerLED_Toggle(void)
{
#if !DEBUG_LED
  PinTgl(SYS_OPT_SystemLed);
#endif
}

void DebugHalt(uint8_t led)
{
  PowerLED(led);
  while (1)
    ;
}

void LED_A(int8_t on)  // debug-LED "A": ==0 ==> OFF, >0 ==> ON, <0 ==> toggle
{
  if (config.hardware_id < 71)
    return;
  if (on > 0)
    PinSet(SYS_OPT_LED_A);
  else if (on == 0)
    PinClr(SYS_OPT_LED_A);
  else
    PinTgl(SYS_OPT_LED_A);
}

void LED_B(int8_t on)  // debug-LED "B": ==0 ==> OFF, >0 ==> ON, <0 ==> toggle
{
  if (config.hardware_id < 71)
    return;
  if (on > 0)
    PinSet(SYS_OPT_LED_B);
  else if (on == 0)
    PinClr(SYS_OPT_LED_B);
  else
    PinTgl(SYS_OPT_LED_B);
}

void Wait(uint16_t ms)
{
  ticker = 0;
  while (ticker <= ms)
    ;
}

void EEPROM_Write(uint8_t pattern)
{
  while (EECR & (1 << EEWE))  // wait until any previous write is finished
    ;
  cli();
  EEAR = 0;              // Byte # 0
  EEDR = pattern;        // Data
  EECR |= (1 << EEMWE);  // Master Write Enable
  EECR |= (1 << EEWE);   // Write Enable
  sei();
}

uint8_t EEPROM_Read(void)
{
  uint8_t x;
  while (EECR & (1 << EEWE))  // wait until any previous write is finished
    ;
  cli();
  EEAR = 0;             // Byte # 0
  EECR |= (1 << EERE);  // Read Enable
  x = EEDR;             // Read Data
  sei();
  return x;
}

uint8_t WasInRunningState(void)
{
  return (was_in_running_state_RAM == LIVE_PATTERN);
}

void SetRunningState(uint8_t on)
{
  if (on)
    was_in_running_state_RAM = LIVE_PATTERN;
  else
    was_in_running_state_RAM = !LIVE_PATTERN;
}

uint8_t WasInActiveState(void)
{
  if (was_in_active_state_RAM == LIVE_PATTERN)  // RAM already set ?
    return 1;
  else if (was_in_active_state_RAM == !LIVE_PATTERN)
    return 0;
  else  // RAM has illegal value, now read state from EEPROM
  {
    was_in_active_state_RAM = EEPROM_Read();
    if (was_in_active_state_RAM == LIVE_PATTERN)
      return 1;
    else
    {
      was_in_active_state_RAM = !LIVE_PATTERN;
      return 0;
    }
  }
}

void SetActiveState(uint8_t on)
{
  static uint8_t old = 0xAA;            // illegal bool value to force update on first call
  if (config.power_switch_is_latching)  // disable the feature if latching switch
    on = 0;
  if (on != old)
  {
    old = on;
    if (on)
      EEPROM_Write((was_in_active_state_RAM = LIVE_PATTERN));  // intentional assignment
    else
      EEPROM_Write((was_in_active_state_RAM = !LIVE_PATTERN));  // intentional assignment
  }
}

//----------------------------------------------------------------------
void HardwareInit_1(void)  // primary hardware init that doesn't need IRQ (timer) functionality
{
  config.firmware_id = FW_Version_ID;
  config.status      = 0;
  IOPortInit();
  TimerInit();
  ComparatorInit();
  COMM_DeInit();
}

void SwitchMainSupply(uint8_t on)
{
  if (!config.pwr_fet_readback)  // old style hardware must config this I/O-pin as output at all times
  {
    PinOpt(SYS_IOPT_19V_FET);
    if (on)
      PinSet(SYS_IOPT_19V_FET);
    else
      PinClr(SYS_IOPT_19V_FET);
    return;
  }

  // come here only with readback-enabled hardware
  if (on)
  {
    PinPue(
        SYS_IOPT_19V_FET);  // Write a "1" to the pin's data register so that it remains high immediately after changing dir to output
    PinOpt(SYS_IOPT_19V_FET);
    PinSet(SYS_IOPT_19V_FET);
    Wait(10);
    PinIpt(SYS_IOPT_19V_FET);  // revert to input pin again (external logic will keep input high by positive feedback)
    PinPud(SYS_IOPT_19V_FET);  // and clear pullup and data register
  }
  else
  {                            // switch off. NOTE: if the BBB has overriden the supply then this will have no effect (as it's done in hardware)
    PinPud(SYS_IOPT_19V_FET);  // clear pullup and data register
    PinOpt(SYS_IOPT_19V_FET);
    PinClr(SYS_IOPT_19V_FET);
    Wait(10);
    PinIpt(SYS_IOPT_19V_FET);  // revert to input pin again (external logic will keep input low by positive feedback)
  }
}

void HardwareInit_2(void)  // secondary hardware init that *DOES* need IRQ (timer) functionality
{
  COMM_DeInit();
  config.auto_reboot = WasInActiveState();
  Wait(2);
  if (PinGet(SYS_IOPT_Relays_MUTE))  // ext Pullup on MUTE means 7.1 hardware
  {
    config.power_switch_is_latching = !PinGet(SYS_IPT_CFG0);
    config.mute_is_monostable       = !PinGet(SYS_IPT_CFG1);
    config.pwr_fet_readback         = 1;
    config.hot_reset                = PinGet(SYS_IOPT_19V_FET) && (WasInRunningState());
    // ^ true == reset occured while system was on (either a spurious reset or BBB has re-progged us)
    config.hardware_id = 71;
    PinIpt(BBB_IPT_HeartBeat_7_1);
    PinPue(BBB_IPT_HeartBeat_7_1);  // enable PU (pin might be floating)
  }
  else  // not a 7.1 hardware
  {
    config.power_switch_is_latching = 0;
    config.hot_reset                = 0;
    config.pwr_fet_readback         = 0;
    PinIpt(BBB_IPT_HeartBeat_7_0);
    PinPue(BBB_IPT_HeartBeat_7_0);  // enable PU (pin might be floating)

    // now check for either 7.0 or 6.0
    PinOpt(SYS_OPT_Pod0);
    PinClr(SYS_OPT_Pod0);
    PinIpt(SYS_OPT_Pod1);
    PinPue(SYS_OPT_Pod1);
    Wait(2);
    // In V7 Pod1 and Pod0 are connected via 10k,
    // so the internal weak pullup cannot pull Pod1 high
    if (PinGet(SYS_OPT_Pod1) == 0)
    {  // V7 hardware
      PinIpt(SYS_OPT_Pod0);
      PinPue(SYS_OPT_Pod0);
      config.hardware_id        = 70;  // 7.0 Hardware
      config.mute_is_monostable = 0;
    }
    else
    {  // V6 hardware
      PinIpt(SYS_OPT_Pod0);
      PinPue(SYS_OPT_Pod0);
      config.hardware_id        = 60;  // 6.0 Hardware
      config.mute_is_monostable = 1;
    }
  }
  PinOpt(SYS_IOPT_Relays_MUTE);
  PinSet(SYS_IOPT_Relays_MUTE);

  if (config.hot_reset)
    config.auto_reboot = 0;

  SwitchMainSupply(config.hot_reset);  // this is actually redundant but set's the pin status correctly for readback
}

void LPC_Reset(void)
{
  switch (config.hardware_id)
  {
    case 70:  // 7.0 hardware
      PinOpt(SYS_OPT_Pod1);
      PinClr(SYS_OPT_Pod1);  // Pull RESET down via diode
      Wait(2);
      PinSet(SYS_OPT_Pod1);
      PinIpt(SYS_OPT_Pod1);
      PinPue(SYS_OPT_Pod1);
      break;
    case 71:  // 7.1 hardware
      PinOpt(LPC_OPT_nReset);
      PinClr(LPC_OPT_nReset);  // Pull RESET down via diode
      Wait(2);
      PinSet(LPC_OPT_nReset);
      PinIpt(LPC_OPT_nReset);
      PinPue(LPC_OPT_nReset);
      break;
      // 6.0 hardware has no LPC reset mechanism
  }
  // LPC-Reset is connected to Pod1 ("P0" on PCB Silkscreen) via diode
}

uint8_t GetAudioEngineSignalPin(void)
{
  return PinGet(LPC_MUTE_REQ);
}

void SetUnmuteSignalPin(uint8_t new_state)
{
  if (new_state)
    PinSet(LPC_UNMUTE_ACK);
  else
    PinClr(LPC_UNMUTE_ACK);
}

// ---------------- special code --------------------

uint8_t mcucsr_mirror __attribute__((section(".noinit")));

// watchdog disable during reset
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void wdt_init(void)
{
  mcucsr_mirror = MCUCSR;  // MCUCSR must be read once before zeroing it
  MCUCSR        = 0;
  wdt_disable();  // disable watchdog just to make sure
#if DISABLE_JTAG
  BitSet(MCUCSR, JTD);
  BitSet(MCUCSR, JTD);
#endif
}

// reset (user function) via watchdog
void FullReset(void)
{
  wdt_enable(WDTO_15MS);  // enable watchdog, shortest timeout is 15ms
  cli();
  while (1)  // stall here
    ;
}

// end of file
