/******************************************************************************/
/** @file		nl_espi_core.c
    @date		2014-06-28
    @brief    	eSPI core for LPC
    @example
    @ingroup  	LPC_ESPI
    @author		Nemanja Nikodijevic 2014-06-28
*******************************************************************************/

#include "io/pins.h"
#include "espi/nl_espi_core.h"

LPC_SSPn_Type* ESPI_SSP = LPC_SSP0;

/*********************************************************************************************************************/

void ESPI_SCS_Select(uint32_t port, uint32_t device)
{
  static volatile uint32_t* const scs[6] = {
    pPinEspi_SCS0w,
    pPinEspi_SCS1w,
    pPinEspi_SCS2w,
    pPinEspi_SCS3w,
    pPinEspi_SCS4w,
    pPinEspi_SCS5w,
  };
  uint8_t s;
  uint8_t i;

  //INVERT
  pinEspi_DMXb = 0;

  if ((device == 1) || (device == 3))
    s = 0;
  else if (device == 2)
    s = 3;
  else if (device == 0)
  {
    s      = 0;
    device = 3;
    port   = 7;
  }
  else
  {  //INVERT
    pinEspi_DMXb = 1;
    return;
  }

  do
  {
    for (i = 0; i < 3; i++)
      *(scs[s + i]) = port & (1 << i);
    s += 3;
  } while ((s <= 3) && (device == 3));

  //INVERT
  pinEspi_DMXb = 1;
}

uint32_t ESPI_Transfer(uint8_t* tx, uint8_t* rx, uint32_t len, TransferCallback cb)
{
  return SPI_DMA_SendReceiveBlocking(ESPI_SSP, tx, rx, len, cb);
}

uint32_t ESPI_TransferNonBlocking(uint8_t* tx, uint8_t* rx, uint32_t len, TransferCallback cb)
{
  return SPI_DMA_SendReceive(ESPI_SSP, tx, rx, len, cb);
}
