#include <stdint.h>
#include "drv/nl_gpio.h"
#include "cmsis/LPC43xx.h"
#include "drv/nl_pin.h"

void PIN_Config(PIN_CFG_T* ioPin)
{
  uint32_t* baseAddr = (uint32_t*) (LPC_SCU_BASE);

  uint8_t mode = ioPin->pullUp | ioPin->pullDown | ioPin->inputBuffer | ioPin->slewRate | ioPin->glitchFilter;

  baseAddr[(PORT_OFFSET * ioPin->pinId.port + PIN_OFFSET * ioPin->pinId.pin) / 4] = mode + ioPin->function;

  if (ioPin->ioType == PIN_TYPE_GPIO)
  {
    NL_GPIO_Dir(&ioPin->gpioId, ioPin->direction);
  }
}
