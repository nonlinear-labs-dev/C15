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
#define VEL_CURVE_COUNT     5

//======== public functions

void POLY_Select_VelTable(uint32_t const curve);
void POLY_Init(void);
void POLY_Process(void);
void POLY_ForceKey(uint16_t const midiKeyNumber, uint16_t const timeLow, uint16_t const timeHigh);
void POLY_KeyLogging(uint16_t const on);

#endif /* NL_TCD_POLY_H_ */
