/******************************************************************************/
/** @file		nl_eeprom.h
    @date		2020-03-17
    @version	0
    @author		KSTR
    @brief		eeprom routines
*******************************************************************************/
#pragma once

#include <stdint.h>

// use this to align the blocks of data you want to store in EEPROM
// example:  uint8_t NL_EEPROM_ALIGN myData[123];
#define NL_EEPROM_ALIGN __attribute__((aligned(4)))  // do not change, of course

typedef enum
{
  EEPROM_BLOCK_NO_ALIGN = 0,
  EEPROM_BLOCK_ALIGN_TO_PAGE
} EepromBlockAlign_T;

typedef enum
{
  EEPROM_READ_MAIN,
  EEPROM_READ_SHADOW,
  EEPROM_READ_BOTH
} EepromRead_T;

void NL_EEPROM_Init(void);  // initialize EEPROM for use

// size can be a non 4-byte multiple
// align set to EEPROM_BLOCK_ALIGN_TO_PAGE forced this and the next assigned blocks to start at a page
uint16_t NL_EEPROM_RegisterBlock(uint16_t const size, EepromBlockAlign_T align);

// data start adr must be 4-byte aligned
// returns 0 if error or busy
uint16_t NL_EEPROM_ReadBlock(uint16_t const handle, void *const data, EepromRead_T const type);

// returns 1 if there was a main vs. shadow mismatch at last read
// flag is cleared by this call, and force-set by ReadBlock()
uint16_t NL_EEPROM_ReadBlockHadToUseBackup(void);

// data start adr must be 4-byte aligned
// returns 0 if error or busy
uint16_t NL_EEPROM_StartWriteBlock(uint16_t const handle, void *const data);

// check this before any new read/write as it might be ignored if still busy
// if the block was registered with page-alignment, though, read is always save and cannot corrupt a pending write
uint16_t NL_EEPROM_Busy(void);

void NL_EEPROM_Process(void);            // cyclic task to perform the multi-page writes, or a full EEPROM erase
void NL_EEPROM_RequestFullErase(void);   // initiate a full EEPROM erase
int  NL_EEPROM_FullEraseFinished(void);  // flag finished full EEPROM erase
