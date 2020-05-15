/******************************************************************************/
/** @file		nl_espi_io.h
    @date		2014-12-15 dtz
    @brief    	eSPI module for 74xx595 and 74xx165 Shift Registers
    @author		Nemanja Nikodijevic 2014-07-16
    @todo 		info: bit/byte order ???
*******************************************************************************/

#ifndef NL_ESPI_IO_H
#define NL_ESPI_IO_H

#define ESPI_SHIFT_IN  0
#define ESPI_SHIFT_OUT 1

#define ESPI_SHIFT_MAX_BYTES 32

#include <stdint.h>

typedef struct
{
  uint8_t* val;
  uint8_t  io;
  uint8_t  num_of_regs;
  uint8_t  espi_port;
  uint8_t  espi_dev;
} ESPI_IO_T;

void ESPI_IO_Init(ESPI_IO_T* io,
                  uint8_t    dir,
                  uint8_t    nreg,
                  uint8_t    port,
                  uint8_t    dev);

uint32_t ESPI_IO_In_Process(ESPI_IO_T* io);
uint32_t ESPI_IO_Out_Process(ESPI_IO_T* io);

#endif
