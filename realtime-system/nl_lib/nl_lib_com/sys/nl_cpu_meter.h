/******************************************************************************/
/** @file		nl_cpu_meter.h
    @date		2015-11-08
    @brief    	CPU usage metering for LPC43xx
    @author		Nemanja Nikodijevic 2015-11-08
*******************************************************************************/
#ifndef _NL_CPU_METER_H_
#define _NL_CPU_METER_H_

#include <stdint.h>
#include "cmsis/LPC43xx.h"

void    CPU_Meter_Init(LPC_TIMERn_Type *TIMx, uint32_t tick_us);
void    CPU_Meter_Task(void);
void    CPU_Meter_Idle(void);
uint8_t CPU_Meter_Get_Percentage(void);
void    CPU_Meter_Update_Usage(void);
void    CPU_Meter_Set_Threshold_Led(uint8_t port, uint8_t pin, uint8_t th);

#endif
