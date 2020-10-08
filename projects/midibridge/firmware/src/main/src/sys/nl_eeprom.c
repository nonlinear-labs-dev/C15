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
#include "playcontroller/playcontroller-defs.h"
#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-function"

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
#define EEPROM_MAX_BLOCKSIZE    (2048)      // 2k
#define NUMBER_OF_EEPROM_BLOCKS (8)         // number of blocks we can handle

static EEPROM_Block             blocks[NUMBER_OF_EEPROM_BLOCKS];
static uint32_t NL_EEPROM_ALIGN buffer[EEPROM_MAX_BLOCKSIZE / 4];
static uint8_t *                target;
static uint16_t                 total;
static uint16_t                 eepromBusy = 0;
static uint16_t                 transfer;
static uint16_t                 remaining;
static volatile uint32_t *      pDest;  // EEPROM page buffer write pointer, volatile because of side effects
static uint32_t *               pSrc;
static uint16_t                 forceAlignNext    = 0;
static uint16_t                 step              = 0;
static uint16_t                 hadToUseBackup    = 0;
static uint16_t                 fullEraseRequest  = 0;
static uint16_t                 fullEraseFinished = 0;

static inline uint16_t align32(uint16_t size)
{
  return (size + 3) & ~3;
}

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
      nextFreeOffset = blocks[i].offset + align32(blocks[i].size) + 4;
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

static int copyAndCRC(void *src, void *dest, uint16_t size, crc_t crc)
{
  memCopy(size, src, dest);
  return (crcFast(dest, size) == crc);
}

/* Read a block of data from EEPROM */
/* returns 1 if success, or 0 in case of failure */
uint16_t NL_EEPROM_ReadBlock(uint16_t const handle, void *const data, EepromRead_T const type)
{
  hadToUseBackup = 0;
  uint8_t index  = handle - 1;
  if (handle < 1 || handle > NUMBER_OF_EEPROM_BLOCKS || blocks[index].handle != handle || data == NULL || fullEraseRequest || NL_EEPROM_Busy())
    return 0;
  uint32_t *startAddrMain   = (uint32_t *) (EEPROM_START + blocks[index].offset);
  uint32_t *startAddrShadow = (uint32_t *) (EEPROM_START + blocks[index].offset + SHADOW_OFFSET);
  uint16_t  size            = blocks[index].size;

  if (type == EEPROM_READ_MAIN)
    return copyAndCRC(startAddrMain + 1, data, size, *startAddrMain);

  else if (type == EEPROM_READ_SHADOW)
    return copyAndCRC(startAddrShadow + 1, data, size, *startAddrShadow);

  else
  {
    if (!copyAndCRC(startAddrMain + 1, data, size, *startAddrMain))
      return (hadToUseBackup = copyAndCRC(startAddrShadow + 1, data, size, *startAddrShadow));
    return 1;
  }
}

/* get any mismatch in last ReadBlock() */
uint16_t NL_EEPROM_ReadBlockHadToUseBackup(void)
{
  uint16_t ret   = hadToUseBackup;
  hadToUseBackup = 0;
  return ret;
}

/* Start write a block of data to EEPROM */
static void StartWriteBlock(uint16_t offset)
{
  pSrc               = buffer;
  pDest              = (uint32_t *) (target + offset);
  remaining          = total;
  transfer           = remaining;
  uint8_t pageOffset = (uint32_t) pDest & 0x7F;  // offset modulo 128
  if (pageOffset + transfer > 128)
    transfer = 128 - pageOffset;  // restrict amount to next 128byte boundary
  // copy to page register
  uint16_t count = transfer >> 2;  // words of data to transfer to page register
  while (count--)                  // writing to the EEPROM page register must use 32bit aligned writes, ...
    *pDest++ = *pSrc++;            // ... we can't use memcpy etc which are byte-wise
  remaining -= transfer;           // update remaining amount of date, if any
  // start burning
  eepromBusy = 1;                                    // start multi-page write, if required
  Chip_EEPROM_StartEraseAndProgramPage(LPC_EEPROM);  // start burning the first chunk
}

/* multi-page write */
static void Process()
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
  while (count--)                  // writing to the EEPROM page register must use 32bit aligned writes, ...
    *pDest++ = *pSrc++;            // ... we can't use memcpy etc which are byte-wise
  remaining -= transfer;
  eepromBusy = 1;
  Chip_EEPROM_StartEraseAndProgramPage(LPC_EEPROM);
}

/* Start write a block of data to EEPROM */
/* returns 1 if success, or 0 in case of failure or busy */
uint16_t NL_EEPROM_StartWriteBlock(uint16_t const handle, void *const data)
{
  uint16_t index = handle - 1;
  total          = align32(blocks[index].size);
  if (handle < 1 || handle > NUMBER_OF_EEPROM_BLOCKS || blocks[index].handle != handle || fullEraseRequest)
    return 0;
  if (total == 0)
    return 1;  // no data --> done
  if (data == NULL || NL_EEPROM_Busy())
    return 0;

  // copy user data to local buffer and calculate CRC
  // we make sure the data starts and ends word aligned, page register write requires this
  buffer[total / 4] = 0;                                                     // fill last word with zero, because CRC may copy non 4-byte multiple
  buffer[0]         = crcFastAndCopy(data, &buffer[1], blocks[index].size);  // get CRC and copy all bytes
  total += 4;                                                                // account for CRC data

  target = (uint8_t *) (EEPROM_START + blocks[index].offset);
  // start write to shadow, with CRC and local buffering
  StartWriteBlock(SHADOW_OFFSET);
  step = 1;  // start step chain
  return 1;
}

/* process full erase cycle */
static void FullEraseProcess(void)
{
  static uint16_t    step     = 0;
  static uint16_t    pageAddr = 0;
  volatile uint32_t *pEepromMem;

  switch (step)
  {
    case 0:
      pageAddr = 0;
      step     = 1;
    case 1:
      pEepromMem = (volatile uint32_t *) EEPROM_ADDRESS(pageAddr, 0);
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
    BB_MSG_WriteMessage2Arg(PLAYCONTROLLER_BB_MSG_TYPE_NOTIFICATION, PLAYCONTROLLER_NOTIFICATION_ID_CLEAR_EEPROM, 1);
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
      // now burn main block
      StartWriteBlock(0);
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
