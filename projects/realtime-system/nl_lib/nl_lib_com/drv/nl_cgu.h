/******************************************************************************/
/** @file		nl_cgu.h
    @date		2015-01-28
    @brief    	CGU functions
    @author		Nemanja Nikodijevic [2015-01-28]
*******************************************************************************/
#ifndef NL_CGU_H
#define NL_CGU_H

#define NL_LPC_CLK                  (204000000ul)
#define NL_LPC_CLK_PERIOD_100PS     (10000000000ul / NL_LPC_CLK)
#define NL_LPC_AUDIO_CLK            (12288000ul)
#define M0_PERIOD_NS                (125000ul)
#define M0_PERIOD_US                (M0_PERIOD_NS / 1000ul)
#define M0_SYSTICKS_PER_PERIOD      (128ul)  // 2^n !! requested systicks per 125us period
#define M0_SYSTICKS_PER_PERIOD_MASK (M0_SYSTICKS_PER_PERIOD - 1ul)
#define M0_SYSTICK_IN_NS            (M0_PERIOD_NS / M0_SYSTICKS_PER_PERIOD)  // period of one systick

#endif
