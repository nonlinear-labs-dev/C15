/******************************************************************************/
/** @file		emphase_ipc.h
    @date		2016-02-29 DTZ
    @brief    	M4 <-> M0 inter processor communication module
    @author		Nemanja Nikodijevic 2015-02-03
*******************************************************************************/
#include <stdint.h>
#include "ipc/emphase_ipc.h"

#define EMPHASE_IPC_KEYBUFFER_SIZE 64
#define EMPHASE_IPC_KEYBUFFER_MASK EMPHASE_IPC_KEYBUFFER_SIZE - 1

static volatile IPC_KEY_EVENT_T* keyBufferData;
static volatile uint32_t*        keyBufferWritePos;
static volatile uint32_t*        keyBufferReadPos;
static volatile uint16_t*        playBufferData;

static void Init_Addr(void)
{
  uint32_t addr = SHARED_MEMORY_BASE;

  keyBufferData = (IPC_KEY_EVENT_T*) (addr);
  addr += EMPHASE_IPC_KEYBUFFER_SIZE * sizeof(IPC_KEY_EVENT_T);

  keyBufferWritePos = (uint32_t*) (addr);
  addr += sizeof(uint32_t);

  keyBufferReadPos = (uint32_t*) (addr);
  addr += sizeof(uint32_t);

  playBufferData = (uint16_t*) (addr);
  addr += EMPHASE_NUMBER_OF_PLAY_DEVICES * sizeof(uint16_t);
}

/******************************************************************************/
/**
*******************************************************************************/
void Emphase_IPC_M4_Init(void)
{
  Init_Addr();

  *keyBufferReadPos = 0;

  uint8_t i;
  for (i = 0; i < EMPHASE_NUMBER_OF_PLAY_DEVICES; i++)
  {
    playBufferData[i] = 0;
  }
}

/******************************************************************************/
/**
*******************************************************************************/
void Emphase_IPC_M0_Init(void)
{
  Init_Addr();

  *keyBufferWritePos = 0;

  uint8_t i;
  for (i = 0; i < EMPHASE_IPC_KEYBUFFER_SIZE; i++)
  {
    keyBufferData[i].key       = 0;
    keyBufferData[i].timeInUs  = 0;
    keyBufferData[i].direction = 0;
  }

  for (i = 0; i < EMPHASE_NUMBER_OF_PLAY_DEVICES; i++)
  {
    playBufferData[i] = 0;
  }
}

/******************************************************************************
  @brief  		Here the M0 write key events to a circular buffer
				that the M4 will read
  @param[in]	keyEvent: A struct containing the index of the key
				and the direction and travel time of the last key action
*******************************************************************************/

void Emphase_IPC_M0_KeyBuffer_WriteKeyEvent(IPC_KEY_EVENT_T keyEvent)
{
  keyBufferData[*keyBufferWritePos] = keyEvent;

  *keyBufferWritePos = (*keyBufferWritePos + 1) & (EMPHASE_IPC_KEYBUFFER_MASK);
}

/******************************************************************************
  @brief		Here the M4 reads key events from a circular buffer
				that have been written by the M0
  @param[in]	pKeyEvent: pointer to an array of key events
				that will be processed by the voice allocation
				maxNumOfEventsToRead: size of the array pointed by pKeyEvent
  @return		Number of new key events (0: nothing to do)
*******************************************************************************/

uint32_t Emphase_IPC_M4_KeyBuffer_ReadBuffer(IPC_KEY_EVENT_T* pKeyEvent, uint8_t maxNumOfEventsToRead)
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
/**	@brief  Functions for both, the M4 and M0 to interface the PlayBuffer.
  			=> see the defines in the header file for details

    Name							ID				range   Emphase Version
    -----------------------------------------------------------------------
 	ESPI_DEV_Pedals[8]		    	0..7			12 Bit
    ESPI_DEV_Pitchbender	    	8				12 Bit
    ESPI_DEV_Aftertouch		    	9				12 Bit
    ESPI_DEV_Ribbons[2]		    	10..11 | Up/Low	12 Bit
    ESPI_DEV_VolPoti		    	12				12 Bit
    ESPI_DEV_Attenuator		    	13				12 Bit
    ESPI_DEV_Detect[4]				14..17 | 0..3	 1 Bit
	ESPI_DEV_PullResistors[8]		18..25 | 0..7	 1 Bit  v4
	ESPI_DEV_PedalsV2_SetPedalState	26..29 | 0..3    4 Bit  v5

*******************************************************************************/
void Emphase_IPC_PlayBuffer_Write(uint8_t id, uint16_t val)
{
  playBufferData[id] = val;
}

/******************************************************************************/
/**	@brief
 	@return
*******************************************************************************/
uint16_t Emphase_IPC_PlayBuffer_Read(uint8_t id)
{
  return playBufferData[id];
}

/******************************************************************************/
/**
*******************************************************************************/

uint32_t Emphase_IPC_KeyBuffer_GetSize()
{
  return EMPHASE_IPC_KEYBUFFER_SIZE;
}
