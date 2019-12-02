/******************************************************************************/
/** @file		nl_wm8731.h
    @date		2015-06-30
    @brief    	Driver for the WM8731 audio codec for LPC43xx (I2C and I2S)
    @author		Nemanja Nikodijevic 2015-06-30
*******************************************************************************/
#ifndef _NL_WM8731_H_
#define _NL_WM8731_H_

#include <stdint.h>
#include "cmsis/LPC43xx.h"

/*******************************************************************/ /**
 * WM8731 address depending on the CSB pin
 *********************************************************************/
/** CSB pin low */
#define WM8731_ADDR_CSB_0 0x1A
/** CSB pin high */
#define WM8731_ADDR_CSB_1 0x1B

/*******************************************************************/ /**
 * WM8731 register addresses
 *********************************************************************/
/** Left Line In */
#define WM8731_REG_LEFT_LINE_IN 0x00
/** Left Line In Both*/
#define WM8731_REG_LEFT_LINE_IN_BOTH 0x01
/** Right Line In */
#define WM8731_REG_RIGHT_LINE_IN 0x02
/** Right Line In Both*/
#define WM8731_REG_RIGHT_LINE_IN_BOTH 0x03
/** Left Headphone Out */
#define WM8731_REG_LEFT_HP_OUT 0x04
/** Left Headphone Out Both*/
#define WM8731_REG_LEFT_HP_OUT_BOTH 0x05
/** Right Headphone Out */
#define WM8731_REG_RIGHT_HP_OUT 0x06
/** Right Headphone Out Both*/
#define WM8731_REG_RIGHT_HP_OUT_BOTH 0x07
/** Analogue Audio Path Control */
#define WM8731_REG_ANALOGUE_PATH_CTRL 0x08
/** Digital Audio Path Control */
#define WM8731_REG_DIGITAL_PATH_CTRL 0x0A
/** Power Down Control */
#define WM8731_REG_POWER_DOWN_CTRL 0x0C
/** Digital Audio Interface Format */
#define WM8731_REG_AUDIO_FORMAT 0x0E
/** Sampling Control */
#define WM8731_REG_SAMPLING_CTRL 0x10
/** Active Control */
#define WM8731_REG_ACTIVE_CTRL 0x12
/** RESET Register */
#define WM8731_REG_RESET 0x1E

/*******************************************************************/ /**
 * WM8731 Left/Right Line In register description
 *********************************************************************/
/** Line In Volume */
#define WM8731_LINE_IN_VOL_0_DB ((0x17 << 0))
/** Line In Mute */
#define WM8731_LINE_IN_MUTE ((1 << 7))

/*******************************************************************/ /**
 * WM8731 Left/Right Headphone Out register description
 *********************************************************************/
/** Headphone Out Volume */
#define WM8731_HP_OUT_VOL_0_DB ((0x79 << 0))
/** Headphone Out Mute */
#define WM8731_HP_OUT_MUTE 0
/** Left/Right Channel Zero Cross Detect Enable bit */
#define WM8731_HP_OUT_CZEN ((1 << 7))

/*******************************************************************/ /**
 * WM8731 Analogue Audio Path Control register description
 *********************************************************************/
/** Microphone Input Level Boost Enable bit */
#define WM8731_MICBOOST ((1 << 0))
/** Microphone Input Mute to ADC bit */
#define WM8731_MUTEMIC ((1 << 1))
/** Mic/Line in Input Select to ADC bit */
#define WM8731_INSEL_MIC  ((1 << 2))
#define WM8731_INSEL_LINE ((0 << 2))
/** Line Out Switches (pg 26 in datasheet) */
#define WM8731_LINE_BYPASS  ((1 << 3))
#define WM8731_MIC_SIDETONE ((1 << 5))
#define WM8731_DAC_SELECT   ((1 << 4))
/** Side Tone attenuation */
#define WM8731_SIDETONE_ATT_6_DB  ((0 << 6))
#define WM8731_SIDETONE_ATT_9_DB  ((1 << 6))
#define WM8731_SIDETONE_ATT_12_DB ((2 << 6))
#define WM8731_SIDETONE_ATT_15_DB ((3 << 6))

/*******************************************************************/ /**
 * WM8731 Digital Audio Path Control register description
 *********************************************************************/
/** ADC High Pass Filter Disable bit */
#define WM8731_ADCHP_DISABLE ((1 << 0))
/** De-Emphasis Control */
#define WM8731_DEEMP_DISABLE ((0 << 1))
#define WM8731_DEEMP_32_KHZ  ((1 << 1))
#define WM8731_DEEMP_44_KHZ  ((2 << 1))
#define WM8731_DEEMP_48_KHZ  ((3 << 1))
/** DAC Soft Mute Control Enable bit */
#define WM8731_DAC_SOFT_MUTE ((1 << 3))
/** Store DC offset when HP Filter disabled */
#define WM8731_HPOR ((1 << 4))

/*******************************************************************/ /**
 * WM8731 Power Down Control register description
 *********************************************************************/
/** Line In Power Down */
#define WM8731_LINE_IN_PD ((1 << 0))
/** Microphone Power Down */
#define WM8731_MIC_PD ((1 << 1))
/** ADC Power Down */
#define WM8731_ADC_PD ((1 << 2))
/** DAC Power Down */
#define WM8731_DAC_PD ((1 << 3))
/** Outputs Power Down */
#define WM8731_OUT_PD ((1 << 4))
/** Oscillator Power Down */
#define WM8731_OSC_PD ((1 << 5))
/** CLKOUT Power Down */
#define WM8731_CLKOUT_PD ((1 << 6))
/** POWEROFF mode enable */
#define WM8731_POWEROFF ((1 << 7))

/*******************************************************************/ /**
 * WM8731 Digital Audio Interface Format register description
 *********************************************************************/
/******* Audio Data Format *******/
/** MSB-first right justified */
#define WM8731_FORMAT_MSBF_RIGHT ((0 << 0))
/** MSB-first left justified */
#define WM8731_FORMAT_MSBF_LEFT ((1 << 0))
/** I2S mode */
#define WM8731_FORMAT_I2S ((2 << 0))
/** DSP mode */
#define WM8731_FORMAT_DSP ((3 << 0))
/******* Audio Data Bit Length *******/
#define WM8731_IWL_16_BIT ((0 << 2))
#define WM8731_IWL_20_BIT ((1 << 2))
#define WM8731_IWL_24_BIT ((2 << 2))
#define WM8731_IWL_32_BIT ((3 << 2))
/** Left Right Polarity */
#define WM8731_POL_RIGHT_HIGH ((1 << 4))
#define WM8731_POL_RIGHT_LOW  ((0 << 4))
/** DAC Left Right Clock Swap */
#define WM8731_LRSWAP ((1 << 5))
/** Master or Slave mode */
#define WM8731_MASTER_MODE ((1 << 6))
/** Bit Clock Invert */
#define WM8731_BCLK_INVERT ((1 << 7))

/*******************************************************************/ /**
 * WM8731 Sampling Control register description
 *********************************************************************/
/** Mode Select (USB or NORMAL) and Oversampling Rate */
#define WM8731_MODE_USB_250_FS    ((1 << 0))
#define WM8731_MODE_USB_272_FS    ((3 << 0))
#define WM8731_MODE_NORMAL_256_FS ((0 << 0))
#define WM8731_MODE_NORMAL_384_FS ((2 << 0))
/** @todo: Sample rate control for ADC and DAC */

void WM8731_Init(LPC_I2Cn_Type *I2Cx, uint8_t addr);

#endif
