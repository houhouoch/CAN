#ifndef _CAN_RX_TASK_H_
#define _CAN_RX_TASK_H_

#include "Commd.h"
#include "FDCAN.h"
#include "Task_Manager.h"


void __On_Slave_Connected_Changed(uint8_t connect);
/*
 * @brief       osThreadId_t CAN_Rx_Task_Create(void)
 */
osThreadId_t CAN_Rx_Task_Create(void);


extern __IO uint8_t can_m2s_ack;
extern __IO uint8_t can_s2m_ack;


#endif
