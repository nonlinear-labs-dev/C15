#ifndef NL_ESPI_DEV_PITCHBENDER_H_
#define NL_ESPI_DEV_PITCHBENDER_H_

#include "stdint.h"

void     ESPI_DEV_Pitchbender_Init(void);
void     ESPI_DEV_Pitchbender_EspiPull(void);
uint16_t ESPI_DEV_Pitchbender_GetValue(void);

#endif
