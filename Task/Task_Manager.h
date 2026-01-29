#ifndef _TASK_MANAGER_H_
#define _TASK_MANAGER_H_
#include "Commd.h"
#include "cmsis_os.h"
#include "freertos.h"
#include "task.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

void __Interface_Lock(void);
void __Interface_Unlock(void);

extern osThreadId_t CAN_Tx_Task_ThreadID;
extern osThreadId_t CAN_Rx_Task_ThreadID;
extern osThreadId_t CAN_IRQ_Task_ThreadID;


#include "iwdg.h"
extern osThreadId_t wdg_task_thread;
enum {
    WDG_FLAG_COM        = (1 << 0),
    WDG_FLAG_GUI        = (1 << 1),
    WDG_FLAG_CAN_IRQ    = (1 << 2),
    WDG_FLAG_CAN_RX     = (1 << 3),
    WDG_FLAG_CAN_TX     = (1 << 4),

    WDG_FLAG_USB_TMC    = (1 << 5),
    //    WDG_FLAG_NORMAL     = (1 << 6),
    //    WDG_FLAG_NET_RPC    = (1 << 7),
    //    WDG_FLAG_NET_RPC_UDP= (1 << 8),
};

void WDG_Task_Feed(uint32_t flag);
void WDG_Task_Read(void);

typedef struct {
    osThreadId_t threadId;              //任务ID
    const osThreadAttr_t *attributes;   //任务创建信息
    uint32_t startHighWaterMark;        //最大使用堆栈线
} User_Task_t;

/**
  * @brief
  * @retval SCPI采样请求
  */
void __SCPI_Sample_Acquire(uint32_t timeout);
void __SCPI_Sample_Release(void);


typedef struct {
    FDCAN_RxHeaderTypeDef header;
    uint8_t data[8];
} User_CAN_Rx_Msg_Def;
/*
 * @brief       Rx消息队列
 * @details
 */
void CAN_Rx_Queue_Send(User_CAN_Rx_Msg_Def *msg);
uint8_t CAN_Rx_Queue_Rec(User_CAN_Rx_Msg_Def *msg);




extern osMutexId_t COM_MutexHandle;





/*
 * @brief
 * @details     消息提示框结构体
 */
typedef struct {
    uint64_t runTime_ms;
    const char *threadName;
    const char *funcName;
    uint8_t type;
    uint8_t *flag;
    struct {
        char *msg;
    } data;
} Msg_Def;

extern osMessageQueueId_t Msg_Queue;

void TaskManager_Init(void);


//CAN发送队列
#include "fdcan.h"
typedef struct {
    uint32_t canId;
    uint8_t *pdata;
    uint16_t *len;
    osThreadId_t threadID;
} CAN_Rx_RequestQueue_t;
void CAN_RX_RequestQueue_Send(CAN_Rx_RequestQueue_t *requestQueue);
uint8_t CAN_RX_RequestQueue_Wait(uint16_t timeout);
uint8_t CAN_RX_RequestQueue_Rec(uint32_t id, const uint8_t *pdata, uint16_t len);

//CAN数据包请求队列
typedef struct {
    uint32_t canId;
    osThreadId_t threadID;
    User_CAN_Rx_Msg_Def *pCANPacket;
} CAN_Packet_RequestQueue_t;




void __CAN_SCPI_Lock(void);
void __CAN_SCPI_Unlock(void);

void MsgQueue_Send(Msg_Def *msg);
uint8_t MsgQueue_Rec(Msg_Def *msg);
uint8_t MsgQueue_Length(void);


void Debug_MsgQueue_Send(Msg_Def *msg, const char *funName);
uint8_t Debug_MsgQueue_Rec(Msg_Def *msg);
uint8_t Debug_MsgQueue_Length(void);

#endif
