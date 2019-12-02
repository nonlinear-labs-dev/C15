// 2015-01-30

#include "cmsis/LPC43xx.h"
#include "cpu/nl_cpu.h"

void CPU_M0_Init(void)
{
  LPC_RGU->RESET_CTRL1 = 1 << 24;  // M0 hold reset
  LPC_CCU1->CLK_M4_M0APP_CFG |= 1;
  LPC_CREG->M0APPMEMMAP = M0_IMAGE_ADDR & ~0xFFF;
  LPC_RGU->RESET_CTRL1  = 0;  // M0 release reset
}
