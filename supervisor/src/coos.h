/******************************************************************************/
/** @file		coos.h
    @author		KSTR
    @brief		COOS co-operative scheduler
				- 7 byte of memory for each task is needed
	@details	If a task takes longer than the sysTick-time this will not
				affect the systems stability. But the next task will be delayed.
				So this results in task jitter.
	@note 		Task overlaps can be prevented by using the phase variable of
				COOS_Task_Add();
*******************************************************************************/
#ifndef NL_SYS_COOS_H_
#define NL_SYS_COOS_H_

#include "stdint.h"

void coos_init(void);

int8_t coos_add_task(void (*task_name)(), uint16_t offset, int16_t period);
int8_t coos_delete_task(const int8_t task_index);
void   coos_dispatch(void);
void   coos_update(void);

#endif
