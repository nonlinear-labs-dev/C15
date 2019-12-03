/*
 * nl_tcd_expon.h
 *
 *  Created on: 21.02.2015
 *      Author: ssc
 */

#ifndef NL_TCD_NL_TCD_EXPON_H_
#define NL_TCD_NL_TCD_EXPON_H_

#include "stdint.h"

#define TIME_UNIT_MS 0.0208333  // a sample period of 48 kHz [in ms]

void EXPON_Init(void);

uint32_t EXPON_Time(int32_t paramVal);

#endif /* NL_TCD_NL_TCD_EXPON_H_ */
