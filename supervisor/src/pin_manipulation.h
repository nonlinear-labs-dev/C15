/******************************************************************************/
/** @file		
    @date		2016-05-29
    @author		Daniel Tzschentke
	@version	2016-05-17
	@brief		
	@info
				PORTx	write to port
				1:high 0:low
				(if input pull-up: 1:on 0:off)
					
				DDRx	port direction	
						1:out 0:in
						*(&x-1)	 address from x - 1 
											
				PINx	read from port	
						*(&x-2)			
*******************************************************************************/
#ifndef GPIO_MANIPULATION_H_
#define GPIO_MANIPULATION_H_

/** @brief set output pin high */
#define PinSet(...)   _PinSet(__VA_ARGS__)
#define PinPue(...)   _PinSet(__VA_ARGS__)  // Pull-Up enable
#define _PinSet(x, y) x |= 1 << y

/** @brief clear output pin */
#define PinClr(...)   _PinClr(__VA_ARGS__)
#define PinPud(...)   _PinClr(__VA_ARGS__)  // Pull-Up disable
#define _PinClr(x, y) x &= ~(1 << y)

/** @brief toggle output pin */
#define PinTgl(...)   _PinTgl(__VA_ARGS__)
#define _PinTgl(x, y) x ^= (1 << (y))

/** @brief configure Pin as output */
#define PinOpt(...)   _PinOpt(__VA_ARGS__)
#define _PinOpt(x, y) *(&x - 1) |= 1 << y

/** @brief configure Pin as input */
#define PinIpt(...)   _PinIpt(__VA_ARGS__)
#define _PinIpt(x, y) *(&x - 1) &= ~(1 << y)

/** @brief read input pin - returns 1 if HIGH */
#define PinGet(...)   _PinGet(__VA_ARGS__)
#define _PinGet(x, y) ((*(&x - 2) & (1 << y)) >> y)

#endif /* GPIO_MANIPULATION_H_ */