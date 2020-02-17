/******************************************************************************/
/** @file		emphase_ipc.h
    @date		2016-02-29 DTZ
    @brief    	M4 <-> M0 inter processor communication module
    @author		Nemanja Nikodijevic 2015-02-03
*******************************************************************************/
#include <stdint.h>
#include "ipc/emphase_ipc.h"

#define IPC_ADC_DEFAULT (2048)

typedef struct
{
  int32_t values[IPC_ADC_NUMBER_OF_CHANNELS][IPC_ADC_BUFFER_SIZE];
  int32_t sum[IPC_ADC_NUMBER_OF_CHANNELS];
} ADC_BUFFER_ARRAY_T;

#define EMPHASE_IPC_KEYBUFFER_SIZE (64)
#define EMPHASE_IPC_KEYBUFFER_MASK (EMPHASE_IPC_KEYBUFFER_SIZE - 1)

// data in shared memory is accessible via pointers only, can't define variables directly
static volatile IPC_KEY_EVENT_T*    keyBufferData;
static volatile uint32_t*           keyBufferWritePos;
static volatile uint32_t*           keyBufferReadPos;
static volatile ADC_BUFFER_ARRAY_T* adcBufferData;
static volatile uint32_t*           adcConfigData;
static volatile int*                adcBufferWriteIndex;
static volatile int*                adcBufferReadIndex;

/******************************************************************************
*	Functions for both the M4 and M0 to interface the PlayBuffers.
******************************************************************************/

/******************************************************************************/
/**	@brief      Read ADC channel value
*   @param[in]	IPC id of the adc channel 0...15
*   @return     adc channel value
******************************************************************************/
int32_t IPC_ReadAdcBuffer(const uint8_t adc_id)
{
  return adcBufferData->values[adc_id][*adcBufferReadIndex];
}

/******************************************************************************/
/**	@brief      Read ADC channel value as average of whole ring buffer contents
*   @param[in]	IPC id of the adc channel 0...15
*   @return     adc channel value
******************************************************************************/
int32_t IPC_ReadAdcBufferAveraged(const uint8_t adc_id)
{
#if 0  // old algo
  int32_t x = 0;
  for (int i = 0; i < IPC_ADC_BUFFER_SIZE; i++)
    x += adcBufferData->values[adc_id][i];
  return x / IPC_ADC_BUFFER_SIZE;
#endif

  return adcBufferData->sum[adc_id] / IPC_ADC_BUFFER_SIZE;
}

/******************************************************************************/
/**	@brief      Read ADC channel value as sum of whole ring buffer contents
*   @param[in]	IPC id of the adc channel 0...15
*   @return     adc channel value
******************************************************************************/
int32_t IPC_ReadAdcBufferSum(const uint8_t adc_id)
{
  return adcBufferData->sum[adc_id];
}

/******************************************************************************/
/**	@brief      Write ADC value (must be called only once per cycle!)
*   @param[in]	IPC id of the adc channel 0...15
*   @param[in]  adc channel value
******************************************************************************/
void IPC_WriteAdcBuffer(const uint8_t adc_id, const int32_t value)
{
  // subtract out the overwritten value and add in new value to sum
  adcBufferData->sum[adc_id] += -(adcBufferData->values[adc_id][*adcBufferWriteIndex]) + value;
  // write value to ring buffer
  adcBufferData->values[adc_id][*adcBufferWriteIndex] = value;
}

/******************************************************************************/
/**	@brief      Advance write buffer index to next position
******************************************************************************/
void IPC_AdcBufferWriteNext(void)
{
  *adcBufferWriteIndex = (*adcBufferWriteIndex + 1) & IPC_ADC_BUFFER_MASK;
}

/******************************************************************************/
/**	@brief      Update read buffer index to current position
******************************************************************************/
void IPC_AdcUpdateReadIndex(void)
{
  *adcBufferReadIndex = *adcBufferWriteIndex;
}

/******************************************************************************/
/**	@brief      Read the ADC line input config for all physical pedal channels
*   @return     config bits [32 bits, 4 bytes from ADC67(MSB) to ADC12(LSB)]
*   for bit masks see  espi/dev/nl_espi_dev_pedals.h
******************************************************************************/
uint32_t IPC_ReadPedalAdcConfig(void)
{
  return *adcConfigData;
}

/******************************************************************************/
/**	@brief      Read the ADC line input config for all physical pedal channels
*   @param{in]  config bits [32 bits, 4 bytes from ADC67(MSB) to ADC12(LSB)]
*   for bit masks see  espi/dev/nl_espi_dev_pedals.h
******************************************************************************/
void IPC_WritePedalAdcConfig(const uint32_t config)
{
  *adcConfigData = config;
}

/******************************************************************************/
/**	@brief   Initialize the pointers to the shared memory
******************************************************************************/
static void Init_Addr(void)
{
  uint32_t addr = SHARED_MEMORY_BASE;

  keyBufferData = (IPC_KEY_EVENT_T*) (addr);
  addr += EMPHASE_IPC_KEYBUFFER_SIZE * sizeof(IPC_KEY_EVENT_T);

  keyBufferWritePos = (uint32_t*) (addr);
  addr += sizeof(uint32_t);

  keyBufferReadPos = (uint32_t*) (addr);
  addr += sizeof(uint32_t);

  adcBufferData = (ADC_BUFFER_ARRAY_T*) (addr);
  addr += sizeof(ADC_BUFFER_ARRAY_T);

  adcConfigData = (uint32_t*) (addr);
  addr += sizeof adcConfigData[0];

  adcBufferWriteIndex = (int*) (addr);
  addr += sizeof *adcBufferWriteIndex;

  adcBufferReadIndex = (int*) (addr);
  addr += sizeof *adcBufferReadIndex;
}

/******************************************************************************/
/**	@brief  setup and clear data in shared memory
*******************************************************************************/
void Emphase_IPC_Init(void)
{
  Init_Addr();

  *keyBufferWritePos = 0;
  *keyBufferReadPos  = 0;

  for (int i = 0; i < EMPHASE_IPC_KEYBUFFER_SIZE; i++)
  {
    keyBufferData[i].key       = 0;
    keyBufferData[i].timeInUs  = 0;
    keyBufferData[i].direction = 0;
  }

  for (int i = 0; i < IPC_ADC_NUMBER_OF_CHANNELS; i++)
  {
    for (int k = 0; k < IPC_ADC_BUFFER_SIZE; k++)
      adcBufferData->values[i][k] = IPC_ADC_DEFAULT;
    adcBufferData->sum[i] = IPC_ADC_DEFAULT * IPC_ADC_BUFFER_SIZE;
  }
  *adcConfigData       = 0;
  *adcBufferReadIndex  = 0;
  *adcBufferWriteIndex = 0;
}

/******************************************************************************/
/**  @brief     Here the M0 write key events to a circular buffer
                that the M4 will read
     @param[in] keyEvent: A struct containing the index of the key
                and the direction and travel time of the last key action
*******************************************************************************/
void Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(const IPC_KEY_EVENT_T keyEvent)
{
  keyBufferData[*keyBufferWritePos] = keyEvent;

  *keyBufferWritePos = (*keyBufferWritePos + 1) & (EMPHASE_IPC_KEYBUFFER_MASK);
}

/******************************************************************************/
/** @brief      Here the M4 reads key events from a circular buffer
                that have been written by the M0
    @param[in]  pKeyEvent: pointer to an array of key events
                that will be processed by the voice allocation
                maxNumOfEventsToRead: size of the array pointed by pKeyEvent
    @return     Number of new key events (0: nothing to do)
*******************************************************************************/
uint32_t Emphase_IPC_M4_KeyBuffer_ReadBuffer(IPC_KEY_EVENT_T* const pKeyEvent, const uint8_t maxNumOfEventsToRead)
{
  uint8_t count = 0;

  while ((*keyBufferReadPos != *keyBufferWritePos) && (count < maxNumOfEventsToRead))
  {
    pKeyEvent[count] = keyBufferData[*keyBufferReadPos];

    *keyBufferReadPos = (*keyBufferReadPos + 1) & (EMPHASE_IPC_KEYBUFFER_MASK);

    count++;
  }

  return count;
}

/******************************************************************************/
/** @brief      Return size of key buffer
*******************************************************************************/
uint32_t Emphase_IPC_KeyBuffer_GetSize()
{
  return EMPHASE_IPC_KEYBUFFER_SIZE;
}
