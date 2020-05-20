/******************************************************************************/
/** @file		nl_coos.c
	@date		2015-01-30
    @author		[2013-07-07 DTZ]
*******************************************************************************/
#include "sys/nl_coos.h"
#include "drv/nl_dbg.h"
#include "sys/nl_status.h"
#include "ipc/emphase_ipc.h"

#define COOS_MAX_TASKS 16  // max number of task the COOS should handle (memory size)

#define LOG_TASK_TIME   (1)
#define DGB_TIMING_PINS (0)

#if DGB_TIMING_PINS
static void DispatchTotalTime(int on)
{
  if (on)
    DBG_GPIO3_2_On();
  else
    DBG_GPIO3_2_Off();
}

static void DispatchTaskTime(int on)
{
  if (on)
    DBG_GPIO3_3_On();
  else
    DBG_GPIO3_3_Off();
}
#endif

typedef struct
{
  void (*pTask)(void);  // pointer to the task
  int32_t countDown;    // delay (ticks) until the function will (next) be run
  int32_t period;       // interval (ticks) between subsequent run
  int32_t run;          // incremented by the scheduler when task is due to execute
#if LOG_TASK_TIME
  int32_t max_time;  // maximum time in systicks this task did take;
#endif
} sTask;

sTask COOS_taskArray[COOS_MAX_TASKS];  // array for the tasks

volatile int32_t taskOverflow = 0;  // signals a task overflow => turn on warning led

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
  COOS_taskArray[index].countDown = phase + 1;
  COOS_taskArray[index].period    = period;
  COOS_taskArray[index].run       = 0;
#if LOG_TASK_TIME
  COOS_taskArray[index].max_time = 0;
#endif
  COOS_taskArray[index].pTask = taskName;
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
#if LOG_TASK_TIME
    COOS_taskArray[taskIndex].max_time = 0;
#endif
    return 0;  // everything ok
  }
}

void COOS_Start(void)
{
  // enable interrupts - start systick
}

/******************************************************************************/
/** @brief    	The dispatcher will run the registered tasks
    @param[]
    @return
*******************************************************************************/
void COOS_Dispatch(void)
{
  uint16_t index;
  uint16_t tasks = 0;

  uint32_t dispatchTime = s.ticker;

#if DGB_TIMING_PINS
  DispatchTotalTime(1);
#endif
  for (index = 0; index < COOS_MAX_TASKS; index++)  // run the next task (if one is ready)
  {
    if (COOS_taskArray[index].run > 0)
    {
#if DGB_TIMING_PINS
      DispatchTaskTime(1);
#endif
      uint32_t taskTime = s.ticker;
      (*COOS_taskArray[index].pTask)();  // run the task
      COOS_taskArray[index].run--;       // decrease the run flag, so postponed tasks will also be handled
      taskTime = s.ticker - taskTime;
      if (taskTime > NL_systemStatus.COOS_maxTaskTime)
        NL_systemStatus.COOS_maxTaskTime = taskTime;
#if DGB_TIMING_PINS
      DispatchTaskTime(0);
#endif
#if LOG_TASK_TIME
      if (taskTime > COOS_taskArray[index].max_time)
        COOS_taskArray[index].max_time = taskTime;
#endif
      tasks++;

      if (COOS_taskArray[index].period == 0)  // if one shot task: remove from taskArray
      {
        COOS_Task_Delete(index);
      }
    }
  }
  if (tasks > NL_systemStatus.COOS_maxTasksPerSlice)
    NL_systemStatus.COOS_maxTasksPerSlice = tasks;
  dispatchTime = s.ticker - dispatchTime;
  if (dispatchTime > NL_systemStatus.COOS_maxDispatchTime)
    NL_systemStatus.COOS_maxDispatchTime = dispatchTime;

  if (taskOverflow)
  {
    DBG_Led_Warning_TimedOn(3);
    taskOverflow = 0;
  }
#if DGB_TIMING_PINS
  DispatchTotalTime(0);
#endif
}

/******************************************************************************/
/** @brief    	Calculates when a task is due to run and sets the run flag when
 	 	 	 	it is. It will not execute any taks!!!
	@note		This function must be called every sysTick
*******************************************************************************/
void COOS_Update(void)
{
  for (int index = 0; index < COOS_MAX_TASKS; index++)  // calculations are made in sysTicks
  {
    if (COOS_taskArray[index].pTask)  // check for registered task
    {
      COOS_taskArray[index].countDown--;

      if (COOS_taskArray[index].countDown <= 0)  // check if task is due to run / <0 for one shot tasks
      {
        COOS_taskArray[index].run++;        // yes, task is due to run -> increase run-flag
        if (COOS_taskArray[index].run > 1)  // any task pending more than once ?
        {
          taskOverflow = 1;
          if (NL_systemStatus.COOS_totalOverruns < 0xFFFF)
            NL_systemStatus.COOS_totalOverruns++;
        }
        if (COOS_taskArray[index].period >= 1)
        {  // schedule periodic task to run again
          COOS_taskArray[index].countDown = COOS_taskArray[index].period;
        }
      }
    }
  }
}

// EOF
