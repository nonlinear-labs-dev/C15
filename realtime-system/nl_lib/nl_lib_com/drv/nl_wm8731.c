/******************************************************************************/
/** @file		nl_wm8731.c
    @date		2015-06-30
    @brief    	Driver for the WM8731 audio codec for LPC43xx (I2C and I2S)
    @author		Nemanja Nikodijevic 2015-06-30
*******************************************************************************/
#include "drv/nl_i2c.h"
#include "drv/nl_wm8731.h"

/*********************************************************************
 * @brief		Write the desired register on the WM8731
 * @param[in]	I2Cx	I2C peripheral selected, should be
 * 					- LPC_I2C0	:I2C0 peripheral
 * 					- LPC_I2C1	:I2C1 peripheral
 * @param[in]	addr	Address of the device on the I2C bus (7-bit)
 * @param[in]	reg		Register address
 * @param[in]	val		New register value
 * @return 		SUCCESS - success; ERROR - failure
 *********************************************************************/
static uint32_t WM8731_WriteReg(LPC_I2Cn_Type *I2Cx, uint8_t addr, uint8_t reg, uint8_t val)
{
  uint8_t tx_buf[2];
  tx_buf[0] = reg;
  tx_buf[1] = val;

  return I2C_Send(I2Cx, addr, tx_buf, 2);
}

/*********************************************************************
 * @brief		Initializes the WM8731 codec
 * @param[in]	I2Cx	I2C peripheral selected, should be
 * 					- LPC_I2C0	:I2C0 peripheral
 * 					- LPC_I2C1	:I2C1 peripheral
 * @param[in]	addr	Address of the device on the I2C bus (7-bit)
 *********************************************************************/
void WM8731_Init(LPC_I2Cn_Type *I2Cx, uint8_t addr)
{
  /* reset chip */
  WM8731_WriteReg(I2Cx, addr, WM8731_REG_RESET, 0);
  /* line in regs */
  WM8731_WriteReg(I2Cx, addr, WM8731_REG_LEFT_LINE_IN_BOTH, WM8731_LINE_IN_VOL_0_DB);
  /* headphone out regs */
  WM8731_WriteReg(I2Cx, addr, WM8731_REG_LEFT_HP_OUT_BOTH, WM8731_HP_OUT_VOL_0_DB);
  /* analogue audio path control */
  WM8731_WriteReg(I2Cx, addr, WM8731_REG_ANALOGUE_PATH_CTRL, (WM8731_MUTEMIC | WM8731_DAC_SELECT));
  /* digital audio path control */
  WM8731_WriteReg(I2Cx, addr, WM8731_REG_DIGITAL_PATH_CTRL, 0);  //WM8731_DAC_SOFT_MUTE);
  /* power down control */
  WM8731_WriteReg(I2Cx, addr, WM8731_REG_POWER_DOWN_CTRL, 0);
  /* digital audio interface format */
  WM8731_WriteReg(I2Cx, addr, WM8731_REG_AUDIO_FORMAT, (WM8731_FORMAT_I2S | WM8731_IWL_32_BIT));
  /* sampling control */
  WM8731_WriteReg(I2Cx, addr, WM8731_REG_SAMPLING_CTRL, WM8731_MODE_NORMAL_256_FS);
  /* active control */
  WM8731_WriteReg(I2Cx, addr, WM8731_REG_ACTIVE_CTRL, 1);
}
