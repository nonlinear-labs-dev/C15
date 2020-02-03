/******************************************************************************/
/** @file		nl_gpio.h
    @date		2015-02-02 DTZ
    @brief    	Basic GPIO functions
    @author		Nemanja Nikodijevic [2015-01-27]
*******************************************************************************/
#ifndef NL_GPIO_H_
#define NL_GPIO_H_

#include "cmsis/LPC43xx.h"
#include "drv/nl_pin.h"

#define NL_GPIO_IN  0
#define NL_GPIO_OUT 1

void    NL_GPIO_Set(GPIO_NAME_T* gpio);
void    NL_GPIO_Clr(GPIO_NAME_T* gpio);
int     NL_GPIO_Tgl(GPIO_NAME_T* gpio, int on);
void    NL_GPIO_Dir(GPIO_NAME_T* gpio, uint8_t dir);
void    NL_GPIO_SetState(GPIO_NAME_T* gpio, uint8_t state);
uint8_t NL_GPIO_Get(GPIO_NAME_T* gpio);

/* functions for setting a whole gpio port */
void     NL_GPIO_SetVal(uint32_t port, uint32_t bitValue);
void     NL_GPIO_ClrVal(uint32_t port, uint32_t bitValue);
uint32_t NL_GPIO_GetVal(uint32_t port);

#endif
