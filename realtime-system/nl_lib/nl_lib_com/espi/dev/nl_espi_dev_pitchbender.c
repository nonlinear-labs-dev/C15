#include <stdint.h>

#include "espi/dev/nl_espi_dev_pitchbender.h"

#include "espi/nl_espi_core.h"
#include "espi/nl_espi_adc.h"

static ESPI_ADC_T pitchbender;

void ESPI_DEV_Pitchbender_Init(void)
{
  ESPI_ADC_Init(&pitchbender, ESPI_ADC_3201, 1, 2);
}

void ESPI_DEV_Pitchbender_EspiPull(void)
{
  ESPI_ADC_Channel_Poll_Blocking(&pitchbender, 0);
}

/// @return 12 adc value, right aligned
uint16_t ESPI_DEV_Pitchbender_GetValue(void)
{
  return ESPI_ADC_Channel_Get(&pitchbender, 0);
}
