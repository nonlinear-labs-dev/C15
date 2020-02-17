/******************************************************************************/
/** @file		ln_ringbuffers.c
    @date		2020-02-15
    @version	0
    @author		KSTR
    @brief		a set of ringbuffers
    @ingroup	nl_tcd_modules
*******************************************************************************/


#include "ln_ringbuffers.h"

// =============
// ============= local constants and types
// =============

#define RBUF_COUNT = 20;  // 20 ringbuffers
#define RBUF_SIZE  = 16;  // containing 16 values each


uint16_t      rbIndex;
uint16_t      valReadIndex;
uint16_t      valWriteIndex;

uint32_t      sum[RBUF_fCOUNT];
uint16_t      values[RBUF_SIZE][RBUF_COUNT];

/*************************************************************************/ /**
* @brief	Init Ring Buffer
******************************************************************************/
void RB_Init_U16(void)
{
  rbIndex = 0;
  valReadIndex = 0;
  valWriteIndex = 0;
  for (int buffer = 0; buffer < RBUF_COUNT; buffer++)
  {
    sum[buffer] = 0;
    for (int position = 0; position < RBUF_SIZE; position++)
      values[position][buffer] = 0;
  }
}

uint16_t RB_Write_U16(const uint16_t newValue)
{
  sum[rbIndex] += newValue - values[valWriteIndex][rbIndex];
  values[valWriteIndex][rbIndex] = newValue;
  return newValue;
}

uint16_t RB_Read_U16(void)
{
  return values[valReadIndex][rbIndex];
}

uint16_t RB_Read_U16_Avg(void)
{
  return sum[rbIndex] / size[rbIndex];
}

uint32_t RB_Read_U16_Sum(void)
{
  return sum[rbIndex];
}

void RB_ReadNext_U16_Avg(void)
{
  valReadIndex++;
  if (valReadIndex >= RBUF_SIZE)
    valReadIndex = 0;
}

void RB_WriteNext_U16_Avg(void)
{
  valWriteIndex++;
  if (valWriteIndex >= RBUF_SIZE)
    valWriteIndex = 0;
}

// EOF
