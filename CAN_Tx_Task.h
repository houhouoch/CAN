#ifndef _CAN_TX_TASK_H_
#define _CAN_TX_TASK_H_

#include "Commd.h"
#include "FDCAN.h"
#include "Task_Manager.h"


/*
 * @brief       osThreadId_t CAN_Tx_Task_Create(void)
 */
osThreadId_t CAN_Tx_Task_Create(void);


#endif
