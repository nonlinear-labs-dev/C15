/******************************************************************************/
/**
	@date		2019-05-21
    @author		KSTR
*******************************************************************************/
#include "coos.h"
#include "pin_manipulation.h"
#include "globals.h"
#include "hardware.h"

#define COOS_MAX_TASKS (NO_OF_TASKS)  // max number of task the COOS should handle (memory size)

typedef struct
{
  void (*pTask)(void);  // pointer to the task
  int16_t countDown;    // delay (ticks) until the function will (next) be run
  int16_t period;       // interval (ticks) between subsequent run
  int16_t run;          // incremented by the scheduler when task is due to execute
} sTask;

sTask COOS_taskArray[COOS_MAX_TASKS];  // array for the tasks

/******************************************************************************/
/** @brief    	init everything with 0
*******************************************************************************/
void coos_init(void)
{
  uint8_t index;

  for (index = 0; index < COOS_MAX_TASKS; index++)
  {
    coos_delete_task(index);
  }
}

/******************************************************************************/
/** @brief		Function to add tasks to the task list
				- periodic tasks
				- one time tasks
    @param[in]  taskName - name of the task (function) that should be registered
	@param[in]	phase - one time delay in sysTicks to generate a phase (offset)
	@param[in]	period - interval in sysTicks between repeated executions of
				the task
				0: execute only once
				negative: do NOT accumulate pending runs
    @return		taskId - position in the taskArray
				-1: error
*******************************************************************************/
int8_t coos_add_task(void (*taskName)(), uint16_t offset, int16_t period)
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
  COOS_taskArray[index].countDown = offset + 1;
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
int8_t coos_delete_task(const int8_t taskIndex)
{
  if (taskIndex < 0 || taskIndex >= COOS_MAX_TASKS)
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

/******************************************************************************/
/** @brief    	The dispatcher will run the registered tasks
    @param[]
    @return
*******************************************************************************/
void coos_dispatch(void)
{
  uint8_t index;

  for (index = 0; index < COOS_MAX_TASKS; index++)  // run the next task (if one is ready)
  {
    if (COOS_taskArray[index].run > 0)
    {
      (*COOS_taskArray[index].pTask)();  // run the task
      COOS_taskArray[index].run--;       // decrease the run flag, so postponed tasks will also be handled

      if (COOS_taskArray[index].period == 0)  // if one shot task: remove from taskArray
      {
        coos_delete_task(index);
      }
    }
  }
}

/******************************************************************************/
/** @brief    	Calculates when a task is due to run and sets the run flag when
 	 	 	 	it is. It will not execute any tasks!!!
	@note		This function must be called every sysTick
*******************************************************************************/
void coos_update(void)
{
  uint8_t index;
  int32_t period;

  for (index = 0; index < COOS_MAX_TASKS; index++)  // calculations are made in sysTicks
  {
    if (COOS_taskArray[index].pTask)  // check for registered task
    {
      COOS_taskArray[index].countDown--;

      if (COOS_taskArray[index].countDown <= 0)  // check if task is due to run / < 0 for one shot tasks
      {
        period = COOS_taskArray[index].period;
        if (period > 0)  // accumulative task ?
        {
          COOS_taskArray[index].run++;  // task is due to run -> increase run-flag
        }
        else
        {
          COOS_taskArray[index].run = 1;  // task is due to run -> set run-flag
          period                    = -period;
        }

        if (period != 0)  // schedule periodic task to run again
          COOS_taskArray[index].countDown = period;
      }
    }
  }
}

// END OF FILE