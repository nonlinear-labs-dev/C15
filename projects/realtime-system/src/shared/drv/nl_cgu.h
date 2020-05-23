/******************************************************************************/
/** @file		nl_cgu.h
    @date		2015-01-28
    @brief    	CGU functions
    @author		Nemanja Nikodijevic [2015-01-28]
*******************************************************************************/
#pragma once

#define NL_LPC_CLK              (204000000ul)
#define NL_LPC_CLK_PERIOD_100PS (10000000000ul / NL_LPC_CLK)
#define NL_LPC_AUDIO_CLK        (12288000ul)

#define M4_PERIOD_US (5ul)  // M0 interrupt period in 1us multiples
#define M4_FREQ_HZ   (1000000 / M4_PERIOD_US)

#define M0_PERIOD_62_5NS (20ul)  // M0 interrupt period in 62.5ns multiples
#define M0_FREQ_HZ       (16000000ul / M0_PERIOD_62_5NS)
