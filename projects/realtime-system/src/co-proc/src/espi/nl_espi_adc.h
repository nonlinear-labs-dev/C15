/******************************************************************************/
/** @file		nl_espi_adc.h
    @date		2014-07-07
    @brief    	eSPI module for ADC
    @example
    @ingroup  	LPC_ESPI
    @author		Nemanja Nikodijevic 2014-07-07
*******************************************************************************/

#ifndef NL_ESPI_ADC_H
#define NL_ESPI_ADC_H

#define ESPI_ADC_PORT   0
#define ESPI_ADC_DEVICE 1

#define ESPI_ADC_3201 1
#define ESPI_ADC_3202 2
#define ESPI_ADC_3204 4
#define ESPI_ADC_3208 8

typedef struct
{
  uint16_t* channel_val;
  uint8_t   channel_num;
  uint8_t   espi_port;
  uint8_t   espi_dev;
} ESPI_ADC_T;

void     ESPI_ADC_Init(ESPI_ADC_T* adc, uint8_t cn, uint8_t port, uint8_t dev);
uint16_t ESPI_ADC_Channel_Get(ESPI_ADC_T* adc, uint8_t ch);
uint32_t ESPI_ADC_Channel_Poll_Blocking(ESPI_ADC_T* adc, uint8_t ch);
uint32_t ESPI_ADC_Channel_Poll_NonBlocking(ESPI_ADC_T* adc, uint8_t ch);

#endif
