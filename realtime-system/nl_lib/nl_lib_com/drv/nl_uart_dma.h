/******************************************************************************/
/** @file		nl_uart_dma.h
    @date		2014-09-08
    @brief    	UART with DMA
    @author		Nemanja Nikodijevic [2014-09-08]
*******************************************************************************/
#ifndef NL_UART_DMA_H_
#define NL_UART_DMA_H_

#include "cmsis/LPC43xx.h"
#include "drv/nl_gpdma.h"

void     UART_DMA_Init(LPC_USARTn_Type *UARTx, uint32_t baudrate);
uint32_t UART_DMA_Send(LPC_USARTn_Type *UARTx, uint8_t *buff, uint32_t len,
                       TransferCallback Callback);
uint32_t UART_DMA_Receive(LPC_USARTn_Type *UARTx, uint8_t *buff, uint32_t len,
                          TransferCallback Callback);

#endif
