/******************************************************************************/
/** @file		nl_uart_dma.c
    @date		2014-09-08
    @brief    	UART with DMA
    @author		Nemanja Nikodijevic [2014-09-08]
*******************************************************************************/
#include "drv/nl_uart_dma.h"
#include "drv/nl_gpdma.h"
#include "drv/nl_cgu.h"
#include "cmsis/lpc43xx_uart.h"
#include "cmsis/lpc43xx_gpdma.h"

/**********************************************************************
 * @brief		Initializes the UART-DMA driver for the desired UART
 * @param[in]	UARTx	Pointer to selected UART peripheral, should be:
 * 					- LPC_USART0	:USART0 peripheral
 * 					- LPC_USART2	:USART2 peripheral
 * 					- LPC_USART3	:USART3 peripheral
 * @param[in]	baudrate	Baud rate
 **********************************************************************/
void UART_DMA_Init(LPC_USARTn_Type *UARTx, uint32_t baudrate)
{
  uint32_t tmp;
  uint32_t uClk = NL_LPC_CLK;
  uint32_t d, m, bestd, bestm;
  uint64_t best_divisor, divisor;
  uint32_t current_error, best_error;
  uint32_t recalcbaud;
  uint32_t dmamux;

  if ((LPC_UART1_Type *) UARTx == LPC_UART1)
    return;

  /* FIFOs are empty */
  UARTx->FCR = (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS | UART_FCR_DMAMODE_SEL);
  // Disable FIFO
  UARTx->FCR = 0;

  // Dummy reading
  while (UARTx->LSR & UART_LSR_RDR)
  {
    tmp = UARTx->RBR;
  }

  UARTx->TER = UART0_2_3_TER_TXEN;
  // Wait for current transmit complete
  while (!(UARTx->LSR & UART_LSR_THRE))
    ;
  // Disable Tx
  UARTx->TER = 0;

  // Dummy reading
  tmp = UARTx->LSR;

  /** calculate baud rate divisors */
  best_error   = 0xFFFFFFFF; /* Worst case */
  bestd        = 0;
  bestm        = 0;
  best_divisor = 0;
  for (m = 1; m <= 15; m++)
  {
    for (d = 0; d < m; d++)
    {
      divisor       = ((uint64_t) uClk << 28) * m / (baudrate * (m + d));
      current_error = divisor & 0xFFFFFFFF;

      tmp = divisor >> 32;

      /* Adjust error */
      if (current_error > ((uint32_t) 1 << 31))
      {
        current_error = -current_error;
        tmp++;
      }

      if (tmp < 1 || tmp > 65536) /* Out of range */
        continue;

      if (current_error < best_error)
      {
        best_error   = current_error;
        best_divisor = tmp;
        bestd        = d;
        bestm        = m;
        if (best_error == 0)
          break;
      }
    } /* end of inner for loop */

    if (best_error == 0)
      break;
  } /* end of outer for loop  */

  if (best_divisor == 0)
    return; /* can not find best match */

  recalcbaud = (uClk >> 4) * bestm / (best_divisor * (bestm + bestd));

  /* reuse best_error to evaluate baud error*/
  if (baudrate > recalcbaud)
    best_error = baudrate - recalcbaud;
  else
    best_error = recalcbaud - baudrate;

  best_error = best_error * 100 / baudrate;

  if (best_error < UART_ACCEPTED_BAUDRATE_ERROR)
  {
    UARTx->LCR |= UART_LCR_DLAB_EN;
    UARTx->DLM = UART_LOAD_DLM(best_divisor);
    UARTx->DLL = UART_LOAD_DLL(best_divisor);
    /* Then reset DLAB bit */
    UARTx->LCR &= (~UART_LCR_DLAB_EN) & UART_LCR_BITMASK;
    UARTx->FDR = (UART_FDR_MULVAL(bestm)
                  | UART_FDR_DIVADDVAL(bestd))
        & UART_FDR_BITMASK;
  }

  tmp        = ((UARTx->LCR & (UART_LCR_DLAB_EN | UART_LCR_BREAK_EN)) | UART_LCR_WLEN8) & UART_LCR_BITMASK;
  UARTx->LCR = tmp;

  UARTx->TER |= UART0_2_3_TER_TXEN;
  UARTx->FCR = (UART_FCR_FIFO_EN | UART_FCR_DMAMODE_SEL);

  /** Setup DMAMUX */
  dmamux = LPC_CREG->DMAMUX;
  if (UARTx == LPC_USART0)
    dmamux |= 0x5 << 2;
  else if (UARTx == LPC_USART2)
    dmamux |= 0x5 << 10;
  else if (UARTx == LPC_USART3)
    dmamux |= 0x5 << 14;
  LPC_CREG->DMAMUX = dmamux;
}

/**********************************************************************
 * @brief		Sends the desired buffer through UART using DMA
 * @param[in]	UARTx	Pointer to selected UART peripheral, should be:
 * 					- LPC_USART0	:USART0 peripheral
 * 					- LPC_USART2	:USART2 peripheral
 * 					- LPC_USART3	:USART3 peripheral
 * @param[in]	buff	Pointer to the buffer
 * @param[in]	len		Length of the buffer
 * @param[in]	Callback	Address of the callback function
 * @return		len - Success; 0 - Failure
 **********************************************************************/
uint32_t UART_DMA_Send(LPC_USARTn_Type *UARTx, uint8_t *buff, uint32_t len,
                       TransferCallback Callback)
{
  NL_GPDMA_ChDesc desc;

  if (UARTx == LPC_USART0)
  {
    desc.channel   = GPDMA_UART_0_TX_CHANNEL;
    desc.dstperiph = 1;
  }
  else if (UARTx == LPC_USART2)
  {
    desc.channel   = GPDMA_UART_2_TX_CHANNEL;
    desc.dstperiph = 5;
  }
  else if (UARTx == LPC_USART3)
  {
    desc.channel   = GPDMA_UART_3_TX_CHANNEL;
    desc.dstperiph = 7;
  }
  else
    return 0;

  desc.length    = len;
  desc.srcperiph = 0;
  desc.srcaddr   = (uint32_t) buff;
  desc.dstaddr   = (uint32_t) &UARTx->THR;
  desc.ccontrol  = 0x86000000;

  if (NL_GPDMA_SetupChannel(&desc, Callback) == ERROR)
    return 0;

  return len;
}

/**********************************************************************
 * @brief		Triggers receiving of the buffer through UART using DMA
 * @param[in]	UARTx	Pointer to selected UART peripheral, should be:
 * 					- LPC_USART0	:USART0 peripheral
 * 					- LPC_USART2	:USART2 peripheral
 * 					- LPC_USART3	:USART3 peripheral
 * @param[in]	buff	Pointer to the buffer
 * @param[in]	len		Length of the buffer
 * @param[in]	Callback	Address of the callback function
 * @return		len - Success; 0 - Failure
 **********************************************************************/
uint32_t UART_DMA_Receive(LPC_USARTn_Type *UARTx, uint8_t *buff, uint32_t len,
                          TransferCallback Callback)
{
  NL_GPDMA_ChDesc desc;

  if (UARTx == LPC_USART0)
  {
    desc.channel   = GPDMA_UART_0_RX_CHANNEL;
    desc.srcperiph = 2;
  }
  else if (UARTx == LPC_USART2)
  {
    desc.channel   = GPDMA_UART_2_RX_CHANNEL;
    desc.srcperiph = 6;
  }
  else if (UARTx == LPC_USART3)
  {
    desc.channel   = GPDMA_UART_3_RX_CHANNEL;
    desc.srcperiph = 8;
  }
  else
    return 0;

  desc.length    = len;
  desc.dstperiph = 0;
  desc.dstaddr   = (uint32_t) buff;
  desc.srcaddr   = (uint32_t) &UARTx->RBR;
  desc.ccontrol  = 0x89000000;

  if (NL_GPDMA_SetupChannel(&desc, Callback) == ERROR)
    return 0;

  return len;
}
