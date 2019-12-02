/*
 * nl_tcd_poly.h
 *
 *  Created on: 30.01.2015
 *      Author: ssc
 */

#ifndef NL_TCD_POLY_H_
#define NL_TCD_POLY_H_

#include "stdint.h"

//======== defines

#define VEL_CURVE_VERY_SOFT 0
#define VEL_CURVE_SOFT      1
#define VEL_CURVE_NORMAL    2
#define VEL_CURVE_HARD      3
#define VEL_CURVE_VERY_HARD 4

//======== public functions

void POLY_Generate_VelTable(uint32_t curve);

void POLY_Init(void);

void POLY_KeyDown(uint32_t voice, uint32_t key, uint32_t timeInUs);
void POLY_KeyUp(uint32_t voice, uint32_t timeInUs);

void POLY_SetUnisonVoices(int32_t numUnisonVoices);

void POLY_UseScale(uint32_t use);
void POLY_SetScaleBase(uint32_t baseKey);
void POLY_SetScaleOffset(uint32_t step, uint32_t offset);

#endif /* NL_TCD_POLY_H_ */
