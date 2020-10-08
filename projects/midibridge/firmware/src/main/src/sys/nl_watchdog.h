/******************************************************************************/
/** @file		nl_watchdog.h
    @date		2020-05-10
    @version	0
    @author		KSTR
    @brief		watchdog
*******************************************************************************/
#pragma once

#include "stdint.h"

void SYS_WatchDogInit(uint32_t timeoutInMs);
void SYS_WatchDogClear(void);
void SYS_Reset(void);
