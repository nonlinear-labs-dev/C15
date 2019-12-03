/******************************************************************************/
/** @file		nl_i2s_dma.c
    @date		2015-07-10
    @brief    	I2S DMA driver for LPC43xx
    @author		Nemanja Nikodijevic 2015-07-10
*******************************************************************************/
#include "cmsis/lpc_types.h"
#include "drv/nl_i2s_dma.h"
#include "drv/nl_cgu.h"

//#define I2S_USE_BASE_AUDIO_CLK

/*********************************************************************
 * @brief		Calculate dividers and bitrate for I2S registers
 * @param[in]	bits	transfer word width
 * @param[in]	freq	sampling frequency in Hz
 * @param[in]	n		address to return N
 * @param[in]	xdiv	address to return X divider
 * @param[in]	ydiv	address to return Y divider
 * @return 		SUCCESS - success; ERROR - failure
 *********************************************************************/
static Status I2S_DMA_CalculateFreq(uint8_t bits, uint32_t freq, int32_t* n, uint16_t* xdiv, uint16_t* ydiv)
{
  uint64_t divider;
  uint8_t  bitrate;
  uint32_t x, y;
  uint16_t dif;
  uint16_t error;
  uint16_t x_divide, y_divide;
  uint16_t ErrorOptimal = 0xFFFF;
  int32_t  N;

  bitrate = 2 * bits - 1;
  /* divider is a fixed point number with 16 fractional bits */
  divider = ((uint64_t)(freq * (bitrate + 1) * 2) << 16) / NL_LPC_CLK;

  /* find N that make x/y <= 1 -> divider <= 2^16 */
  for (N = 64; N > 0; N--)
  {
    if ((divider * N) < (1 << 16))
      break;
  }

  if (N == 0)
    return ERROR;

  divider *= N;

  for (y = 255; y > 0; y--)
  {
    x = y * divider;
    if (x & (0xFF000000))
      continue;
    dif = x & 0xFFFF;
    if (dif > 0x8000)
      error = 0x10000 - dif;
    else
      error = dif;
    if (error == 0)
    {
      y_divide = y;
      break;
    }
    else if (error < ErrorOptimal)
    {
      ErrorOptimal = error;
      y_divide     = y;
    }
  }
  x_divide = ((uint64_t) y_divide * freq * (bitrate + 1) * N * 2) / NL_LPC_CLK;
  if (x_divide >= 256)
    x_divide = 0xFF;
  if (x_divide == 0)
    x_divide = 1;

  *n    = N;
  *xdiv = x_divide;
  *ydiv = y_divide;

  return SUCCESS;
}

/********************************************************************/ /**
 * @brief		Initialize I2S DMA
 * @param[in]	I2Sx I2S peripheral selected, should be: LPC_I2S0/1
 * @param[in]	bits		transfer word width (8, 16, 32)
 * @param[in]	freq		sampling frequency in Hz
 * @param[in]	mode		transfer mode, related to board wiring
 * @return 		none
 *********************************************************************/
void I2S_DMA_Init(LPC_I2Sn_Type* I2Sx, uint8_t bits, uint32_t freq, uint8_t mode)
{
  uint32_t wordwidth;
  int32_t  n;
  uint16_t xdiv, ydiv;

  if (bits == 32 || bits == 24)
  {
    wordwidth = I2S_DA_WORDWIDTH_32;
    bits      = 32;
  }
  else if (bits == 16)
    wordwidth = I2S_DA_WORDWIDTH_16;
  else if (bits == 8)
    wordwidth = I2S_DA_WORDWIDTH_8;
  else
    return;

  if (I2Sx == LPC_I2S0)
  {
    /** in conflict with the default SSP0 DMA usage */
    LPC_CREG->DMAMUX &= ~(0xF << 18);
    /** set DMA peripheral 9 an 10 to I2S0 TX and RX */
    LPC_CREG->DMAMUX |= (0x5 << 18);
  }

  I2Sx->DAO = wordwidth | I2S_DA_RESET | I2S_DA_WS_HALFPERIOD(31);
  I2Sx->DAI = wordwidth | I2S_DA_RESET | I2S_DA_WS_HALFPERIOD(31);  //I2S_DA_WS_SEL

  /** Set MODE registers */
#ifndef I2S_USE_BASE_AUDIO_CLK
  I2Sx->TXMODE = I2S_TXMODE_MCENA;
#else
  I2Sx->TXMODE = I2S_TXMODE_MCENA | I2S_TXMODE_CLKSEL_BASE_AUDIO;
#endif
  if (mode == I2S_MODE_4WIRE)
    I2Sx->RXMODE = I2S_RXMODE_4PIN;
  else
    I2Sx->RXMODE = I2S_RXMODE_MCENA;

    /** Setup frequency for TX only, because SCK is shared (4 pin mode) */
#ifndef I2S_USE_BASE_AUDIO_CLK
  I2S_DMA_CalculateFreq(bits, freq, &n, &xdiv, &ydiv);

  I2Sx->TXBITRATE = n - 1;
  I2Sx->TXRATE    = ydiv | (xdiv << 8);

  if (mode == I2S_MODE_IO_MASTER)
  {
    I2Sx->RXBITRATE = n - 1;
    I2Sx->RXRATE    = ydiv | (xdiv << 8);
  }
#else
  LPC_CREG->CREG6 |= ((1 << 12) | (1 << 14));
  I2Sx->TXBITRATE = ((NL_LPC_AUDIO_CLK / freq) / (2 * bits)) - 1;

#endif

  /** Setup DMA registers */
  I2Sx->DMA1 = I2S_DMA_TX_ENABLE | I2S_DMA_TX_DEPTH(4);
  I2Sx->DMA2 = I2S_DMA_RX_ENABLE | I2S_DMA_RX_DEPTH(4);

  /** Start the I2S */
  I2Sx->DAO &= ~I2S_DA_RESET;
  I2Sx->DAI &= ~I2S_DA_RESET;
}

/**********************************************************************
 * @brief		Sends the desired buffer via I2S using DMA
 * @param[in]	I2SPx	Pointer to selected I2S peripheral, should be:
 * 					- LPC_I2S0	:I2S0 peripheral
 * 					- LPC_I2S1	:I2S1 peripheral
 * @param[in]	buff	Pointer to the buffer
 * @param[in]	len		Length of the buffer
 * @param[in]	Callback	Address of the callback function
 * @return		len - Success; 0 - Failure
 **********************************************************************/
uint32_t I2S_DMA_Send(LPC_I2Sn_Type* I2Sx, uint32_t* buff, uint32_t len, TransferCallback Callback)
{
  NL_GPDMA_ChDesc desc;

  if (I2Sx == LPC_I2S0)
  {
    desc.channel   = GPDMA_I2S_TX_CHANNEL;
    desc.dstperiph = 9;
  }
  else if (I2Sx == LPC_I2S1)
  {
    desc.channel   = GPDMA_I2S_TX_CHANNEL;
    desc.dstperiph = 3;
  }
  else
    return 0;

  desc.length    = len;
  desc.srcperiph = 0;
  desc.srcaddr   = (uint32_t) buff;
  desc.dstaddr   = (uint32_t) &I2Sx->TXFIFO;
  desc.ccontrol  = GPDMA_CCONTROL_I | GPDMA_CCONTROL_SI | GPDMA_CCONTROL_D
      | GPDMA_CCONTROL_DWIDTH_WORD | GPDMA_CCONTROL_SWIDTH_WORD
      | GPDMA_CCONTROL_DBSIZE_32 | GPDMA_CCONTROL_SBSIZE_32;  //0x8648000;

  if (NL_GPDMA_SetupChannel(&desc, Callback) == ERROR)
    return 0;

  return len;
}

/**********************************************************************
 * @brief		Triggers receiving of the buffer via I2S using DMA
 * @param[in]	I2SPx	Pointer to selected I2S peripheral, should be:
 * 					- LPC_I2S0	:I2S0 peripheral
 * 					- LPC_I2S1	:I2S1 peripheral
 * @param[in]	buff	Pointer to the buffer
 * @param[in]	len		Length of the buffer
 * @param[in]	Callback	Address of the callback function
 * @return		len - Success; 0 - Failure
 **********************************************************************/
uint32_t I2S_DMA_Receive(LPC_I2Sn_Type* I2Sx, uint32_t* buff, uint32_t len, TransferCallback Callback)
{
  NL_GPDMA_ChDesc desc;

  if (I2Sx == LPC_I2S0)
  {
    desc.channel   = GPDMA_I2S_RX_CHANNEL;
    desc.srcperiph = 10;
  }
  else if (I2Sx == LPC_I2S1)
  {
    desc.channel   = GPDMA_I2S_RX_CHANNEL;
    desc.srcperiph = 4;
  }
  else
    return 0;

  desc.length    = len;
  desc.dstperiph = 0;
  desc.dstaddr   = (uint32_t) buff;
  desc.srcaddr   = (uint32_t) &I2Sx->RXFIFO;
  desc.ccontrol  = GPDMA_CCONTROL_I | GPDMA_CCONTROL_DI | GPDMA_CCONTROL_S
      | GPDMA_CCONTROL_DWIDTH_WORD | GPDMA_CCONTROL_SWIDTH_WORD
      | GPDMA_CCONTROL_DBSIZE_32 | GPDMA_CCONTROL_SBSIZE_32;  //0x8948000;

  if (NL_GPDMA_SetupChannel(&desc, Callback) == ERROR)
    return 0;

  return len;
}

/**********************************************************************
 * @brief		Checks whether the TX DMA channel is busy
 * @return		1 - busy; 0 - not busy
 **********************************************************************/
uint32_t I2S_DMA_TxBusy(void)
{
  return NL_GPDMA_ChannelBusy(GPDMA_I2S_TX_CHANNEL);
}
