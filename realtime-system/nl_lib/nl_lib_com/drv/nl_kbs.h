/******************************************************************************/
/**	@file		ND_KEYBOARD.h
	@brief    	Scans Keys of the Fatar Keyboard TP8S 61 Keys
	@date		2015-02-05 DTZ
  	@author		Daniel Tzschentke [2012-05-29]
*******************************************************************************/
#ifndef NL_KBS_H_
#define NL_KBS_H_

#include "drv/nl_pin.h"

#define NUM_KEYS 64

typedef struct
{
  GPIO_NAME_T* line[4];
  GPIO_NAME_T* key[8];
  GPIO_NAME_T* dmx;
} KBS_PINS_T;

void KBS_Init(void);
void KBS_Config(KBS_PINS_T* pins);
void KBS_Process(void);

void KBS_Test_ScanLines(void);
void KBS_Test_CycleLines(void);

#endif
