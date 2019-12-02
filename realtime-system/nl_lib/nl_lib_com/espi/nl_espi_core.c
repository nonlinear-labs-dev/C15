/******************************************************************************/
/** @file		nl_espi_core.c
    @date		2014-06-28
    @brief    	eSPI core for LPC
    @example
    @ingroup  	LPC_ESPI
    @author		Nemanja Nikodijevic 2014-06-28
*******************************************************************************/

#include "drv/nl_gpio.h"
#include "espi/nl_espi_core.h"

static LPC_SSPn_Type* ESPI_SSP = NULL;

static ESPI_PINS_T* pins = NULL;

void ESPI_Config(LPC_SSPn_Type* SSPx, ESPI_PINS_T* espi_pins)
{
  ESPI_SSP = SSPx;
  pins     = espi_pins;
}
/*********************************************************************************************************************/
void ESPI_Init(uint32_t clkRateInHz)
{
  /** init SSP peripheral */
  SPI_DMA_Init(ESPI_SSP, SPI_MODE_MASTER, clkRateInHz);
}

void ESPI_SAP_Set(void)
{
  NL_GPIO_Set(pins->sap);
}

void ESPI_SAP_Clr(void)
{
  NL_GPIO_Clr(pins->sap);
}

void ESPI_SCS_Select(uint32_t port, uint32_t device)
{
  uint8_t s;
  uint8_t i;

  //INVERT
  NL_GPIO_Clr(pins->dmx);

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
  {
    //INVERT
    NL_GPIO_Set(pins->dmx);
    ;
    return;
  }

  do
  {
    for (i = 0; i < 3; i++)
    {
      if (port & (1 << i))
        NL_GPIO_Set(pins->scs[s + i]);
      else
        NL_GPIO_Clr(pins->scs[s + i]);
    }
    s += 3;
  } while ((s <= 3) && (device == 3));

  //INVERT
  NL_GPIO_Set(pins->dmx);
}

uint32_t ESPI_Transfer(uint8_t* tx, uint8_t* rx, uint32_t len, TransferCallback cb)
{
  //return SPI_DMA_SendReceive(ESPI_SSP, tx, rx, len, cb);
  return SPI_DMA_SendReceiveBlocking(ESPI_SSP, tx, rx, len, cb);
}

uint32_t ESPI_TransferNonBlocking(uint8_t* tx, uint8_t* rx, uint32_t len, TransferCallback cb)
{
  return SPI_DMA_SendReceive(ESPI_SSP, tx, rx, len, cb);
}
