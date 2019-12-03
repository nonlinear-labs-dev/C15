/******************************************************************************/
/** @file		nl_i2c.h
    @date		2015-06-24
    @brief    	I2C driver for LPC43xx
    @author		Nemanja Nikodijevic 2015-06-24
*******************************************************************************/
#ifndef _NL_I2C_H_
#define _NL_I2C_H_

#include <stdint.h>
#include "cmsis/LPC43xx.h"

/*******************************************************************/ /**
 * I2C Control Set register description
 *********************************************************************/
#define I2C_I2CONSET_AA   ((0x04))                                    /*!< Assert acknowledge flag */
#define I2C_I2CONSET_SI   ((0x08))                                    /*!< I2C interrupt flag */
#define I2C_I2CONSET_STO  ((0x10))                                    /*!< STOP flag */
#define I2C_I2CONSET_STA  ((0x20))                                    /*!< START flag */
#define I2C_I2CONSET_I2EN ((0x40))                                    /*!< I2C interface enable */

/*******************************************************************/ /**
 * I2C Control Clear register description
 *********************************************************************/
/** Assert acknowledge Clear bit */
#define I2C_I2CONCLR_AAC ((1 << 2))
/** I2C interrupt Clear bit */
#define I2C_I2CONCLR_SIC ((1 << 3))
/** START flag Clear bit */
#define I2C_I2CONCLR_STAC ((1 << 5))
/** I2C interface Disable bit */
#define I2C_I2CONCLR_I2ENC ((1 << 6))

/********************************************************************/ /**
 * I2C Status Code definition (I2C Status register)
 *********************************************************************/
/* Return Code in I2C status register */
#define I2C_STAT_CODE_BITMASK ((0xF8))

/* Master transmit mode -------------------------------------------- */
/** A start condition has been transmitted */
#define I2C_I2STAT_M_TX_START ((0x08))
/** A repeat start condition has been transmitted */
#define I2C_I2STAT_M_TX_RESTART ((0x10))
/** SLA+W has been transmitted, ACK has been received */
#define I2C_I2STAT_M_TX_SLAW_ACK ((0x18))
/** SLA+W has been transmitted, NACK has been received */
#define I2C_I2STAT_M_TX_SLAW_NACK ((0x20))
/** Data has been transmitted, ACK has been received */
#define I2C_I2STAT_M_TX_DAT_ACK ((0x28))
/** Data has been transmitted, NACK has been received */
#define I2C_I2STAT_M_TX_DAT_NACK ((0x30))
/** Arbitration lost in SLA+R/W or Data bytes */
#define I2C_I2STAT_M_TX_ARB_LOST ((0x38))

void    I2C_Init(LPC_I2Cn_Type *I2Cx, uint32_t clk);
uint8_t I2C_Send(LPC_I2Cn_Type *I2Cx, uint8_t addr, uint8_t *buff, uint8_t cnt);

#endif
