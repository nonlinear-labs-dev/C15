/******************************************************************************/
/** @file		nl_coos.c
	@date		2015-01-30
    @author		[2013-07-07 DTZ]
*******************************************************************************/
#include <sys/nl_coos.h>
#include "drv/nl_dbg.h"

#define COOS_MAX_TASKS 48  // max number of task the COOS should handle (memory size)

typedef struct
{
  void (*pTask)(void);  // pointer to the task
  int32_t countDown;    // delay (ticks) until the function will (next) be run
  int32_t period;       // interval (ticks) between subsequent run
  int32_t run;          // incremented by the scheduler when task is due to execute
} sTask;

sTask COOS_taskArray[COOS_MAX_TASKS];  // array for the tasks

volatile int32_t taskOverflow      = 0;  // signals a task overflow => turn on warning led
volatile int32_t checkTaskOverflow = 0;
volatile uint8_t sleep             = 1;

/******************************************************************************/
/** @brief    	init everything with 0
*******************************************************************************/
void COOS_Init(void)
{
  uint8_t index;

  for (index = 0; index < COOS_MAX_TASKS; index++)
  {
    COOS_Task_Delete(index);
  }
}

/******************************************************************************/
/** @brief		Function to add tasks to the task list
				- periodic tasks
				- one time tasks
    @param[in]  taskName - name of the task (function) that should be registered
	@param[in]	phase - one time delay in sysTicks to generate a phase (offset)
	@param[in]	period - intervall in sysTicks between repeated execusions of
				the task
				0: execute only once
    @return		taskId - position in the taskArray
				-1: error
*******************************************************************************/
int32_t COOS_Task_Add(void (*taskName)(), uint32_t phase, uint32_t period)
{
  uint8_t index = 0;

  while ((COOS_taskArray[index].pTask != 0) && (index < COOS_MAX_TASKS))  // check for space in the task array
  {
    index++;
  }

  if (index == COOS_MAX_TASKS)  // is the end of the task list accomplished?
  {
    return -1;  // task list is full: return error
  }

  /* there is a space in the taskArray - add task */
  COOS_taskArray[index].pTask     = taskName;
  COOS_taskArray[index].countDown = phase + 1;
  COOS_taskArray[index].period    = period;
  COOS_taskArray[index].run       = 0;

  return index;  // so task can be deleted
}

/******************************************************************************/
/** @brief
    @param[in]	taskIndex
    			number of the task (id)
    @return		 0  everything ok
    			-1	error: no task at this location, nothing to delete
*******************************************************************************/
int32_t COOS_Task_Delete(const uint8_t taskIndex)
{
  if (COOS_taskArray[taskIndex].pTask == 0)
  {
    return -1;  // error: no task at this location, nothing to delete
  }
  else
  {
    /* delete task */
    COOS_taskArray[taskIndex].pTask     = 0x0000;
    COOS_taskArray[taskIndex].countDown = 0;
    COOS_taskArray[taskIndex].period    = 0;
    COOS_taskArray[taskIndex].run       = 0;
    return 0;  // everything ok
  }
}

void COOS_Start(void)
{
  // enable interrupts - start systick
}

void COOS_GoToSleep(void)
{
  // enter processor idle mode
}

void COOS_GoToArtificialSleep(void)
{
  while (sleep == 1)
    ;
  sleep = 1;
}

/******************************************************************************/
/** @brief    	The dispatcher will run the registered tasks
    @param[]
    @return
*******************************************************************************/
void COOS_Dispatch(void)
{
  //DBG_Pod(POD_2, ON);															// monitor the duration of the dispatch function

  uint8_t index;

  for (index = 0; index < COOS_MAX_TASKS; index++)  // run the next task (if one is ready)
  {
    if (COOS_taskArray[index].run > 0)
    {
      (*COOS_taskArray[index].pTask)();  // run the task
      COOS_taskArray[index].run--;       // decrease the run flag, so postponed tasks will also be handled

      if (COOS_taskArray[index].period == 0)  // if one shot task: remove from taskArray
      {
        COOS_Task_Delete(index);
      }
    }
  }

  //DBG_Pod(POD_2, OFF);														// monitor the duration of the dispatch function

  if (taskOverflow == 0)  // no task overflow -> everything all right -> goto sleep
  {
    sleep = 1;
    checkTaskOverflow--;
    COOS_GoToArtificialSleep();
  }
  else
  {
    taskOverflow--;  // task overflow -> try to catch up -> go an other round
  }
}

/******************************************************************************/
/** @brief    	Calculates when a task is due to run and sets the run flag when
 	 	 	 	it is. It will not execute any taks!!!
	@note		This function must be called every sysTick
*******************************************************************************/
void COOS_Update(void)
{
  uint8_t index, over = 0;
  sleep = 0;

#if 1  // check for task overrun
  if (checkTaskOverflow > 0)
  {
    taskOverflow++;  // error: Dispatch() took longer than one time slot
                     //		DBG_Led_Warning_On();
                     //		COOS_Task_Add(DBG_Led_Warning_Off, 20000, 0);							// stays on for 2.5 sec
  }
  else
  {
    checkTaskOverflow++;  // this flag must be reseted by Dispatch() before Dispatch is called again, otherwise: task overflow
  }
#endif

  for (index = 0; index < COOS_MAX_TASKS; index++)  // calculations are made in sysTicks
  {
    if (COOS_taskArray[index].pTask)  // check for registered task
    {
      COOS_taskArray[index].countDown--;

      if (COOS_taskArray[index].countDown <= 0)  // check if task is due to run / <0 for one shot tasks
      {
        COOS_taskArray[index].run++;        // yes, task is due to run -> increase run-flag
        if (COOS_taskArray[index].run > 1)  // any task pending more than once
          over = 1;
        if (COOS_taskArray[index].period >= 1)
        {  // schedule periodic task to run again
          COOS_taskArray[index].countDown = COOS_taskArray[index].period;
        }
      }
    }
  }
  if (over)
  {
    DBG_Led_Warning_On();
    COOS_Task_Add(DBG_Led_Warning_Off, 2000, 0);  // stays on for 250ms sec
  }
}
