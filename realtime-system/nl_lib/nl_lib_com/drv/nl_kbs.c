/******************************************************************************/
/**	@file		nl_keybed.c
	@brief    	This file is the equivalent to ND_KEYBOARD.h
				Scans Keys of the Fatar Keyboard TP8S 61 Keys
	@date		2015-02-26 DTZ
  	@author		Stephan Schmitt, Daniel Tzschentke [2012-12-20]
*******************************************************************************/
#include "ipc/emphase_ipc.h"
#include "stdint.h"
#include "drv/nl_kbs.h"
#include "drv/nl_pin.h"
#include "drv/nl_gpio.h"
#include "usb/nl_usb_midi.h"
#include "sys/delays.h"

#define DELAY_CYCLES        \
  DELAY_HUNDRED_CLK_CYCLES; \
  DELAY_HUNDRED_CLK_CYCLES; \
  DELAY_ONE_CLK_CYCLE;      \
  DELAY_ONE_CLK_CYCLE;      \
  DELAY_ONE_CLK_CYCLE;      \
  DELAY_ONE_CLK_CYCLE;

static KBS_PINS_T* kbsPins;

uint8_t GetKeys(void);

void SetHwLine(uint8_t line);
void CheckForStateChanges(uint8_t baseKey);
void Delay(uint32_t multi);

static uint8_t  keyState[NUM_KEYS] = {};
static uint32_t keyTime[NUM_KEYS]  = {};  // multiples of systick
static uint32_t timerTick          = 0;   // time base for all the functions
static uint8_t  upperSwitches      = 0;
static uint8_t  lowerSwitches      = 0;

/* variables for optimization */
static uint32_t linePort;
static uint32_t lineFirstPin;
static uint32_t lineOffValue;
static uint32_t keyPort;
static uint32_t keyFirstPin;

/******************************************************************************/
/**	@warning	The GetHw Functions are optimized. They will only work with
				HW-Pins starting at pin 0 on a port.
*******************************************************************************/
void GetHwUpperSwitches(void)
{
  upperSwitches = (uint8_t)(NL_GPIO_GetVal(keyPort));

#if 0  // slower but flexible
	upperSwitches = (uint8_t) (NL_GPIO_GetVal(kbsPins->key[0]->port) >> (kbsPins->key[0]->pin));
#endif

#if 0  // worst case test
	static uint8_t state = 0;
	if (state == 0)
	{
		upperSwitches = 0xFF;
		state = 0;
	}
	else
	{
		lowerSwitches = 0x00;
		state = 1;
	}
#endif
}

void GetHwLowerSwitches(void)
{
  lowerSwitches = (uint8_t)(NL_GPIO_GetVal(keyPort));

#if 0  // slower but flexible
	lowerSwitches = (uint8_t) (NL_GPIO_GetVal(kbsPins->key[0]->port) >> (kbsPins->key[0]->pin));
#endif

#if 0  // worst case test
	static uint8_t state = 0;
	if (state == 0)
	{
		upperSwitches = 0x00;
		state = 0;
	}
	else
	{
		lowerSwitches = 0xFF;
		state = 1;
	}
#endif
}

/******************************************************************************/
/**	@brief  	General initialization of the input and output pins. (public)
*******************************************************************************/
void KBS_Init(void)
{
  SetHwLine(1);  // Init the output pins
}

/******************************************************************************/
/**	@brief
*******************************************************************************/
void KBS_Config(KBS_PINS_T* pins)
{
  kbsPins      = pins;
  linePort     = kbsPins->line[0]->port;
  lineFirstPin = kbsPins->line[0]->pin;
  lineOffValue = (0x000000F << kbsPins->line[0]->pin);

  keyPort     = kbsPins->key[0]->port;
  keyFirstPin = kbsPins->key[0]->pin;
}

/******************************************************************************/
/**	@brief  	Sends the number of a Line (0...15) binary coded to 4 outputs
				(private)
				This function can only be used with a 0-alligned port
	@details 	The four pins drive a 'binary to 1-of-n hardware decoder'.
	@param		line number - 0..15
*******************************************************************************/
inline void SetHwLine(uint8_t line)
{
  NL_GPIO_Set(kbsPins->dmx);
  NL_GPIO_ClrVal(linePort, lineOffValue);
  NL_GPIO_SetVal(linePort, line << lineFirstPin);
  NL_GPIO_Clr(kbsPins->dmx);
}

/******************************************************************************/
/**	@brief  	general keyboard process function - it should be called 
				regularly (public)
	@details	for a high time resolution 100 us are great
				for a low time resolution 500 are acceptable
	@param		key
*******************************************************************************/
void KBS_Process(void)
{
  timerTick++;

  // 1
  GetHwUpperSwitches();
  SetHwLine(0);
  DELAY_CYCLES;
  GetHwLowerSwitches();
  SetHwLine(3);
  CheckForStateChanges(0);

  // 2
  GetHwUpperSwitches();
  SetHwLine(2);
  DELAY_CYCLES;
  GetHwLowerSwitches();
  SetHwLine(5);
  CheckForStateChanges(8);

  // 3
  GetHwUpperSwitches();
  SetHwLine(4);
  DELAY_CYCLES;
  GetHwLowerSwitches();
  SetHwLine(7);
  CheckForStateChanges(16);

  // 4
  GetHwUpperSwitches();
  SetHwLine(6);
  DELAY_CYCLES;
  GetHwLowerSwitches();
  SetHwLine(9);
  CheckForStateChanges(24);

  // 5
  GetHwUpperSwitches();
  SetHwLine(8);
  DELAY_CYCLES;
  GetHwLowerSwitches();
  SetHwLine(11);
  CheckForStateChanges(32);

  // 6
  GetHwUpperSwitches();
  SetHwLine(10);
  DELAY_CYCLES;
  GetHwLowerSwitches();
  SetHwLine(13);
  CheckForStateChanges(40);

  // 7
  GetHwUpperSwitches();
  SetHwLine(12);
  DELAY_CYCLES;
  GetHwLowerSwitches();
  SetHwLine(15);
  CheckForStateChanges(48);

  // 8
  GetHwUpperSwitches();
  SetHwLine(14);
  DELAY_CYCLES;
  GetHwLowerSwitches();
  SetHwLine(1);
  CheckForStateChanges(56);
}

/******************************************************************************/
/**	@brief  	handles changes of the key state
	@param		key
				state:
				- 0 = 000: key off, fully released
				- 1 = 001: key off, between the two contact points
				- 3 = 011: key off, fully pressed -> will go on
				- 4 = 100: key on, fully released -> will go off
				- 5 = 101: key on, between the two contact points
				- 7 = 111: key on, fully pressed
*******************************************************************************/
void DetectNotes(uint8_t key, uint8_t state)
{
  IPC_KEY_EVENT_T keyEvent;

  if (state == 1)  // upper contact closed or lower contact opened
  {
    keyTime[key]  = timerTick;                // start time measuring
    keyState[key] = (keyState[key] & 4) | 1;  // store changed state
  }
  else if (state == 3)  // lower contact closed
  {
    if (keyState[key] & 4)  // already on - no key-on event
    {
      keyState[key] = 7;  // store changed state
    }
    else
    {
      //volatile uint32_t diff = timerTick - keyTime[key];					// finish time measuring
      keyEvent.timeInUs  = (timerTick - keyTime[key]) * 125;
      keyEvent.key       = key;
      keyEvent.direction = KEY_DIR_DN;

      Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(keyEvent);

      keyState[key] = 7;  // key is down
    }
  }
  else  // state == 0: upper contact opened
  {
    if (!(keyState[key] & 4))  // already off - no key-off event
    {
      keyState[key] = 0;  // store changed state
    }
    else
    {
      //volatile uint32_t diff = timerTick - keyTime[key];					// finish time measuring
      keyEvent.timeInUs  = (timerTick - keyTime[key]) * 125;
      keyEvent.key       = key;
      keyEvent.direction = KEY_DIR_UP;

      Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(keyEvent);

      keyState[key] = 0;  // key is up
    }
  }
}

/******************************************************************************/
/**	@brief  	reads the state of 8 lower switches; if the state of the key has
				changed, detectNotes() is called	(private)
	@param		baseKey: base key of a group of 8 keys
				Emphase HW Version >= V2 Rev.A
				ReadKeysFromOddLine(uint8_t key);
*******************************************************************************/
void CheckForStateChanges(uint8_t baseKey)
{
  uint8_t state = ((!(lowerSwitches & 1)) << 1) | (!(upperSwitches & 1));

  if (state != (keyState[baseKey] & 3))
    DetectNotes(baseKey, state);

  baseKey++;
  state = ((!(lowerSwitches & 2)) << 1) | (!(upperSwitches & 2));

  if (state != (keyState[baseKey] & 3))
    DetectNotes(baseKey, state);

  baseKey++;
  state = ((!(lowerSwitches & 4)) << 1) | (!(upperSwitches & 4));

  if (state != (keyState[baseKey] & 3))
    DetectNotes(baseKey, state);

  baseKey++;
  state = ((!(lowerSwitches & 8)) << 1) | (!(upperSwitches & 8));

  if (state != (keyState[baseKey] & 3))
    DetectNotes(baseKey, state);

  baseKey++;
  state = ((!(lowerSwitches & 16)) << 1) | (!(upperSwitches & 16));

  if (state != (keyState[baseKey] & 3))
    DetectNotes(baseKey, state);

  baseKey++;
  state = ((!(lowerSwitches & 32)) << 1) | (!(upperSwitches & 32));

  if (state != (keyState[baseKey] & 3))
    DetectNotes(baseKey, state);

  baseKey++;
  state = ((!(lowerSwitches & 64)) << 1) | (!(upperSwitches & 64));

  if (state != (keyState[baseKey] & 3))
    DetectNotes(baseKey, state);

  baseKey++;
  state = ((!(lowerSwitches & 128)) << 1) | (!(upperSwitches & 128));

  if (state != (keyState[baseKey] & 3))
    DetectNotes(baseKey, state);
}

/******************************************************************************/
/** @brief	This test function will scan all the 16 lines every call.
*******************************************************************************/
void KBS_Test_ScanLines(void)
{
  uint8_t cnt = 0;
  for (; cnt <= 16; cnt++)
  {
    SetHwLine(cnt);
  }
}

/******************************************************************************/
/** @brief  This test function will increment the selected line every call.
*******************************************************************************/
void KBS_Test_CycleLines(void)
{
  static uint8_t cnt = 0;
  SetHwLine(cnt);

  cnt++;
  if (cnt >= 16)
  {
    cnt = 0;
  }
}

#if 0  // old SetLine function
	NL_GPIO_Set(kbsPins->dmx);
	NL_GPIO_ClrVal(kbsPins->line[0]->port, (0x000000F << kbsPins->line[0]->pin) );
	NL_GPIO_SetVal(kbsPins->line[0]->port, (line << kbsPins->line[0]->pin) );
	NL_GPIO_Clr(kbsPins->dmx);
#endif
