/*
 * nl_tcd_adc_work.h
 *
 *  Created on: 13.03.2015
 *      Author: ssc
 */

#ifndef TCD_NL_TCD_ADC_WORK_H_
#define TCD_NL_TCD_ADC_WORK_H_

#include "stdint.h"

#include "../../../shared/lpc-defs.h"

#define NON_RETURN       0
#define RETURN_TO_ZERO   1
#define RETURN_TO_CENTER 2

//------- public functions

void ADC_WORK_Init1(void);
void ADC_WORK_Init2(void);

void ADC_WORK_Process1(void);
void ADC_WORK_Process2(void);
void ADC_WORK_Process3(void);
void ADC_WORK_Process4(void);
//void ADC_WORK_Suspend(void);
//void ADC_WORK_Resume(void);

void ADC_WORK_WriteHWValueForBB(uint32_t hwSourceId, uint32_t value);
void ADC_WORK_SendBBMessages(void);

void ADC_WORK_SetRibbon1EditMode(uint32_t mode);
void ADC_WORK_SetRibbon1EditBehaviour(uint32_t behaviour);
void ADC_WORK_SetRibbon1Behaviour(uint32_t behaviour);
void ADC_WORK_SetRibbon2Behaviour(uint32_t behaviour);
void ADC_WORK_SetRibbonRelFactor(uint32_t factor);
void ADC_WORK_SetRibbonCalibration(uint16_t length, uint16_t* data);
void ADC_WORK_SetRawSensorMessages(uint32_t flag);

uint32_t ADC_WORK_GetRibbon1Behaviour(void);
uint32_t ADC_WORK_GetRibbon2Behaviour(void);

void ADC_WORK_Select_BenderTable(uint32_t curve);
void ADC_WORK_Select_AftertouchTable(uint32_t curve);

#endif /* TCD_NL_TCD_ADC_WORK_H_ */
