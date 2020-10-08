#pragma once

#include "stdint.h"

#define PEDAL_CHECK_PINCONFIG 0b00001010  // 10
#define PEDAL_TIP_TO_5V       0b00000001  // 1
#define PEDAL_TIP_TO_PULLUP   0b00000010  // 2
#define PEDAL_RING_TO_5V      0b00000100  // 4
#define PEDAL_RING_TO_PULLUP  0b00001000  // 8
#define PEDAL_DEFAULT_OFF     0b00000000  // 0

void     ESPI_DEV_Pedals_Init(void);
uint16_t ESPI_DEV_Pedals_GetValue(uint8_t id);
void     ESPI_DEV_Pedals_EspiPullChannel_Blocking(uint8_t channel);

void ESPI_DEV_Pedals_SetPedalState(uint8_t pedal, uint8_t state);

void ESPI_DEV_Pedals_PullResistors_EspiSend(void);
void ESPI_DEV_Pedals_PullResistors_EspiSendIfChanged(void);

uint8_t ESPI_DEV_Pedals_Detect_GetValue(void);
void    ESPI_DEV_Pedals_Detect_EspiPull(void);
