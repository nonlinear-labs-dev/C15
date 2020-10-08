/**********************************************************************
 * Filename:    crc.h
 * Description: A header file describing the various CRC standards.
 * Notes:       
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/
#pragma once

#include <stdint.h>

typedef uint32_t crc_t;

void  crcInit(void);
crc_t crcFast(const void* src, uint16_t const nBytes);
crc_t crcFastAndCopy(const void* src, void* dest, uint16_t const nBytes);
