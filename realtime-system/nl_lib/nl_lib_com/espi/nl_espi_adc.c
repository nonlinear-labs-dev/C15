/******************************************************************************/
/** @file		nl_espi_adc.c
    @date		2014-07-07
    @brief    	eSPI module for ADC
    @example
    @ingroup  	LPC_ESPI
    @author		Nemanja Nikodijevic 2014-07-07
*******************************************************************************/

#include <stdlib.h>
#include "espi/nl_espi_core.h"
#include "espi/nl_espi_adc.h"

/** NOTE: Tcsh - min 500ns, time between two chip-select triggers... */

static ESPI_ADC_T* polled_adc = NULL;
static uint8_t     polled_channel;
static uint8_t     txb[3], rxb[3];

static void ESPI_ADC_Callback(uint32_t status)
{
  ESPI_SCS_Select(ESPI_PORT_OFF, polled_adc->espi_dev);

  if (status != SUCCESS)
    return;

  if (polled_adc->channel_num == ESPI_ADC_3201)
    polled_adc->channel_val[polled_channel] = ((rxb[0] & 0x1F) << 7) | (rxb[1] >> 1);
  else
    polled_adc->channel_val[polled_channel] = ((rxb[1] & 0xF) << 8) | rxb[2];
}

void ESPI_ADC_Init(ESPI_ADC_T* adc, uint8_t cn, uint8_t port, uint8_t dev)
{
  uint8_t i;

  adc->channel_num = cn;
  adc->espi_port   = port;
  adc->espi_dev    = dev;
  adc->channel_val = (uint16_t*) malloc(cn * sizeof(uint16_t));

  for (i = 0; i < cn; i++)
    adc->channel_val[i] = 0;
}

uint16_t ESPI_ADC_Channel_Get(ESPI_ADC_T* adc, uint8_t ch)
{
  if (ch >= adc->channel_num)
    return 0;
  return adc->channel_val[ch];
}

uint32_t ESPI_ADC_Channel_Poll_Blocking(ESPI_ADC_T* adc, uint8_t ch)
{
  if (ch >= adc->channel_num)
    return 0;

  ESPI_SCS_Select(adc->espi_port, adc->espi_dev);

  if (adc->channel_num == ESPI_ADC_3202)
  {
    txb[0] = 0x1;
    txb[1] = 0xA0 | (ch << 6);
    txb[2] = 0;
  }
  else
  {
    txb[0] = 0x6 | (ch >> 2);
    txb[1] = ch << 6;
    txb[2] = 0;
  }

  polled_adc     = adc;
  polled_channel = ch;

  return ESPI_Transfer(txb, rxb, 3, ESPI_ADC_Callback);
}

uint32_t ESPI_ADC_Channel_Poll_NonBlocking(ESPI_ADC_T* adc, uint8_t ch)
{
  if (ch >= adc->channel_num)
    return 0;

  ESPI_SCS_Select(adc->espi_port, adc->espi_dev);

  if (adc->channel_num == ESPI_ADC_3202)
  {
    txb[0] = 0x1;
    txb[1] = 0xA0 | (ch << 6);
    txb[2] = 0;
  }
  else
  {
    txb[0] = 0x6 | (ch >> 2);
    txb[1] = ch << 6;
    txb[2] = 0;
  }

  polled_adc     = adc;
  polled_channel = ch;

  return ESPI_TransferNonBlocking(txb, rxb, 3, ESPI_ADC_Callback);
}
