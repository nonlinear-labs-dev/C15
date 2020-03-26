/******************************************************************************/
/** @file		nl_eeprom.c
    @date		2020-03-17
    @version	0
    @author		KSTR
    @brief		eeprom routines
*******************************************************************************/
#include "sys/nl_eeprom.h"
#include "eeprom_18xx_43xx.h"
#include <string.h>
#include <stdio.h>

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Page used for storage */
#define PAGE_ADDR 0x01 /* Page number */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Read data from EEPROM */
/* size must be multiple of 4 bytes */
static void EEPROM_Read(uint32_t pageOffset, uint32_t pageAddr, uint32_t *ptr, uint32_t size)
{
  uint32_t  i          = 0;
  uint32_t *pEepromMem = (uint32_t *) EEPROM_ADDRESS(pageAddr, pageOffset);
  for (i = 0; i < size / 4; i++)
  {
    ptr[i] = pEepromMem[i];
  }
}

/* Erase a page in EEPROM */
static void EEPROM_Erase(uint32_t pageAddr)
{
  uint32_t  i          = 0;
  uint32_t *pEepromMem = (uint32_t *) EEPROM_ADDRESS(pageAddr, 0);
  for (i = 0; i < EEPROM_PAGE_SIZE / 4; i++)
  {
    pEepromMem[i] = 0;
  }
  Chip_EEPROM_EraseAndProgramPage(LPC_EEPROM);
}

/* Write data to a page in EEPROM */
/* size must be multiple of 4 bytes */
static void EEPROM_Write(uint32_t pageOffset, uint32_t pageAddr, uint32_t *ptr, uint32_t size)
{
  uint32_t  i          = 0;
  uint32_t *pEepromMem = (uint32_t *) EEPROM_ADDRESS(pageAddr, pageOffset);

  if (size > EEPROM_PAGE_SIZE - pageOffset)
    size = EEPROM_PAGE_SIZE - pageOffset;

  for (i = 0; i < size / 4; i++)
  {
    pEepromMem[i] = ptr[i];
  }
  Chip_EEPROM_EraseAndProgramPage(LPC_EEPROM);
}

/* Initialize eeprom access */
void NL_EEPROM_Init(void)
{
  Chip_EEPROM_Init(LPC_EEPROM);
  Chip_EEPROM_SetAutoProg(LPC_EEPROM, EEPROM_AUTOPROG_OFF);

#if 0  // test code
  uint32_t buffer[EEPROM_PAGE_SIZE / sizeof(uint32_t)];
  EEPROM_Read(0, PAGE_ADDR, buffer, EEPROM_PAGE_SIZE);
  EEPROM_Erase(PAGE_ADDR);

  buffer[0] = 0x12345678;
  buffer[1] = 0x55AAA55A;
  EEPROM_Write(0, PAGE_ADDR, buffer, 128);

  buffer[0] = 0;
  buffer[1] = 0;
  EEPROM_Read(0, PAGE_ADDR, buffer, EEPROM_PAGE_SIZE);
#endif
}
