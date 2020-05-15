/******************************************************************************/
/** @file		nl_midi.c
    @date		2015-09-28
    @brief    	MIDI framework
    @author		Nemanja Nikodijevic 2015-09-28
*******************************************************************************/
#include "midi/nl_midi.h"
#include "cmsis/lpc43xx_uart.h"

static LPC_USARTn_Type *MIDI_UART = NULL;

static MIDI_Message_Callback MIDI_Msg_Callback = NULL;

static float MIDI_Note_Freq[] = { 8.1758, 8.66196, 9.17702, 9.72272, 10.3009, 10.9134, 11.5623, 12.2499, 12.97827, 13.75, 14.5676, 15.4338 };

static void MIDI_UART_Callback(uint8_t MIDI_UART_Buff)
{
  static MIDI_Message MIDI_Cur_Message;
  static uint8_t      curDataByte = 0;

  if (MIDI_UART_Buff & 0x80)
  {
    MIDI_Cur_Message.m_status = MIDI_UART_Buff;
    curDataByte               = 0;
  }
  else if (MIDI_Cur_Message.m_status)
  {
    if (curDataByte)
    {
      /* Currently supporting only 3 byte messages via UART */
      MIDI_Cur_Message.m_data1 = MIDI_UART_Buff;
      if (MIDI_Msg_Callback)
      {
        MIDI_Msg_Callback(&MIDI_Cur_Message);
        curDataByte = 0;
      }
    }
    else
    {
      MIDI_Cur_Message.m_data0 = MIDI_UART_Buff;
      curDataByte++;
    }
  }
}

static void MIDI_UART_Handler(LPC_USARTn_Type *UARTx)
{
  if ((UARTx->IIR & UART_IIR_INTID_MASK) == UART_IIR_INTID_RDA)
  {
    MIDI_UART_Callback((uint8_t)(UARTx->RBR & 0xFF));
  }
}

void UART0_IRQHandler(void)
{
  MIDI_UART_Handler(LPC_USART0);
}

void UART2_IRQHandler(void)
{
  MIDI_UART_Handler(LPC_USART2);
}

void UART3_IRQHandler(void)
{
  MIDI_UART_Handler(LPC_USART3);
}

void MIDI_UART_Init(LPC_USARTn_Type *UARTx, MIDI_Message_Callback midi_cb)
{
  uint32_t tmp;

  MIDI_UART         = UARTx;
  MIDI_Msg_Callback = midi_cb;

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
  // baud rate generation 204 MHz PCLK -> 31250 for MIDI
  UARTx->LCR |= UART_LCR_DLAB_EN;
  UARTx->DLM = UART_LOAD_DLM(408);
  UARTx->DLL = UART_LOAD_DLL(408);
  /* Then reset DLAB bit */
  UARTx->LCR &= (~UART_LCR_DLAB_EN) & UART_LCR_BITMASK;
  UARTx->FDR = (UART_FDR_MULVAL(1)
                | UART_FDR_DIVADDVAL(0))
      & UART_FDR_BITMASK;

  tmp        = ((UARTx->LCR & (UART_LCR_DLAB_EN | UART_LCR_BREAK_EN)) | UART_LCR_WLEN8) & UART_LCR_BITMASK;
  UARTx->LCR = tmp;

  UARTx->TER |= UART0_2_3_TER_TXEN;
  UARTx->FCR = UART_FCR_FIFO_EN;

  UARTx->IER = UART_IER_RBRINT_EN;

  if (UARTx == LPC_USART0)
    NVIC_EnableIRQ(USART0_IRQn);
  else if (UARTx == LPC_USART2)
    NVIC_EnableIRQ(USART2_IRQn);
  else if (UARTx == LPC_USART3)
    NVIC_EnableIRQ(USART3_IRQn);
}

float MIDI_Get_Note_Freq(uint8_t note)
{
  float   base;
  uint8_t i;

  if (note > 127)
    return 0.0;

  base = MIDI_Note_Freq[note % 12];
  note /= 12;

  for (i = 0; i < note; i++)
    base *= 2;

  return base;
}
