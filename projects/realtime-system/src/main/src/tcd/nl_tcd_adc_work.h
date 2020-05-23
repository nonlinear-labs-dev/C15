/*
 * nl_tcd_adc_work.h
 *
 *  Created on: 13.03.2015
 *      Author: ssc
 */
#pragma once

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

void ADC_WORK_WriteHWValueForUI(uint16_t const hwSourceId, uint16_t const value);
void ADC_WORK_SendUIMessages(void);
void ADC_WORK_WriteHWValueForAE(uint16_t const hwSourceId, uint16_t const value);

void ADC_WORK_SetRibbon1EditMode(uint16_t const mode);
void ADC_WORK_SetRibbon1EditBehaviour(uint16_t const behaviour);
void ADC_WORK_SetRibbon1Behaviour(uint16_t const behaviour);
void ADC_WORK_SetRibbon2Behaviour(uint16_t const behaviour);
void ADC_WORK_SetRibbon1OutputValue(uint16_t const value);
void ADC_WORK_SetRibbon2OutputValue(uint16_t const value);
void ADC_WORK_SetRibbonRelFactor(uint16_t const factor);
void ADC_WORK_SetRibbonCalibration(uint16_t const length, uint16_t* data);
void ADC_WORK_SetRawSensorMessages(uint16_t const flag);

uint32_t ADC_WORK_GetRibbon1Behaviour(void);
uint32_t ADC_WORK_GetRibbon2Behaviour(void);

void ADC_WORK_Select_BenderTable(uint16_t const curve);
void ADC_WORK_Select_AftertouchTable(uint16_t const curve);
