/******************************************************************************/
/** @file		nl_i2c.c
    @date		2015-06-24
    @brief    	I2C driver for LPC43xx
    @author		Nemanja Nikodijevic 2015-06-24
*******************************************************************************/
#include "drv/nl_i2c.h"
#include "drv/nl_cgu.h"
#include "cmsis/lpc_types.h"

#define SFSI2C0_CONFIGURE_STANDARD_FAST_MODE      (1 << 3 | 1 << 11)
#define SFSI2C0_CONFIGURE_FASTPLUS_HIGHSPEED_MODE (2 << 1 | 1 << 3 | 1 << 7 | 1 << 10 | 1 << 11)

/********************************************************************/ /**
 * @brief		Generate a start condition on I2C bus (in master mode only)
 * @param[in]	I2Cx I2C peripheral selected, should be:
 * 					- LPC_I2C0	:I2C0 peripheral
 * 					- LPC_I2C1	:I2C1 peripheral
 * @return 		value of I2C status register after generate a start condition
 *********************************************************************/
static uint32_t I2C_Start(LPC_I2Cn_Type *I2Cx)
{
  I2Cx->CONCLR = I2C_I2CONCLR_SIC;  // Clear interrupt
  I2Cx->CONSET = I2C_I2CONSET_STA;  // Send "Start"

  // Wait for complete
  while (!(I2Cx->CONSET & I2C_I2CONSET_SI))
    ;
  I2Cx->CONCLR = I2C_I2CONCLR_STAC;
  return (I2Cx->STAT & I2C_STAT_CODE_BITMASK);
}

/********************************************************************/ /**
 * @brief		Send a byte
 * @param[in]	I2Cx I2C peripheral selected, should be:
 * 					- LPC_I2C0	:I2C0 peripheral
 * 					- LPC_I2C1	:I2C1 peripheral
 * @param[in]	databyte	sent data
 * @return 		value of I2C status register after sending
 *********************************************************************/
static uint32_t I2C_SendByte(LPC_I2Cn_Type *I2Cx, uint8_t databyte)
{
  /* Make sure start bit is not active */
  if (I2Cx->CONSET & I2C_I2CONSET_STA)
  {
    I2Cx->CONCLR = I2C_I2CONCLR_STAC;
  }
  I2Cx->DAT    = databyte & 0xFF;
  I2Cx->CONCLR = I2C_I2CONCLR_SIC;

  while (!(I2Cx->CONSET & I2C_I2CONSET_SI))
    ;
  return (I2Cx->STAT & I2C_STAT_CODE_BITMASK);
}

/********************************************************************/ /**
 * @brief		Generate a stop condition on I2C bus (in master mode only)
 * @param[in]	I2Cx I2C peripheral selected, should be:
 * 					- LPC_I2C0	:I2C0 peripheral
 * 					- LPC_I2C1	:I2C1 peripheral
 * @return 		None
 *********************************************************************/
static void I2C_Stop(LPC_I2Cn_Type *I2Cx)
{

  /* Make sure start bit is not active */
  if (I2Cx->CONSET & I2C_I2CONSET_STA)
  {
    I2Cx->CONCLR = I2C_I2CONCLR_STAC;
  }
  I2Cx->CONSET = I2C_I2CONSET_STO;
  I2Cx->CONCLR = I2C_I2CONCLR_SIC;
}

/*********************************************************************
 * @brief		Initializes the I2Cx peripheral with specified parameter.
 * @param[in]	I2Cx	I2C peripheral selected, should be
 * 					- LPC_I2C0	:I2C0 peripheral
 * 					- LPC_I2C1	:I2C1 peripheral
 * @param[in]	clk Target clock rate value to initialized I2C
 * 				peripheral (Hz)
 * @return 		None
 *********************************************************************/
void I2C_Init(LPC_I2Cn_Type *I2Cx, uint32_t clk)
{
  uint32_t tem;

  if (I2Cx == LPC_I2C0)
  {
    if (clk >= 1000000)  // Fast mode plus: 1MHz, high speed 3.4MHz
      LPC_SCU->SFSI2C0 = SFSI2C0_CONFIGURE_FASTPLUS_HIGHSPEED_MODE;
    else  // standard 100KHz, fast 400KHz
      LPC_SCU->SFSI2C0 = SFSI2C0_CONFIGURE_STANDARD_FAST_MODE;
  }
  else if (I2Cx == LPC_I2C1)
  {
#define SFSP2_3_CONFIGURE_I2C1_SDA (0x00000001 | (1 << 6) | (1 << 7))
#define SFSP2_4_CONFIGURE_I2C1_SCL (0x00000001 | (1 << 6) | (1 << 7))
    LPC_SCU->SFSP2_3 = SFSP2_3_CONFIGURE_I2C1_SDA; /* SDA */
    LPC_SCU->SFSP2_4 = SFSP2_4_CONFIGURE_I2C1_SCL;
  }
  else
  {
    /* not an I2C */
    return;
  }

  /* Set clock rate */
  if (clk < 1000)  //make sure SCLH,SCLL not exceed its 16bit value
    return;

  tem        = NL_LPC_CLK / clk;
  I2Cx->SCLH = (uint32_t)(tem / 2);
  I2Cx->SCLL = (uint32_t)(tem - I2Cx->SCLH);
  /* Set I2C operation to default */
  I2Cx->CONCLR = (I2C_I2CONCLR_AAC | I2C_I2CONCLR_SIC | I2C_I2CONCLR_STAC | I2C_I2CONCLR_I2ENC);
  /* Enable the I2C peripheral */
  I2Cx->CONSET = I2C_I2CONSET_I2EN;
}

/*********************************************************************
 * @brief		Send the buffer to the desired address on I2C
 * @param[in]	I2Cx	I2C peripheral selected, should be
 * 					- LPC_I2C0	:I2C0 peripheral
 * 					- LPC_I2C1	:I2C1 peripheral
 * @param[in]	addr	Address of the device on the I2C bus (7-bit)
 * @param[in]	buff	Buffer pointer
 * @param[in]	cnt		Number of bytes in buffer
 * @return 		SUCCESS - success; ERROR - failure
 *********************************************************************/
uint8_t I2C_Send(LPC_I2Cn_Type *I2Cx, uint8_t addr, uint8_t *buff, uint8_t cnt)
{
  uint32_t res, i;

  if (I2Cx != LPC_I2C0 && I2Cx != LPC_I2C1)
    return ERROR;

  res = I2C_Start(I2Cx);
  if ((res != I2C_I2STAT_M_TX_START) && (res != I2C_I2STAT_M_TX_RESTART))
    goto tx_error;

  res = I2C_SendByte(I2Cx, (addr << 1));
  if (res != I2C_I2STAT_M_TX_SLAW_ACK)
    goto tx_error;

  for (i = 0; i < cnt; i++)
  {
    res = I2C_SendByte(I2Cx, buff[i]);
    if (res != I2C_I2STAT_M_TX_DAT_ACK)
      goto tx_error;
  }

  I2C_Stop(I2Cx);
  return SUCCESS;

tx_error:
  I2C_Stop(I2Cx);
  return ERROR;
}
