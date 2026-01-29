#include "CAN_IRQ_Task.h"
#include "CAN_Rx_Task.h"
#include "FDCAN.h"

#include "CAN_Table.h"
const osThreadAttr_t CAN_IRQ_attributes = {
    .name = "CAN_IRQ_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityHigh + 1,
};
static void CAN_IRQ_Task(void *argument);

osThreadId_t CAN_IRQ_Task_ThreadID = NULL;

/*
 * @brief       osThreadId_t CAN_IRQ_Task_Create(void)
 */
osThreadId_t CAN_IRQ_Task_Create(void)
{
    CAN_IRQ_Task_ThreadID = osThreadNew(CAN_IRQ_Task, (void *)&CAN_IRQ_attributes, &CAN_IRQ_attributes);
    return CAN_IRQ_Task_ThreadID;
}

uint8_t CAN_RX_freeCount = 0;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
osStaticSemaphoreDef_t CAN_Recv_IRQControlBlock;
const osSemaphoreAttr_t CAN_Recv_IRQ_attributes = {
    .name = "CAN_Recv_IRQ",
    .cb_mem = &CAN_Recv_IRQControlBlock,
    .cb_size = sizeof(CAN_Recv_IRQControlBlock),
};
osSemaphoreId_t CAN_Recv_Semaph = NULL;



static void CAN_IRQ_Task(void *argument)
{
    if(argument != NULL) {
        //        thisTask.threadId = osThreadGetId();
        //        thisTask.attributes = (const osThreadAttr_t *)argument;
    }
    /* Infinite loop */
    if(CAN_Recv_Semaph == NULL) {
        CAN_Recv_Semaph = osSemaphoreNew(100, 0, &CAN_Recv_IRQ_attributes);
    }
    /* Activate Rx FIFO 0 new message notification on both FDCAN instances */
    if(HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        Error_Handler();
    }
    //    user_printf(__func__,"CAN RX IRQ Task is Running!");
    while(1)
    {
        WDG_Task_Feed(WDG_FLAG_CAN_IRQ);
        //接收CAN数据
        if(osSemaphoreAcquire(CAN_Recv_Semaph, 200) == osOK)
        {
            CAN_RX_freeCount = HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0);
            while(CAN_RX_freeCount)
            {
                /* Retrieve Rx messages from RX FIFO0 */
                FDCAN_RxHeaderTypeDef RxHeader1 = {0};
                static uint8_t can1_rxbuf[16] = {0};
                memset(can1_rxbuf, 0, sizeof(can1_rxbuf));
                if(HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader1, can1_rxbuf) != HAL_OK) {
                    Error_Handler();
                }
                if(RxHeader1.RxFrameType == FDCAN_REMOTE_FRAME) {
                    if(IF_Power_Get_Hw_ID() == 0)
                    {
                        if((RxHeader1.Identifier & CAN_SCPI_M2S_ID) == CAN_SCPI_M2S_ID) {
                            int32_t canid = RxHeader1.Identifier - CAN_SCPI_M2S_ID;
                            int8_t slaveid = canid / 0x100;
                            {
                                switch(RxHeader1.RxFrameType) {
                                    case FDCAN_REMOTE_FRAME:
                                        can_m2s_ack |= 1;
                                        break;
                                }
                            }
                        }
                    } else {
                        if((RxHeader1.Identifier & CAN_SCPI_S2M_ID) == CAN_SCPI_S2M_ID) {
                            //主机->从机
                            int32_t canid = RxHeader1.Identifier - CAN_SCPI_S2M_ID;
                            int8_t slaveid = canid / 0x100;
                            if(slaveid > 0)
                            {
                                if(slaveid == ((IF_Option_Get_CAN_Groups_ID() << 4) | IF_Power_Get_Hw_ID())) {
                                    switch(RxHeader1.RxFrameType) {
                                        case FDCAN_REMOTE_FRAME:
                                            can_s2m_ack |= 1;
                                            break;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    User_CAN_Rx_Msg_Def can_Rx_Msg = {0};
                    memcpy(&can_Rx_Msg.header, &RxHeader1, sizeof(FDCAN_RxHeaderTypeDef));
                    memcpy(can_Rx_Msg.data, can1_rxbuf, 8);
                    CAN_Rx_Queue_Send(&can_Rx_Msg);
                }
                CAN_RX_freeCount = HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0);
            }
        }
        //        thisTask.startHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        //        thisTask = thisTask;
    }
}



