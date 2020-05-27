// LED command processing
// KSTR 2019-06-07

#include <stdint.h>
#include <stddef.h>
#include "globals.h"
#include "hardware.h"
#include "led.h"

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

const int8_t LCA_NOP[]         = { 0 };                                     // does nothing
const int8_t LCA_OFF[]         = { OFF(200), 0 };                           // turn OFF, refresh port every 200ms
const int8_t LCA_ON[]          = { ON(200), 0 };                            // turn ON, refresh port every 200ms
const int8_t LCA_50ms_BLINK[]  = { ON(50), OFF(50), 0 };                    // 50ms ON, 50ms OFF loop
const int8_t LCA_100ms_BLINK[] = { ON(100), OFF(100), 0 };                  // 100ms ON, 100ms OFF loop
const int8_t LCA_200ms_BLINK[] = { ON(200), OFF(200), 0 };                  // 200ms ON, 200ms OFF loop
const int8_t LCA_500ms_BLINK[] = { ON(500), OFF(500), 0 };                  // 500ms ON, 500ms OFF loop
const int8_t LCA_BLINK_STBY[]  = { ON(50), OFF(1950), 0 };                  // 50ms ON, 2seconds OFF loop
const int8_t LCA_ON_GLITCH1[]  = { ON(950), OFF(50), 0 };                   // 1sec ON, 50ms OFF loop
const int8_t LCA_ON_GLITCH2[]  = { ON(850), OFF(50), ON(50), OFF(50), 0 };  // 1sec ON, 50ms OFF loop

static const int8_t *    pLca_start = LCA_NOP;  // command pointer
static const int8_t *    pLca       = LCA_NOP;  // ^
static volatile uint16_t wait       = 0;        // timer variable

static uint8_t override   = 0;  // flag for an overriding LED sequence
static uint8_t last_state = 0;  // temp variable to update LED when override is over

// override variables
static const int8_t *pOLca_start = LCA_NOP;  // command pointer
static const int8_t *pOLca       = LCA_NOP;  // ^
static uint16_t      Owait       = 0;        // timer variable

// special function "hardware and firmware ID" that is handled in code, not data
static uint8_t show_id = 0;  // triggering
static uint8_t step    = 0;  // step-chain variable
static uint8_t hw_blinks;    // blink count for HW revision
static uint8_t fw_blinks;    // blink count for FW revision

Led_t Led;  // public handler

void LED(uint8_t on)
{
  if (!override)  // no physical output when in overrride
    PowerLED(on);
  last_state = on;
}

void Normal(void)
{
  if (!pLca)
    return;
  if (wait)  // timer running ?
  {
    wait--;
    return;
  }

  int8_t c = *pLca;  // get command
  if (c == 0)        // end of sequence ?
  {
    pLca = pLca_start;  // reset sequence
    return;
  }

  pLca++;      // forward to next command
  LED(c > 0);  // apply cmd to hardware LED (unless overriden), >0 means ON
  if (c < 0)
  {
    if (c == -128)
      c = 127;  // catch 2's complement corner case
    else
      c = -c;
  }
  wait = (c - 1);
}

void Override(void)
{
  if (!pOLca)
    return;
  if (Owait)  // timer running ?
  {
    Owait--;
    return;
  }

  int8_t c;

  if (show_id)  // special case "hardware and firmware ID"
  {
    pOLca = &c;  // force command address to local variable
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
        show_id = 0;
        break;  // stop chain
    }
  }

  c = *pOLca;  // get current command
  if (c == 0)  // end of sequence ?
  {
    pOLca = pOLca_start = NULL;  // stop chain
    override            = 0;
    PowerLED(last_state);
    return;
  }

  pOLca++;          // forward to next command
  PowerLED(c > 0);  // apply cmd to hardware LED
  if (c < 0)
  {
    if (c == -128)
      c = 127;  // catch 2's complement corner case
    else
      c = -c;
  }
  Owait = (c - 1);
}

void LED_Process(void)
{
  if (override)
    Override();  // process override only when needed
  Normal();      // call normal sequence handler, it just does no physical output when overddiden
}

void LCA_Set(const int8_t *p)  // set a normal command
{
  pLca = pLca_start = p;
  wait              = 0;
}

void LCA_OSet(const int8_t *p)  // set an override command
{
  pOLca = pOLca_start = p;
  Owait               = 0;
  override            = 1;
}

void Led_t::On(void)
{
  LCA_Set(LCA_ON);
};
void Led_t::Off(void)
{
  LCA_Set(LCA_OFF);
};
void Led_t::Blink_VeryFast(void)
{
  LCA_Set(LCA_50ms_BLINK);
};
void Led_t::Blink_Fast(void)
{
  LCA_Set(LCA_100ms_BLINK);
};
void Led_t::Blink_Medium(void)
{
  LCA_Set(LCA_200ms_BLINK);
};
void Led_t::Blink_Slow(void)
{
  LCA_Set(LCA_500ms_BLINK);
};
void Led_t::Blink_Standby(void)
{
  LCA_Set(LCA_BLINK_STBY);
};
void Led_t::On_IndicateGlitch1(void)
{
  LCA_Set(LCA_ON_GLITCH1);
};
void Led_t::On_IndicateGlitch2(void)
{
  LCA_Set(LCA_ON_GLITCH2);
};

void Led_t::Show_Hardware_and_Firmware_ID(void)  // handle special case
{
  LCA_OSet(LCA_NOP);           // set a dummy pointer to have a valid one
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

// end of file