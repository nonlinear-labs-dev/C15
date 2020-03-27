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
/* in 32bit word chunks */
int NL_EEPROM_Read(uint32_t const wordIndex, uint32_t *const ptr, uint32_t const count)
{
  if ((count == 0) || (wordIndex + count > 127 * 32))
    return 0;
  uint32_t *pEepromMem = (uint32_t *) EEPROM_ADDRESS(0, wordIndex * 4);
  for (uint32_t i = 0; i < count; i++)
    ptr[i] = pEepromMem[i];
  return 1;
}

/* Erase a page in EEPROM */
void NL_EEPROM_EraseBlocking(uint32_t const pageAddr)
{
  uint32_t *pEepromMem = (uint32_t *) EEPROM_ADDRESS(pageAddr, 0);
  for (uint32_t i = 0; i < EEPROM_PAGE_SIZE / 4; i++)
    pEepromMem[i] = 0;
  Chip_EEPROM_EraseAndProgramPage(LPC_EEPROM);
}

/* Write data to a page in EEPROM */
/* in 32bit word chunks */
int NL_EEPROM_WriteBlocking(uint32_t const wordIndex, uint32_t *ptr, uint32_t count)
{
  if ((count == 0) || (wordIndex + count > 127 * 32))
    return 0;
  uint32_t *pEepromMem = (uint32_t *) EEPROM_ADDRESS(0, wordIndex * 4);

  while (count--)
  {
    *pEepromMem++ = *ptr++;
    if (((pEepromMem - (uint32_t *) EEPROM_START) & 0x1F) == 0)
      Chip_EEPROM_EraseAndProgramPage(LPC_EEPROM);
  }
  if (((pEepromMem - (uint32_t *) EEPROM_START) & 0x1F) != 0)
    Chip_EEPROM_EraseAndProgramPage(LPC_EEPROM);
  return 1;
}

static int       step    = 0;
static uint32_t  wcount  = 0;
static uint32_t *pMem    = NULL;
static uint32_t *pEeprom = NULL;

int NL_EEPROM_StartWrite(uint32_t const wordIndex, uint32_t const *const ptr, uint32_t const count)
{
  if ((step != 0) || (count == 0) || (wordIndex + count > 127 * 32))
    return 0;
  pEeprom = (uint32_t *) EEPROM_ADDRESS(0, wordIndex * 4);
  pMem    = (uint32_t *) ptr;
  wcount  = count;
  step    = 1;
  return 1;
}

void NL_EEPROM_Process()
{
  if (step == 0)
    return;
  switch (step)
  {
    case 1:
      while (wcount--)
      {
        *pEeprom++ = *pMem++;
        if (((pEeprom - (uint32_t *) EEPROM_START) & 0x1F) == 0)
        {  // page is full
          Chip_EEPROM_StartEraseAndProgramPage(LPC_EEPROM);
          if (wcount)  // data left ?
            step = 2;  // then wait for write finished and continue
          else
            step = 3;  // then wait for write finished and exit
          return;
        }
      }
      if (((pEeprom - (uint32_t *) EEPROM_START) & 0x1F) != 0)
      {  // fractional page left
        Chip_EEPROM_StartEraseAndProgramPage(LPC_EEPROM);
        step = 3;  // then wait for write finished and exit
        return;
      }
      step = 0;
      return;
    case 2:  // wait for write finished and continue
      if (Chip_EEPROM_PollIntStatus(LPC_EEPROM))
        step = 1;
      return;
    case 3:  // then wait for write finished and exit
      if (Chip_EEPROM_PollIntStatus(LPC_EEPROM))
        step = 0;
      return;
  }
}

/* Initialize eeprom access */
void NL_EEPROM_Init(void)
{
  Chip_EEPROM_Init(LPC_EEPROM);
  Chip_EEPROM_SetAutoProg(LPC_EEPROM, EEPROM_AUTOPROG_OFF);

#if 0  // test code
  uint32_t buffer[70];

  NL_EEPROM_Read(10, buffer, 70);

  for (int i = 0; i < 70; i++)
	buffer[i] = i;
  NL_EEPROM_WriteBlocking(10, buffer, 70);

  for (int i = 0; i < 70; i++)
	buffer[i] = 0;

  NL_EEPROM_Read(10, buffer, 70);
#endif
}
