/******************************************************************************/
/** @file		nl_espi_core.h
    @date		2014-06-28
    @brief    	eSPI core for LPC
    @example
    @ingroup  	LPC_ESPI
    @author		Nemanja Nikodijevic 2014-06-28
*******************************************************************************/
#pragma once

#include "drv/nl_spi_dma.h"
#include "io/pins.h"

#define ESPI_CPOL_0 SSP_CPOL_HI
#define ESPI_CPOL_1 SSP_CPOL_LO
#define ESPI_CPHA_0 SSP_CPHA_FIRST
#define ESPI_CPHA_1 SSP_CPHA_SECOND

#define ESPI_PORT_OFF 7

extern LPC_SSPn_Type* ESPI_SSP;

inline static void ESPI_Init(uint32_t clkRateInHz)
{
  /** init SSP peripheral */
  SPI_DMA_Init(ESPI_SSP, SPI_MODE_MASTER, clkRateInHz);
}

inline static uint16_t ESPI_GetCRDIV(void)
{
  return SPI_DMA_GetCRDIV(ESPI_SSP);
}

inline static void ESPI_SetCRDIV(uint16_t const crdiv)
{
  SPI_DMA_SetCRDIV(ESPI_SSP, crdiv);
}

static inline void ESPI_SAP_Set(void)
{
  pinEspi_SAPb = 1;
}

static inline void ESPI_SAP_Clr(void)
{
  pinEspi_SAPb = 0;
}

void     ESPI_SCS_Select(uint32_t port, uint32_t device);
uint32_t ESPI_Transfer(uint8_t* tx, uint8_t* rx, uint32_t len, TransferCallback cb);
uint32_t ESPI_TransferNonBlocking(uint8_t* tx, uint8_t* rx, uint32_t len, TransferCallback cb);
