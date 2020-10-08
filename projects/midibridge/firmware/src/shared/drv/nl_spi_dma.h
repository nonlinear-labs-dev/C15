/******************************************************************************/
/** @file		nl_spi_dma.h
    @date		2015-02-12 dtz
    @brief    	SPI with DMA capability driver for LPC18xx
    @example
    @ingroup  	SPI_DMA
    @author		Nemanja Nikodijevic 2013-04-05
*******************************************************************************/
#pragma once

#include "cmsis/LPC43xx.h"
#include "cmsis/lpc43xx_ssp.h"
#include "cmsis/lpc_types.h"
#include "drv/nl_gpdma.h"
#include "drv/nl_cgu.h"

#define SPI_MODE_MASTER ((uint32_t)(0))

/**********************************************************************
 * @brief		Initializes the SPI-DMA driver for the desired SSP
 * @param[in]	SSPx	Pointer to selected SSP peripheral, should be:
 * 					- LPC_SSP0	:SSP0 peripheral
 * 					- LPC_SSP1	:SSP1 peripheral
 * @param[in]	mode	SPI Mode, should be:
 * 					- SSP_MASTER_MODE	:SPI master
 * 					- SSP_SLAVE_MODE	:SPI slave
 * @param[in]	clk_rate	Clock Rate in Hz
 **********************************************************************/
inline static void SPI_DMA_Init(LPC_SSPn_Type *const SSPx, uint32_t const mode, uint32_t const clk_rate)
{
  uint32_t prescale, cr0_div;

  if ((SSPx != LPC_SSP0) && (SSPx != LPC_SSP1))
    return;

  SSPx->CR0 = SSP_CPHA_SECOND | SSP_CPOL_HI | SSP_FRAME_SPI | SSP_DATABIT_8;
  SSPx->CR1 = mode;

  // Set clock rate for SSP peripheral
  // Find pre-scaler and divider values without using divisions at runtime (M0 core has no DIV instruction)
  for (prescale = 2; prescale < 256; prescale += 2)
    if (NL_LPC_CLK / clk_rate < 256 * prescale)
    {  // found a pre-scaler that results in a divider less < 256
      for (cr0_div = 0; cr0_div < 256; cr0_div++)
        if (prescale * (cr0_div + 1) >= NL_LPC_CLK / clk_rate)
          goto found;  // found a divider that results in the first clock-rate >= required
    }
  // code should never come to here, make sure you call with realistic target clock rates to avoid fails

found:
  /* Write computed prescaler and divider back to register */
  SSPx->CR0 &= (~SSP_CR0_SCR(0xFF)) & SSP_CR0_BITMASK;
  SSPx->CR0 |= (SSP_CR0_SCR(cr0_div)) & SSP_CR0_BITMASK;
  SSPx->CPSR = prescale & SSP_CPSR_BITMASK;

  SSPx->DMACR |= (SSP_DMA_TX | SSP_DMA_RX);
  SSPx->CR1 |= SSP_CR1_SSP_EN;
}

inline uint16_t SPI_DMA_GetCRDIV(LPC_SSPn_Type *const SSPx)
{
  return (SSPx->CR0 >> 8) & 0xFF;
}

inline void SPI_DMA_SetCRDIV(LPC_SSPn_Type *const SSPx, uint16_t const crdiv)
{
  SSPx->CR0 &= (~SSP_CR0_SCR(0xFF)) & SSP_CR0_BITMASK;
  SSPx->CR0 |= (SSP_CR0_SCR(crdiv)) & SSP_CR0_BITMASK;
}

// -----------------

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
