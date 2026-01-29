#include "COM_Task.h"
#include "Modbus_master_IO.h"
#include "Modbus_master_Power.h"
#include "Modbus_master_List.h"
#include "Modbus_master_Delayer.h"
#include "Modbus_master_BatTester.h"

//任务
const osThreadAttr_t COM_attributes = {
    .name = "COM_Task",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};

//信号量
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
osStaticSemaphoreDef_t COM_Recv_IRQControlBlock;
const osSemaphoreAttr_t COM_Recv_IRQ_attributes = {
    .name = "COM_Recv_IRQ",
    .cb_mem = &COM_Recv_IRQControlBlock,
    .cb_size = sizeof(COM_Recv_IRQControlBlock),
};
static osSemaphoreId_t COM_Recv_Semaph = {0};



static void COM_Task(void *argument);



//
#define UART_BUFFER_LENGTH 256
uint8_t USART5_Rx_DMA_Buffer[UART_BUFFER_LENGTH] = {0};
static uint8_t USART5_Rx_Buffer[UART_BUFFER_LENGTH] = {0};
static uint8_t USART5_Rx_Buffer_Length = 0;
void __onUSART5_RxCplt(void)
{
    //停止DMA
    HAL_UART_DMAStop(&huart5);
    //接收了多长的数据
    uint16_t dma_rx_len = UART_BUFFER_LENGTH - __HAL_DMA_GET_COUNTER(huart5.hdmarx);
    if(dma_rx_len > 0) {
        if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        {
            if(COM_Recv_Semaph != NULL) {
                if(USART5_Rx_Buffer_Length == 0) {
                    memcpy(USART5_Rx_Buffer, USART5_Rx_DMA_Buffer, dma_rx_len);
                    USART5_Rx_Buffer_Length = dma_rx_len;
                } else {
                    while(1);
                }
                //任务处理
                osSemaphoreRelease(COM_Recv_Semaph);
            }
        }
        memset(USART5_Rx_DMA_Buffer, 0, sizeof(USART5_Rx_DMA_Buffer));
    }
}

static void __Power_Reset(void) {
    HAL_GPIO_WritePin(Power_Boot_GPIO_Port, Power_Boot_Pin, GPIO_PIN_RESET);
    osDelay(50 / portTICK_PERIOD_MS);
    HAL_GPIO_WritePin(Power_NRST_GPIO_Port, Power_NRST_Pin, GPIO_PIN_RESET);
    osDelay(50 / portTICK_PERIOD_MS);
    HAL_GPIO_WritePin(Power_NRST_GPIO_Port, Power_NRST_Pin, GPIO_PIN_SET);
}



/*
 * @brief       osThreadId_t COM_Task_Create(void)
 * @details     创建串口通讯任务
 */
osThreadId_t COM_Task_Create(void)
{
    COM_Recv_Semaph = osSemaphoreNew(5, 0, &COM_Recv_IRQ_attributes);
    {
        __Power_Reset();
        __HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
    }
    osThreadId_t hreadId = NULL;
    hreadId = osThreadNew(COM_Task, (void *)&COM_attributes, &COM_attributes);
    return hreadId;
}







#include "gui_guider.h"
static void COM_Task(void *argument)
{
    User_Task_t thisTask = {0};
    if(argument != NULL) {
        thisTask.threadId = osThreadGetId();
        thisTask.attributes = (const osThreadAttr_t *)argument;
    }
    do {
        if(MB_OK == Modbus_Read_ModelInfo()) {
            PowerInfo.StateBar.bits.Power_Link = 1;
        } else {
            osDelay(50);
        }
        //连续20次通讯不上,则复位单片机
        static uint8_t failedCount = 0;
        if(failedCount < 100) {
            failedCount++;
        } else {
            __Power_Reset();
            failedCount = 0;
            osDelay(1000);
        }
    } while(PowerInfo.StateBar.bits.Power_Link == 0);
    //
    while(1)
    {
        WDG_Task_Feed(WDG_FLAG_COM);
        thisTask = thisTask;
        thisTask.startHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        void Tick(TickType_t ms);
        Tick(10);
    }
}



uint16_t comRunTime = 0;
static TickType_t tick[2] = {0};
TickType_t waittime;
void COM_Mutex_Lock(void)
{
    tick[0] = HAL_GetTick();
    //    vTaskDelayUntil(&waittime,5);
    osStatus_t res = osMutexAcquire(COM_MutexHandle, 3000);
    if(res == osErrorTimeout) {
        user_printf(__func__, "COM Lock Failed %d", res);
        osMutexAcquire(COM_MutexHandle, osWaitForever);
    }
}

void COM_Mutex_Unlock(void)
{
    osMutexRelease(COM_MutexHandle);
    tick[1] = HAL_GetTick();
    comRunTime = tick[1] - tick[0];
}


uint8_t *Modbus_Receive(void)
{
    if(osSemaphoreAcquire(COM_Recv_Semaph, 100 / portTICK_PERIOD_MS) == osOK)
    {
        if(USART5_Rx_Buffer_Length > 0) {
            uint8_t *pbuffer = USART5_Rx_Buffer;
            uint16_t buffer_length = USART5_Rx_Buffer_Length;
            //计算CRC
            if(userCRC_Calculate(pbuffer, buffer_length) == 0) {
                //核对地址和功能码
                if(MB_SLAVE_ADDR == pbuffer[0])  {
                    return pbuffer;
                }
            }
        }
    }
    return NULL;
}

void Modbus_Clean_Receive(void)
{
    //清空接收缓存区
    USART5_Rx_Buffer_Length = 0;
    memset(USART5_Rx_Buffer, 0, sizeof(USART5_Rx_Buffer));
    HAL_UART_Receive_DMA(&huart5, USART5_Rx_DMA_Buffer, UART_BUFFER_LENGTH);
}

typedef struct {
    uint8_t (*mb_funtion)(void); //Modbus函数
    const char *functionName;    //项目名称
    uint32_t period;            //工作周期,单位:ms
    uint8_t enable;             //使能
    int32_t nextTime;
} Modbus_Table_Def;

Modbus_Table_Def  Modbus_Table[] = {
    {
        .mb_funtion = Modbus_Read_PowerState,
        .functionName = "获取输出状态",
        .period = 20,
        .enable = 1,  //打开
    }, {
        .mb_funtion = Modbus_Read_PowerOutSet,
        .functionName = "获取实际设置参数",
        .period = 100,
        .enable = 1,  //打开
    }, {
        .mb_funtion = Modbus_Read_PowerOut,
        .functionName = "获取输出回读",
        .period = 20,
        .enable = 1,  //打开
    }, {
        .mb_funtion = Modbus_Read_Temperature,
        .functionName = "获取当前温度",
        .period = 3000,
        .enable = 1,  //打开
    }, {
        .mb_funtion = Modbus_Read_List,
        .functionName = "获取List回读",
        .period = 50,
        .enable = 1,  //打开
    }, {
        .mb_funtion = Modbus_Read_Delayer,
        .functionName = "获取Delayer回读",
        .period = 50,
        .enable = 1,  //打开
    }, {
        .mb_funtion = Modbus_Read_BatTester,
        .functionName = "获取BatTester回读",
        .period = 50,
        .enable = 1,  //打开
    }, {
        .mb_funtion = Modbus_BatTester_Get_ErrParam,
        .functionName = "获取BatTester异常数据",
        .period = 50,
        .enable = 1,  //打开
    },



    {
        .mb_funtion = Modbus_Calib_Read_Count,
        .functionName = "获取校准点数",
        .period = 1000,
        .enable = 1,  //打开
    },
    {
        .mb_funtion = Modbus_Read_ModelInfo,
        .functionName = "获取模组信息",
        .period = 1000,
        .enable = 1,  //打开
    },
    {
        .mb_funtion = Modbus_Read_Model_Data,
        .functionName = "获取模组数据",
        .period = 500,
        .enable = 1,  //打开
    },
};

#include "MsgBox.h"

void Tick(TickType_t ms)
{
    static TickType_t tickCount = 0;
    vTaskDelayUntil(&tickCount, ms / portTICK_PERIOD_MS);
    enum {
        COM_STATE_DISLINKED = 0,
        COM_STATE_LINKED,
    };
    static uint8_t COM_state = COM_STATE_DISLINKED;
    #if 1
    #else
    COM_state = COM_STATE_LINKED;
    #endif
    static uint16_t dislinkedCount = 0;
    #if 1
    switch(COM_state)
    {
        case COM_STATE_DISLINKED: {
            osDelay(100 / portTICK_PERIOD_MS);
            if(MB_OK != Modbus_Update_OutSet()) { break; }
            #ifdef __BAT_TESTER_EN
            if(MB_OK != Modbus_BatTester_UpdateSet()) { break; }
            #endif
            if(MB_OK != Modbus_List_UpdateSet()) { break; }
            if(MB_OK != Modbus_Delayer_UpdateSet()) { break; }
            if(MB_OK != Modbus_Read_ModelInfo()) { break; }
            COM_state = COM_STATE_LINKED;
            State_Def status_temp = {0};
            status_temp.bits.dislinked = 1;
            Modbus_Clean_PowerState(status_temp.byte);
            PowerInfo.StateBar.bits.Power_Link = 1;
            dislinkedCount = 0;
        }
        break;
        case COM_STATE_LINKED: {
            #if 1
            if(PowerInfo.State.bits.dislinked == 1) {
                if(dislinkedCount > 50) {
                    COM_state = COM_STATE_DISLINKED;
                } else {
                    dislinkedCount++;
                }
            } else {
                dislinkedCount = 0;
            }
            #endif
            #if 1
            for(uint8_t i = 0; i < ARRAY_COUNT(Modbus_Table);)
            {
                Modbus_Table_Def *pItem = &Modbus_Table[i];
                if((pItem->enable != 0) && (pItem->mb_funtion != NULL))
                {
                    pItem->nextTime -= ms;
                    if(pItem->nextTime > 0) {
                        i++;
                    } else {
                        pItem->nextTime = pItem->period;
                        //
                        uint8_t result = 0;
                        //                        COM_Mutex_Lock();
                        //                        {
                        result = pItem->mb_funtion();
                        //                        }
                        //                        COM_Mutex_Unlock();
                        #if 0
                        i++;
                        #else
                        if(result == MB_OK) {
                            i++;
                        } else {
                            break;
                        }
                        #endif
                    }
                } else {
                    i++;
                }
            }
            #endif
            uint32_t errorCount = Modbus_Get_ErrorCount();
            if(errorCount != 0)
            {
                PowerInfo.StateBar.bits.Power_Link = 0;
                COM_state = COM_STATE_DISLINKED;
                Modbus_Clean_ErrorCount();
                //MsgBox_Printf(0, NULL, "Power Link Failed!");
            }
        }
        break;
    }
    #endif
}


