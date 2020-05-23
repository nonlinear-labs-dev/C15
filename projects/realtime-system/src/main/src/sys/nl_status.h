/******************************************************************************/
/** @file		nl_status.h
    @date		2020-04-14
    @version	0
    @author		KSTR
    @brief		system status global variable
*******************************************************************************/
#pragma once

#include "stdint.h"

typedef struct __attribute__((packed))
{
  uint32_t M4_ticker;
  uint16_t COOS_totalOverruns;
  uint16_t COOS_maxTasksPerSlice;
  uint16_t COOS_maxTaskTime;
  uint16_t COOS_maxDispatchTime;
  uint16_t BB_MSG_bufferOvers;
  uint16_t TCD_usbJams;
  uint16_t M0_ADCTime;
  uint16_t M0_KbsIrqOver;
} NL_systemStatus_T;

extern NL_systemStatus_T NL_systemStatus;

void     NL_STAT_GetData(uint16_t *buffer);
uint16_t NL_STAT_GetDataSize(void);
