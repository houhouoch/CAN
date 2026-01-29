#include "Task_Manager.h"

//接口锁
static osMutexId_t Interface_MutexHandle;
static const osMutexAttr_t Interface_Mutex_attributes = {
    .name = "Interface_Mutex",
    .attr_bits = osMutexRecursive,
};

//消息弹窗
static osMessageQueueId_t MsgBox_Queue = NULL;
static const osMessageQueueAttr_t MsgBoxQueue_attributes = {
    .name = "MsgBoxQueue",
};


osMessageQueueId_t Debug_MsgBox_Queue = NULL;
static const osMessageQueueAttr_t Debug_MsgBoxQueue_attributes = {
    .name = "Debug_MsgBoxQueue",
};

//SCPI采样请求
static osMessageQueueId_t SCPI_SampleReq_Queue = NULL;
static const osMessageQueueAttr_t SCPI_SampleReqQueue_attributes = {
    .name = "SCPISampleReqQueue",
};

osMutexId_t COM_MutexHandle = NULL;
const osMutexAttr_t COM_Mutex_attributes = {
    .name = "COM_Mutex",
    .attr_bits = osMutexRecursive,
};


enum {
    osFlags_SCPI_Sample = (1 << 0),
    osFlags_SCPI_CAN_RX = (1 << 1),
};

void TaskManager_Init(void)
{
    Interface_MutexHandle = osMutexNew(&Interface_Mutex_attributes);
    MsgBox_Queue = osMessageQueueNew(5, sizeof(Msg_Def), &MsgBoxQueue_attributes);
    //DEBUG
    Debug_MsgBox_Queue = osMessageQueueNew(50, sizeof(Msg_Def), &Debug_MsgBoxQueue_attributes);
    SCPI_SampleReq_Queue = osMessageQueueNew(10, sizeof(osThreadId_t), &SCPI_SampleReqQueue_attributes);
    COM_MutexHandle = osMutexNew(&COM_Mutex_attributes);
}



/**
  * @brief
  * @retval SCPI采样请求
  */
void __SCPI_Sample_Acquire(uint32_t timeout) {
    osThreadId_t threadID = osThreadGetId();
    //user_printf(__func__, "__SCPI_Sample_Acquire:0x%#x\r\n",threadID);
    osThreadFlagsClear(osFlags_SCPI_Sample);
    osMessageQueuePut(SCPI_SampleReq_Queue, &threadID, 0, 0);
    osThreadFlagsWait(osFlags_SCPI_Sample, osFlagsWaitAll, timeout);
}

void __SCPI_Sample_Release(void) {
    osThreadId_t threadID = NULL;
    while(1) {
        if(osOK == osMessageQueueGet(SCPI_SampleReq_Queue, &threadID, 0, 0)) {
            //user_printf(__func__, "__SCPI_Sample_Release:0x%#x\r\n",threadID);
            osThreadFlagsSet(threadID, osFlags_SCPI_Sample);
        } else {
            break;
        }
    }
}


void __Interface_Lock(void) {
    //    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        osStatus_t res = osMutexAcquire(Interface_MutexHandle, 3000);
        if(res == osErrorTimeout) {
            user_printf(__func__, "Interface Lock Fail %d", res);
            osMutexAcquire(Interface_MutexHandle, osWaitForever);
        }
    }
}
void __Interface_Unlock(void) {
    //    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        osMutexRelease(Interface_MutexHandle);
    }
}


static osMessageQueueId_t CAN_Rx_Queue = NULL;
static const osMessageQueueAttr_t CAN_Rx_Queue_attributes = {
    .name = "CAN_Rx_Queue",
};

static void __CAN_Rx_Queue_Create(void)
{
    CAN_Rx_Queue = osMessageQueueNew(100, sizeof(User_CAN_Rx_Msg_Def), &CAN_Rx_Queue_attributes);
}
__IO uint16_t can_count = 0;
/*
 * @brief       CAN IRQ Rx消息队列
 * @details
 */
void CAN_Rx_Queue_Send(User_CAN_Rx_Msg_Def *msg)
{
    if(CAN_Rx_Queue == NULL) {
        __CAN_Rx_Queue_Create();
    }
    if(msg == NULL) {
        return ;
    }
    //中断中发送不能等待
    osMessageQueuePut(CAN_Rx_Queue, msg, 0, 10);
    can_count += 1;
}

uint8_t CAN_Rx_Queue_Rec(User_CAN_Rx_Msg_Def *msg)
{
    if(CAN_Rx_Queue == NULL) {
        __CAN_Rx_Queue_Create();
    }
    if(msg == NULL) {
        return 0;
    }
    osStatus_t result = osMessageQueueGet(CAN_Rx_Queue, msg, NULL, 500);
    if(osOK == result) {
        can_count -= 1;
    }
    return (osOK == result);
}










static osMessageQueueId_t CAN_Rx_RequestQueue = NULL;
static const osMessageQueueAttr_t CAN_Rx_RequestQueue_attributes = {
    .name = "CAN_Rx_Queue",
};


/*
 * @brief       事件队列
 * @details
 */
void CAN_RX_RequestQueue_Send(CAN_Rx_RequestQueue_t *requestQueue)
{
    if(CAN_Rx_RequestQueue == NULL) {
        CAN_Rx_RequestQueue = osMessageQueueNew(10, sizeof(CAN_Rx_RequestQueue_t), &CAN_Rx_RequestQueue_attributes);
    }
    if(requestQueue == NULL) {
        return ;
    }
    osThreadFlagsClear(osFlags_SCPI_CAN_RX);
    osMessageQueuePut(CAN_Rx_RequestQueue, requestQueue, 0, 5);
}

uint8_t CAN_RX_RequestQueue_Wait(uint16_t timeout) {
    uint8_t result = (0 != (osFlags_SCPI_CAN_RX & osThreadFlagsWait(osFlags_SCPI_CAN_RX, osFlagsWaitAll, timeout)));
    osThreadFlagsClear(osFlags_SCPI_CAN_RX);
    return result;
}

uint8_t CAN_RX_RequestQueue_Rec(uint32_t id, const uint8_t *pdata, uint16_t len)
{
    if(CAN_Rx_RequestQueue == NULL) {
        CAN_Rx_RequestQueue = osMessageQueueNew(10, sizeof(CAN_Rx_RequestQueue_t), &CAN_Rx_RequestQueue_attributes);
    }
    CAN_Rx_RequestQueue_t requestQueue = {0};
    if(osOK == osMessageQueueGet(CAN_Rx_RequestQueue, &requestQueue, NULL, 0)) {
        if(id == requestQueue.canId) {
            memcpy(requestQueue.pdata, pdata, len);
            *requestQueue.len = len;
            osThreadFlagsSet(requestQueue.threadID, osFlags_SCPI_CAN_RX);
            return osOK;
        }
    }
    return osError;
}


static osMessageQueueId_t CAN_Packet_RequestQueue = NULL;
static const osMessageQueueAttr_t CAN_Packet_RequestQueue_attributes = {
    .name = "CAN_Packet_Queue",
};

/*
 * @brief       事件队列
 * @details
 */
#if 0
void CAN_Packet_RequestQueue_Send(uint32_t id)
{
    if(CAN_Packet_RequestQueue == NULL) {
        CAN_Packet_RequestQueue = osMessageQueueNew(10, sizeof(CAN_Packet_RequestQueue_t), &CAN_Packet_RequestQueue_attributes);
    }
    CAN_Packet_RequestQueue_t request = {0};
    request.canId = id;
    request.threadID = osThreadGetId();
    osMessageQueuePut(CAN_Rx_RequestQueue, &request, 0, 5);
}

void CAN_Packet_RequestQueue_Wait(uint16_t timeout) {
    osThreadFlagsWait(osFlags_SCPI_CAN_RX, osFlagsWaitAll, timeout);
}

uint8_t CAN_Packet_RequestQueue_Rec(uint32_t id)
{
    if(CAN_Rx_RequestQueue == NULL) {
        CAN_Rx_RequestQueue = osMessageQueueNew(10, sizeof(CAN_Packet_RequestQueue_t), &CAN_Rx_RequestQueue_attributes);
    }
    static CAN_Packet_RequestQueue_t request = {0};
    if(osOK == osMessageQueueGet(CAN_Rx_RequestQueue, &request, NULL, 0)) {
        if(id == request.canId) {
            osThreadFlagsSet(request.threadID, osFlags_SCPI_CAN_RX);
            return 1;
        }
    }
}



#else
void CAN_Packet_RequestQueue_Send(CAN_Packet_RequestQueue_t *requestPacket)
{
    if(CAN_Packet_RequestQueue == NULL) {
        CAN_Packet_RequestQueue = osMessageQueueNew(10, sizeof(CAN_Packet_RequestQueue_t), &CAN_Packet_RequestQueue_attributes);
    }
    if(requestPacket == NULL) {
        return ;
    }
    osMessageQueuePut(CAN_Rx_RequestQueue, requestPacket, 0, 5);
}

void CAN_Packet_RequestQueue_Wait(uint16_t timeout) {
    osThreadFlagsWait(osFlags_SCPI_CAN_RX, osFlagsWaitAll, timeout);
}


CAN_Packet_RequestQueue_t requestTable[20] = {0};
uint8_t CAN_Packet_RequestQueue_Rec(uint32_t id, User_CAN_Rx_Msg_Def *msgPacket)
{
    if(CAN_Rx_RequestQueue == NULL) {
        CAN_Rx_RequestQueue = osMessageQueueNew(10, sizeof(CAN_Packet_RequestQueue_t), &CAN_Rx_RequestQueue_attributes);
    }
    CAN_Packet_RequestQueue_t requestQueue = {0};
    if(osOK == osMessageQueueGet(CAN_Rx_RequestQueue, &requestQueue, NULL, 0)) {
        if(id == requestQueue.canId) {
            memcpy(requestQueue.pCANPacket, msgPacket, sizeof(User_CAN_Rx_Msg_Def));
            osThreadFlagsSet(requestQueue.threadID, osFlags_SCPI_CAN_RX);
            return osOK;
        }
    }
    return osError;
}
#endif




//接口锁
static osMutexId_t CAN_SCPI_MutexHandle;
static const osMutexAttr_t CAN_SCPI_Mutex_attributes = {
    .name = "CAN_SCPI_Mutex",
    .attr_bits = osMutexRecursive,
};

void __CAN_SCPI_Lock(void) {
    if(CAN_SCPI_MutexHandle == NULL) {
        CAN_SCPI_MutexHandle = osMutexNew(&CAN_SCPI_Mutex_attributes);
    }
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        osStatus_t res = osMutexAcquire(CAN_SCPI_MutexHandle, 3000);
        if(res == osErrorTimeout) {
            user_printf(__func__, "CAN SCPI Lock Fail %d", res);
            osMutexAcquire(CAN_SCPI_MutexHandle, osWaitForever);
        }
    }
}
void __CAN_SCPI_Unlock(void) {
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        osMutexRelease(CAN_SCPI_MutexHandle);
    }
}



/*
 * @brief       提示框消息队列
 * @details
 */
void MsgQueue_Send(Msg_Def *msg)
{
    if(msg == NULL) {
        return ;
    }
    osMessageQueuePut(MsgBox_Queue, msg, 0, 0);
}

uint8_t MsgQueue_Rec(Msg_Def *msg)
{
    if(MsgBox_Queue == NULL) {
        return 0;
    }
    if(msg == NULL) {
        return 0;
    }
    return (osOK == osMessageQueueGet(MsgBox_Queue, msg, NULL, 0));
}

uint8_t MsgQueue_Length(void)
{
    if(MsgBox_Queue == NULL) {
        return 0;
    }
    return osMessageQueueGetCount(MsgBox_Queue);
}

















/*
 * @brief       提示框消息队列
 * @details
 */
void Debug_MsgQueue_Send(Msg_Def *msg, const char *funName)
{
    if(Debug_MsgBox_Queue == NULL) { return; }
    if(msg == NULL) {
        return ;
    }
    //写入线程名字
    msg->runTime_ms = PowerInfo.Read.runTime_ms;
    msg->funcName = funName;
    msg->threadName = osThreadGetName(osThreadGetId());
    if(osOK != osMessageQueuePut(Debug_MsgBox_Queue, msg, 0, 0)) {
        vPortFree(msg->data.msg);
    }
}

uint8_t Debug_MsgQueue_Rec(Msg_Def *msg)
{
    if(Debug_MsgBox_Queue == NULL) {
        return 0;
    }
    if(msg == NULL) {
        return 0;
    }
    return (osOK == osMessageQueueGet(Debug_MsgBox_Queue, msg, NULL, 0));
}

uint8_t Debug_MsgQueue_Length(void)
{
    if(Debug_MsgBox_Queue == NULL) {
        return 0;
    }
    return osMessageQueueGetCount(Debug_MsgBox_Queue);
}







