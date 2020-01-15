/******************************************************************************/
/** @file		nl_ehc_ctrl.h
    @date		2020-01-10
    @version	0
    @author		KSTR
    @brief		handlers for external controllers (pedals etc) processing
    @ingroup	nl_tcd_modules
*******************************************************************************/

#ifndef NL_EHC_CTRL_H_
#define NL_EHC_CTRL_H_

#include <stdint.h>

typedef enum
{
  UNKNOWN,
  POT,
  RHEOSTAT,
  SWITCH,
  CV
} ControllerType_T;

void NL_EHC_InitControllers(void);
void NL_EHC_DeInitControllers(void);
void NL_EHC_ProcessControllers(void);

#endif
//EOF
