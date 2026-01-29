#include "CAN_IRQ_Task.h"
#include "FDCAN.h"

#include "CAN_Table.h"

#include "UserQueue.h"
#include "MsgBox.h"

//任务
const osThreadAttr_t CAN_Tx_attributes = {
    .name = "CAN_Tx_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal + 1,
};
static void CAN_Tx_Task(void *argument);

osThreadId_t CAN_Tx_Task_ThreadID = NULL;
/*
 * @brief       osThreadId_t CAN_Tx_Task_Create(void)
 */
osThreadId_t CAN_Tx_Task_Create(void)
{
    CAN_Tx_Task_ThreadID = osThreadNew(CAN_Tx_Task, (void *)&CAN_Tx_attributes, &CAN_Tx_attributes);
    return CAN_Tx_Task_ThreadID;
}



/*
 * @brief       void Power_Set_Updata_CAN(void)
 * @details     同步更新电源设置到从机(仅主机发送)
 */
#include "CAN_Table.h"
__IO int16_t synccount = 0;
uint16_t syncTime[2] = {500 - 100, 500};
uint8_t CAN_Master_Set_Update(void)
{
    __Interface_Lock();
    if(synccount == 0)
    {
        uint8_t canUpdate_event = 0;
        canUpdate_event = PowerInfo.Read.canUpdata_event;
        PowerInfo.Read.canUpdata_event = 0;
        if(canUpdate_event != 0)
        {
            Modbus_Update_OutSet_temp();
            switch(IF_Power_Get_Power_Mode()) {
                case POWER_MODE_CAN_PARALLEL_MASTER:
                case POWER_MODE_CAN_SERIES_MASTER:
                case POWER_MODE_CAN_SYNERGY_MASTER: {
                    synccount = 100;
                }
                break;
                default:
                    synccount = 0;
            }
            switch(IF_Power_Get_Power_Mode())
            {
                    #if defined(MULT_CHANNEL)
                case POWER_MODE_CAN_SYNERGY_SLAVE: {
                    if(PowerInfo.Read.syncRequest != 0) {
                        CAN_OutSet_Synergy_Slave_Update();
                        PowerInfo.Read.syncRequest = 0;
                    }
                }
                break;
                #endif
                default:
                    break;
            }
            switch(IF_Power_Get_Power_Mode())
            {
                case POWER_MODE_CAN_PARALLEL_MASTER: {
                    CAN_OutSet_Parallel_Master_Update();
                }
                break;
                case POWER_MODE_CAN_SERIES_MASTER: {
                    CAN_OutSet_Series_Master_Update();
                }
                break;
                #if defined(MULT_CHANNEL)
                case POWER_MODE_CAN_SYNERGY_MASTER: {
                    CAN_OutSet_Synergy_Master_Update();
                }
                break;
                #endif
            }
        }
        if(PowerInfo.Read.syncRequest != 0) {
            PowerInfo.Read.syncRequest = 0;
            switch(IF_Power_Get_Power_Mode()) {
                case POWER_MODE_CAN_PARALLEL_MASTER:
                case POWER_MODE_CAN_SERIES_MASTER:
                case POWER_MODE_CAN_SYNERGY_MASTER: {
                    synccount = 100;
                }
                break;
                default:
                    synccount = 0;
            }
        }
    } else {
        synccount -= 50;
        if(synccount <= 0) {
            synccount = 0;
            //
            switch(IF_Power_Get_Power_Mode())
            {
                case POWER_MODE_CAN_PARALLEL_MASTER:
                case POWER_MODE_CAN_SERIES_MASTER:
                case POWER_MODE_CAN_SYNERGY_MASTER: {
                    gpio_set(&gpio_powerEN_out, 1);
                    osDelay(5);
                    gpio_set(&gpio_powerEN_out, 0);
                }
                break;
            }
        }
    }
    __Interface_Unlock();
}




//上报CAN数据
/*
POWER_MODE_NORMAL:不上报数据

POWER_MODE_CAN_PARALLEL_MASTER:
POWER_MODE_CAN_SERIES_MASTER:
POWER_MODE_CAN_SYNERGY_MASTER:
POWER_MODE_CAN_PARALLEL_SLAVE:
POWER_MODE_CAN_SERIES_SLAVE:
POWER_MODE_CAN_SYNERGY_SLAVE:上报数据
 */
uint8_t CAN_Device_Read_Update(void)
{
    switch(IF_Power_Get_Power_Mode())
    {
        case POWER_MODE_NORMAL:
            break;
        case POWER_MODE_CAN_PARALLEL_MASTER:
        case POWER_MODE_CAN_SERIES_MASTER:
        case POWER_MODE_CAN_PARALLEL_SLAVE:
        case POWER_MODE_CAN_SERIES_SLAVE: {
            CAN_OutSet_Read_Update();
        }
        break;
    }
}

#if defined(MULT_CHANNEL)
uint8_t CAN_Device_Read_Update_MultCH(void)
{
    CAN_OutSet_Read_Update_MultCH();
}
#endif



//异常状态处理
uint8_t CAN_Device_Error_Process_Pall(void)
{
    static TickType_t lastTick;
    static uint8_t initFlag = 0;
    if(initFlag == 0) {
        initFlag = 1;
        lastTick = xTaskGetTickCount();
    }
    static int16_t offcount = 0;
    uint8_t onflag = 0;
    uint8_t off_requeset = 0;
    TickType_t diffTick = xTaskGetTickCount() - lastTick;
    lastTick = xTaskGetTickCount();
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.ParallelSlaveInfo;
    uint8_t infoSize = ARRAY_COUNT(PowerInfo.Read.ParallelSlaveInfo);
    for(uint8_t i = 0; i < infoSize; ++i) {
        //CAN通讯超时
        if(pSlaveInfo[i].data_ready != 0)
        {
            if(pSlaveInfo[i].timeout > 0) {
                pSlaveInfo[i].timeout -= diffTick;
            }
            if(pSlaveInfo[i].timeout <= 0) {
                if(__CAN_isMode(POWER_MODE_CAN_PARALLEL_MASTER)) {
                    if(i == 0) {
                        MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Master_is_Disconnected));
                        __On_Slave_Connected_Changed(0);
                    } else {
                        MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Slave_is_Disconnected), i);
                        __On_Slave_Connected_Changed(0);
                    }
                }
                pSlaveInfo[i].data_ready = 0;
                memset(&pSlaveInfo[i], 0, sizeof(pSlaveInfo[i]));
                pSlaveInfo[i].powerType = 0xff;
                continue;
            }
        } else {
            continue;
        }
        extern uint8_t __Slave_State_Event_Process(State_Def state, State_Def event);
        off_requeset |= (__Slave_State_Event_Process(pSlaveInfo[i].State, pSlaveInfo[i].Event) != 0);
        onflag |= (pSlaveInfo[i].State.bits.onoff != 0);
    }
    if(off_requeset && (onflag || (IF_Power_Get_On_Off_Now() != 0))) {
        if(offcount > 0) {
            offcount -= diffTick;
        } else {
            offcount = 200;
            IF_Power_Set_On_Off(0);
        }
    } else {
        offcount = 200;
    }
}
uint8_t CAN_Device_Error_Process_Series(void)
{
    static TickType_t lastTick;
    static uint8_t initFlag = 0;
    if(initFlag == 0) {
        initFlag = 1;
        lastTick = xTaskGetTickCount();
    }
    static int16_t offcount = 0;
    uint8_t onflag = 0;
    uint8_t off_requeset = 0;
    TickType_t diffTick = xTaskGetTickCount() - lastTick;
    lastTick = xTaskGetTickCount();
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.SeriesSlaveInfo;
    uint8_t infoSize = ARRAY_COUNT(PowerInfo.Read.SeriesSlaveInfo);
    for(uint8_t i = 0; i < infoSize; ++i) {
        //CAN通讯超时
        if(pSlaveInfo[i].data_ready != 0)
        {
            if(pSlaveInfo[i].timeout > 0) {
                pSlaveInfo[i].timeout -= diffTick;
            }
            if(pSlaveInfo[i].timeout <= 0) {
                if(__CAN_isMode(POWER_MODE_CAN_SERIES_MASTER)) {
                    if(i == 0) {
                        MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Master_is_Disconnected));
                        __On_Slave_Connected_Changed(0);
                    } else {
                        MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Slave_is_Disconnected), i);
                        __On_Slave_Connected_Changed(0);
                    }
                }
                pSlaveInfo[i].data_ready = 0;
                memset(&pSlaveInfo[i], 0, sizeof(pSlaveInfo[i]));
                pSlaveInfo[i].powerType = 0xff;
                continue;
            }
        } else {
            continue;
        }
        extern uint8_t __Slave_State_Event_Process(State_Def state, State_Def event);
        off_requeset |= (__Slave_State_Event_Process(pSlaveInfo[i].State, pSlaveInfo[i].Event) != 0);
        onflag |= (pSlaveInfo[i].State.bits.onoff != 0);
    }
    if(off_requeset && (onflag || (IF_Power_Get_On_Off_Now() != 0))) {
        if(offcount > 0) {
            offcount -= diffTick;
        } else {
            offcount = 200;
            IF_Power_Set_On_Off(0);
        }
    } else {
        offcount = 200;
    }
}
uint8_t CAN_Device_Error_Process_Synergy(void)
{
    static TickType_t lastTick;
    static uint8_t initFlag = 0;
    if(initFlag == 0) {
        initFlag = 1;
        lastTick = xTaskGetTickCount();
    }
    static int16_t offcount = 0;
    uint8_t onflag = 0;
    uint8_t off_requeset = 0;
    TickType_t diffTick = xTaskGetTickCount() - lastTick;
    lastTick = xTaskGetTickCount();
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.SynergySlaveInfo;
    uint8_t infoSize = ARRAY_COUNT(PowerInfo.Read.SynergySlaveInfo);
    for(uint8_t i = 0; i < infoSize; ++i) {
        //CAN通讯超时
        if(pSlaveInfo[i].data_ready != 0)
        {
            if(pSlaveInfo[i].timeout > 0) {
                pSlaveInfo[i].timeout -= diffTick;
            }
            if(pSlaveInfo[i].timeout <= 0) {
                if(__CAN_isMode(POWER_MODE_CAN_SYNERGY_MASTER)) {
                    if(i == 0) {
                        MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Master_is_Disconnected));
                        __On_Slave_Connected_Changed(0);
                    } else {
                        MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Slave_is_Disconnected), i);
                        __On_Slave_Connected_Changed(0);
                    }
                }
                pSlaveInfo[i].data_ready = 0;
                memset(&pSlaveInfo[i], 0, sizeof(pSlaveInfo[i]));
                pSlaveInfo[i].powerType = 0xff;
                continue;
            }
        } else {
            continue;
        }
        extern uint8_t __Slave_State_Event_Process(State_Def state, State_Def event);
        off_requeset |= (__Slave_State_Event_Process(pSlaveInfo[i].State, pSlaveInfo[i].Event) != 0);
        onflag |= (pSlaveInfo[i].State.bits.onoff != 0);
    }
    if(off_requeset && (onflag || (IF_Power_Get_On_Off_Now() != 0))) {
        if(offcount > 0) {
            offcount -= diffTick;
        } else {
            offcount = 200;
            IF_Power_Set_On_Off(0);
        }
    } else {
        offcount = 200;
    }
}

#if defined(MULT_CHANNEL)
uint8_t CAN_Device_Error_Process_MultCH(void)
{
    static TickType_t lastTick;
    static uint8_t initFlag = 0;
    if(initFlag == 0) {
        initFlag = 1;
        lastTick = xTaskGetTickCount();
    }
    static int16_t offcount = 0;
    uint8_t onflag = 0;
    uint8_t off_requeset = 0;
    TickType_t diffTick = xTaskGetTickCount() - lastTick;
    lastTick = xTaskGetTickCount();
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.MultSlaveInfo;
    uint8_t infoSize = ARRAY_COUNT(PowerInfo.Read.MultSlaveInfo);
    for(uint8_t i = 0; i < infoSize; ++i) {
        //CAN通讯超时
        if(pSlaveInfo[i].data_ready != 0)
        {
            if(pSlaveInfo[i].timeout > 0) {
                pSlaveInfo[i].timeout -= diffTick;
            }
            if(pSlaveInfo[i].timeout <= 0) {
                if(i == 0) {
                    MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Master_is_Disconnected));
                    __On_Slave_Connected_Changed(0);
                } else {
                    MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Slave_is_Disconnected), i);
                    __On_Slave_Connected_Changed(0);
                }
                pSlaveInfo[i].data_ready = 0;
                memset(&pSlaveInfo[i], 0, sizeof(pSlaveInfo[i]));
                pSlaveInfo[i].powerType = 0xff;
                continue;
            }
        } else {
            continue;
        }
    }
}
#endif


typedef struct {
    uint8_t (*can_funtion)(void); //函数
    const char *functionName;    //项目名称
    uint32_t period;            //工作周期,单位:ms
    uint8_t enable;             //使能
    int32_t nextTime;
} CAN_Table_Def;

CAN_Table_Def  CAN_Table[] = {
    {
        .can_funtion = CAN_Master_Set_Update,
        .functionName = "主机上报设置",
        .period = 50,
        .enable = 1,  //打开
    }, {
        .can_funtion = CAN_Device_Read_Update,
        .functionName = "CAN设备上报输出回读",
        .period = 50,
        .enable = 1,  //打开
    },




    {
        .can_funtion = CAN_Device_Error_Process_Pall,
        .functionName = "并联异常状态处理",
        .period = 50,
        .enable = 1,  //打开
    }, {
        .can_funtion = CAN_Device_Error_Process_Series,
        .functionName = "串联异常状态处理",
        .period = 50,
        .enable = 1,  //打开
    },

    #if defined(MULT_CHANNEL)
    {
        .can_funtion = CAN_Device_Error_Process_Synergy,
        .functionName = "协同异常状态处理",
        .period = 50,
        .enable = 1,  //打开
    }, {
        .can_funtion = CAN_Device_Error_Process_MultCH,
        .functionName = "多通道异常状态处理",
        .period = 50,
        .enable = 1,  //打开
    }, {
        .can_funtion = CAN_Device_Read_Update_MultCH,
        .functionName = "多通道CAN设备上报输出回读",
        .period = 50,
        .enable = 1,  //打开
    },
    #endif
};








typedef StaticSemaphore_t osStaticSemaphoreDef_t;
osStaticSemaphoreDef_t CAN_Tx_SempControlBlock;
const osSemaphoreAttr_t CAN_Tx_Semp_attributes = {
    .name = "CAN_Tx_Semp",
    .cb_mem = &CAN_Tx_SempControlBlock,
    .cb_size = sizeof(CAN_Tx_SempControlBlock),
};
osSemaphoreId_t CAN_Tx_Semp = NULL;




static void __CAN_Process_Standard(User_CAN_Rx_Msg_Def *msg) {}
static void __CAN_Process_Extended(User_CAN_Rx_Msg_Def *msg);
static User_Task_t thisTask = {0};
static void CAN_Tx_Task(void *argument)
{
    if(argument != NULL) {
        thisTask.threadId = osThreadGetId();
        thisTask.attributes = (const osThreadAttr_t *)argument;
    }
    if(HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE, 0) != HAL_OK) {
        Error_Handler();
    }
    if(HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_FIFO_EMPTY, 0) != HAL_OK) {
        Error_Handler();
    }
    if(HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_ABORT_COMPLETE, 0) != HAL_OK) {
        Error_Handler();
    }
    if(CAN_Tx_Semp == NULL) {
        CAN_Tx_Semp = osSemaphoreNew(1, 0, &CAN_Tx_Semp_attributes);
    }
    //
    TickType_t delayTick = xTaskGetTickCount();
    //    user_printf(__func__,"CAN TX Task is Running!");
    uint16_t peroid = 10;
    while(1)
    {
        for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Table); i++)
        {
            CAN_Table_Def *pItem = &CAN_Table[i];
            if((pItem->enable != 0) && (pItem->can_funtion != NULL))
            {
                pItem->nextTime -= peroid;
                if(pItem->nextTime > 0) {
                } else {
                    pItem->nextTime = pItem->period;
                    uint8_t result = 0;
                    result = pItem->can_funtion();
                }
            }
        }
        //        FDCAN_ErrorCountersTypeDef erroCount = {0};
        //        HAL_FDCAN_GetErrorCounters(&hfdcan1, &erroCount);
        //        if((erroCount.TxErrorCnt != 0) || (erroCount.RxErrorCnt != 0)){
        //            user_printf(__func__, "FDCAN ERROR: TX:%d   RX:%d", erroCount.TxErrorCnt, erroCount.RxErrorCnt);
        //        }
        WDG_Task_Feed(WDG_FLAG_CAN_TX);
        //不同工作模式下的IO控制
        switch(IF_Power_Get_Power_Mode())
        {
            //并联
            case POWER_MODE_CAN_PARALLEL_MASTER: {
                gpio_set(&gpio_master_salve_sel, 1);
            }
            break;
            case POWER_MODE_CAN_PARALLEL_SLAVE: {
                gpio_set(&gpio_master_salve_sel, 0);
            }
            break;
            //串联
            case POWER_MODE_CAN_SERIES_MASTER: {
                gpio_set(&gpio_master_salve_sel, 0);
            }
            break;
            case POWER_MODE_CAN_SERIES_SLAVE: {
                gpio_set(&gpio_master_salve_sel, 0);
            }
            break;
            //协同
            case POWER_MODE_CAN_SYNERGY_MASTER: {
                gpio_set(&gpio_master_salve_sel, 0);
            }
            break;
            case POWER_MODE_CAN_SYNERGY_SLAVE: {
                gpio_set(&gpio_master_salve_sel, 0);
            }
            break;
            default: {
                gpio_set(&gpio_master_salve_sel, 0);
            }
            break;
        }
        switch(IF_Power_Get_Power_Mode())
        {
            //并联
            case POWER_MODE_CAN_PARALLEL_MASTER:
            case POWER_MODE_CAN_SERIES_MASTER:
            case POWER_MODE_CAN_SYNERGY_MASTER:
                //                case POWER_MODE_CAN_PARALLEL_SLAVE:
                //                case POWER_MODE_CAN_SERIES_SLAVE:
                //                case POWER_MODE_CAN_SYNERGY_SLAVE:
            {
                gpio_set(&gpio_can_ctrl_0, 0);
                gpio_set(&gpio_can_ctrl_1, 1);
            }
            break;
            default: {
                gpio_set(&gpio_can_ctrl_0, 1);//0);
                gpio_set(&gpio_can_ctrl_1, 0);
            }
            break;
        }
        vTaskDelayUntil(&delayTick, peroid);
        thisTask.startHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        thisTask = thisTask;
    }
}





