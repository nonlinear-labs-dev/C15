/******************************************************************************/
/** @file		nl_tcd_pedals.h
    @date		2020-01-10
    @version	0
    @author		KSTR
    @brief		external controllers (pedals etc) processing
    @ingroup	nl_tcd_modules
*******************************************************************************/

#ifndef NL_TCD_PEDALS_H_
#define NL_TCD_PEDALS_H_

#include <stdint.h>

void NL_TCD_PEDALS_Init(void);
void NL_TCD_PEDALS_DeInit(void);
void NL_TCD_PEDALS_Process(void);

#endif
//EOF
