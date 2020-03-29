/******************************************************************************/
/** @file		nl_eeprom.h
    @date		2020-03-17
    @version	0
    @author		KSTR
    @brief		eeprom routines
*******************************************************************************/
#ifndef NL_SYS_EEPROM_H_
#define NL_SYS_EEPROM_H_

#include <stdint.h>

void NL_EEPROM_Init(void);

uint16_t NL_EEPROM_RegisterBlock(uint16_t const size);
uint16_t NL_EEPROM_ReadBlock(uint16_t const handle, void *const data);
uint16_t NL_EEPROM_StartWriteBlock(uint16_t const handle, void *const data);
uint16_t NL_EEPROM_Busy(void);
void     NL_EEPROM_Process();

#endif
