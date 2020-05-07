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
#include "drv/nl_cgu.h"

inline void RIT_Init_IntervalInUs(uint32_t const time_us)
{
  uint32_t cmp_value;

  /* Initialize RITimer */
  LPC_RITIMER->COMPVAL = 0xFFFFFFFF;
  LPC_RITIMER->MASK    = 0x00000000;
  LPC_RITIMER->CTRL    = 0x0C;
  LPC_RITIMER->COUNTER = 0x00000000;

  /* Determine approximate compare value based on clock rate and passed interval */
  cmp_value = (uint32_t)(NL_LPC_CLK / 1000000ul * time_us);

  /* Set timer compare value */
  LPC_RITIMER->COMPVAL = cmp_value;

  /* Set timer enable clear bit to clear timer to 0 whenever
	 * counter value equals the contents of RICOMPVAL */
  LPC_RITIMER->CTRL |= (1 << 1);

  NVIC_EnableIRQ(RITIMER_IRQn);
}

inline void RIT_Init_IntervalInNs(uint32_t const ns)
{
  uint32_t cmp_value;

  /* Initialize RITimer */
  LPC_RITIMER->COMPVAL = 0xFFFFFFFF;
  LPC_RITIMER->MASK    = 0x00000000;
  LPC_RITIMER->CTRL    = 0x0C;
  LPC_RITIMER->COUNTER = 0x00000000;

  /* Determine approximate compare value based on clock rate and passed interval */
  cmp_value = (uint32_t)(((NL_LPC_CLK / 1000000ul) * ns) / 1000ul);

  /* Set timer compare value */
  LPC_RITIMER->COMPVAL = cmp_value;

  /* Set timer enable clear bit to clear timer to 0 whenever
	 * counter value equals the contents of RICOMPVAL */
  LPC_RITIMER->CTRL |= (1 << 1);

  NVIC_EnableIRQ(RITIMER_IRQn);
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

#endif
