/******************************************************************************/
/** @file		nl_rit.h
    @date		2015-01-30
    @brief    	RIT driver for LPC43xx
    @example
    @ingroup  	LPC_RIT
    @author		Nemanja Nikodijevic 2014-08-15
*******************************************************************************/
#ifndef NL_RIT_H
#define NL_RIT_H

#include <stdint.h>

void RIT_Init_IntervalInUs(uint32_t time_us);
void RIT_Init_IntervalInNs(uint32_t ns);
void RIT_ClearInt(void);

#endif
