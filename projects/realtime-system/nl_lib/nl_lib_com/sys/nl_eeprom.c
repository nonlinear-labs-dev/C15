/******************************************************************************/
/** @file		nl_eeprom.c
    @date		2020-03-17
    @version	1
    @author		KSTR
    @brief		eeprom routines
*******************************************************************************/
#include "sys/nl_eeprom.h"
#include "eeprom_18xx_43xx.h"
#include "sys/crc.h"
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

#define EEPROM_SIZE             (63 * 128)  // 63 blocks with 128 bytes each, do NOT change
#define EEPROM_MAX_BLOCKSIZE    (2048)      // 2k max size per block, for local buffer sizing,
#define NUMBER_OF_EEPROM_BLOCKS (16)        // number of blocks we can handle

#define ALIGN32(X) (((X) + 3) & ~3)  // macro to a align a size to next 4byte multiple

static EEPROM_Block blocks[NUMBER_OF_EEPROM_BLOCKS];        // main blocks
static EEPROM_Block shadowBlocks[NUMBER_OF_EEPROM_BLOCKS];  // shadow/backup blocks
static uint32_t     buffer[EEPROM_MAX_BLOCKSIZE / 4];
static uint16_t     eepromBusy = 0;
static uint16_t     transfer;
static uint16_t     remaining;
static uint32_t *   pDest;
static uint32_t *   pSrc;
static uint16_t     forceAlignNext = 0;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Register a block of data for EEPROM access */
/* returns a handle, or 0 in case of failure */
uint16_t NL_EEPROM_RegisterBlock(uint16_t const size, EepromBlockAlign_T align)
{
  if (size > EEPROM_MAX_BLOCKSIZE)
    return 0;
  uint16_t returnHandle   = 0;
  uint16_t nextFreeOffset = 0;
  for (uint16_t i = 0; i < NUMBER_OF_EEPROM_BLOCKS; i++)
  {
    if (returnHandle == 0 && blocks[i].handle == 0)
    {                            // first free block found, register it
      blocks[i].handle = i + 1;  // avoid 0 as a handle
      blocks[i].size   = size;
      if (align == EEPROM_BLOCK_ALIGN_TO_PAGE)
      {
        nextFreeOffset = (nextFreeOffset + 127) & ~127;  // bump offset to next page boundary
        forceAlignNext = 1;                              // remind to page-align next block also
      }
      else
      {                      // not aligned
        if (forceAlignNext)  // previous block requested alignment ?
        {
          nextFreeOffset = (nextFreeOffset + 127) & ~127;  // bump offset to next page boundary
          forceAlignNext = 0;                              // next block's alignment can be arbitrary again
        }
      }
      blocks[i].offset = nextFreeOffset;
      returnHandle     = blocks[i].handle;
    }
    // add up sizes (aligned to next 32bit boundary), include 4 bytes of CRC (regardless of size of crc_t)
    if (blocks[i].handle)
      nextFreeOffset = blocks[i].offset + ALIGN32(blocks[i].size) + 4;
    if (nextFreeOffset > EEPROM_SIZE)
    {  // too much to fit in EEPROM
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

/* simple mem copy, allows for non 32bit-aligned amounts */
static inline void memCopy(uint16_t size, uint32_t *pSrc, uint32_t *pDest)
{
  uint16_t count = size >> 2;  // number or 32bit words
  while (count--)
    *pDest++ = *pSrc++;
  count = size & 3;  // number of bytes left
  if (!count)
    return;
  uint8_t *pSrcB  = (uint8_t *) pSrc;
  uint8_t *pDestB = (uint8_t *) pDest;
  while (count--)
    *pDestB++ = *pSrcB++;
}

/* Read a block of data from EEPROM */
/* returns 1 if success, or 0 in case of failure */
uint16_t NL_EEPROM_ReadBlock(uint16_t const handle, void *const data)
{
  uint8_t index = handle - 1;
  if (handle < 1 || handle > NUMBER_OF_EEPROM_BLOCKS || blocks[index].handle != handle
      || data == NULL || eepromBusy)
    return 0;
  uint32_t *startAddr = (uint32_t *) (EEPROM_START + blocks[index].offset);
  if (crcFast((uint8_t const *) (startAddr + 1), blocks[index].size) != *startAddr)
    return 0;  // CRC failed
  memCopy(blocks[index].size, startAddr + 1, (uint32_t *) data);
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
  remaining += 4;  // account for CRC
  transfer           = remaining;
  uint8_t pageOffset = blocks[index].offset & 0x7F;  // offset modulo 128
  if (pageOffset + transfer > 128)
    transfer = 128 - pageOffset;  // restrict amount to next 128byte boundary
  // copy
  pSrc           = (uint32_t *) data;
  pDest          = (uint32_t *) (EEPROM_START + blocks[index].offset);
  *pDest++       = crcFast((uint8_t const *) data, blocks[index].size);  // poke CRC in first word
  uint16_t count = (transfer - 4) >> 2;                                  // words remaining data to transfer, without CRC
  while (count--)
    *pDest++ = *pSrc++;
  count = (transfer - 4) & 3;  // any bytes remaining, 0..3, can happen only at the end of data
  if (count)
  {  // we must use 32bit aligned write to write the EEPROM data
    uint32_t word  = 0;
    uint8_t *pByte = (uint8_t *) pSrc;
    while (count--)
    {  // fill up last word with the source data bytes
      word <<= 8;
      word |= *pByte++;
    }
    *pDest++ = word;
  }
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
    transfer = 128;                //restrict amount to next 128byte boundary
  uint16_t count = transfer >> 2;  // words remaining data to transfer
  while (count--)                  // 32bit words
    *pDest++ = *pSrc++;
  count = transfer & 3;  // any bytes remaining, 0..3, can happen only at the end of data
  if (count)
  {  // we must use 32bit aligned writes to write the EEPROM data
    uint32_t word  = 0;
    uint8_t *pByte = (uint8_t *) pSrc;
    while (count--)
    {  // fill up last word with the source data bytes
      word <<= 8;
      word |= *pByte++;
    }
    *pDest++ = word;
  }
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
#if 0  // test code
  NL_EEPROM_RegisterBlock(113, EEPROM_BLOCK_NO_ALIGN);
  NL_EEPROM_RegisterBlock(12,  EEPROM_BLOCK_ALIGN_TO_PAGE);
  NL_EEPROM_RegisterBlock(114, EEPROM_BLOCK_NO_ALIGN);
  NL_EEPROM_RegisterBlock(115, EEPROM_BLOCK_NO_ALIGN);
#endif
}
