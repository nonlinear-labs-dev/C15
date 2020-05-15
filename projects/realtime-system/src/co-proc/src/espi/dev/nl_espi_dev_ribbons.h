

#ifndef NL_ESPI_DEV_RIBBONS_H_
#define NL_ESPI_DEV_RIBBONS_H_

#include "stdint.h"

#define UPPER_RIBBON 0
#define LOWER_RIBBON 1

void     ESPI_DEV_Ribbons_Init(void);
void     ESPI_DEV_Ribbons_EspiPull_Upper(void);
void     ESPI_DEV_Ribbons_EspiPull_Lower(void);
uint16_t ESPI_DEV_Ribbons_GetValue(uint8_t id);

#endif
