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

typedef struct
{
  uint32_t key;
  uint32_t timeInUs;
  int32_t  direction;
} IPC_KEY_EVENT_T;

void     Emphase_IPC_Init(void);
void     Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(IPC_KEY_EVENT_T keyEvent);
uint32_t Emphase_IPC_M4_KeyBuffer_ReadBuffer(IPC_KEY_EVENT_T* keyEvent, uint8_t maxNumOfMsgsToRead);

uint32_t Emphase_IPC_KeyBuffer_GetSize();

int32_t  IPC_ReadAdcBuffer(uint8_t id);
int32_t  IPC_ReadAdcBufferAveraged(uint8_t id);
void     IPC_WriteAdcBuffer(uint8_t id, int32_t value);
void     IPC_AdcBufferWriteNext(void);
void     IPC_AdcUpdateReadIndex(void);
uint32_t IPC_ReadPedalAdcConfig(void);
void     IPC_WritePedalAdcConfig(uint32_t config);

#endif
