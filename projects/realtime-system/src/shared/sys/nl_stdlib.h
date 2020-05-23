/******************************************************************************/
/** @file		nl_stdlib.h
    @date		2020-05-07
    @version	0
    @author		KSTR
    @brief		some common functions
*******************************************************************************/
#pragma once

#include <stdint.h>
#include <stddef.h>

static inline void *memset(void *const pMem, uint8_t const c, size_t n)
{
  uint8_t *p = (uint8_t *) pMem;
  while (n--)
    *(p++) = c;
  return pMem;
}

static inline void *memcpy(void *dest, void *src, size_t n)
{
  uint8_t *p = (uint8_t *) dest;
  while (n--)
    *(p++) = *((uint8_t *) src++);
  return dest;
}
