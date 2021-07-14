// LED command processing
// KSTR 2019-06-07

#include <stdint.h>
#include <stddef.h>
#include "globals.h"
#include "hardware.h"
#include "led.h"

Led_t Led;  // public handler

// LED command array:
// A command string is terminated with a zero.
// The absolute value of each command values represent a wait time,
// the sign determines LED on (positive) or off (negative).
// The wait time is in task ticks, defined as LED_TIMESLICE in milliseconds.
// If a 0 wait time would result when less than LED_TIMESLICE milliseconds are requested,
// then at least 1 timeslice will be used.
// If the wait time in ticks is larger than a signed 8-bit int can represent,
// then a compile time error is generated ("-Werror=narrowing" compiler switch must be set!).
// The command string is specified with ON(ms) or OFF(ms) macros.

#define t(ms)   ((ms) >= LED_TIMESLICE ? (ms) / LED_TIMESLICE : 1)
#define ON(ms)  (+t(ms))
#define OFF(ms) (-t(ms))

// the LED display sequences
const int8_t SEQ_NOP[]         = { 0 };                                     // does nothing
const int8_t SEQ_OFF[]         = { OFF(200), 0 };                           // turn OFF, refresh port every 200ms
const int8_t SEQ_ON[]          = { ON(200), 0 };                            // turn ON, refresh port every 200ms
const int8_t SEQ_25ms_BLINK[]  = { ON(25), OFF(25), 0 };                    // 25ms ON, 25ms OFF loop
const int8_t SEQ_50ms_BLINK[]  = { ON(50), OFF(50), 0 };                    // 50ms ON, 50ms OFF loop
const int8_t SEQ_100ms_BLINK[] = { ON(100), OFF(100), 0 };                  // 100ms ON, 100ms OFF loop
const int8_t SEQ_200ms_BLINK[] = { ON(200), OFF(200), 0 };                  // 200ms ON, 200ms OFF loop
const int8_t SEQ_500ms_BLINK[] = { ON(500), OFF(500), 0 };                  // 500ms ON, 500ms OFF loop
const int8_t SEQ_BLINK_STBY[]  = { ON(25), OFF(2975), 0 };                  // 25ms ON, 3seconds OFF loop
const int8_t SEQ_ON_GLITCH1[]  = { ON(950), OFF(50), 0 };                   // 1sec ON, 50ms OFF loop
const int8_t REQ_ON_GLITCH2[]  = { ON(850), OFF(50), ON(50), OFF(50), 0 };  // 1sec ON, 50ms OFF loop
const int8_t SEQ_PWR_CYCLING[] = { ON(50), OFF(50), 0 };                    // rapid 50ms blinking

static const int8_t *    pSeq_start = SEQ_NOP;  // sequence pointer
static const int8_t *    pSeq       = SEQ_NOP;  // ^
static volatile uint16_t wait       = 0;        // timer variable

// special function "hardware and firmware ID" that is handled in code, not data
static uint8_t hw_blinks;    // blink count for HW revision
static uint8_t fw_blinks;    // blink count for FW revision
static uint8_t show_id = 0;  // triggering

static uint8_t doPowerCyclingOverride = 0;

static int8_t showPowerCyclingOverride(void);
static int8_t showHWandFWids(void);
static int8_t getNextCmd(void);
static void   displayCmd(const int8_t c);
static void   applyWait(int8_t c);
static void   DisplayCmdString(void);
static void   Seq_Set(const int8_t *seq);

void Led_t::On(void)
{
  Seq_Set(SEQ_ON);
};

void Led_t::Off(void)
{
  Seq_Set(SEQ_OFF);
};

void Led_t::Blink_VeryFast(void)
{
  Seq_Set(SEQ_25ms_BLINK);
};

void Led_t::Blink_Fast(void)
{
  Seq_Set(SEQ_100ms_BLINK);
};

void Led_t::Blink_Medium(void)
{
  Seq_Set(SEQ_200ms_BLINK);
};

void Led_t::Blink_Slow(void)
{
  Seq_Set(SEQ_500ms_BLINK);
};

void Led_t::Blink_Standby(void)
{
  Seq_Set(SEQ_BLINK_STBY);
};

void Led_t::On_IndicateGlitch1(void)
{
  Seq_Set(SEQ_ON_GLITCH1);
};

void Led_t::On_IndicateGlitch2(void)
{
  Seq_Set(REQ_ON_GLITCH2);
};

void Led_t::PowerCyclingOverride(uint8_t on)
{
  doPowerCyclingOverride = on;
};

void Led_t::Show_Hardware_and_Firmware_ID(void)  // handle special case
{
  show_id = 1;                 // flag special case
  switch (config.hardware_id)  // setup blink counter according to HW revision
  {
    case 70:
      hw_blinks = 2;
      break;  // 7.0
    case 71:
      hw_blinks = 3;
      break;  // 7.1
    default:
      hw_blinks = 1;
      break;  // 6.0 assumed
  }
  fw_blinks = FW5_Version_ID + 1;  // set blink counter according to firmware revision
}

static int8_t showPowerCyclingOverride(void)
{
  static const int8_t *pSeq = SEQ_PWR_CYCLING;
  int8_t               c    = *pSeq++;
  if (!c)
    pSeq = SEQ_PWR_CYCLING;
  return c;
}

static int8_t showHWandFWids(void)
{
  static uint8_t step = 0;
  int8_t         c;
  switch (step)
  {
    case 0:
      c = OFF(1000);
      step++;
      break;  // 1 sec OFF : prefix
    case 1:
      c = ON(200);
      step++;
      break;  // 200ms ON, then...
    case 2:
      c = OFF(800);
      step += --hw_blinks ? -1 : +1;
      break;  // ... 800ms OFF, and repeat if more
    case 3:
      c = OFF(1700);
      step++;
      break;  // 2.5 total OFF : delimiter
    case 4:
      c = ON(800);
      step++;
      break;  // 800ms ON, then...
    case 5:
      c = OFF(200);
      step += --fw_blinks ? -1 : +1;
      break;  // ... 200ms OFF, and repeat if more
    case 6:
      c = OFF(2300);
      step++;
      break;  // 2.5 sec total OFF : postfix
    default:
      c       = 0;
      show_id = 0;  // stop
      step    = 0;  // reset sequence
      break;
  }
  return c;
}

void LED_Process(void)
{
  DisplayCmdString();
}

static void DisplayCmdString(void)
{
  if (wait)  // timer running ?
  {
    wait--;
    return;
  }
  int8_t c = getNextCmd();
  displayCmd(c);
  applyWait(c);
}

static int8_t getNextCmd(void)
{
  int8_t c = *pSeq++;
  if (!c)
    pSeq = pSeq_start;
  if (show_id)
    return showHWandFWids();
  if (doPowerCyclingOverride)
    return showPowerCyclingOverride();
  return c;
}

static void displayCmd(const int8_t c)
{
  if (!c)
    return;
  PowerLED(c > 0);
}

static void applyWait(int8_t c)
{
  if (!c)
    return;
  if (c < 0)
  {
    if (c == -128)
      c++;
    c = -c;
  }
  wait = c - 1;
}

static void Seq_Set(const int8_t *seq)
{
  pSeq_start = pSeq = seq;
  wait              = 0;
}

// end of file