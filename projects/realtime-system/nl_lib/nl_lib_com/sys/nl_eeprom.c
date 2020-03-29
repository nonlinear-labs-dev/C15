/******************************************************************************/
/** @file		nl_eeprom.c
    @date		2020-03-17
    @version	1
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
typedef struct __attribute__((packed))
{
  uint16_t handle;  // handle for access (0:free, >0 used)
  uint16_t offset;  // byte offset within the 16k address space
  uint16_t size;    // number of bytes in block
} EEPROM_Block;

#define NUMBER_OF_EEPROM_BLOCKS (16)         // number of blocks we can handle
#define EEPROM_SIZE             (127 * 128)  // 127 blocks with 128 bytes each
#define EEPROM_MAX_BLOCKSIZE    (2048)       // 2k max size per block, for local buffer

#define ALIGN32(X) (((X) + 3) & ~3)  // align to next 4byte boundary

static EEPROM_Block blocks[NUMBER_OF_EEPROM_BLOCKS];
static uint32_t     buffer[EEPROM_MAX_BLOCKSIZE / 4];
static uint16_t     eepromBusy = 0;
static uint16_t     transfer;
static uint16_t     remaining;
static uint32_t *   pDest;
static uint32_t *   pSrc;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Register a block of data for EEPROM access */
/* returns a handle, or 0 in case of failure */
uint16_t NL_EEPROM_RegisterBlock(uint16_t const size)
{
  if (size > EEPROM_MAX_BLOCKSIZE)
    return 0;
  uint16_t returnHandle = 0;
  uint16_t endOfData    = 0;
  for (uint16_t i = 0; i < NUMBER_OF_EEPROM_BLOCKS; i++)
  {
    if (returnHandle == 0 && blocks[i].handle == 0)
    {                            // first free block found, register it
      blocks[i].handle = i + 1;  // avoid 0 as a handle
      blocks[i].size   = size;
      blocks[i].offset = endOfData;
      returnHandle     = blocks[i].handle;
    }
    endOfData += ALIGN32(blocks[i].size);  // add up sizes (aligned to next 32bit boundary)
    if (endOfData > EEPROM_SIZE)           // to much to fit in EEPROM ?
    {
      if (returnHandle)
      {  // undo registering
        blocks[returnHandle - 1].handle = 0;
        blocks[returnHandle - 1].offset = 0;
        blocks[returnHandle - 1].size   = 0;
      }
      return 0;
    }
  }
  return returnHandle;
}

/* simple word-wise mem copy */
static inline void memCopy(uint16_t size, uint32_t *pSrc, uint32_t *pDest)
{
  size /= 4;
  while (size--)
    *pDest++ = *pSrc++;
}

/* Read a block of data from EEPROM */
/* returns 1 if success, or 0 in case of failure */
uint16_t NL_EEPROM_ReadBlock(uint16_t const handle, void *const data)
{
  uint8_t index = handle - 1;
  if (handle < 1 || handle > NUMBER_OF_EEPROM_BLOCKS || blocks[index].handle != handle
      || data == NULL || eepromBusy)
    return 0;
  memCopy(blocks[index].size, (uint32_t *) (EEPROM_START + blocks[index].offset), (uint32_t *) data);
  return 1;
}

/* Start write a block of data to EEPROM */
/* returns 1 if success, or 0 in case of failure or busy */
uint16_t NL_EEPROM_StartWriteBlock(uint16_t const handle, void *const data)
{
  uint8_t index = handle - 1;
  remaining     = ALIGN32(blocks[index].size);
  if (handle < 1 || handle > NUMBER_OF_EEPROM_BLOCKS || blocks[index].handle != handle)
    return 0;
  if (remaining == 0)
    return 1;  // no data --> done
  if (data == NULL || eepromBusy)
    return 0;
  transfer           = remaining;
  uint8_t pageOffset = blocks[index].offset & 0x7F;  // offset modulo 128
  if (pageOffset + transfer > 128)
    transfer = 128 - pageOffset;  //restrict amount to next 128byte boundary
  // copy
  pDest          = (uint32_t *) (EEPROM_START + blocks[index].offset);
  pSrc           = (uint32_t *) data;
  uint16_t count = transfer / 4;
  while (count--)
    *pDest++ = *pSrc++;
  remaining -= transfer;                             // update remaining amount of date, if any
  Chip_EEPROM_StartEraseAndProgramPage(LPC_EEPROM);  // start burning the first chunk
  memCopy(remaining, pSrc, buffer);                  // copy rest of data to local buffer, if any
  pSrc       = buffer;                               // adjust to new read address
  eepromBusy = 1;                                    // start multi-page write
  return 1;
}

/* multi-page write */
void NL_EEPROM_Process()
{
  if (!eepromBusy)
    return;  // nothing to do
  if (!Chip_EEPROM_PollIntStatus(LPC_EEPROM))
    return;  // still burning
  eepromBusy = 0;
  if (remaining == 0)  // nothing left to burn --> job done
    return;
  transfer = remaining;
  if (transfer > 128)
    transfer = 128;  //restrict amount to next 128byte boundary
  uint16_t count = transfer / 4;
  while (count--)  // 32bit words
    *pDest++ = *pSrc++;
  remaining -= transfer;
  Chip_EEPROM_StartEraseAndProgramPage(LPC_EEPROM);
  eepromBusy = 1;
}

/* Check EEPROM is busy (write not finished) */
/* returns != 0 if busy, 0 if not busy (or failure */
uint16_t NL_EEPROM_Busy(void)
{
  return eepromBusy;
}

/* Initialize EEPROM access */
void NL_EEPROM_Init(void)
{
  Chip_EEPROM_Init(LPC_EEPROM);
  Chip_EEPROM_SetAutoProg(LPC_EEPROM, EEPROM_AUTOPROG_OFF);
}
