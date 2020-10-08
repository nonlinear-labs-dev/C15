/******************************************************************************/
/** @file		emphase_ipc.h
    @date		2016-03-01 DTZ
    @brief    	M4 <-> M0 inter processor communication module
    @author		Nemanja Nikodijevic 2015-02-03
*******************************************************************************/
#ifndef EMPHASE_IPC_H
#define EMPHASE_IPC_H
#include <stdint.h>
#include "globals.h"

#if LPC_DGB_ADC_STRESS_TEST
// use a free-running 0..4095 counter rather than real ADC values
// counter is incremented when adcBufferWriteIndex is advanced
#warning "ADC stress test is on, no real ADC values will be used"
static uint16_t adc_val;
#endif

// ID's for IPC
// M0-->M4
// 1. ADC's
#define IPC_ADC_PEDAL1_TIP         (0)
#define IPC_ADC_PEDAL1_RING        (1)
#define IPC_ADC_PEDAL1_DETECT      (2)  // treated as ADC, for de-glitching
#define IPC_ADC_PEDAL2_TIP         (3)
#define IPC_ADC_PEDAL2_RING        (4)
#define IPC_ADC_PEDAL2_DETECT      (5)  // treated as ADC, for de-glitching
#define IPC_ADC_PEDAL3_TIP         (6)
#define IPC_ADC_PEDAL3_RING        (7)
#define IPC_ADC_PEDAL3_DETECT      (8)  // treated as ADC, for de-glitching
#define IPC_ADC_PEDAL4_TIP         (9)
#define IPC_ADC_PEDAL4_RING        (10)
#define IPC_ADC_PEDAL4_DETECT      (11)  // treated as ADC, for de-glitching
#define IPC_ADC_PITCHBENDER        (12)
#define IPC_ADC_AFTERTOUCH         (13)
#define IPC_ADC_RIBBON1            (14)
#define IPC_ADC_RIBBON2            (15)
#define IPC_ADC_NUMBER_OF_CHANNELS (16)

// ADC ring buffers
// Must be 2^N in size and <= 16. This also determines the averaging.
// Size should NOT be larger than the number of aquisitions between M4 read-out operations
#define IPC_ADC_BUFFER_SIZE (16)
#define IPC_ADC_BUFFER_MASK (IPC_ADC_BUFFER_SIZE - 1)
#define IPC_ADC_DEFAULT     (2048)

typedef struct
{
  int32_t values[IPC_ADC_NUMBER_OF_CHANNELS][IPC_ADC_BUFFER_SIZE];
  int32_t sum[IPC_ADC_NUMBER_OF_CHANNELS];
} ADC_BUFFER_ARRAY_T;

#define EMPHASE_IPC_KEYBUFFER_SIZE (64)  // number of key events that can be processed in 125us
#define EMPHASE_IPC_KEYBUFFER_MASK (EMPHASE_IPC_KEYBUFFER_SIZE - 1)
#define IPC_KEYBUFFER_KEYMASK      (0x3F)
#define IPC_KEYBUFFER_NOTEON       (0x40)
#define IPC_KEYBUFFER_TIME_SHIFT   (7)

typedef struct
{
  uint32_t keyBufferData[EMPHASE_IPC_KEYBUFFER_SIZE];
#ifdef CORE_M4
  volatile
#endif
      uint32_t       keyBufferWritePos;
  uint32_t           keyBufferReadPos;
  ADC_BUFFER_ARRAY_T adcBufferData;
  uint32_t           adcConfigData;
  uint32_t           adcBufferWriteIndex;
  uint32_t           adcBufferReadIndex;
  volatile uint32_t  ticker;
  uint32_t           ADCTicks;
  uint32_t           RitCrtlReg;
#if LPC_KEYBED_DIAG
  int16_t keyOnOffCntr[64];
#endif
} SharedData_T;

extern SharedData_T s;

void Emphase_IPC_Init(void);

/******************************************************************************
*	Functions for both the M4 and M0 to interface the PlayBuffers.
******************************************************************************/
/******************************************************************************/
/**  @brief     Here the M0 write key events to a circular buffer
                that the M4 will read
     @param[in] keyEvent: A struct containing the index of the key
                and the direction and travel time of the last key action
*******************************************************************************/
static inline void Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(uint32_t const keyEvent)
{
  // !! this is a potentially critical section !!
  // Emphase_IPC_M4_KeyBuffer_ReadBuffer() should not run while we are here
  // because it uses keyBufferWritePos in a compare. Unless keyBufferWritePos
  // isn't update so fast as to overrun keyBufferReadPos nothing bad will happe, though.
  // NOTE : No check is done anyway if the write overruns the buffer, we rely
  // on the buffer being big enough to avoid this.
  s.keyBufferData[s.keyBufferWritePos] = keyEvent;
  s.keyBufferWritePos                  = (s.keyBufferWritePos + 1) & (EMPHASE_IPC_KEYBUFFER_MASK);
}

/******************************************************************************/
/** @brief      Here the M4 reads key events from a circular buffer
                that have been written by the M0
    @param[in]  pKeyEvent: pointer to an array of key events
                that will be processed by the voice allocation
                maxNumOfEventsToRead: size of the array pointed by pKeyEvent
    @return     Number of new key events (0: nothing to do)
*******************************************************************************/
static inline uint32_t Emphase_IPC_M4_KeyBuffer_ReadBuffer(uint32_t* const pKeyEvent, uint8_t const maxNumOfEventsToRead)
{
  // !! this is a potentially critical section !!
  // Emphase_IPC_M0_KeyBuffer_WriteKeyEvent() should not run while we are here
  // because it updates keyBufferWritePos.
  // As long as keyBufferWritePos, which is volatile from M4's view, doesn't update so fast
  // that it overruns keyBufferReadPos (which is extremely unlikely, only when Emphase_IPC_M4_KeyBuffer_ReadBuffer
  // is interrupted/stalling for a really long time), nothing bad will happen, though.
  uint8_t count = 0;
  while ((s.keyBufferReadPos != s.keyBufferWritePos) && (count < maxNumOfEventsToRead))
  {
    pKeyEvent[count]   = s.keyBufferData[s.keyBufferReadPos];
    s.keyBufferReadPos = (s.keyBufferReadPos + 1) & (EMPHASE_IPC_KEYBUFFER_MASK);
    count++;
  }
  return count;
}

/******************************************************************************/
/** @brief      Return size of key buffer
*******************************************************************************/
static inline uint32_t Emphase_IPC_KeyBuffer_GetSize()
{
  return EMPHASE_IPC_KEYBUFFER_SIZE;
}

/******************************************************************************/
/**	@brief      Read ADC channel value
*   @param[in]	IPC id of the adc channel 0...15
*   @return     adc channel value
******************************************************************************/
static inline int32_t IPC_ReadAdcBuffer(uint8_t const adc_id)
{
  // M0 may advance adcBufferReadIndex while we are reading values for a number
  // of ADCs during an M4 time-slice. This means values of different ADCs may
  // not all be from the same conversion cycle of M0. Since the conversion cycle
  // runs 16 times per M4 time-slice the error is neglegible, though.
  return s.adcBufferData.values[adc_id][s.adcBufferReadIndex];
}

/******************************************************************************/
/**	@brief      Read ADC channel value as average of whole ring buffer contents
*   @param[in]	IPC id of the adc channel 0...15
*   @return     adc channel value
******************************************************************************/
static inline int32_t IPC_ReadAdcBufferAveraged(uint8_t const adc_id)
{
  // Again, values in the buffers for different ADCs may not be in sync with
  // M4's time-slice, but because of the averaging the error is even smaller.
  // More importantly, M0 may be just in the middle of its non-atomic
  // read-modify-write operation in IPC_WriteAdcBuffer, but again the error
  // is very small because only a delta is added/subtracted.
  return s.adcBufferData.sum[adc_id] / IPC_ADC_BUFFER_SIZE;
}

/******************************************************************************/
/**	@brief      Read ADC channel value as sum of whole ring buffer contents
*   @param[in]	IPC id of the adc channel 0...15
*   @return     adc channel value
******************************************************************************/
static inline int32_t IPC_ReadAdcBufferSum(uint8_t const adc_id)
{
  // see notes for IPC_ReadAdcBufferAveraged
  return s.adcBufferData.sum[adc_id];
}

/******************************************************************************/
/**	@brief      Write ADC value (must be called only once per cycle!)
*   @param[in]	IPC id of the adc channel 0...15
*   @param[in]  adc channel value
******************************************************************************/
#if LPC_DGB_ADC_STRESS_TEST
static inline void IPC_WriteAdcBuffer(uint8_t const adc_id, int32_t value)
{
  value = adc_val;
#else
static inline void IPC_WriteAdcBuffer(uint8_t const adc_id, int32_t const value)
{
#endif
  // see notes for IPC_ReadAdcBufferAveraged above
  asm volatile("cpsid i");  // IRQs off, make critical section as short as possible (M0 IRQs are long because of keybed scanner)
  // subtract out the overwritten value and add in new value to sum
  s.adcBufferData.sum[adc_id] += -(s.adcBufferData.values[adc_id][s.adcBufferWriteIndex]) + value;
  // write value to ring buffer
  s.adcBufferData.values[adc_id][s.adcBufferWriteIndex] = value;
  asm volatile("cpsie i");  // IRQs on
}

/******************************************************************************/
/**	@brief      Advance write buffer index to next position
******************************************************************************/
static inline void IPC_AdcBufferWriteNext(void)
{
  s.adcBufferWriteIndex = (s.adcBufferWriteIndex + 1) & IPC_ADC_BUFFER_MASK;
#if LPC_DGB_ADC_STRESS_TEST

  adc_val = (adc_val + 1) & 4095;
#endif
}

/******************************************************************************/
/**	@brief      Update read buffer index to current position
******************************************************************************/
static inline void IPC_AdcUpdateReadIndex(void)
{
  s.adcBufferReadIndex = s.adcBufferWriteIndex;
}

/******************************************************************************/
/**	@brief      Read the ADC line input config for all physical pedal channels
*   @return     config bits [32 bits, 4 bytes from ADC67(MSB) to ADC12(LSB)]
*   for bit masks see  espi/dev/nl_espi_dev_pedals.h
******************************************************************************/
static inline uint32_t IPC_ReadPedalAdcConfig(void)
{
  return s.adcConfigData;
}

/******************************************************************************/
/**	@brief      Read the ADC line input config for all physical pedal channels
*   @param{in]  config bits [32 bits, 4 bytes from ADC67(MSB) to ADC12(LSB)]
*   for bit masks see  espi/dev/nl_espi_dev_pedals.h
******************************************************************************/
static inline void IPC_WritePedalAdcConfig(uint32_t const config)
{
  s.adcConfigData = config;
}

/******************************************************************************/
/** @brief      Timer functions)
*******************************************************************************/

static inline void IPC_SetADCTime(uint32_t const ticks)
{
  if (ticks > s.ADCTicks)
    s.ADCTicks = ticks;
}

static inline uint32_t IPC_GetAndResetADCTime(void)
{
  uint32_t tmp = s.ADCTicks;
  s.ADCTicks   = 0;
  return tmp;
}

#endif
