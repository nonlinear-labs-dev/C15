/******************************************************************************/
/**	@file		nl_kbs.h
	@brief    	Scans Keys of the Fatar Keyboard TP8S 61 Keys
	@date		2015-02-05 DTZ
  	@author		Daniel Tzschentke [2012-05-29]
*******************************************************************************/
#pragma once

#define NUM_KEYS 64

void KBS_Init(void);
extern void (*KBS_Process)(void);
