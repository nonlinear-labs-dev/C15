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
#include "spibb/nl_bb_msg.h"
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
#define SHADOW_OFFSET           (64 * 128)  // offset to shadow pages
#define EEPROM_MAX_BLOCKSIZE    (2048)      // 2k max size per block, for local buffer sizing,
#define NUMBER_OF_EEPROM_BLOCKS (8)         // number of blocks we can handle

#define ALIGN32(X) (((X) + 3) & ~3)  // macro to a align a size to next 4byte multiple

static EEPROM_Block blocks[NUMBER_OF_EEPROM_BLOCKS];
static uint32_t     buffer[EEPROM_MAX_BLOCKSIZE / 4];
static uint16_t     eepromBusy = 0;
static uint16_t     transfer;
static uint16_t     remaining;
static uint32_t *   pDest;
static uint32_t *   pSrc;
static uint16_t     forceAlignNext    = 0;
static uint16_t     step              = 0;
static uint16_t     savedIndex        = 0;
static uint16_t     hadToUseBackup    = 0;
static uint16_t     fullEraseRequest  = 0;
static uint16_t     fullEraseFinished = 0;
/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Register a block of data for EEPROM access */
/* returns a handle, or 0 in case of failure (too large, or no more blocks) */
uint16_t NL_EEPROM_RegisterBlock(uint16_t const size, EepromBlockAlign_T align)
{
  if (size > EEPROM_MAX_BLOCKSIZE)
    return 0;
  uint16_t returnHandle   = 0;
  uint16_t nextFreeOffset = 0;
  for (uint16_t i = 0; i < NUMBER_OF_EEPROM_BLOCKS; i++)
  {
    if (returnHandle == 0 && blocks[i].handle == 0)
    {  // first free block found, register it
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
      blocks[i].handle = i + 1;  // avoid 0 as a handle
      blocks[i].size   = size;
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
  hadToUseBackup = 0;
  uint8_t index  = handle - 1;
  if (handle < 1 || handle > NUMBER_OF_EEPROM_BLOCKS || blocks[index].handle != handle || data == NULL || fullEraseRequest || NL_EEPROM_Busy())
    return 0;
  uint32_t *startAddr = (uint32_t *) (EEPROM_START + blocks[index].offset);
  if (crcFast((uint8_t const *) (startAddr + 1), blocks[index].size) != *startAddr)
  {  // CRC failed on main block, so try shadow block (which was written before main block)
    startAddr = (uint32_t *) (EEPROM_START + blocks[index].offset + SHADOW_OFFSET);
    if (crcFast((uint8_t const *) (startAddr + 1), blocks[index].size) != *startAddr)
      return 0;          // CRC failed on both, no valid data
    hadToUseBackup = 1;  // mark mismatch
  }
  memCopy(blocks[index].size, startAddr + 1, (uint32_t *) data);
  return 1;
}

/* get any mismatch in last ReadBlock() */
uint16_t NL_EEPROM_ReadBlockHadToUseBackup(void)
{
  uint16_t ret   = hadToUseBackup;
  hadToUseBackup = 0;
  return ret;
}

/* Start write a block of data to EEPROM */
static void StartWriteBlock(uint16_t size, uint32_t *src, uint16_t offset, int raw)
{
  remaining = ALIGN32(size);
  if (!raw)
    remaining += 4;  // account for CRC
  transfer           = remaining;
  uint8_t pageOffset = offset & 0x7F;  // offset modulo 128
  if (pageOffset + transfer > 128)
    transfer = 128 - pageOffset;  // restrict amount to next 128byte boundary
  // copy
  pSrc  = src;
  pDest = (uint32_t *) (EEPROM_START + offset);
  if (!raw)
    *pDest++ = crcFast((uint8_t const *) src, size);  // poke CRC in first word
  uint16_t count = (transfer - (raw ? 0 : 4)) >> 2;   // words remaining data to transfer, without CRC
  while (count--)
    *pDest++ = *pSrc++;
  count = transfer & 3;  // any bytes remaining, 0..3, can happen only at the end of data
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
  eepromBusy = 1;                                    // start multi-page write, if required
  Chip_EEPROM_StartEraseAndProgramPage(LPC_EEPROM);  // start burning the first chunk
  if (!raw)
  {
    memCopy(remaining, pSrc, buffer);  // copy rest of data to local buffer, if any
    pSrc = buffer;                     // adjust to new read address
  }
}

/* multi-page write */
void Process()
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
    transfer = 128;                // restrict amount to next 128byte boundary
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
  eepromBusy = 1;
  Chip_EEPROM_StartEraseAndProgramPage(LPC_EEPROM);
}

/* Start write a block of data to EEPROM */
/* returns 1 if success, or 0 in case of failure or busy */
uint16_t NL_EEPROM_StartWriteBlock(uint16_t const handle, void *const data)
{
  uint16_t index = handle - 1;
  uint8_t  total = ALIGN32(blocks[index].size);
  if (handle < 1 || handle > NUMBER_OF_EEPROM_BLOCKS || blocks[index].handle != handle || fullEraseRequest)
    return 0;
  if (total == 0)
    return 1;  // no data --> done
  if (data == NULL || NL_EEPROM_Busy())
    return 0;
  savedIndex = index;  // keep index for main write later
  // start write to shadow, with CRC and local buffering
  StartWriteBlock(total, (uint32_t *) data, blocks[savedIndex].offset + SHADOW_OFFSET, 0);
  step = 1;  // start step chain
  return 1;
}

/* process full erase cycle */
void FullEraseProcess(void)
{
  static uint16_t step     = 0;
  static uint16_t pageAddr = 0;
  uint32_t *      pEepromMem;

  switch (step)
  {
    case 0:
      pageAddr = 0;
      step     = 1;
    case 1:
      pEepromMem = (uint32_t *) EEPROM_ADDRESS(pageAddr, 0);
      for (uint32_t i = 0; i < EEPROM_PAGE_SIZE / 4; i++)
        pEepromMem[i] = 0;
      Chip_EEPROM_StartEraseAndProgramPage(LPC_EEPROM);
      step = 2;
    case 2:
      if (!Chip_EEPROM_PollIntStatus(LPC_EEPROM))
        return;  // still burning
      if (++pageAddr <= 126)
        step = 1;
      else
      {
        fullEraseRequest  = 0;
        fullEraseFinished = 1;
        step              = 0;
      }
  }
}

/* multi-page write */
void NL_EEPROM_Process(void)
{
  if (fullEraseRequest && step == 0)
  {
    FullEraseProcess();
    step = 0;
  }
  if (NL_EEPROM_FullEraseFinished())
  {
    BB_MSG_WriteMessage2Arg(BB_MSG_TYPE_NOTIFICATION, NOTIFICATION_ID_CLEAR_EEPROM, 1);
    BB_MSG_SendTheBuffer();
  }
  if (step == 0)
    return;
  switch (step)
  {
    case 1:  // wait for shadow write finished
      Process();
      if (eepromBusy)
        return;  // still writing
      // now copy shadow block to main block, no CRC and no buffering required
      StartWriteBlock(ALIGN32(blocks[savedIndex].size),
                      (uint32_t *) (EEPROM_START + blocks[savedIndex].offset + SHADOW_OFFSET),
                      blocks[savedIndex].offset, 1);  // start write to main, no buffering
      step = 2;
      return;
    case 2:  // wait for main write finished
      Process();
      if (eepromBusy)
        return;  // still writing
      step = 0;  // job done, reset chain
      return;
  }
}

/* Check EEPROM is busy (write not finished) */
/* returns != 0 if busy, 0 if not busy (or failure */
uint16_t NL_EEPROM_Busy(void)
{
  return step != 0 || eepromBusy;
}

/* Erase complete EEPROM */
static void FullEraseBlocking(void)
{
  for (uint32_t pageAddr = 0; pageAddr < 126; pageAddr++)
  {
    uint32_t *pEepromMem = (uint32_t *) EEPROM_ADDRESS(pageAddr, 0);
    for (uint32_t i = 0; i < EEPROM_PAGE_SIZE / 4; i++)
      pEepromMem[i] = 0;
    Chip_EEPROM_EraseAndProgramPage(LPC_EEPROM);
  }
}

/* start a full EEPROM erase */
void NL_EEPROM_RequestFullErase(void)
{
  if (!fullEraseRequest)
  {
    fullEraseRequest  = 1;
    fullEraseFinished = 0;
  }
}

/* return 1 if full erase has finished */
int NL_EEPROM_FullEraseFinished(void)
{
  if (!fullEraseFinished)
    return 0;
  fullEraseFinished = 0;
  return 1;
}

/* Initialize EEPROM access */
void NL_EEPROM_Init(void)
{
  Chip_EEPROM_Init(LPC_EEPROM);
  Chip_EEPROM_SetAutoProg(LPC_EEPROM, EEPROM_AUTOPROG_OFF);

  //  FullEraseBlocking();

#if 0  // test code
  NL_EEPROM_RegisterBlock(113, EEPROM_BLOCK_NO_ALIGN);
  NL_EEPROM_RegisterBlock(12,  EEPROM_BLOCK_ALIGN_TO_PAGE);
  NL_EEPROM_RegisterBlock(114, EEPROM_BLOCK_NO_ALIGN);
  NL_EEPROM_RegisterBlock(115, EEPROM_BLOCK_NO_ALIGN);
#endif
}
