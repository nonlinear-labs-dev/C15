#pragma once

#include "cmsis/LPC43xx.h"

// clang-format off


//
// ----- SCU pin configuration register -----
//
#define SFS_DIF (1 << 7)  // disable input filter
#define SFS_EIF (0)       // enable input filter
#define SFS_DIB (0)       // disable input buffer
#define SFS_EIB (1 << 6)  // enable input buffer
#define SFS_DHS (0)       // disable high speed
#define SFS_EHS (1 << 5)  // enable high speed
#define SFS_DPU (1 << 4)  // disable pull-up
#define SFS_EPU (0)       // enable pull-up
#define SFS_DPD (0)       // disable pull-down
#define SFS_EPD (1 << 3)  // enable pull-down


// "memory clobber" keeps the compiler from re-ordering instructions
#define __CL__ asm volatile ("" ::: "memory");

// volatile casts keep the compiler from optimizing away "unnecessary" accesses
#define __pIO32__ (volatile uint32_t *)
#define __pIO16__ (volatile uint16_t *)
#define __pIO8__  (volatile uint8_t  *)


// ----- access the pin control register -----
// the pin control details the physical pin, notably which of the many multiplexed functions will be used,
// for example setting it up as GPIO and setting attributes like pullups, input buffer (required for any
// pin whose state is to be read), glitch filter, speed select etc.

// macro uses expansion to the SFPSx_y members of the "LPC_SCU" struct from LPC43xx.h
// "port" and "pin" must be resolvable to literals at compile time
// the indirection *(&item)) helps the compiler to resolve literal memory addresses at compile time
// the memory clobber keeps instructions sequence as written
#define SFSP(port, pin) __CL__ (*( __pIO32__ &LPC_SCU->SFSP##port##_##pin))
// usage : SFSP(5, 2) = SFS_EIB + SFS_DPU + 0; // PORT5[2] pin function GPIO (GPIO2[11]) with readback, no pulls

//
// ----- GPIO lines access -----
// GPIO lines can be accessed with four different mechanisms:
// Bit alias access (uint8_t) :
//    read returns line level in bit 0, bits 7..1 read always o
//    write uses only bit 0, bits 7..1 are discarded
// Word alias access, as Byte (uint8_t), Halfword(uint16_t) and Word(uint32_t) :
//    read returns all 1's if line is high, else all 0's
//    write uses the or'ed state of all bits, only if all bit are 0 then the line output is low, otherwise high
//
// See "LPC43xx User Manual" for details, notably the mapping of the GPIO bank and bit numbers to physical pins

// GPIO direction control :
// the indirection *(&item)) helps the compiler to resolve literal memory addresses at compile time
// the memory clobber keeps instructions sequence as written
#define GPIO_DIR_IN(bank, bit)  __CL__ *( __pIO32__ &LPC_GPIO_PORT->DIR[(bank)] ) &= ~(1 << bit )
#define GPIO_DIR_OUT(bank, bit) __CL__ *( __pIO32__ &LPC_GPIO_PORT->DIR[(bank)] ) |=  (1 << bit )

// create index into the B[] and W[] array of "LPC_SCU" struct
#define __GPIOID(bank, bit)     ((bank) * 32 + (bit))

// pointers to the IO pin's bit and word alias registers :
#define pGPIO_Bit(bank, bit)      __pIO8__  &LPC_GPIO_PORT->B[__GPIOID(bank, bit)]
#define pGPIO_Byte(bank, bit)     __pIO8__  &LPC_GPIO_PORT->W[__GPIOID(bank, bit)]
#define pGPIO_Halfword(bank, bit) __pIO16__ &LPC_GPIO_PORT->W[__GPIOID(bank, bit)]
#define pGPIO_Word(bank, bit)     __pIO32__ &LPC_GPIO_PORT->W[__GPIOID(bank, bit)]

// direct access to the IO pin's bit and word alias registers :
#define GPIO_Bit(bank, bit)      *( pGPIO_Bit(bank, bit)      )
#define GPIO_Byte(bank, bit)     *( pGPIO_Byte(bank, bit)     )
#define GPIO_Halfword(bank, bit) *( pGPIO_Halfword(bank, bit) )
#define GPIO_Word(bank, bit)     *( pGPIO_Word(bank, bit)     )
