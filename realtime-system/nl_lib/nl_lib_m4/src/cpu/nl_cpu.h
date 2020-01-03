/******************************************************************************/
/** @file		nl_cpu.h
    @date		2015-01-30
    @author		Daniel Tzschentke [2015-01-30]
*******************************************************************************/
#ifndef NL_CPU
#define NL_CPU

#define M0APP_IRQ_PRIORITY 7

#define IFLASH_BANKA_ADDR 0x1A000000
#define IFLASH_BANKB_ADDR 0x1B000000

#define M0_IMAGE_ADDR IFLASH_BANKB_ADDR

void CPU_M0_Init(void);

#endif
