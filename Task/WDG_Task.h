#ifndef _WDG_TASK_H_
#define _WDG_TASK_H_

#include "Commd.h"
#include "Task_Manager.h"

/*
 * @brief       osThreadId_t WDG_Task_Create(void)
 * @details     创建看门狗WDG任务
 */
osThreadId_t WDG_Task_Create(void);

#endif
