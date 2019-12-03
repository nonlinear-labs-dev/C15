/******************************************************************************/
/** @file		nl_dbg.h
    @date		2013-03-08
    @version	0.01
    @author		Daniel Tzschentke [2012--]
    @brief		This module is a debug interface
 	 	 	 	- LEDs
 	 	 	 	- buttons
 	 	 	 	- logic-analyzer IOs
    @ingroup	e2_mainboard
*******************************************************************************/
#ifndef NL_DBG_H_
#define NL_DBG_H_

#include <stdint.h>
#include "cmsis/LPC43xx.h"
#include "drv/nl_pin.h"

/** note: check emphase_v5.c for exact led names */
typedef struct
{
  GPIO_NAME_T* pod[8];
  GPIO_NAME_T* led_cpu;
  GPIO_NAME_T* led_audio;
  GPIO_NAME_T* led_error;
  GPIO_NAME_T* led_warning;
  GPIO_NAME_T* led_heartbeat;
} DBG_PINS_T;

#if 0
extern void (* const DBG_Pod_On[8])(void);
extern void (* const DBG_Pod_Off[8])(void);
#endif

void DBG_Init(void);
void DBG_Config(LPC_USARTn_Type* UARTx, uint32_t speed, DBG_PINS_T* dbgpins);

uint32_t DBG_Write(char* str);

void DBG_Process(void);
void DBG_Led(GPIO_NAME_T* led, uint8_t state);
void DBG_Led_Error_On(void);
void DBG_Led_Warning_On(void);
void DBG_Led_Cpu_On(void);
void DBG_Led_Audio_On(void);

void DBG_Led_Error_Off(void);
void DBG_Led_Warning_Off(void);
void DBG_Led_Cpu_Off(void);
void DBG_Led_Audio_Off(void);

#if 0
void DBG_Pod_0_On(void);
void DBG_Pod_1_On(void);
void DBG_Pod_2_On(void);
void DBG_Pod_3_On(void);
void DBG_Pod_4_On(void);
void DBG_Pod_5_On(void);
void DBG_Pod_6_On(void);
void DBG_Pod_7_On(void);

void DBG_Pod_0_Off(void);
void DBG_Pod_1_Off(void);
void DBG_Pod_2_Off(void);
void DBG_Pod_3_Off(void);
void DBG_Pod_4_Off(void);
void DBG_Pod_5_Off(void);
void DBG_Pod_6_Off(void);
void DBG_Pod_7_Off(void);
#endif

/** emphase v5 functions and wrappers */
void DBG_Process_M4(void);
void DBG_Process_M0(void);

void DBG_Led_Task_Overflow_On(void);
void DBG_Led_Task_Overflow_Off(void);

#endif /* NL_DBG_H_ */
