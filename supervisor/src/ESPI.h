// initial ESPI override to set all LEDs on TopCover and PanelUnit
// KSTR 2019-06-21

#ifndef __ESPI__
#define __ESPI__

#include <stdint.h>

void ESPI_StartOverride(void);
void ESPI_Process(void);  // step-chain task

#endif

extern int8_t ESPI_Process_ID;

// end of file
