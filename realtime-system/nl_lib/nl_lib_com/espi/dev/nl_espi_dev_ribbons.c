#include <stdint.h>

#include "espi/dev/nl_espi_dev_ribbons.h"

#include "espi/nl_espi_core.h"
#include "espi/nl_espi_adc.h"

ESPI_ADC_T ribbons;

void ESPI_DEV_Ribbons_Init(void)
{
  ESPI_ADC_Init(&ribbons, ESPI_ADC_3202, 1, 1);
}

void ESPI_DEV_Ribbons_EspiPull_Upper(void)
{
  ESPI_ADC_Channel_Poll_Blocking(&ribbons, UPPER_RIBBON);
}

void ESPI_DEV_Ribbons_EspiPull_Lower(void)
{
  ESPI_ADC_Channel_Poll_Blocking(&ribbons, LOWER_RIBBON);
}

/// @return 12 adc value, right aligned
uint16_t ESPI_DEV_Ribbons_GetValue(uint8_t id)
{
  return ESPI_ADC_Channel_Get(&ribbons, id);
}
