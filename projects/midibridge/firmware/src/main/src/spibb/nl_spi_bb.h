/******************************************************************************/
/** @file		nl_spi_bb.h
    @date		2014-01-31
    @brief    	SPI to BeagleBone communication layer for LPC18xx
    @example
    @ingroup  	SPI_BB
    @author		Nemanja Nikodijevic 2014-01-31
*******************************************************************************/
#pragma once

#include "drv/nl_spi_dma.h"

#define MSG_PRESET_A     0x0100
#define MSG_PRESET_B     0x0200
#define MSG_PARAM        0x0400
#define MSG_MORPH_POS    0x0800
#define MSG_NOTIFICATION 0x8000

#define SPI_BB_DMA_SEND_CHAN 2
#define SPI_BB_BUFFER_SIZE   1024
#define SPI_BB_BUFFER_NUM    2

#define SPI_BB_BUFFER_FREE 0
#define SPI_BB_BUFFER_BUSY 1

typedef void (*MessageCallback)(uint16_t type, uint16_t length, uint16_t* data);

void     SPI_BB_Init(MessageCallback msg_cb);
void     SPI_BB_Reset(void);
void     SPI_BB_ToggleHeartbeat(void);
void     SPI_BB_Polling(void);
uint32_t SPI_BB_Send(uint8_t* buff, uint32_t len);  /// ssc: we need an implementation on message level
