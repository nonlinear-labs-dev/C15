/******************************************************************************/
/** @file		nl_status.c
    @date		2020-04-14
    @version	0
    @author		KSTR
    @brief		system status global variable
*******************************************************************************/
#include "sys/nl_status.h"
#include "ipc/emphase_ipc.h"

NL_systemStatus_T NL_systemStatus = {
  .COOS_totalOverruns    = 0,
  .COOS_maxTasksPerSlice = 0,
  .COOS_maxTaskTime      = 0,
  .COOS_maxDispatchTime  = 0,
  .BB_MSG_bufferOvers    = 0,
  .TCD_usbJams           = 0,
  .M0_KBSTime            = 0,
  .M0_SchedulerTime      = 0
};

uint16_t NL_STAT_GetDataSize(void)
{
  return sizeof(NL_systemStatus) / sizeof(uint16_t);
}

void NL_STAT_GetData(uint16_t *buffer)
{
  buffer[0] = NL_systemStatus.COOS_totalOverruns;
  buffer[1] = NL_systemStatus.COOS_maxTasksPerSlice;
  buffer[2] = NL_systemStatus.COOS_maxTaskTime;
  buffer[3] = NL_systemStatus.COOS_maxDispatchTime;
  buffer[4] = NL_systemStatus.BB_MSG_bufferOvers;
  buffer[5] = NL_systemStatus.TCD_usbJams;
  buffer[6] = NL_systemStatus.M0_KBSTime = IPC_GetAndResetKBSTime();
  buffer[7] = NL_systemStatus.M0_SchedulerTime = IPC_GetAndResetSchedulerTime();

  NL_systemStatus.COOS_totalOverruns    = 0;
  NL_systemStatus.COOS_maxTasksPerSlice = 0;
  NL_systemStatus.COOS_maxTaskTime      = 0;
  NL_systemStatus.COOS_maxDispatchTime  = 0;
  NL_systemStatus.BB_MSG_bufferOvers    = 0;
  NL_systemStatus.TCD_usbJams           = 0;
  NL_systemStatus.M0_KBSTime            = 0;
  NL_systemStatus.M0_SchedulerTime      = 0;
}
