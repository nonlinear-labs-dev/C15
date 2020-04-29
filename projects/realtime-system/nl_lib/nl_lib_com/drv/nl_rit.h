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
#include "cmsis/LPC43xx.h"

void RIT_Init_IntervalInUs(uint32_t time_us);
void RIT_Init_IntervalInNs(uint32_t ns);

static inline void RIT_ClearInt(void)
{
  LPC_RITIMER->CTRL |= 1;
};

static inline uint32_t RIT_GetCtrlReg(void)
{
  return LPC_RITIMER->CTRL;
};

static inline uint32_t RIT_GetIntFlag(void)
{
  return (LPC_RITIMER->CTRL & 1);
};

static inline uint32_t RIT_GetCounter(void)
{
  return LPC_RITIMER->COUNTER;
};

static inline uint32_t RIT_GetCompval(void)
{
  return LPC_RITIMER->COMPVAL;
};

#endif
