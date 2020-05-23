/******************************************************************************/
/** @file		nl_ehc_ctrl.h
    @date		2020-01-10
    @version	0
    @author		KSTR
    @brief		handlers for external controllers (pedals etc) processing
    @ingroup	nl_tcd_modules
*******************************************************************************/
#pragma once

#include <stdint.h>

void NL_EHC_InitControllers(void);
void NL_EHC_ProcessControllers1(void);
void NL_EHC_ProcessControllers2(void);
void NL_EHC_ProcessControllers3(void);
void NL_EHC_SetLegacyPedalType(const uint16_t controller, uint16_t type);
void NL_EHC_SetEHCconfig(const uint16_t cmd, uint16_t data);
void NL_EHC_RequestToSendEHCdata(void);
void NL_EHC_Enable(uint16_t flag);
void NL_EHC_ForceEepromUpdate(void);
