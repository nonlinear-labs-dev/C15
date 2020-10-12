/******************************************************************************/
/**	@file 	co-proc.c
	@date	2020-05-27 KSTR
  	@author	2015-01-31 DTZ

  	@note	!!!!!! USE "optimized most", -O3, for compiling !!!!!!

*******************************************************************************/

#pragma GCC diagnostic ignored "-Wmain"

#include <stdint.h>
#include "cmsis/LPC43xx.h"
#include "drv/nl_rit.h"
#include "drv/nl_cgu.h"
#include "sys/nl_version.h"
#include "io/pins.h"
#include "version.h"
/******************************************************************************/
volatile char dummy;

void dummyFunction(const char* string)
{
  while (*string)
  {
    dummy = *string++;
  }
}

void main(void)
{
  // referencing the version string so compiler won't optimize it away
  dummyFunction(VERSION_STRING);
  dummyFunction(GetC15Version());
  dummyFunction(GetC15Build());

  RIT_Init_IntervalInHz(M0_FREQ_HZ);

  while (1)
    ;
}

/******************************************************************************/

// __attribute__((section(".ramfunc")))
void M0_RIT_OR_WWDT_IRQHandler(void)
{
  RIT_ClearInt();
}
