/******************************************************************************/
/** @file		nl_uda1380.c
    @date		2015-10-11
    @brief    	Driver for the UDA1380 audio codec for LPC43xx (I2C and I2S)
    @author		Nemanja Nikodijevic 2015-10-11
*******************************************************************************/
#include "drv/nl_i2c.h"
#include "drv/nl_uda1380.h"

/*********************************************************************
 * @brief		Write the desired register on the UDA1380
 * @param[in]	I2Cx	I2C peripheral selected, should be
 * 					- LPC_I2C0	:I2C0 peripheral
 * 					- LPC_I2C1	:I2C1 peripheral
 * @param[in]	addr	Address of the device on the I2C bus (7-bit)
 * @param[in]	reg		Register address
 * @param[in]	val		New register value
 * @return 		SUCCESS - success; ERROR - failure
 *********************************************************************/
static uint32_t UDA1380_WriteReg(LPC_I2Cn_Type *I2Cx, uint8_t addr, uint8_t reg, uint16_t val)
{
  uint8_t tx_buf[3];
  tx_buf[0] = reg;
  tx_buf[1] = val >> 8;
  tx_buf[2] = val & 0xFF;

  return I2C_Send(I2Cx, addr, tx_buf, 3);
}

/*********************************************************************
 * @brief		Initializes the UDA1380 codec
 * @param[in]	I2Cx	I2C peripheral selected, should be
 * 					- LPC_I2C0	:I2C0 peripheral
 * 					- LPC_I2C1	:I2C1 peripheral
 * @param[in]	addr	Address of the device on the I2C bus (7-bit)
 *********************************************************************/
void UDA1380_Init(LPC_I2Cn_Type *I2Cx, uint8_t addr)
{
  /* reset regs */
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_SOFT_RESET, 0x0000);
  /* sys regs init */
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_EVALM_CLK, UDA_EVALM_CLK_DEFAULT);
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_BUS_CTRL, 0x0000);
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_POWER_CTRL, (UDA_POWER_ON_PLL | UDA_POWER_ON_HP | UDA_POWER_ON_DAC | UDA_POWER_ON_BIAS | UDA_POWER_ON_ADC_PGA_LR));
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_ANALOG_CTRL, 0x0000);
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_HPAMP_CTRL, UDA_HPAMP_EN_SCP);
  /* interpolation filter init */
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_MASTER_VOL_CTRL, 0x0000);
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_MIXER_VOL_CTRL, 0x0000);
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_MODE_CTRL, 0x0000);
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_MUTE_CTRL, UDA_MUTE_DE_48_KHZ);
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_MIXER_FILTER_CTRL, 0x0000);
  /* decimator init */
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_DEC_VOL_CTRL, 0x0000);
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_PGA_CTRL, 0x0000);
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_ADC_CTRL, 0x0001);
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_AGC_CTRL, 0x0000);
  /* enable clocks */
  UDA1380_WriteReg(I2Cx, addr, UDA_REG_EVALM_CLK, (UDA_EVALM_CLK_DEFAULT | UDA_EVALM_CLK_SYSDIV_512_FS | UDA_EVALM_CLK_EN_DAC | UDA_EVALM_CLK_EN_ADC | UDA_EVALM_CLK_ADC_WSPLL | UDA_EVALM_CLK_DAC_WSPLL));
}
