/******************************************************************************/
/** @file		nl_spi_dma.h
    @date		2015-02-12 dtz
    @brief    	SPI with DMA capability driver for LPC18xx
    @example
    @ingroup  	SPI_DMA
    @author		Nemanja Nikodijevic 2013-04-05
*******************************************************************************/

#ifndef NL_SPI_DMA_H_
#define NL_SPI_DMA_H_

#include "cmsis/LPC43xx.h"
#include "cmsis/lpc43xx_ssp.h"
#include "cmsis/lpc_types.h"
#include "drv/nl_gpdma.h"

#define SPI_MODE_MASTER ((uint32_t)(0))

void SPI_DMA_Init(LPC_SSPn_Type *SSPx, uint32_t mode, uint32_t clk_rate);

void SPI_DMA_SwitchMode(LPC_SSPn_Type *SSPx, uint32_t pol_pha);
void SPI_DMA_SwitchModeNonBlocking(LPC_SSPn_Type *SSPx, uint32_t pol_pha);

uint32_t SPI_DMA_Send(LPC_SSPn_Type *  SSPx,
                      uint8_t *        buff,
                      uint32_t         len,
                      TransferCallback Callback);

uint32_t SPI_DMA_Receive(LPC_SSPn_Type *  SSPx,
                         uint8_t *        buff,
                         uint32_t         len,
                         TransferCallback Callback);

uint32_t SPI_DMA_SendReceive(LPC_SSPn_Type *  SSPx,
                             uint8_t *        tx_buff,
                             uint8_t *        rx_buff,
                             uint32_t         len,
                             TransferCallback Callback);

uint32_t SPI_DMA_SendReceiveBlocking(LPC_SSPn_Type *  SSPx,
                                     uint8_t *        tx_buff,
                                     uint8_t *        rx_buff,
                                     uint32_t         len,
                                     TransferCallback Callback);

uint32_t SPI_DMA_ChannelBusy(uint32_t chan);

#endif /* NL_SPI_DMA_H_ */
