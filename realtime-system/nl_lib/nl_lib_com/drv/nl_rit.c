/******************************************************************************/
/** @file		nl_rit.c
    @date		2015-02-03 DTZ
    @brief    	RIT driver for LPC43xx
    @ingroup  	LPC_RIT
    @author		Nemanja Nikodijevic 2014-08-15
*******************************************************************************/
#include "cmsis/LPC43xx.h"
#include "drv/nl_cgu.h"
#include "drv/nl_rit.h"

void RIT_Init_IntervalInUs(uint32_t time_us)
{
  uint32_t cmp_value;

  /* Initialize RITimer */
  LPC_RITIMER->COMPVAL = 0xFFFFFFFF;
  LPC_RITIMER->MASK    = 0x00000000;
  LPC_RITIMER->CTRL    = 0x0C;
  LPC_RITIMER->COUNTER = 0x00000000;

  /* Determine approximate compare value based on clock rate and passed interval */
  cmp_value = (uint32_t)(NL_LPC_CLK / 1000000 * time_us);

  /* Set timer compare value */
  LPC_RITIMER->COMPVAL = cmp_value;

  /* Set timer enable clear bit to clear timer to 0 whenever
	 * counter value equals the contents of RICOMPVAL */
  LPC_RITIMER->CTRL |= (1 << 1);

  NVIC_EnableIRQ(RITIMER_IRQn);
}

/******************************************************************************/
/**	@brief  	cpu_clk / 1G * ns
*******************************************************************************/
void RIT_Init_IntervalInNs(uint32_t ns)
{
  uint32_t cmp_value;

  /* Initialize RITimer */
  LPC_RITIMER->COMPVAL = 0xFFFFFFFF;
  LPC_RITIMER->MASK    = 0x00000000;
  LPC_RITIMER->CTRL    = 0x0C;
  LPC_RITIMER->COUNTER = 0x00000000;

  /* Determine approximate compare value based on clock rate and passed interval */
  cmp_value = (uint32_t)(((NL_LPC_CLK / 1000000) * ns) / 1000);

  /* Set timer compare value */
  LPC_RITIMER->COMPVAL = cmp_value;

  /* Set timer enable clear bit to clear timer to 0 whenever
	 * counter value equals the contents of RICOMPVAL */
  LPC_RITIMER->CTRL |= (1 << 1);

  NVIC_EnableIRQ(RITIMER_IRQn);
}

void RIT_ClearInt(void)
{
  LPC_RITIMER->CTRL |= 1;
}
