/******************************************************************************/
/** @file		nl_pin.h
    @date		2015-01-27
    @author		[2015-01-27 DTZ]
*******************************************************************************/
#ifndef NL_PIN_H
#define NL_PIN_H

#include <stdint.h>

typedef struct
{
  uint8_t port;
  uint8_t pin;
} PIN_NAME_T;

typedef PIN_NAME_T GPIO_NAME_T;

#define PIN_TYPE_GPIO    0
#define PIN_TYPE_PIN     1
#define PIN_GPIO_DIR_IN  0
#define PIN_GPIO_DIR_OUT 1

typedef struct
{
  PIN_NAME_T  pinId;
  GPIO_NAME_T gpioId;
  uint8_t     ioType;
  uint8_t     function;
  uint8_t     glitchFilter;  // ZIF
  uint8_t     inputBuffer;   // EZI
  uint8_t     pullUp;
  uint8_t     pullDown;
  uint8_t     slewRate;  // EHS
  uint8_t     direction;
} PIN_CFG_T;

void PIN_Config(PIN_CFG_T* pin);

#define PORT_OFFSET 0x80
#define PIN_OFFSET  0x04

#define PIN_PDN_ON     (1 << 3)
#define PIN_PDN_OFF    (0 << 3)
#define PIN_PUP_ON     (0 << 4)
#define PIN_PUP_OFF    (1 << 4)
#define PIN_SRATE_SLOW (0 << 5)
#define PIN_SRATE_FAST (1 << 5)
#define PIN_INBUF_ON   (1 << 6)
#define PIN_INBUF_OFF  (0 << 6)
#define PIN_FILTER_ON  (0 << 7)
#define PIN_FILTER_OFF (1 << 7)

#endif
