/******************************************************************************/
/** @file		emphase_ipc.h
    @date		2016-03-01 DTZ
    @brief    	M4 <-> M0 inter processor communication module
    @author		Nemanja Nikodijevic 2015-02-03
*******************************************************************************/
#pragma once

#include <stdint.h>
#include "globals.h"

typedef struct
{
  int dummy;
} SharedData_T;

extern SharedData_T s;

void Emphase_IPC_Init(void);
