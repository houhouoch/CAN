#ifndef _CAN_TASK_H_
#define _CAN_TASK_H_

#include "Commd.h"
#include "FDCAN.h"
#include "Task_Manager.h"

/*
 * @brief       osThreadId_t CAN_Task_Create(void)
 * @details     创建CAN通讯任务
 */
osThreadId_t CAN_Task_Create(void);


#endif
