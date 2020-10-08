/******************************************************************************/
/** @file		nl_rit.h
    @date		2015-01-30
    @brief    	RIT driver for LPC43xx
    @example
    @ingroup  	LPC_RIT
    @author		Nemanja Nikodijevic 2014-08-15
*******************************************************************************/
#pragma once

#include <stdint.h>
#include "cmsis/LPC43xx.h"
#include "drv/nl_cgu.h"

inline void RIT_Init_IntervalInHz(uint32_t const freq_hz)
{
  /* Initialize RITimer */
  LPC_RITIMER->COMPVAL = 0xFFFFFFFF;
  LPC_RITIMER->MASK    = 0x00000000;
  LPC_RITIMER->CTRL    = 0x0C;
  LPC_RITIMER->COUNTER = 0x00000000;

  /* Set timer compare value */
  LPC_RITIMER->COMPVAL = NL_LPC_CLK / freq_hz;

  /* Set timer enable clear bit to clear timer to 0 whenever
	 * counter value equals the contents of RICOMPVAL */
  LPC_RITIMER->CTRL |= (1 << 1);

  NVIC_EnableIRQ(RITIMER_IRQn);
}

inline void RIT_SetCompVal(uint32_t const value)
{
  LPC_RITIMER->COMPVAL = value;
}

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
