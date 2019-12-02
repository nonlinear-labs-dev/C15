/******************************************************************************/
/** @file		nl_espi_core.h
    @date		2014-06-28
    @brief    	eSPI core for LPC
    @example
    @ingroup  	LPC_ESPI
    @author		Nemanja Nikodijevic 2014-06-28
*******************************************************************************/

#ifndef NL_ESPI_CORE_H
#define NL_ESPI_CORE_H

#include "drv/nl_spi_dma.h"
#include "drv/nl_pin.h"

#define ESPI_CPOL_0 SSP_CPOL_HI
#define ESPI_CPOL_1 SSP_CPOL_LO
#define ESPI_CPHA_0 SSP_CPHA_FIRST
#define ESPI_CPHA_1 SSP_CPHA_SECOND

#define ESPI_PORT_OFF 7

typedef struct
{
  GPIO_NAME_T* scs[6];
  GPIO_NAME_T* dmx;
  GPIO_NAME_T* sap;
} ESPI_PINS_T;

void ESPI_Init(uint32_t clkRateInHz);
void ESPI_Config(LPC_SSPn_Type* SSPx, ESPI_PINS_T* espi_pins);

void     ESPI_SAP_Set(void);
void     ESPI_SAP_Clr(void);
void     ESPI_SCS_Select(uint32_t port, uint32_t device);
uint32_t ESPI_Transfer(uint8_t* tx, uint8_t* rx, uint32_t len, TransferCallback cb);
uint32_t ESPI_TransferNonBlocking(uint8_t* tx, uint8_t* rx, uint32_t len, TransferCallback cb);

#endif
