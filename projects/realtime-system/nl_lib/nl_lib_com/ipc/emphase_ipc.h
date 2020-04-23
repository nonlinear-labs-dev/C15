/******************************************************************************/
/** @file		emphase_ipc.h
    @date		2016-03-01 DTZ
    @brief    	M4 <-> M0 inter processor communication module
    @author		Nemanja Nikodijevic 2015-02-03
*******************************************************************************/
#ifndef EMPHASE_IPC_H
#define EMPHASE_IPC_H
#include <stdint.h>

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

#define KEY_DIR_UP 1
#define KEY_DIR_DN -1

typedef struct
{
  uint32_t key;
  uint32_t timeInUs;
  int32_t  direction;
} IPC_KEY_EVENT_T;

typedef struct
{
  int32_t values[IPC_ADC_NUMBER_OF_CHANNELS][IPC_ADC_BUFFER_SIZE];
  int32_t sum[IPC_ADC_NUMBER_OF_CHANNELS];
} ADC_BUFFER_ARRAY_T;

#define EMPHASE_IPC_KEYBUFFER_SIZE (64)  // number of key events that can be processed in 125us
#define EMPHASE_IPC_KEYBUFFER_MASK (EMPHASE_IPC_KEYBUFFER_SIZE - 1)

typedef struct
{
  IPC_KEY_EVENT_T    keyBufferData[EMPHASE_IPC_KEYBUFFER_SIZE];
  volatile uint32_t  keyBufferWritePos;
  volatile uint32_t  keyBufferReadPos;
  ADC_BUFFER_ARRAY_T adcBufferData;
  uint32_t           adcConfigData;
  uint32_t           adcBufferWriteIndex;
  uint32_t           adcBufferReadIndex;
  volatile uint16_t  timer;
  volatile uint16_t  KBSTicks;
  volatile uint16_t  SchedulerTicks;
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
static inline void Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(const IPC_KEY_EVENT_T keyEvent)
{
  // !! this is a potentially critical section !!
  // Emphase_IPC_M4_KeyBuffer_ReadBuffer() should not be called while we are here
  // because it uses keyBufferWritePos in a compare
  // NOTE : No check is done if the write overruns the buffer, we rely
  // on the buffer being big enough to avoid this
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
static inline uint32_t Emphase_IPC_M4_KeyBuffer_ReadBuffer(IPC_KEY_EVENT_T* const pKeyEvent, const uint8_t maxNumOfEventsToRead)
{
  // !! this is a potentially critical section !!
  // Emphase_IPC_M0_KeyBuffer_WriteKeyEvent() should not be called while we are here
  // because it updates keyBufferWritePos
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
static inline int32_t IPC_ReadAdcBuffer(const uint8_t adc_id)
{
  return s.adcBufferData.values[adc_id][s.adcBufferReadIndex];
}

/******************************************************************************/
/**	@brief      Read ADC channel value as average of whole ring buffer contents
*   @param[in]	IPC id of the adc channel 0...15
*   @return     adc channel value
******************************************************************************/
static inline int32_t IPC_ReadAdcBufferAveraged(const uint8_t adc_id)
{
  return s.adcBufferData.sum[adc_id] / IPC_ADC_BUFFER_SIZE;
}

/******************************************************************************/
/**	@brief      Read ADC channel value as sum of whole ring buffer contents
*   @param[in]	IPC id of the adc channel 0...15
*   @return     adc channel value
******************************************************************************/
static inline int32_t IPC_ReadAdcBufferSum(const uint8_t adc_id)
{
  return s.adcBufferData.sum[adc_id];
}

/******************************************************************************/
/**	@brief      Write ADC value (must be called only once per cycle!)
*   @param[in]	IPC id of the adc channel 0...15
*   @param[in]  adc channel value
******************************************************************************/
static inline void IPC_WriteAdcBuffer(const uint8_t adc_id, const int32_t value)
{
  // subtract out the overwritten value and add in new value to sum
  s.adcBufferData.sum[adc_id] += -(s.adcBufferData.values[adc_id][s.adcBufferWriteIndex]) + value;
  // write value to ring buffer
  s.adcBufferData.values[adc_id][s.adcBufferWriteIndex] = value;
}

/******************************************************************************/
/**	@brief      Advance write buffer index to next position
******************************************************************************/
static inline void IPC_AdcBufferWriteNext(void)
{
  s.adcBufferWriteIndex = (s.adcBufferWriteIndex + 1) & IPC_ADC_BUFFER_MASK;
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
static inline void IPC_WritePedalAdcConfig(const uint32_t config)
{
  s.adcConfigData = config;
}

/******************************************************************************/
/** @brief      Timer functions (2.5us units as per M0 IRQ setup)
*******************************************************************************/
static inline uint16_t IPC_GetTimer(void)
{
  return s.timer;
}

static inline void IPC_ResetTimer(void)
{
  s.timer = 0;
}

static inline void IPC_IncTimer(void)
{
  s.timer++;
}

static inline void IPC_SetKBSTime(uint16_t ticks)
{
  if (ticks > s.KBSTicks)
    s.KBSTicks = ticks;
}

static inline uint16_t IPC_GetAndResetKBSTime(void)
{
  uint16_t tmp = s.KBSTicks;
  s.KBSTicks   = 0;
  return tmp;
}

static inline void IPC_SetSchedulerTime(uint16_t ticks)
{
  if (ticks > s.SchedulerTicks)
    s.SchedulerTicks = ticks;
}

static inline uint16_t IPC_GetAndResetSchedulerTime(void)
{
  uint16_t tmp     = s.SchedulerTicks;
  s.SchedulerTicks = 0;
  return tmp;
}

#endif
