// global compile time settings and options
// KSTR 2019-06-07

#ifndef __globals_h__
#define __globals_h__

//----------- for debugging
#define DEBUG_LED    (0)  // set this != 0 to overide phyical LED with debug outputs
#define DISABLE_JTAG (1)  // set this to != 0 to disable JTAG and make ports PC2&PC3 available

// debug output functions
#if DEBUG_LED
#define DebugLed(x)                                              \
  {                                                              \
    (x) ? PinSet(SYS_OPT_SystemLed) : PinClr(SYS_OPT_SystemLed); \
  }
#define DebugLedTgl() PinTgl(SYS_OPT_SystemLed)
#else
#define DebugLed(x)
#define DebugLedTgl()
#endif

// COMPILE-TIME OPTIONS :

/****
Firmware versions and Internal ID Numbers
The ID Numbers+1 will be displayed as LED blinks during Power-Up, after HW- Version
	5.0			= 0
	5.1			= 0
	5.2			= 0
	etc
****/
#define FW5_Version_ID (1)
#define FW_Version_ID  (50 + FW5_Version_ID)

// --------- define this != 0 to check and display severe ePC status error
#define __CHECK_EPC__ (0)
/* 
	When defined, this checks whether sensed ePC active indicator did change state
	after actuating ePC power button. If it didn't change, something is
	wrong and system LED shows an Error Pattern. 
	Check is performed after the initial start-up wait time (~20sec).
	Also, after start-up the signal is periodically checked and
	an Error pattern is displayed on LED when not present.
*/

// --------- define this != 0 to check and catch severe BBB status error
#define __CHECK_BBB__ (0)
/* 
	When defined, this checks whether BBB signals its 'alive' signal.
	If not, system LED shows an Error Pattern.
	Check is performed after the initial start-up wait time (~20sec).
	Also, after start-up the signal is periodically checked and
	an Error pattern is displayed on LED when not present.
*/

// -------------- step-chain process time-slices and offsets (in ms)
// times should be preferably the same as much as possible to maintain good load balancing via offsets
#define TSLICE (50)  // ~50ms seems to be a good overall slice

#define SM_TIMESLICE        TSLICE        // granularity of the main state machine
#define LED_TIMESLICE       TSLICE        // shortest ON or OFF time
#define AUDIO_TIMESLICE     TSLICE        // 50ms is long enough for relay settling
#define BBB_TIMESLICE       (2 * TSLICE)  // 100ms should be OK for power button min ON/OFF time
#define ePC_TIMESLICE       (2 * TSLICE)  // must be longer than 50 (ms) according to Intel NUC power button spec
#define PSW_TIMESLICE       TSLICE        // polling interval for power button/switch
#define ESPI_TIMESLICE      (1)           // initial ESPI override to set all LEDs on TopCover and PanelUnit, max speed
#define MONENGINE_TIMESLICE (10)          // 10ms, do not change (code in the LPC depends on it)

#define NO_OF_TASKS 8  // 8 tasks total, see above

#define TS_OFFSET (TSLICE / NO_OF_TASKS)  // distribute tasks evenly

#define UNMUTE_TIMEOUT   (30000ul)  // default timeout in ms from power up to audio unmute (overriden by audio engine status scanning, if present)
#define BOOTING_FINISHED (15000ul)  // estimated booting time in ms (after which ePC/BBB finally respond to "power down" requests, and power button is armed again)

#endif

// end of file
