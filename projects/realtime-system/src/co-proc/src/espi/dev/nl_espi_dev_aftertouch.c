#include <stdint.h>

#include "espi/dev/nl_espi_dev_aftertouch.h"

#include "espi/nl_espi_core.h"
#include "espi/nl_espi_adc.h"

ESPI_ADC_T aftertouch;

void ESPI_DEV_Aftertouch_Init(void)
{
  ESPI_ADC_Init(&aftertouch, ESPI_ADC_3201, 0, 1);
}

void ESPI_DEV_Aftertouch_EspiPull(void)
{
  ESPI_ADC_Channel_Poll_Blocking(&aftertouch, 0);
}

uint32_t ESPI_DEV_Aftertouch_EspiPullRet(void)
{
  return ESPI_ADC_Channel_Poll_Blocking(&aftertouch, 0);
}

/// @return 12 adc value, right aligned
uint16_t ESPI_DEV_Aftertouch_GetValue(void)
{
  return ESPI_ADC_Channel_Get(&aftertouch, 0);
}
