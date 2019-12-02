/******************************************************************************/
/** @file		nl_uda1380.h
    @date		2015-10-11
    @brief    	Driver for the UDA1380 audio codec for LPC43xx (I2C and I2S)
    @author		Nemanja Nikodijevic 2015-10-11
*******************************************************************************/
#ifndef _NL_UDA1380_H_
#define _NL_UDA1380_H_

#include <stdint.h>
#include "cmsis/LPC43xx.h"

/*******************************************************************/ /**
 * UDA1380 address depending on the L3MODE pin
 *********************************************************************/
/** L3MODE pin low */
#define UDA1380_ADDR_L3MODE_0 0x18
/** L3MODE pin high */
#define UDA1380_ADDR_L3MODE_1 0x1A

/**********************************************************************
 * UDA1380 register addresses
 *********************************************************************/
/******* System Settings *******************************************/
/** Evaluation modes and clock settings */
#define UDA_REG_EVALM_CLK 0x00
/** I2S bus IO settings */
#define UDA_REG_BUS_CTRL 0x01
/** Power control settings */
#define UDA_REG_POWER_CTRL 0x02
/** Analog mixer settings */
#define UDA_REG_ANALOG_CTRL 0x03
/** Heaphone amp settings */
#define UDA_REG_HPAMP_CTRL 0x04
/******* Interpolation filter ***************************************/
/** Master volume control */
#define UDA_REG_MASTER_VOL_CTRL 0x10
/** Mixer volume control */
#define UDA_REG_MIXER_VOL_CTRL 0x11
/** Mode selection settings */
#define UDA_REG_MODE_CTRL 0x12
/** Mute control settings*/
#define UDA_REG_MUTE_CTRL 0x13
/** Mixer and filter settings */
#define UDA_REG_MIXER_FILTER_CTRL 0x14
/******* Decimator **************************************************/
/** Decimator volume control */
#define UDA_REG_DEC_VOL_CTRL 0x20
/** PGA settings and mute */
#define UDA_REG_PGA_CTRL 0x21
/** ADC settings */
#define UDA_REG_ADC_CTRL 0x22
/** AGC settings */
#define UDA_REG_AGC_CTRL 0x23
/******* Software reset **********************************************/
/** Software reset control */
#define UDA_REG_SOFT_RESET 0x7F

/*******************************************************************/ /**
 * Evaluation modes and clock settings register description
 *********************************************************************/
/** Default value */
#define UDA_EVALM_CLK_DEFAULT 0x0502
/** Enable ADC clock */
#define UDA_EVALM_CLK_EN_ADC ((1 << 11))
/** Enable DAC clock */
#define UDA_EVALM_CLK_EN_DAC ((1 << 9))
/** ADC clock - use WSPLL */
#define UDA_EVALM_CLK_ADC_WSPLL ((1 << 5))
/** DAC clock - use WSPLL */
#define UDA_EVALM_CLK_DAC_WSPLL ((1 << 4))
/** Dividers for system clock input */
#define UDA_EVALM_CLK_SYSDIV_256_FS ((0 << 2))
#define UDA_EVALM_CLK_SYSDIV_384_FS ((1 << 2))
#define UDA_EVALM_CLK_SYSDIV_512_FS ((2 << 2))
#define UDA_EVALM_CLK_SYSDIV_768_FS ((3 << 2))

/*******************************************************************/ /**
 * Power control settings register description
 *********************************************************************/
/** Power-on WSPLL */
#define UDA_POWER_ON_PLL ((1 << 15))
/** Power-on Headphone driver */
#define UDA_POWER_ON_HP ((1 << 13))
/** Power-on DAC */
#define UDA_POWER_ON_DAC ((1 << 10))
/** Power-on BIAS */
#define UDA_POWER_ON_BIAS ((1 << 8))
/** Enable control AVC */
#define UDA_POWER_ENABLE_AVC ((1 << 7))
/** Power-on AVC */
#define UDA_POWER_ON_AVC ((1 << 6))
/** Power-on LNA */
#define UDA_POWER_ON_LNA ((1 << 4))
/** Power-on PGAL */
#define UDA_POWER_ON_PGAL ((1 << 3))
/** Power-on ADCL */
#define UDA_POWER_ON_ADCL ((1 << 2))
/** Power-on PGAR */
#define UDA_POWER_ON_PGAR ((1 << 1))
/** Power-on ADCR */
#define UDA_POWER_ON_ADCR ((1 << 0))
/** Power-on ADCLR and PGALR */
#define UDA_POWER_ON_ADC_PGA_LR (0xF)

/*******************************************************************/ /**
 * Headphone amp settings register description
 *********************************************************************/
/** Short-circuit protection enable */
#define UDA_HPAMP_EN_SCP ((1 << 9) | 2)

/*******************************************************************/ /**
 * Mute control settings register description
 *********************************************************************/
/** Master mute enable */
#define UDA_MUTE_MASTER_EN ((1 << 14))
/** De-emphasis */
#define UDA_MUTE_DE_32_KHZ ((1 << 8) | 1)
#define UDA_MUTE_DE_44_KHZ ((2 << 8) | 2)
#define UDA_MUTE_DE_48_KHZ ((3 << 8) | 3)
#define UDA_MUTE_DE_96_KHZ ((4 << 8) | 4)

void UDA1380_Init(LPC_I2Cn_Type *I2Cx, uint8_t addr);

#endif
