/******************************************************************************/
/** @file		nl_status.h
    @date		2020-04-14
    @version	0
    @author		KSTR
    @brief		system status global variable
*******************************************************************************/
#ifndef NL_SYS_STATUS_H_
#define NL_SYS_STATUS_H_

#include "stdint.h"

typedef struct __attribute__((packed))
{
  uint16_t COOS_totalOverruns;
  uint16_t COOS_maxTasksPerSlice;
  uint16_t COOS_maxTaskTime;
  uint16_t COOS_maxDispatchTime;
  uint16_t BB_MSG_bufferOvers;
  uint16_t TCD_usbJams;
  uint16_t M0_KBSTime;
  uint16_t M0_SchedulerTime;
} NL_systemStatus_T;

extern NL_systemStatus_T NL_systemStatus;

void     NL_STAT_GetData(uint16_t *buffer);
uint16_t NL_STAT_GetDataSize(void);

#endif
