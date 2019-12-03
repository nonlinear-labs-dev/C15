// hardware defintions and support routines
// KSTR 2019-06-07

#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include <avr/io.h>
#include <stdint.h>
#include "bit_manipulation.h"
#include "pin_manipulation.h"

//----------------- PORT PINS ---------------
// System
#define SYS_ADC_19V           PORTA, 7  // Sense of incoming 19V supply
#define SYS_OPT_SystemLed     PORTD, 7  // LED on BackPanel, or inside on-board PowerSwitch
#define SYS_IPT_nPwrSwitch    PORTB, 2  // Pushbutton onBackPanel, or on-board PowerSwitch
#define SYS_IOPT_19V_FET      PORTB, 1  // In/Out: Main system power high side FET switch (19V), which may be overridden by BBB
#define SYS_IOPT_Relays_MUTE  PORTB, 0  // In/Out: not used, or Mute Coil of Bi-Stable Relay if old Audio Section, Input: Hardware ID: 0=7.0,  1=7.1 (Pullup)
#define SYS_OPT_Relays_UNMUTE PORTB, 3  // UnMute Relay (normally closed), orUnMute Coil of Bi-Stable Relay if old Audio Section

// Supply sequencing
#define SYS_OPT_5V_FET PORTC, 0  // Optional 5V high side FET switch
#define SYS_OPT_3V_FET PORTC, 1  // Optional 3.3V high side FET switch

// Ribbon and Panel LED initializing
#define SYS_OPT_ESPI_DISABLE PORTA, 2  // disable the ESPI access from BBB
#define SYS_OPT_ESPI_SAP     PORTA, 1  // override SAP (data latch) signal
#define SYS_OPT_ESPI_SCK     PORTA, 0  // override SCK (data clock) signal

// Configuration
#define SYS_IPT_CFG0 PORTB, 4      // Power switch type: 0(jumper closed)=latching,  1:(jumper open)=momentary
#define SYS_IPT_CFG1 PORTC, 7      // Muting Relays type: 0(jumper closed)=active Unmute relay,  1:(jumper open)=bi-stable relay
#define SYS_OPT_Pod1 SYS_IPT_CFG1  // needed for legacy HW7.0 detecting

// ePC control
#define EPC_OPT_Pwr            PORTA, 5  // Power Button
#define EPC_IPT_nSystemRunning PORTA, 6  // "active" indicator , inverted signalling!

// LPC control
#define LPC_OPT_nReset PORTC, 6        // Reset LPC (via or-ing diode)
#define SYS_OPT_Pod0   LPC_OPT_nReset  // needed for legacy HW7.0 detecting
#define LPC_MUTE_REQ   PORTA, 3
#define LPC_UNMUTE_ACK PORTA, 4

// Beagle Bone control
#define BBB_OPT_ShutDown      PORTD, 3  // Power Button
#define BBB_IPT_SystemRunning PORTD, 4  // "active" indicator
#define BBB_IPT_HeartBeat_7_1 PORTD, 5  // nonlinux system heartbeat for 7.1 hardware
#define BBB_IPT_HeartBeat_7_0 PORTD, 0  // nonlinux system heartbeat for 6.0 and 7.0 hardware
#define BBB_SER_RXD           PORTD, 0  // serial ifc to BBB  Receive Line
#define BBB_SER_TXD           PORTD, 1  // serial ifc to BBB  Transmit Line

// test & debug GPIOs
#define SYS_OPT_LED_A PORTC, 2
#define SYS_OPT_LED_B PORTC, 3

// unused pins
#define PORT_B5 PORTB, 5
#define PORT_B6 PORTB, 6
#define PORT_B7 PORTB, 7
#define PORT_C4 PORTC, 4
#define PORT_C5 PORTC, 5
#define PORT_D1 PORTD, 1
#define PORT_D2 PORTD, 2
#define PORT_D6 PORTD, 6

//-------------- PROCEDURES ------------------

void HardwareInit_1(void);  // primary hardware init that doesn't need IRQ (timer) functionality
void HardwareInit_2(void);  // secondary hardware init that *DOES* need IRQ (timer) functionality

void LPC_Reset(void);  // reset LPC if hardware is capable of this

void ArmComparatorIRQ(void);  // call this after power good has been detected

void PowerLED(uint8_t on);   // turn hardware LED on when "on" != 0
void PowerLED_Toggle(void);  // toggle hardware LED
// NOTE: if DEBUG_LED is defined to be != 0 in globals.h then no phyiscal output!!

void DebugHalt(uint8_t led);  // display a state via LED and go to a hard stop. Not overridden or disabled by "DEBUG_LED", use with caution!
void Wait(uint16_t ms);       // blocking wait (interrupts must be enabled!)
void FullReset(void);         // reset uC to a full cold start via Watchdog function, takes 15ms

uint8_t GetPowerSwitch(void);  // get current state of hardware power switch/button

void LED_A(int8_t on);  // debug-LED "A": ==0 ==> OFF, >0 ==> ON, <0 ==> toggle
void LED_B(int8_t on);  // debug-LED "B": ==0 ==> OFF, >0 ==> ON, <0 ==> toggle

void SwitchMainSupply(uint8_t on);  // switch main supply on/off (19V high-side FET)

uint8_t WasInRunningState(void);
void    SetRunningState(uint8_t on);
uint8_t WasInActiveState(void);
void    SetActiveState(uint8_t on);

uint8_t GetAudioEngineSignalPin(void);
void    SetUnmuteSignalPin(uint8_t new_state);

//-------------- VARIABLES

extern volatile uint16_t ticker;        // system ticker
extern volatile uint8_t  power_failed;  // flag to indicate a power-fail

typedef enum
{
  RS_STANDBY  = 0x00,
  RS_BOOTING  = 0x01,
  RS_RUNNING  = 0x02,
  RS_SHUTDOWN = 0x03
} run_state_t;

struct config_t
{  // master configuration variable, set be the Init routines
  uint8_t     power_switch_is_latching;
  uint8_t     mute_is_monostable;
  uint8_t     hot_reset;
  uint8_t     auto_reboot;
  uint8_t     pwr_fet_readback;
  uint8_t     hardware_id;  // decimal, either 71, 70, or 60, which means "7.1", "7.0", "6.0"
  uint8_t     firmware_id;  // decimal, 50 means 5.0, etc
  uint8_t     status;       // system status
  void        SetRunState(run_state_t state);
  run_state_t GetRunState(void);
};
extern config_t config;  // make available globally

// bit numbers for the status flags (Bit 7 is always 1 !!)
#define STAT_SYSTEM_STATUS0         (5)  // Bits 0 and 1 :   00=Standby; 01==Booting; 10==Running; 11==Shutdown
#define STAT_SYSTEM_STATUS1         (6)  // Bits 0 and 1 :   00=Standby; 01==Booting; 10==Running; 11==Shutdown
#define STAT_SYSTEM_LOCKED          (4)  // 1== System is locked against power down
#define STAT_UNMUTED                (0)  // 1== Audio path is unmuted
#define STAT_MUTING_OVERRIDE_VALUE  (1)  // 1== Audio path is unmuted
#define STAT_MUTING_OVERRIDE_ENABLE (2)  // 1== mute status is overridden
#define STAT_BIT3                   (3)  // unused

#endif

// end of file
