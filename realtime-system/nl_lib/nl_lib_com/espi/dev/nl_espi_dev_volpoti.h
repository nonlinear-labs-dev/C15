#ifndef NL_ESPI_DEV_VOLPOTI_H_
#define NL_ESPI_DEV_VOLPOTI_H_

#include "stdint.h"

void     ESPI_DEV_VolPoti_Init(void);
void     ESPI_DEV_VolPoti_EspiPull(void);
uint16_t ESPI_DEV_VolPoti_GetValue(void);
void     ESPI_DEV_VolPoti_ProcessNbNs(void);

#endif
