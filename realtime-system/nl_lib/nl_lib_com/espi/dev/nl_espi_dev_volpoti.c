#include <stdint.h>

#include "espi/dev/nl_espi_dev_volpoti.h"

#include "espi/nl_espi_core.h"
#include "espi/nl_espi_adc.h"

static ESPI_ADC_T volpoti;

void ESPI_DEV_VolPoti_Init(void)
{
  ESPI_ADC_Init(&volpoti, ESPI_ADC_3201, 3, 1);
}

void ESPI_DEV_VolPoti_EspiPull(void)
{
  ESPI_ADC_Channel_Poll_Blocking(&volpoti, 0);
}

void ESPI_DEV_VolPoti_ProcessNbNs(void)
{
  ESPI_ADC_Channel_Poll_NonBlocking(&volpoti, 0);
}

/// @return 12 adc value, right aligned
uint16_t ESPI_DEV_VolPoti_GetValue(void)
{
  return ESPI_ADC_Channel_Get(&volpoti, 0);
}
