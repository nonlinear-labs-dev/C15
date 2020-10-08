/******************************************************************************/
/** @file		nl_status.h
    @date		2020-04-14
    @version	0
    @author		KSTR
    @brief		system status global variable
*******************************************************************************/
#pragma once

#include <stdint.h>
#include "globals.h"

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
  uint16_t DroppedMidiMessageBuffers;
#if LPC_KEYBED_DIAG
  uint16_t MissedKeybedEventsScanner;
  uint16_t MissedKeybedEventsTCD;
#endif
} NL_systemStatus_T;

extern NL_systemStatus_T NL_systemStatus;

uint16_t NL_STAT_CheckMissedKeybedEvents(void);  // == 0 means all counters are zero
void     NL_STAT_ClearData(void);
void     NL_STAT_GetData(uint16_t *buffer);
uint16_t NL_STAT_GetDataSize(void);
#if LPC_KEYBED_DIAG
void     NL_STAT_GetKeyData(uint16_t *buffer);
uint16_t NL_STAT_GetKeyDataSize(void);
#endif
