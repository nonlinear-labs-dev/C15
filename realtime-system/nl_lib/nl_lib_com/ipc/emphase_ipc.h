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

#define EMPHASE_NUMBER_OF_PLAY_DEVICES 30

#ifndef STACK_SIZE
#define STACK_SIZE 0x800
#endif

#ifndef HEAP_SIZE
#define HEAP_SIZE 0x6000
#endif

#define EMPHASE_IPC_PEDAL_1_ADC_RING 7
#define EMPHASE_IPC_PEDAL_1_ADC_TIP  6
#define EMPHASE_IPC_PEDAL_2_ADC_RING 5
#define EMPHASE_IPC_PEDAL_2_ADC_TIP  4
#define EMPHASE_IPC_PEDAL_3_ADC_RING 3
#define EMPHASE_IPC_PEDAL_3_ADC_TIP  2
#define EMPHASE_IPC_PEDAL_4_ADC_RING 1
#define EMPHASE_IPC_PEDAL_4_ADC_TIP  0

#define EMPHASE_IPC_PITCHBENDER_ADC 8
#define EMPHASE_IPC_AFTERTOUCH_ADC  9
#define EMPHASE_IPC_RIBBON_1_ADC    10
#define EMPHASE_IPC_RIBBON_2_ADC    11

#define EMPHASE_IPC_VOLUME_POTI_ADC 12
#define EMPHASE_IPC_ATTENUATOR_ADC  13

#define EMPHASE_IPC_PEDAL_1_DETECT 17
#define EMPHASE_IPC_PEDAL_2_DETECT 16
#define EMPHASE_IPC_PEDAL_3_DETECT 15
#define EMPHASE_IPC_PEDAL_4_DETECT 14

#define EMPHASE_IPC_PEDAL_1_STATE 26
#define EMPHASE_IPC_PEDAL_2_STATE 27
#define EMPHASE_IPC_PEDAL_3_STATE 28
#define EMPHASE_IPC_PEDAL_4_STATE 29

typedef struct
{
  uint32_t key;
  uint32_t timeInUs;
  int32_t  direction;
} IPC_KEY_EVENT_T;

void     Emphase_IPC_PlayBuffer_Write(uint8_t id, uint16_t val);
uint16_t Emphase_IPC_PlayBuffer_Read(uint8_t id);

void Emphase_IPC_M0_Init(void);
void Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(IPC_KEY_EVENT_T keyEvent);

void     Emphase_IPC_M4_Init(void);
uint32_t Emphase_IPC_M4_KeyBuffer_ReadBuffer(IPC_KEY_EVENT_T* keyEvent, uint8_t maxNumOfMsgsToRead);

uint32_t Emphase_IPC_KeyBuffer_GetSize();

#endif
