/******************************************************************************/
/** @file		nl_tcd_valloc.c
    @date		2013-04-22
    @version	0.02
    @author		Stephan Schmitt[2012-07-03]
    @brief		<tbd>
    @ingroup	nl_tcd_modules
*******************************************************************************/

#include "nl_tcd_valloc.h"

#include "ipc/emphase_ipc.h"
#include "nl_tcd_poly.h"
#include "nl_tcd_msg.h"

//------- modul local variables

static IPC_KEY_EVENT_T keyEvent[32];  // array for new events read from the ring buffer for keybed events

static uint8_t numAllocVoices;  // number of voices that can be allocated - depending on unison

static uint8_t numAssigned;  // number of assigned voices

static uint8_t oldestAssigned;                     // index of the earliest assigned voice
static uint8_t youngestAssigned;                   // index of the last assigned voice
static uint8_t previousAssigned[NUM_VOICES] = {};  // pointers to the previous assigned voice
static uint8_t nextAssigned[NUM_VOICES]     = {};  // pointers to the next assigned voice

static uint8_t oldestReleased;                 // index of the earliest released voice
static uint8_t youngestReleased;               // index of the last released voice
static uint8_t nextReleased[NUM_VOICES] = {};  // pointers to the next released voice

static int8_t voiceState[NUM_VOICES] = {};  // 0 ... 60: pressed, number of the key, -1: released

/******************************************************************************
	@brief		VALLOC_Init: initialisation for the voice allocation variables
				and	arrays
	@param		num
*******************************************************************************/

void VALLOC_Init(uint32_t num)
{
  numAllocVoices = num;

  uint32_t i;

  for (i = 0; i < numAllocVoices; i++)
  {
    voiceState[i] = -1;  // all voices released, no voice assigned
  }

  numAssigned = 0;

  oldestAssigned   = 0;
  youngestAssigned = 0;

  oldestReleased   = 0;
  youngestReleased = numAllocVoices - 1;

  for (i = 0; i < numAllocVoices - 1; i++)
  {
    nextReleased[i] = i + 1;
  }
}

/******************************************************************************
	@brief		VALLOC_Process: reading new key events from the ring buffer,
				performing the voice allocation
				and calling the Start and Release functions of the envelopes

				The next key-on event will assigned to the oldest voice.
				If there are free voices available: it is the earliest released voice.
				If there are no free voices: it is the earliest assigned.

*******************************************************************************/

void VALLOC_Process(void)
{
  uint32_t i;
  uint32_t v;

  uint32_t numKeyEvents = Emphase_IPC_M4_KeyBuffer_ReadBuffer(keyEvent, 32);  // reads the latest key up/down events from the ring buffer shared with the M0

  for (i = 0; i < numKeyEvents; i++)
  {
    uint32_t k = keyEvent[i].key;

    if (keyEvent[i].direction == KEY_DIR_UP)  //--- releasing a key
    {
      for (v = 0; v < numAllocVoices; v++)
      {
        if (voiceState[v] == k)  // finding a voice that is assigned to the key
        {
          POLY_KeyUp(v, keyEvent[i].timeInUs);

          nextReleased[youngestReleased] = v;  // the last youngest released voice gets the pointer to this voice
          youngestReleased               = v;  // this voice is now the youngest released voice

          if (numAssigned == numAllocVoices)  // after a period of "overload"
          {
            oldestReleased = v;  // this voice is the first released
          }

          numAssigned--;

          if (oldestAssigned == v)  // sorting the remaining assigned voices
          {
            oldestAssigned = nextAssigned[v];  // list gets shorter at the left end
          }
          else if (youngestAssigned == v)
          {
            youngestAssigned = previousAssigned[v];  // list gets shorter at the right end
          }
          else
          {
            nextAssigned[previousAssigned[v]] = nextAssigned[v];      // connecting the left side of the gap to the right side
            previousAssigned[nextAssigned[v]] = previousAssigned[v];  // connecting the right side of the gap to the left side
          }

          voiceState[v] = -1;  // updating the voice state

          break;
        }
      }
    }
    else  //--- pressing a key
    {
      if (numAssigned < numAllocVoices)  // there is still a free voice
      {
        if (numAssigned == 0)  // all voices are free
        {
          oldestAssigned   = oldestReleased;  // reset oldest- and youngestAssigned
          youngestAssigned = oldestReleased;
        }

        v              = oldestReleased;   // we take the voice that has been released for the longest time
        oldestReleased = nextReleased[v];  // the second oldest released voice now becomes the oldest

        numAssigned++;
      }
      else  // all voices are assigned = "overload"
      {
        v = oldestAssigned;  // we "steal" the voice that has been assigned for the longest time

        oldestAssigned = nextAssigned[v];  // the second oldest assigned voice now becomes the oldest
      }

      POLY_KeyDown(v, k, keyEvent[i].timeInUs);

      nextAssigned[youngestAssigned] = v;                 // the last youngest assigned voice gets the pointer to this voice
      previousAssigned[v]            = youngestAssigned;  // and becomes its "previous assigned voice"
      youngestAssigned               = v;                 // the new assigned voice is now the youngest

      voiceState[v] = k;  // updating the voice state
    }
  }

  MSG_SendMidiBuffer();
}
