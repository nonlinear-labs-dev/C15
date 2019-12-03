/******************************************************************************/
/** @file		bit manipulation
    @date		2016-05-17
    @author		Daniel Tzschentke
	@version	2016-05-17
	@brief		Bit functions are made for manipulating single bits of a 
				register/byte without affecting other bits.
*******************************************************************************/
#ifndef BIT_MANIPULATION_H_
#define BIT_MANIPULATION_H_

#include <avr/io.h>

#define BitSet(register, bit) register |= (1 << (bit))
#define BitClr(register, bit) register &= ~(1 << (bit))
#define BitTgl(register, bit) register ^= (1 << (bit))
//#define BitGet(register, bit)   (_SFR_BYTE(register) & (1 << (bit)))
#define BitGet(register, bit) ((_SFR_BYTE(register) & (1 << (bit))) >> bit)

#endif /* BIT_MANIPULATION_H_ */