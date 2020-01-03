/******************************************************************************/
/** @file		nl_espi_attenuator.h
    @date		2015-02-19 DTZ
    @brief    	eSPI module for Attenuator
    @author		Nemanja Nikodijevic 2014-07-07
*******************************************************************************/
#ifndef NL_ESPI_ATTENUATOR_H
#define NL_ESPI_ATTENUATOR_H

#include "espi/nl_espi_core.h"

#define ESPI_ATTENUATOR_PORT   2
#define ESPI_ATTENUATOR_DEVICE 1

void     ESPI_DEV_Attenuator_Init(void);
void     ESPI_Attenuator_Channel_Set(uint8_t ch, uint8_t val);
uint32_t ESPI_Attenuator_EspiSend(uint8_t ch);
uint32_t ESPI_Attenuator_EspiSendIfChanged(uint8_t ch);

#endif
