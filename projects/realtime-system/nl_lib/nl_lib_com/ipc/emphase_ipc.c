/******************************************************************************/
/** @file		emphase_ipc.h
    @date		2016-02-29 DTZ
    @brief    	M4 <-> M0 inter processor communication module
    @author		Nemanja Nikodijevic 2015-02-03
*******************************************************************************/
#include <stdint.h>
#include "ipc/emphase_ipc.h"
#include "drv/nl_rit.h"

#define IPC_ADC_DEFAULT (2048)

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
} SharedData_T;

//
// Places the variable(s) in the shared memory block.
// This block must be defined in both LPC4337_M0_MEM.ld and LPC4337_M4_MEM.ld identically.
// The section ".shared" must be defined in both LPC4337_M0_MEM.ld and LPC4337_M4_MEM.ld identically.
// When the size of all shared variables is larger than the block the linker will throw an error.
// To make sure all variable are at the same memory location and have the same layout,
// use only one struct and embed your variables there.
// The section is defined as "noinit" so you must clear the data yourself.
//
__attribute__((section(".shared"))) SharedData_T s;

// double check the used memory
void CheckSizeAtCompileTime(void)
{
  (void) sizeof(char[-!(sizeof(s) < 4000)]);  // must be less than the 4096 bytes in shared mem
}

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
  return s.adcBufferData.values[adc_id][s.adcBufferReadIndex];
}

/******************************************************************************/
/**	@brief      Read ADC channel value as average of whole ring buffer contents
*   @param[in]	IPC id of the adc channel 0...15
*   @return     adc channel value
******************************************************************************/
int32_t IPC_ReadAdcBufferAveraged(const uint8_t adc_id)
{
  return s.adcBufferData.sum[adc_id] / IPC_ADC_BUFFER_SIZE;
}

/******************************************************************************/
/**	@brief      Read ADC channel value as sum of whole ring buffer contents
*   @param[in]	IPC id of the adc channel 0...15
*   @return     adc channel value
******************************************************************************/
int32_t IPC_ReadAdcBufferSum(const uint8_t adc_id)
{
  return s.adcBufferData.sum[adc_id];
}

/******************************************************************************/
/**	@brief      Write ADC value (must be called only once per cycle!)
*   @param[in]	IPC id of the adc channel 0...15
*   @param[in]  adc channel value
******************************************************************************/
void IPC_WriteAdcBuffer(const uint8_t adc_id, const int32_t value)
{
  // subtract out the overwritten value and add in new value to sum
  s.adcBufferData.sum[adc_id] += -(s.adcBufferData.values[adc_id][s.adcBufferWriteIndex]) + value;
  // write value to ring buffer
  s.adcBufferData.values[adc_id][s.adcBufferWriteIndex] = value;
}

/******************************************************************************/
/**	@brief      Advance write buffer index to next position
******************************************************************************/
void IPC_AdcBufferWriteNext(void)
{
  s.adcBufferWriteIndex = (s.adcBufferWriteIndex + 1) & IPC_ADC_BUFFER_MASK;
}

/******************************************************************************/
/**	@brief      Update read buffer index to current position
******************************************************************************/
void IPC_AdcUpdateReadIndex(void)
{
  s.adcBufferReadIndex = s.adcBufferWriteIndex;
}

/******************************************************************************/
/**	@brief      Read the ADC line input config for all physical pedal channels
*   @return     config bits [32 bits, 4 bytes from ADC67(MSB) to ADC12(LSB)]
*   for bit masks see  espi/dev/nl_espi_dev_pedals.h
******************************************************************************/
uint32_t IPC_ReadPedalAdcConfig(void)
{
  return s.adcConfigData;
}

/******************************************************************************/
/**	@brief      Read the ADC line input config for all physical pedal channels
*   @param{in]  config bits [32 bits, 4 bytes from ADC67(MSB) to ADC12(LSB)]
*   for bit masks see  espi/dev/nl_espi_dev_pedals.h
******************************************************************************/
void IPC_WritePedalAdcConfig(const uint32_t config)
{
  s.adcConfigData = config;
}
/******************************************************************************/
/**	@brief  setup and clear data in shared memory
*******************************************************************************/
void Emphase_IPC_Init(void)
{
  s.keyBufferWritePos = 0;
  s.keyBufferReadPos  = 0;

  for (int i = 0; i < EMPHASE_IPC_KEYBUFFER_SIZE; i++)
  {
    s.keyBufferData[i].key       = 0;
    s.keyBufferData[i].timeInUs  = 0;
    s.keyBufferData[i].direction = 0;
  }

  for (int i = 0; i < IPC_ADC_NUMBER_OF_CHANNELS; i++)
  {
    for (int k = 0; k < IPC_ADC_BUFFER_SIZE; k++)
      s.adcBufferData.values[i][k] = IPC_ADC_DEFAULT;
    s.adcBufferData.sum[i] = IPC_ADC_DEFAULT * IPC_ADC_BUFFER_SIZE;
  }
  s.adcConfigData       = 0;
  s.adcBufferReadIndex  = 0;
  s.adcBufferWriteIndex = 0;
  s.timer               = 0;
}

/******************************************************************************/
/**  @brief     Here the M0 write key events to a circular buffer
                that the M4 will read
     @param[in] keyEvent: A struct containing the index of the key
                and the direction and travel time of the last key action
*******************************************************************************/
void Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(const IPC_KEY_EVENT_T keyEvent)
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
uint32_t Emphase_IPC_M4_KeyBuffer_ReadBuffer(IPC_KEY_EVENT_T* const pKeyEvent, const uint8_t maxNumOfEventsToRead)
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
uint32_t Emphase_IPC_KeyBuffer_GetSize()
{
  return EMPHASE_IPC_KEYBUFFER_SIZE;
}

/******************************************************************************/
/** @brief      Return timeslice counter in 1/102MHz units (~10ns)
*******************************************************************************/
uint16_t IPC_GetTimer(void)
{
  return s.timer;
}
void IPC_ResetTimer(void)
{
  s.timer = 0;
}
void IPC_IncTimer(void)
{
  s.timer += 1;
}
