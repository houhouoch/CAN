#include "CAN_Task.h"
#include "FDCAN.h"
//任务
const osThreadAttr_t CAN_attributes = {
    .name = "CAN_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};
const osThreadAttr_t CAN_IRQ_attributes = {
    .name = "CAN_IRQ_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal + 1,
};
static void CAN_Task(void *argument);
static void CAN_IRQ_Task(void *argument);


/*
 * @brief       osThreadId_t CAN_Task_Create(void)
 * @details     创建CAN通讯任务
 */
osThreadId_t CAN_Task_Create(void)
{
    osThreadNew(CAN_Task, (void *)&CAN_attributes, &CAN_attributes);
    osThreadNew(CAN_IRQ_Task, (void *)&CAN_IRQ_attributes, &CAN_IRQ_attributes);
    return 0;
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


/*
 * @brief       void Power_Set_Updata_CAN(void)
 * @details     同步更新电源设置到从机(仅主机发送)
 */
void Power_Set_Updata_CAN(void)
{
    //    osKernelLock();
    if(PowerInfo.Read.canUpdata_event != 0)
    {
        PowerInfo.Read.canUpdata_event = 0;
        switch(IF_Power_Get_Power_Mode())
        {
            case POWER_MODE_CAN_PARALLEL_MASTER:
            case POWER_MODE_CAN_SERIES_MASTER: {
                #if 1
                CANFD_Send_Extended(CAN_VSR_UP_WRITE_MASTER, (uint8_t *)&PowerInfo.outSet.VSR_Up);
                CANFD_Send_Extended(CAN_VSR_DOWN_WRITE_MASTER, (uint8_t *)&PowerInfo.outSet.VSR_Down);
                CANFD_Send_Extended(CAN_ISR_UP_WRITE_MASTER, (uint8_t *)&PowerInfo.outSet.ISR_Up);
                CANFD_Send_Extended(CAN_ISR_DOWN_WRITE_MASTER, (uint8_t *)&PowerInfo.outSet.ISR_Down);
                #endif
                float volt_set = IF_Power_Get_OutVolt();
                CANFD_Send_Extended(CAN_VOL_WRITE_MASTER, (uint8_t *)&volt_set);
                float curr_set = IF_Power_Get_OutCurr();
                CANFD_Send_Extended(CAN_CURR_WRITE_MASTER, (uint8_t *)&curr_set);
                float ovp_set = PowerInfo.outSet.ovp_set;
                CANFD_Send_Extended(CAN_OVP_WRITE_MASTER, (uint8_t *)&ovp_set);
                float ocp_set = PowerInfo.outSet.ocp_set;
                CANFD_Send_Extended(CAN_OCP_WRITE_MASTER, (uint8_t *)&ocp_set);
                float ResIn_set = PowerInfo.outSet.ResIn_set;
                CANFD_Send_Extended(CAN_RIN_WRITE_MASTER, (uint8_t *)&ResIn_set);
                #if 1
                uint8_t data[8] = {0};
                data[0] = PowerInfo.outSet.onoff;
                data[1] = PowerInfo.outSet.ovp_onoff;
                data[2] = PowerInfo.outSet.ocp_onoff;
                data[3] = PowerInfo.outSet.SlewRate_Mode;
                CANFD_Send_Extended(CAN_STATE_WRITE_MASTER, data);
                #endif
            }
            break;
        }
        extern __IO uint16_t synccount;
        synccount = 0;
    }
    //    osKernelUnlock();
}

/*
 * @brief       void Power_Read_CAN_Updata_Task(void)
 * @details      电源回读CAN上传任务
 */
void Power_Read_CAN_Updata_Task(void)
{
    switch(IF_Power_Get_Power_Mode())
    {
        case POWER_MODE_CAN_PARALLEL_MASTER:
        case POWER_MODE_CAN_SERIES_MASTER: {
            float volt = PowerInfo.Read.volt_read;
            float curr = PowerInfo.Read.curr_read;
            float power = PowerInfo.Read.power_read;
            CANFD_Send_Extended(CAN_VOL_READ_MASTER, (uint8_t *)&volt);
            CANFD_Send_Extended(CAN_CURR_READ_MASTER, (uint8_t *)&curr);
            CANFD_Send_Extended(CAN_POWER_READ_MASTER, (uint8_t *)&power);
            uint8_t stateData[8] = {0};
            memcpy(&stateData[0], &PowerInfo.State, sizeof(State_Def));
            CANFD_Send_Extended(CAN_STATE_READ_MASTER, stateData);
            memcpy(&stateData[0], &PowerInfo.Event, sizeof(State_Def));
            CANFD_Send_Extended(CAN_EVENT_READ_MASTER, stateData);
        }
        break;
        case POWER_MODE_CAN_PARALLEL_SLAVE:
        case POWER_MODE_CAN_SERIES_SLAVE: {
            uint32_t id_base = (IF_Option_Get_Slave_ID() * 0x100);
            uint8_t stateData[8] = {0};
            memcpy(&stateData[0], &PowerInfo.State, sizeof(State_Def));
            CANFD_Send_Extended(id_base + 4, stateData);
            memcpy(&stateData[0], &PowerInfo.Event, sizeof(State_Def));
            CANFD_Send_Extended(id_base + 5, stateData);
            float volt = PowerInfo.Read.volt_read;
            CANFD_Send_Extended(id_base + 1, (uint8_t *)&volt);
            float curr = PowerInfo.Read.curr_read;
            CANFD_Send_Extended(id_base + 2, (uint8_t *)&curr);
            float power = PowerInfo.Read.power_read;
            CANFD_Send_Extended(id_base + 3, (uint8_t *)&power);
        }
        break;
    }
    //不同工作模式下的IO控制
    switch(IF_Power_Get_Power_Mode())
    {
        case POWER_MODE_CAN_PARALLEL_MASTER: {
            gpio_set(&gpio_master_salve_sel, 1);
        }
        break;
        case POWER_MODE_CAN_PARALLEL_SLAVE: {
            gpio_set(&gpio_master_salve_sel, 0);
        }
        break;
        case POWER_MODE_CAN_SERIES_MASTER: {
            gpio_set(&gpio_master_salve_sel, 0);
        }
        break;
        case POWER_MODE_CAN_SERIES_SLAVE: {
            gpio_set(&gpio_master_salve_sel, 0);
        }
        break;
        default: {
            gpio_set(&gpio_master_salve_sel, 1);
        }
        break;
    }
    gpio_set(&gpio_can_ctrl_0, 0);
    gpio_set(&gpio_can_ctrl_1, 1);
}


#include "UserQueue.h"
#include "MsgBox.h"
static void __On_Slave_Connected_Changed(uint8_t connect) {
    if((IF_Power_Get_Power_Mode() != POWER_MODE_CAN_PARALLEL_MASTER)
            && (IF_Power_Get_Power_Mode() != POWER_MODE_CAN_SERIES_MASTER)) {
        return ;
    }
    IF_Power_Set_On_Off(0);
    //    if(connect != 0){
    //
    //    }
}



static void __CAN_Process_Standard(User_CAN_Rx_Msg_Def *msg) {}
static void __CAN_Process_Extended(User_CAN_Rx_Msg_Def *msg);
User_Task_t thisTask = {0};
static void CAN_Task(void *argument)
{
    if(argument != NULL) {
        thisTask.threadId = osThreadGetId();
        thisTask.attributes = (const osThreadAttr_t *)argument;
    }
    //
    TickType_t lastTick = xTaskGetTickCount();
    TickType_t delayTick = xTaskGetTickCount();
    while(1)
    {
        WDG_Task_Feed(WDG_FLAG_CAN);
        vTaskDelayUntil(&delayTick, 50);
        {
            static int16_t offcount = 0;
            uint8_t onflag = 0;
            uint8_t off_requeset = 0;
            TickType_t diffTick = xTaskGetTickCount() - lastTick;
            lastTick = xTaskGetTickCount();
            for(uint8_t i = 0; i < ARRAY_COUNT(PowerInfo.Read.SlaveInfo); ++i) {
                //CAN通讯超时
                if(PowerInfo.Read.SlaveInfo[i].data_ready != 0)
                {
                    if(PowerInfo.Read.SlaveInfo[i].timeout > 0) {
                        PowerInfo.Read.SlaveInfo[i].timeout -= diffTick;
                    }
                    if(PowerInfo.Read.SlaveInfo[i].timeout <= 0) {
                        if(i == 0) {
                            MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Master_is_Disconnected));
                        } else {
                            MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Slave_is_Disconnected), i);
                            __On_Slave_Connected_Changed(0);
                        }
                        PowerInfo.Read.SlaveInfo[i].data_ready = 0;
                        memset(&PowerInfo.Read.SlaveInfo[i], 0, sizeof(PowerInfo.Read.SlaveInfo[i]));
                        continue;
                    }
                } else {
                    continue;
                }
                extern uint8_t __Slave_State_Event_Process(State_Def state, State_Def event);
                off_requeset |= (__Slave_State_Event_Process(PowerInfo.Read.SlaveInfo[i].State, PowerInfo.Read.SlaveInfo[i].Event) != 0);
                onflag |= (PowerInfo.Read.SlaveInfo[i].State.bits.onoff != 0);
            }
            if(off_requeset && (onflag || (IF_Power_Get_On_Off() != 0))) {
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
        //发送Power数据
        Power_Set_Updata_CAN();
        //上报CAN数据
        Power_Read_CAN_Updata_Task();
        thisTask.startHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        thisTask = thisTask;
    }
}
static void CAN_IRQ_Task(void *argument)
{
    if(argument != NULL) {
        thisTask.threadId = osThreadGetId();
        thisTask.attributes = (const osThreadAttr_t *)argument;
    }
    /* Infinite loop */
    if(CAN_Recv_Semaph == NULL) {
        CAN_Recv_Semaph = osSemaphoreNew(5, 0, &CAN_Recv_IRQ_attributes);
    }
    /* Activate Rx FIFO 0 new message notification on both FDCAN instances */
    if(HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
        Error_Handler();
    }
    while(1)
    {
        WDG_Task_Feed(WDG_FLAG_CAN_IRQ);
        //接收CAN数据
        if(osSemaphoreAcquire(CAN_Recv_Semaph, 200) == osOK)
        {
            CAN_RX_freeCount = HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0);
            while(CAN_RX_freeCount--)
            {
                /* Retrieve Rx messages from RX FIFO0 */
                FDCAN_RxHeaderTypeDef RxHeader1 = {0};
                static uint8_t can1_rxbuf[16] = {0};
                memset(can1_rxbuf, 0, sizeof(can1_rxbuf));
                if(HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader1, can1_rxbuf) != HAL_OK) {
                    Error_Handler();
                }
                #if 1
                if(((RxHeader1.IdType == FDCAN_STANDARD_ID) && (1))
                        || ((RxHeader1.IdType == FDCAN_EXTENDED_ID) && (1)))
                {
                    User_CAN_Rx_Msg_Def can_Rx_Msg = {0};
                    memcpy(&can_Rx_Msg.header, &RxHeader1, sizeof(FDCAN_RxHeaderTypeDef));
                    memcpy(can_Rx_Msg.data, can1_rxbuf, 8);
                    switch(can_Rx_Msg.header.IdType)
                    {
                        case FDCAN_STANDARD_ID:
                            __CAN_Process_Standard(&can_Rx_Msg);
                            break;
                        case FDCAN_EXTENDED_ID:
                            __CAN_Process_Extended(&can_Rx_Msg);
                            break;
                    }
                }
                #endif
            }
        }
        thisTask.startHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        thisTask = thisTask;
    }
}


/*
 * @brief       uint8_t __CAN_Power_Set_Process(User_CAN_Rx_Msg_Def *msg)
 * @details     CAN接收数据包处理
 */
uint32_t count_can_rx = 0;
uint8_t __CAN_Power_Set_Process(User_CAN_Rx_Msg_Def *msg)
{
    if(msg == NULL) {
        return 0;
    }
    switch(msg->header.Identifier)
    {
        case CAN_STATE_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                uint8_t onoff_temp = msg->data[0];
                uint8_t ovp_onoff_temp = msg->data[1];
                uint8_t ocp_onoff_temp = msg->data[2];
                IF_Power_Set_On_Off(onoff_temp);
                IF_Power_Set_OVP_onoff(ovp_onoff_temp);
                IF_Power_Set_OCP_onoff(ocp_onoff_temp);
                uint8_t SRMode_temp = 0;
                SRMode_temp = msg->data[3];
                IF_Power_Set_SlewRate_Mode(SRMode_temp);
            }
        }
        break;
        case CAN_VOL_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                float volt_set_temp = 0;
                memcpy(&volt_set_temp, msg->data, sizeof(float));
                IF_Power_Set_OutVolt(volt_set_temp);
            }
        }
        break;
        case CAN_CURR_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                float curr_set_temp = 0;
                memcpy(&curr_set_temp, msg->data, sizeof(float));
                IF_Power_Set_OutCurr(curr_set_temp);
            }
        }
        break;
        case CAN_OVP_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                float ovp_set_temp = 0;
                memcpy(&ovp_set_temp, msg->data, sizeof(float));
                IF_Power_Set_OVP(ovp_set_temp);
            }
        }
        break;
        case CAN_OCP_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                float ocp_set_temp = 0;
                memcpy(&ocp_set_temp, msg->data, sizeof(float));
                IF_Power_Set_OCP(ocp_set_temp);
            }
        }
        break;
        case CAN_RIN_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                float rin_temp = 0;
                memcpy(&rin_temp, msg->data, sizeof(float));
                IF_Power_Set_ResIn(rin_temp);
            }
        }
        break;
        case CAN_SR_MODE_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                uint8_t SRMode_temp = 0;
                SRMode_temp = msg->data[0];
                IF_Power_Set_SlewRate_Mode(SRMode_temp);
            }
        }
        break;
        case CAN_VSR_UP_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                float VSR_temp = 0;
                memcpy(&VSR_temp, msg->data, sizeof(float));
                IF_Power_Set_VSR_Up(VSR_temp);
            }
        }
        break;
        case CAN_VSR_DOWN_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                float VSR_temp = 0;
                memcpy(&VSR_temp, msg->data, sizeof(float));
                IF_Power_Set_VSR_Down(VSR_temp);
            }
        }
        break;
        case CAN_ISR_UP_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                float ISR_temp = 0;
                memcpy(&ISR_temp, msg->data, sizeof(float));
                IF_Power_Set_ISR_Up(ISR_temp);
            }
        }
        break;
        case CAN_ISR_DOWN_WRITE_MASTER: {
            if((IF_Power_Get_Power_Mode() == POWER_MODE_CAN_PARALLEL_SLAVE)
                    || (IF_Power_Get_Power_Mode() == POWER_MODE_CAN_SERIES_SLAVE)) {
                float ISR_temp = 0;
                memcpy(&ISR_temp, msg->data, sizeof(float));
                IF_Power_Set_ISR_Down(ISR_temp);
            }
        }
        break;
        default :
            return 0;
    }
    return 1;
}





#include "MsgBox.h"


uint8_t __Slave_State_Event_Process(State_Def state, State_Def event)
{
    if((state.bits.acFault != 0)
            || (state.bits.ovp != 0)
            || (state.bits.ocp != 0)
            || (state.bits.otp != 0)
            || (state.bits.opp != 0)
            || (state.bits.fan_error != 0)
            || (state.bits.pfc_error != 0)
            || (state.bits.font_ocp != 0)
            || (state.bits.sense != 0)
            || (state.bits.pfc_hot != 0))
    {
        return 1;
    }
    return 0;
}





uint8_t __CAN_Power_Read_Process(User_CAN_Rx_Msg_Def *msg)
{
    if(msg == NULL) {
        return 0;
    }
    float temp = 0;
    //仅在
    switch(IF_Power_Get_Power_Mode())
    {
        case POWER_MODE_CAN_PARALLEL_MASTER:
        case POWER_MODE_CAN_PARALLEL_SLAVE:
        case POWER_MODE_CAN_SERIES_MASTER:
        case POWER_MODE_CAN_SERIES_SLAVE:
            break;
        //其他模式不接收数据
        default:
            return 0;
    }
    switch(msg->header.Identifier)
    {
        //主机(0)
        case CAN_VOL_READ_MASTER:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[0].volt_read = temp;
            break;
        case CAN_CURR_READ_MASTER:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[0].curr_read = temp;
            break;
        case CAN_POWER_READ_MASTER:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[0].power_read = temp;
            break;
        case CAN_STATE_READ_MASTER:
            memcpy(&PowerInfo.Read.SlaveInfo[0].State, &msg->data[0], sizeof(State_Def));
            break;
        case CAN_EVENT_READ_MASTER:
            memcpy(&PowerInfo.Read.SlaveInfo[0].Event, &msg->data[0], sizeof(State_Def));
            break;
        //从机(1)
        case CAN_VOL_READ_SLAVE_1:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[1].volt_read = temp;
            break;
        case CAN_CURR_READ_SLAVE_1:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[1].curr_read = temp;
            break;
        case CAN_POWER_READ_SLAVE_1:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[1].power_read = temp;
            break;
        case CAN_STATE_READ_SLAVE_1:
            memcpy(&PowerInfo.Read.SlaveInfo[1].State, &msg->data[0], sizeof(State_Def));
            break;
        case CAN_EVENT_READ_SLAVE_1:
            memcpy(&PowerInfo.Read.SlaveInfo[1].Event, &msg->data[0], sizeof(State_Def));
            break;
        //从机(2)
        case CAN_VOL_READ_SLAVE_2:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[2].volt_read = temp;
            break;
        case CAN_CURR_READ_SLAVE_2:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[2].curr_read = temp;
            break;
        case CAN_POWER_READ_SLAVE_2:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[2].power_read = temp;
            break;
        case CAN_STATE_READ_SLAVE_2:
            memcpy(&PowerInfo.Read.SlaveInfo[2].State, &msg->data[0], sizeof(State_Def));
            break;
        case CAN_EVENT_READ_SLAVE_2:
            memcpy(&PowerInfo.Read.SlaveInfo[2].Event, &msg->data[0], sizeof(State_Def));
            break;
        //从机(3)
        case CAN_VOL_READ_SLAVE_3:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[3].volt_read = temp;
            break;
        case CAN_CURR_READ_SLAVE_3:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[3].curr_read = temp;
            break;
        case CAN_POWER_READ_SLAVE_3:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[3].power_read = temp;
            break;
        case CAN_STATE_READ_SLAVE_3:
            memcpy(&PowerInfo.Read.SlaveInfo[3].State, &msg->data[0], sizeof(State_Def));
            break;
        case CAN_EVENT_READ_SLAVE_3:
            memcpy(&PowerInfo.Read.SlaveInfo[3].Event, &msg->data[0], sizeof(State_Def));
            break;
        //从机(4)
        case CAN_VOL_READ_SLAVE_4:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[4].volt_read = temp;
            break;
        case CAN_CURR_READ_SLAVE_4:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[4].curr_read = temp;
            break;
        case CAN_POWER_READ_SLAVE_4:
            memcpy(&temp, msg->data, sizeof(float));
            PowerInfo.Read.SlaveInfo[4].power_read = temp;
            break;
        case CAN_STATE_READ_SLAVE_4:
            memcpy(&PowerInfo.Read.SlaveInfo[4].State, &msg->data[0], sizeof(State_Def));
            break;
        case CAN_EVENT_READ_SLAVE_4:
            memcpy(&PowerInfo.Read.SlaveInfo[4].Event, &msg->data[0], sizeof(State_Def));
            break;
        default :
            return 0;
    }
    switch(msg->header.Identifier)
    {
        //主机(0)
        case CAN_VOL_READ_MASTER:
        case CAN_CURR_READ_MASTER:
        case CAN_POWER_READ_MASTER:
        case CAN_STATE_READ_MASTER:
        case CAN_EVENT_READ_MASTER:
            if(PowerInfo.Read.SlaveInfo[0].timeout <= 0) {
                MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Master_is_Connected));
            }
            PowerInfo.Read.SlaveInfo[0].timeout = 1000;
            PowerInfo.Read.SlaveInfo[0].data_ready = 1;
            break;
        //从机(1)
        case CAN_VOL_READ_SLAVE_1:
        case CAN_CURR_READ_SLAVE_1:
        case CAN_POWER_READ_SLAVE_1:
        case CAN_STATE_READ_SLAVE_1:
        case CAN_EVENT_READ_SLAVE_1:
            if(PowerInfo.Read.SlaveInfo[1].timeout <= 0) {
                MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Slave_is_Connected), 1);
                __On_Slave_Connected_Changed(1);
            }
            PowerInfo.Read.SlaveInfo[1].timeout = 1000;
            PowerInfo.Read.SlaveInfo[1].data_ready = 1;
            break;
        //从机(2)
        case CAN_VOL_READ_SLAVE_2:
        case CAN_CURR_READ_SLAVE_2:
        case CAN_POWER_READ_SLAVE_2:
        case CAN_STATE_READ_SLAVE_2:
        case CAN_EVENT_READ_SLAVE_2:
            if(PowerInfo.Read.SlaveInfo[2].timeout <= 0) {
                MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Slave_is_Connected), 2);
                __On_Slave_Connected_Changed(1);
            }
            PowerInfo.Read.SlaveInfo[2].timeout = 1000;
            PowerInfo.Read.SlaveInfo[2].data_ready = 1;
            break;
            #if 1
        //从机(3)
        case CAN_VOL_READ_SLAVE_3:
        case CAN_CURR_READ_SLAVE_3:
        case CAN_POWER_READ_SLAVE_3:
        case CAN_STATE_READ_SLAVE_3:
        case CAN_EVENT_READ_SLAVE_3:
            if(PowerInfo.Read.SlaveInfo[3].timeout <= 0) {
                MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Slave_is_Connected), 3);
                __On_Slave_Connected_Changed(1);
            }
            PowerInfo.Read.SlaveInfo[3].timeout = 1000;
            PowerInfo.Read.SlaveInfo[3].data_ready = 1;
            break;
        //从机(4)
        case CAN_VOL_READ_SLAVE_4:
        case CAN_CURR_READ_SLAVE_4:
        case CAN_POWER_READ_SLAVE_4:
        case CAN_STATE_READ_SLAVE_4:
        case CAN_EVENT_READ_SLAVE_4:
            if(PowerInfo.Read.SlaveInfo[4].timeout <= 0) {
                MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Slave_is_Connected), 4);
                __On_Slave_Connected_Changed(1);
            }
            PowerInfo.Read.SlaveInfo[4].timeout = 1000;
            PowerInfo.Read.SlaveInfo[4].data_ready = 1;
            break;
            #endif
    }
    return 1;
}




void __CAN_Process_Extended(User_CAN_Rx_Msg_Def *msg)
{
    if(msg == NULL) {
        return ;
    }
    switch(msg->header.Identifier)
    {
        default: {
            if(__CAN_Power_Set_Process(msg) != 0) {
                break;
            }
            if(__CAN_Power_Read_Process(msg) != 0) {
                break;
            }
        }
        break;
    }
}




