/******************************************************************************/
/** @file		nl_gpdma.h
    @date		2014-10-09
    @brief    	Basic GPDMA functions
    @author		Nemanja Nikodijevic [2014-10-09]
*******************************************************************************/
#pragma once

#include "cmsis/LPC43xx.h"
#include "cmsis/lpc_types.h"

#define GPDMA_SPI_0_TX_CHANNEL 0
#define GPDMA_SPI_0_RX_CHANNEL 1
#define GPDMA_SPI_1_TX_CHANNEL 2
#define GPDMA_SPI_1_RX_CHANNEL 3

#define GPDMA_UART_0_TX_CHANNEL 4
#define GPDMA_UART_0_RX_CHANNEL 5

#define GPDMA_UART_2_TX_CHANNEL 6
#define GPDMA_UART_2_RX_CHANNEL 7
#define GPDMA_UART_3_TX_CHANNEL 6
#define GPDMA_UART_3_RX_CHANNEL 7

#define GPDMA_I2S_TX_CHANNEL 6
#define GPDMA_I2S_RX_CHANNEL 7

#define GPDMA_CCONTROL_SBSIZE_1  ((0 << 12))
#define GPDMA_CCONTROL_SBSIZE_4  ((1 << 12))
#define GPDMA_CCONTROL_SBSIZE_8  ((2 << 12))
#define GPDMA_CCONTROL_SBSIZE_16 ((3 << 12))
#define GPDMA_CCONTROL_SBSIZE_32 ((4 << 12))

#define GPDMA_CCONTROL_DBSIZE_1  ((0 << 15))
#define GPDMA_CCONTROL_DBSIZE_4  ((1 << 15))
#define GPDMA_CCONTROL_DBSIZE_8  ((2 << 15))
#define GPDMA_CCONTROL_DBSIZE_16 ((3 << 15))
#define GPDMA_CCONTROL_DBSIZE_32 ((4 << 15))

#define GPDMA_CCONTROL_SWIDTH_BYTE  ((0 << 18))
#define GPDMA_CCONTROL_SWIDTH_HWORD ((1 << 18))
#define GPDMA_CCONTROL_SWIDTH_WORD  ((2 << 18))

#define GPDMA_CCONTROL_DWIDTH_BYTE  ((0 << 21))
#define GPDMA_CCONTROL_DWIDTH_HWORD ((1 << 21))
#define GPDMA_CCONTROL_DWIDTH_WORD  ((2 << 21))

#define GPDMA_CCONTROL_S  ((1 << 24))
#define GPDMA_CCONTROL_D  ((1 << 25))
#define GPDMA_CCONTROL_SI ((1 << 26))
#define GPDMA_CCONTROL_DI ((1 << 27))
#define GPDMA_CCONTROL_I  ((1 << 31))

typedef void (*TransferCallback)(uint32_t);

typedef struct
{
  uint8_t  channel;
  uint32_t srcaddr;
  uint32_t dstaddr;
  uint32_t length;
  uint32_t srcperiph;
  uint32_t dstperiph;
  uint32_t ccontrol;
} NL_GPDMA_ChDesc;

void     NL_GPDMA_Init(uint8_t ch);
void     NL_GPDMA_Poll(void);
Status   NL_GPDMA_SetupChannel(NL_GPDMA_ChDesc* desc, TransferCallback callback);
uint32_t NL_GPDMA_ChannelBusy(uint8_t ch);
