#ifndef _COM_TASK_H_
#define _COM_TASK_H_

#include "Commd.h"
#include "usart.h"
#include "Task_Manager.h"

/*
 * @brief       osThreadId_t COM_Task_Create(void)
 * @details     创建串口通讯任务
 */
osThreadId_t COM_Task_Create(void);



#endif
