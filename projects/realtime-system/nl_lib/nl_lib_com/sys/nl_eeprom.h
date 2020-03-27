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
int  NL_EEPROM_Read(uint32_t const wordIndex, uint32_t *const ptr, uint32_t const count);
void NL_EEPROM_EraseBlocking(uint32_t const pageAddr);
int  NL_EEPROM_WriteBlocking(uint32_t const wordIndex, uint32_t *ptr, uint32_t count);
int  NL_EEPROM_StartWrite(uint32_t const wordIndex, uint32_t const *const ptr, uint32_t const count);
void NL_EEPROM_Process();

#endif
