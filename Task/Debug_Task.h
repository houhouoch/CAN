#ifndef _DEBUG_TASK_H_
#define _DEBUG_TASK_H_

#include "Commd.h"
#include "usart.h"
#include "Task_Manager.h"

/*
 * @brief       osThreadId_t Debug_Task_Create(void)
 * @details     创建调试串口通讯任务
 */
osThreadId_t Debug_Task_Create(void);


#endif
