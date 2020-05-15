/******************************************************************************/
/** @file		nl_spi_dma.c
    @date		2015-02-12 dtz
    @brief    	SPI with DMA capability driver for LPC18xx
    @example
    @ingroup  	SPI_DMA
    @author		Nemanja Nikodijevic 2013-04-05
*******************************************************************************/
#include "drv/nl_spi_dma.h"
#include "cmsis/lpc43xx_gpdma.h"
#include "cmsis/lpc43xx_ssp.h"
#include "cmsis/lpc43xx_cgu.h"
#include "drv/nl_cgu.h"

/**********************************************************************
 * @brief		Initializes the SPI-DMA driver for the desired SSP
 * @param[in]	SSPx	Pointer to selected SSP peripheral, should be:
 * 					- LPC_SSP0	:SSP0 peripheral
 * 					- LPC_SSP1	:SSP1 peripheral
 * @param[in]	pol_pha	SPI Mode, could be:
 * 					- SSP_CPOL_HI
 * 					- SSP_CPOL_LO
 * 					- SSP_CPHA_FIRST
 * 					- SSP_CPHA_SECOND
 **********************************************************************/
void SPI_DMA_SwitchMode(LPC_SSPn_Type *SSPx, uint32_t pol_pha)
{
  static uint32_t a = 0;
  SSPx->CR0 &= ~(3 << 6);  // clear CPOL and CPHA
  SSPx->CR0 |= pol_pha;

  SPI_DMA_SendReceiveBlocking(SSPx, (uint8_t *) &a, NULL, 1, NULL);
}

void SPI_DMA_SwitchModeNonBlocking(LPC_SSPn_Type *SSPx, uint32_t pol_pha)
{
  static uint32_t a = 0;
  SSPx->CR0 &= ~(3 << 6);  // clear CPOL and CPHA
  SSPx->CR0 |= pol_pha;

  SPI_DMA_SendReceive(SSPx, (uint8_t *) &a, NULL, 1, NULL);
}

/**********************************************************************
 * @brief		Sends the desired buffer through SPI using DMA
 * @param[in]	SSPx	Pointer to selected SSP peripheral, should be:
 * 					- LPC_SSP0	:SSP0 peripheral
 * 					- LPC_SSP1	:SSP1 peripheral
 * @param[in]	buff	Pointer to the buffer
 * @param[in]	len		Length of the buffer
 * @param[in]	Callback	Address of the callback function
 * @return		len - Success; 0 - Failure
 **********************************************************************/
uint32_t SPI_DMA_Send(LPC_SSPn_Type *SSPx, uint8_t *buff, uint32_t len,
                      TransferCallback Callback)
{
  NL_GPDMA_ChDesc desc;

  if (SSPx == LPC_SSP0)
  {
    desc.channel   = GPDMA_SPI_0_TX_CHANNEL;
    desc.dstperiph = 10;
  }
  else if (SSPx == LPC_SSP1)
  {
    desc.channel   = GPDMA_SPI_1_TX_CHANNEL;
    desc.dstperiph = 12;
  }
  else
    return 0;

  desc.length    = len;
  desc.srcperiph = 0;
  desc.srcaddr   = (uint32_t) buff;
  desc.dstaddr   = (uint32_t) &SSPx->DR;
  desc.ccontrol  = 0x86009000;

  if (NL_GPDMA_SetupChannel(&desc, Callback) == ERROR)
    return 0;

  return len;
}

/**********************************************************************
 * @brief		Triggers receiving of the buffer through SPI using DMA
 * @param[in]	SSPx	Pointer to selected SSP peripheral, should be:
 * 					- LPC_SSP0	:SSP0 peripheral
 * 					- LPC_SSP1	:SSP1 peripheral
 * @param[in]	buff	Pointer to the buffer
 * @param[in]	len		Length of the buffer
 * @param[in]	Callback	Address of the callback function
 * @return		len - Success; 0 - Failure
 **********************************************************************/
uint32_t SPI_DMA_Receive(LPC_SSPn_Type *SSPx, uint8_t *buff, uint32_t len,
                         TransferCallback Callback)
{
  NL_GPDMA_ChDesc desc;

  if (SSPx == LPC_SSP0)
  {
    desc.channel   = GPDMA_SPI_0_RX_CHANNEL;
    desc.srcperiph = 9;
  }
  else if (SSPx == LPC_SSP1)
  {
    desc.channel   = GPDMA_SPI_1_RX_CHANNEL;
    desc.srcperiph = 11;
  }
  else
    return 0;

  desc.length    = len;
  desc.dstperiph = 0;
  desc.dstaddr   = (uint32_t) buff;
  desc.srcaddr   = (uint32_t) &SSPx->DR;
  desc.ccontrol  = 0x89009000;

  if (NL_GPDMA_SetupChannel(&desc, Callback) == ERROR)
    return 0;

  return len;
}

/**********************************************************************
 * @brief		Triggers sending and receiving through SPI using DMA
 * @param[in]	SSPx	Pointer to selected SSP peripheral, should be:
 * 					- LPC_SSP0	:SSP0 peripheral
 * 					- LPC_SSP1	:SSP1 peripheral
 * @param[in]	tx_buff	Pointer to the send buffer
 * @param[in]	rx_buff	Pointer to the receive buffer
 * @param[in]	len		Length of the buffer
 * @param[in]	Callback	Address of the callback function
 * @return		len - Success; 0 - Failure
 **********************************************************************/
uint32_t SPI_DMA_SendReceive(LPC_SSPn_Type *SSPx, uint8_t *tx_buff, uint8_t *rx_buff, uint32_t len,
                             TransferCallback Callback)
{
  if (SPI_DMA_Send(SSPx, tx_buff, len, NULL) == 0)
    return 0;
  if (SPI_DMA_Receive(SSPx, rx_buff, len, Callback) == 0)
    return 0;

  return len;
}

/**********************************************************************
 * @brief		Performs sending and receiving through SPI using DMA
 * @param[in]	SSPx	Pointer to selected SSP peripheral, should be:
 * 					- LPC_SSP0	:SSP0 peripheral
 * 					- LPC_SSP1	:SSP1 peripheral
 * @param[in]	tx_buff	Pointer to the send buffer
 * @param[in]	rx_buff	Pointer to the receive buffer
 * @param[in]	len		Length of the buffer
 * @param[in]	Callback	Address of the callback function
 * @return		len - Success; 0 - Failure
 **********************************************************************/
uint32_t SPI_DMA_SendReceiveBlocking(LPC_SSPn_Type *SSPx, uint8_t *tx_buff, uint8_t *rx_buff, uint32_t len,
                                     TransferCallback Callback)
{
  uint8_t ch;

  if (SSPx == LPC_SSP0)
    ch = GPDMA_SPI_0_RX_CHANNEL;
  else if (SSPx == LPC_SSP1)
    ch = GPDMA_SPI_1_RX_CHANNEL;
  else
    return 0;

  if (SPI_DMA_Send(SSPx, tx_buff, len, NULL) == 0)
    return 0;

  if (rx_buff == NULL)
  {
    uint8_t buffer[len];  // allocate a temp buffer on the stack
    if (SPI_DMA_Receive(SSPx, buffer, len, Callback) == 0)
      return 0;
  }
  else
  {
    if (SPI_DMA_Receive(SSPx, rx_buff, len, Callback) == 0)
      return 0;
  }

  while (NL_GPDMA_ChannelBusy(ch))
    NL_GPDMA_Poll();

  return len;
}

/**********************************************************************
 * @brief		Checks whether the DMA channel is busy
 * @param[in]	chan	Desired DMA channel
 * @return		1 - busy; 0 - not busy
 **********************************************************************/
uint32_t SPI_DMA_ChannelBusy(uint32_t chan)
{
  return NL_GPDMA_ChannelBusy((uint8_t) chan);
}
