/******************************************************************************/
/** @file		nl_gpio.c
    @date		2015-01-27
    @brief    	Basic GPIO functions
    @author		Nemanja Nikodijevic [2015-01-27]
*******************************************************************************/
#include "nl_gpio.h"

void NL_GPIO_Set(GPIO_NAME_T* gpio)
{
  LPC_GPIO_PORT->SET[gpio->port] = (uint32_t)(1 << gpio->pin);
}

void NL_GPIO_Clr(GPIO_NAME_T* gpio)
{
  LPC_GPIO_PORT->CLR[gpio->port] = (uint32_t)(1 << gpio->pin);
}

void NL_GPIO_Dir(GPIO_NAME_T* gpio, uint8_t dir)
{
  if (dir == NL_GPIO_IN)
    LPC_GPIO_PORT->DIR[gpio->port] &= (uint32_t)(~(1 << gpio->pin));
  else if (dir == NL_GPIO_OUT)
    LPC_GPIO_PORT->DIR[gpio->port] |= (uint32_t)(1 << gpio->pin);
}

uint8_t NL_GPIO_Get(GPIO_NAME_T* gpio)
{
  if (LPC_GPIO_PORT->PIN[gpio->port] & (1 << gpio->pin))
    return 1;
  else
    return 0;
}

void NL_GPIO_SetState(GPIO_NAME_T* gpio, uint8_t state)
{
  if (state)
    NL_GPIO_Set(gpio);
  else
    NL_GPIO_Clr(gpio);
}

void NL_GPIO_SetVal(uint32_t port, uint32_t bitValue)
{
  LPC_GPIO_PORT->SET[port] = bitValue;
}

void NL_GPIO_ClrVal(uint32_t port, uint32_t bitValue)
{
  LPC_GPIO_PORT->CLR[port] = bitValue;
}

uint32_t NL_GPIO_GetVal(uint32_t port)
{
  return LPC_GPIO_PORT->PIN[port];
}
