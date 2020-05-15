/******************************************************************************/
/** @file		nl_watchdog.h
    @date		2020-05-10
    @version	0
    @author		KSTR
    @brief		watchdog
*******************************************************************************/
#ifndef NL_WATCHDOG_H_
#define NL_WATCHDOG_H_

#include "stdint.h"

void SYS_WatchDogInit(uint32_t timeoutInMs);
void SYS_WatchDogClear(void);
void SYS_Reset(void);

#endif
