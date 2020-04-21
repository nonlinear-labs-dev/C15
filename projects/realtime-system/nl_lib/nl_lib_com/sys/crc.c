/**********************************************************************
 * Filename:    crc.c
 * Description: Slow and fast implementations of the CRC standards.
 * Notes:       The parameters for each supported CRC standard are
 *				defined in the header file crc.h.  The implementations
 *				here should stand up to further additions to that list.
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/
#include "crc.h"

#define POLYNOMIAL        0x04C11DB7
#define INITIAL_REMAINDER 0xFFFFFFFF
#define FINAL_XOR_VALUE   0xFFFFFFFF
#define WIDTH             (8 * sizeof(crc_t))
#define TOPBIT            (1 << (WIDTH - 1))

/*********************************************************************
 * Function:    reflect*()
 * Description: Reorder the bits of a binary sequence, by reflecting
 *				them about the middle position.
 * Returns:		The reflection of the original data.
 *********************************************************************/
static inline uint32_t reflect32(uint32_t value)
{
#ifdef CORE_M4
  asm("rbit %0, %0"
      : "=r"(value)
      : "0"(value));
#endif
  return value;
}

static inline uint8_t reflect8(uint8_t value)
{
#ifdef CORE_M4
  asm("mov %0, %0, lsl #12"
      : "=r"(value)
      : "0"(value));
  asm("rbit %0, %0"
      : "=r"(value)
      : "0"(value));
  asm("mov %0, %0, lsr #12"
      : "=r"(value)
      : "0"(value));
#endif
  return value;
}

static crc_t crcTable[256];

/*********************************************************************
 * Function:    crcInit()
 * Description: Populate the partial CRC lookup table.
 * Notes:		This function must be rerun any time the CRC standard
 *				is changed.  If desired, it can be run "offline" and
 *				the table results stored in an embedded system's ROM.
 * Returns:		None defined.
 *********************************************************************/
void crcInit(void)
{
  crc_t    remainder;
  uint16_t dividend;
  uint8_t  bit;
  // Compute the remainder of each possible dividend.
  for (dividend = 0; dividend < 256; ++dividend)
  {
    // Start with the dividend followed by zeros.
    remainder = dividend << (WIDTH - 8);
    // Perform modulo-2 division, a bit at a time.
    for (bit = 8; bit > 0; --bit)
    {
      // Try to divide the current data bit.
      if (remainder & TOPBIT)
        remainder = (remainder << 1) ^ POLYNOMIAL;
      else
        remainder = (remainder << 1);
    }
    // Store the result into the table.
    crcTable[dividend] = remainder;
  }
}

/*********************************************************************
 * Function:    crcFast()
 * Description: Compute the CRC of a given message.
 * Notes:		crcInit() must be called first.
 * Returns:		The CRC of the message.
 *********************************************************************/
crc_t crcFast(const void *src, uint16_t const nBytes)
{
  crc_t    remainder = INITIAL_REMAINDER;
  uint8_t  data;
  uint8_t *s = (uint8_t *) src;
  uint16_t byte;
  // Divide the message by the polynomial, a byte at a time.
  for (byte = 0; byte < nBytes; ++byte)
  {
    data      = reflect8(*s++) ^ (remainder >> (WIDTH - 8));
    remainder = crcTable[data] ^ (remainder << 8);
  }
  // The final remainder is the CRC.
  return reflect32(remainder) ^ FINAL_XOR_VALUE;
}

crc_t crcFastAndCopy(const void *src, void *dest, uint16_t const nBytes)
{
  crc_t    remainder = INITIAL_REMAINDER;
  uint8_t  data;
  uint8_t *s = (uint8_t *) src;
  uint8_t *d = (uint8_t *) dest;
  uint16_t byte;
  // Divide the message by the polynomial, a byte at a time.
  for (byte = 0; byte < nBytes; ++byte)
  {
    data      = reflect8(*d++ = *s++) ^ (remainder >> (WIDTH - 8));
    remainder = crcTable[data] ^ (remainder << 8);
  }
  // The final remainder is the CRC.
  return reflect32(remainder) ^ FINAL_XOR_VALUE;
}
