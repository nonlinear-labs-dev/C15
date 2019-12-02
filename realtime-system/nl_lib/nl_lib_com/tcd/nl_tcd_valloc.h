/******************************************************************************/
/** @file		nl_tcd_valloc.h
    @date		2013-04-22
    @version	0.02
    @author		Stephan Schmitt [2012-07-03]
    @brief		<tbd>
*******************************************************************************/

#ifndef VEL_VALLOC_H_
#define VEL_VALLOC_H_

#include "stdint.h"

//------- global defines

#define VALLOC_NUM_KEYS 61

#define NUM_VOICES 24  // max. number of voices (Array-size)

//------- public functions

void VALLOC_Init(uint32_t num);
void VALLOC_Process(void);

#endif /* VEL_VALLOC_H_ */
