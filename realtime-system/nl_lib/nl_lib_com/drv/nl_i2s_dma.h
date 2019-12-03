/******************************************************************************/
/** @file		nl_i2s_dma.h
    @date		2015-07-10
    @brief    	I2S DMA driver for LPC43xx
    @author		Nemanja Nikodijevic 2015-07-10
*******************************************************************************/
#ifndef _NL_I2S_DMA_H_
#define _NL_I2S_DMA_H_

#include <stdint.h>
#include "cmsis/LPC43xx.h"
#include "drv/nl_gpdma.h"

/*******************************************************************/ /**
 * DAI/DAO registers description
 *********************************************************************/
/** WORDWIDTH bits */
#define I2S_DA_WORDWIDTH_8  ((0 << 0))
#define I2S_DA_WORDWIDTH_16 ((1 << 0))
#define I2S_DA_WORDWIDTH_32 ((3 << 0))
/** MONO bit */
#define I2S_DA_MONO ((1 << 2))
/** STOP bit */
#define I2S_DA_STOP ((1 << 3))
/** RESET bit */
#define I2S_DA_RESET ((1 << 4))
/** WS_SEL bit */
#define I2S_DA_WS_SEL ((1 << 5))
/** WS_HALFPERIOD bits */
#define I2S_DA_WS_HALFPERIOD(x) (((x & 0x1F) << 6))
/** MUTE bit (DAO only) */
#define I2S_DAO_MUTE ((1 << 15))

/*******************************************************************/ /**
 * TXMODE register description
 *********************************************************************/
/** TXCLKSEL bits */
#define I2S_TXMODE_CLKSEL_DIVIDER    ((0 << 0))
#define I2S_TXMODE_CLKSEL_BASE_AUDIO ((1 << 0))
#define I2S_TXMODE_CLKSEL_RX_MCLK    ((2 << 0))
/** TX4PIN bit */
#define I2S_TXMODE_4PIN ((1 << 2))
/** Enable clock to TXMCLK output bit */
#define I2S_TXMODE_MCENA ((1 << 3))

/*******************************************************************/ /**
 * RXMODE register description
 *********************************************************************/
/** RXCLKSEL bits */
#define I2S_RXMODE_CLKSEL_DIVIDER    ((0 << 0))
#define I2S_RXMODE_CLKSEL_BASE_OR_RX ((1 << 0))
#define I2S_RXMODE_CLKSEL_TX_MCLK    ((2 << 0))
/** RX4PIN bit */
#define I2S_RXMODE_4PIN ((1 << 2))
/** Enable clock to RXMCLK output bit */
#define I2S_RXMODE_MCENA ((1 << 3))

/*******************************************************************/ /**
 * DMA registers description
 *********************************************************************/
/** RX DMA enable bit */
#define I2S_DMA_RX_ENABLE ((1 << 0))
/** TX DMA enable bit */
#define I2S_DMA_TX_ENABLE ((1 << 1))
/** RX_DEPTH field */
#define I2S_DMA_RX_DEPTH(x) (((x & 0xF) << 8))
/** TX_DEPTH field */
#define I2S_DMA_TX_DEPTH(x) (((x & 0xF) << 16))

/*******************************************************************/ /**
 * Mode init field description
 *********************************************************************/
/** 4-wire mode */
#define I2S_MODE_4WIRE 0
/** Both input and output master mode */
#define I2S_MODE_IO_MASTER 1

void I2S_DMA_Init(LPC_I2Sn_Type *I2Sx, uint8_t bits, uint32_t freq, uint8_t mode);

uint32_t I2S_DMA_Send(LPC_I2Sn_Type *I2Sx, uint32_t *buff, uint32_t len, TransferCallback Callback);
uint32_t I2S_DMA_Receive(LPC_I2Sn_Type *I2Sx, uint32_t *buff, uint32_t len, TransferCallback Callback);

uint32_t I2S_DMA_TxBusy(void);
#endif
