/******************************************************************************/
/** @file		nl_status.c
    @date		2020-04-14
    @version	0
    @author		KSTR
    @brief		system status global variable
*******************************************************************************/
#include "sys/nl_status.h"
#include "ipc/emphase_ipc.h"
#include "drv/nl_cgu.h"

#pragma GCC diagnostic ignored "-Wunused-function"

NL_systemStatus_T NL_systemStatus = {
  .M4_ticker             = 0,
  .COOS_totalOverruns    = 0,
  .COOS_maxTasksPerSlice = 0,
  .COOS_maxTaskTime      = 0,
  .COOS_maxDispatchTime  = 0,
  .BB_MSG_bufferOvers    = 0,
  .TCD_usbJams           = 0,
  .M0_ADCTime            = 0,
  .M0_KbsIrqOver         = 0,
};

uint16_t NL_STAT_GetDataSize(void)
{
  return sizeof(NL_systemStatus) / sizeof(uint16_t);
}

static uint16_t M4TicksToUS(uint32_t ticks)
{
  uint32_t ret = M4_PERIOD_US * ticks;
  if (ret > 65535)
    ret = 65535;
  return (uint16_t) ret;
}

static uint16_t M0JiffiesToNS(uint16_t jiffies)
{
  uint32_t ret = (NL_LPC_CLK_PERIOD_100PS * jiffies + 4) / 10;  // rounded
  if (ret > 65535)
    ret = 65535;
  return (uint16_t) ret;
}

void NL_STAT_GetData(uint16_t *buffer)
{
  uint32_t ticker = NL_systemStatus.M4_ticker = (s.ticker) - NL_systemStatus.M4_ticker;
  NL_systemStatus.M4_ticker                   = s.ticker;
  *(buffer++)                                 = ticker & 0xFFFF;
  *(buffer++)                                 = ticker >> 16;
  *(buffer++)                                 = NL_systemStatus.COOS_totalOverruns;
  *(buffer++)                                 = NL_systemStatus.COOS_maxTasksPerSlice;
  *(buffer++)                                 = M4TicksToUS(NL_systemStatus.COOS_maxTaskTime);
  *(buffer++)                                 = M4TicksToUS(NL_systemStatus.COOS_maxDispatchTime);
  *(buffer++)                                 = NL_systemStatus.BB_MSG_bufferOvers;
  *(buffer++)                                 = NL_systemStatus.TCD_usbJams;
  *(buffer++) = NL_systemStatus.M0_ADCTime = M4TicksToUS(IPC_GetAndResetADCTime());
  *(buffer++) = NL_systemStatus.M0_KbsIrqOver = (s.RitCrtlReg != 0) ? s.RitCrtlReg & 1 : 0xFFFF;
  s.RitCrtlReg                                = 0;

  NL_systemStatus.COOS_totalOverruns    = 0;
  NL_systemStatus.COOS_maxTasksPerSlice = 0;
  NL_systemStatus.COOS_maxTaskTime      = 0;
  NL_systemStatus.COOS_maxDispatchTime  = 0;
  NL_systemStatus.BB_MSG_bufferOvers    = 0;
  NL_systemStatus.TCD_usbJams           = 0;
}
