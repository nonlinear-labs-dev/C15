/******************************************************************************/
/** @file		emphase_ipc.h
    @date		2016-03-01 DTZ
    @brief    	M4 <-> M0 inter processor communication module
    @author		Nemanja Nikodijevic 2015-02-03
*******************************************************************************/
#ifndef EMPHASE_IPC_H
#define EMPHASE_IPC_H
#include <stdint.h>

#define KEY_DIR_UP 1
#define KEY_DIR_DN -1

#define SHARED_MEMORY_BASE 0x10088000

#ifndef STACK_SIZE
#define STACK_SIZE 0x800
#endif

#ifndef HEAP_SIZE
#define HEAP_SIZE 0x6000
#endif

// M0-->M4
#define EMPHASE_IPC_PEDAL_1_ADC_RING 7
#define EMPHASE_IPC_PEDAL_1_ADC_TIP  6
#define EMPHASE_IPC_PEDAL_2_ADC_RING 5
#define EMPHASE_IPC_PEDAL_2_ADC_TIP  4
#define EMPHASE_IPC_PEDAL_3_ADC_RING 3
#define EMPHASE_IPC_PEDAL_3_ADC_TIP  2
#define EMPHASE_IPC_PEDAL_4_ADC_RING 1
#define EMPHASE_IPC_PEDAL_4_ADC_TIP  0
#define EMPHASE_IPC_PEDAL_FIRST_ID   0
#define EMPHASE_IPC_PEDAL_LAST_ID    7

#define EMPHASE_IPC_PITCHBENDER_ADC 8
#define EMPHASE_IPC_AFTERTOUCH_ADC  9
#define EMPHASE_IPC_RIBBON_1_ADC    10
#define EMPHASE_IPC_RIBBON_2_ADC    11

#define EMPHASE_IPC_UNUSED_1 13

#define EMPHASE_IPC_PEDAL_1_DETECT 17
#define EMPHASE_IPC_PEDAL_2_DETECT 16
#define EMPHASE_IPC_PEDAL_3_DETECT 15
#define EMPHASE_IPC_PEDAL_4_DETECT 14

// pedal ADC with averaging option
// counter ID always must be sum ID +1
#define EMPHASE_IPC_PEDAL_1_ADC_RING_SUM  30
#define EMPHASE_IPC_PEDAL_1_ADC_RING_CNTR 31
#define EMPHASE_IPC_PEDAL_1_ADC_TIP_SUM   32
#define EMPHASE_IPC_PEDAL_1_ADC_TIP_CNTR  33
#define EMPHASE_IPC_PEDAL_2_ADC_RING_SUM  34
#define EMPHASE_IPC_PEDAL_2_ADC_RING_CNTR 35
#define EMPHASE_IPC_PEDAL_2_ADC_TIP_SUM   36
#define EMPHASE_IPC_PEDAL_2_ADC_TIP_CNTR  37
#define EMPHASE_IPC_PEDAL_3_ADC_RING_SUM  38
#define EMPHASE_IPC_PEDAL_3_ADC_RING_CNTR 39
#define EMPHASE_IPC_PEDAL_3_ADC_TIP_SUM   40
#define EMPHASE_IPC_PEDAL_3_ADC_TIP_CNTR  41
#define EMPHASE_IPC_PEDAL_4_ADC_RING_SUM  42
#define EMPHASE_IPC_PEDAL_4_ADC_RING_CNTR 43
#define EMPHASE_IPC_PEDAL_4_ADC_TIP_SUM   44
#define EMPHASE_IPC_PEDAL_4_ADC_TIP_CNTR  45

// M4-->M0
#define EMPHASE_IPC_PEDAL_SUM_RESET 12  // flag to reset M0's summing of pedal ADC channel's value

#define EMPHASE_IPC_PEDAL_1_STATE 26
#define EMPHASE_IPC_PEDAL_2_STATE 27
#define EMPHASE_IPC_PEDAL_3_STATE 28
#define EMPHASE_IPC_PEDAL_4_STATE 29

#define EMPHASE_NUMBER_OF_PLAY_DEVICES 46

typedef struct
{
  uint32_t key;
  uint32_t timeInUs;
  int32_t  direction;
} IPC_KEY_EVENT_T;

void    Emphase_IPC_PlayBuffer_Write(uint8_t id, int32_t val);
void    Emphase_IPC_PlayBuffer_WriteSummed(uint8_t id, int32_t val);
void    Emphase_IPC_PlayBuffer_ResetSummed(uint8_t id);
int32_t Emphase_IPC_PlayBuffer_Read(uint8_t id);
int32_t Emphase_IPC_PlayBuffer_ReadSummed(uint8_t id);

void Emphase_IPC_M0_Init(void);
void Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(IPC_KEY_EVENT_T keyEvent);

void     Emphase_IPC_M4_Init(void);
uint32_t Emphase_IPC_M4_KeyBuffer_ReadBuffer(IPC_KEY_EVENT_T* keyEvent, uint8_t maxNumOfMsgsToRead);

uint32_t Emphase_IPC_KeyBuffer_GetSize();

#endif
