#include "CAN_Rx_Task.h"
#include "FDCAN.h"

#include "CAN_SCPI.h"
#include "CAN_Table.h"
//任务
const osThreadAttr_t CAN_Rx_attributes = {
    .name = "CAN_Rx_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal + 1,
};
static void CAN_RX_Task(void *argument);

osThreadId_t CAN_Rx_Task_ThreadID = NULL;

/*
 * @brief       osThreadId_t CAN_Rx_Task_Create(void)
 */
osThreadId_t CAN_Rx_Task_Create(void)
{
    CAN_Rx_Task_ThreadID = osThreadNew(CAN_RX_Task, (void *)&CAN_Rx_attributes, &CAN_Rx_attributes);
    return CAN_Rx_Task_ThreadID;
}




#include "UserQueue.h"
#include "MsgBox.h"
#include "Modbus_master_Power.h"
void __On_Slave_Connected_Changed(uint8_t connect) {
    switch(IF_Power_Get_Power_Mode()) {
        case POWER_MODE_CAN_PARALLEL_MASTER:
        case POWER_MODE_CAN_SERIES_MASTER:
        case POWER_MODE_CAN_SYNERGY_MASTER:
            if(connect == 0) {
                IF_Power_Set_On_Off(0);
            }
            break;
        case POWER_MODE_CAN_PARALLEL_SLAVE:
        case POWER_MODE_CAN_SERIES_SLAVE:
        case POWER_MODE_CAN_SYNERGY_SLAVE:
            if(connect == 0) {
                IF_Power_Set_On_Off(0);
                Modbus_Update_OutSet_temp();
                Modbus_Power_OutPut_Sync();
            }
            break;
        case POWER_MODE_EXT_V:
            return ;
    }
    //    if((IF_Power_Get_Power_Mode() != POWER_MODE_CAN_PARALLEL_MASTER)
    //            && (IF_Power_Get_Power_Mode() != POWER_MODE_CAN_SERIES_MASTER)) {
    //        return ;
    //    }
    //    IF_Power_Set_On_Off(0);
    //    if(connect != 0){
    //
    //    }
}





static void __CAN_Process_Standard(User_CAN_Rx_Msg_Def *msg) {}
static void __CAN_Process_Extended(User_CAN_Rx_Msg_Def *msg);
static User_Task_t thisTask = {0};
static void CAN_RX_Task(void *argument)
{
    if(argument != NULL) {
        thisTask.threadId = osThreadGetId();
        thisTask.attributes = (const osThreadAttr_t *)argument;
    }
    //    user_printf(__func__,"CAN RX Task is Running!");
    while(1)
    {
        WDG_Task_Feed(WDG_FLAG_CAN_RX);
        User_CAN_Rx_Msg_Def can_Rx_Msg = {0};
        if(CAN_Rx_Queue_Rec(&can_Rx_Msg)) {
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
        thisTask.startHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        thisTask = thisTask;
    }
}

/*
 * @brief       uint8_t __CAN_Power_Set_Process(User_CAN_Rx_Msg_Def *msg)
 * @details     CAN接收数据包处理
 */
uint32_t count_can_rx = 0;



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




typedef struct {
    uint8_t packNo;
    uint8_t totalCount;

    uint8_t buffer[256];
    uint8_t bufferLen;
} CAN_Rx_Buffer;
__IO uint8_t can_m2s_ack = 0;
__IO uint8_t can_tx_end = 0;
__IO uint8_t can_s2m_ack = 0;
__IO uint8_t can_rx_end = 0;

uint8_t __CAN_Send_MultPack_M2S(uint8_t id, const uint8_t *pdata, uint8_t len)
{
    uint8_t total_pack = (len / 6) + ((len % 6) ? 1 : 0) ; // 计算总包数（含CRC）
    uint8_t total_len = len;
    uint8_t retryCount = 3;
retry:
    can_m2s_ack = 0;
    if(retryCount != 0) {
        if(total_pack != 0) {
            for(uint8_t seq = 0; seq < total_pack; seq++) {
                uint8_t tx_data[8] = {0};
                memset(tx_data, 0, 8);
                tx_data[0] = seq;
                tx_data[1] = total_len;
                uint8_t offset = seq * 6;
                if(len >= 6) {
                    memcpy(&tx_data[2], pdata + offset, 6);
                    len -= 6;
                } else {
                    memcpy(&tx_data[2], pdata + offset, len);
                    len -= len;
                }
                //                printf("CAN TX:%x\r\n", CAN_SCPI_M2S_ID + (id * 0x100));
                //                for(uint8_t i = 0; i < 8; ++i) {
                //                    printf("%02X ", tx_data[i]);
                //                }
                //                printf("\r\n");
                CANFD_Send_Extended(CAN_SCPI_M2S_ID + (id * 0x100), tx_data);
                //等待回复
                uint16_t waitTime = 200;
                while(can_m2s_ack == 0) {
                    if(waitTime == 0) {
                        retryCount--;
                        goto retry;
                    } else {
                        waitTime--;
                    }
                    osDelay(1);
                }
                can_m2s_ack = 0;
            }
        } else {
            uint8_t tx_data[8] = {0};
            tx_data[0] = 0;
            tx_data[1] = 0;
            CANFD_Send_Extended(CAN_SCPI_M2S_ID + (id * 0x100), tx_data);
        }
    }
    //等待处理完毕
    uint16_t waitTime = 200;
    //    while(can_tx_end == 0) {
    //        if(waitTime == 0) {
    //            break;
    //        }else{
    //            waitTime--;
    //        }
    //        osDelay(1);
    //    }
    return (retryCount != 0) & (waitTime != 0);
}

uint8_t __CAN_Send_MultPack_S2M(uint8_t id, const uint8_t *pdata, uint8_t len)
{
    uint8_t total_pack = (len / 6) + ((len % 6) ? 1 : 0) ; // 计算总包数（含CRC）
    uint8_t total_len = len;
    uint8_t retryCount = 3;
retry:
    can_s2m_ack = 0;
    if(retryCount != 0) {
        if(total_pack != 0) {
            for(uint8_t seq = 0; seq < total_pack; seq++) {
                uint8_t tx_data[8] = {0};
                memset(tx_data, 0, 8);
                tx_data[0] = seq;
                tx_data[1] = total_len;
                uint8_t offset = seq * 6;
                if(len >= 6) {
                    memcpy(&tx_data[2], pdata + offset, 6);
                    len -= 6;
                } else {
                    memcpy(&tx_data[2], pdata + offset, len);
                    len -= len;
                }
                //                printf("CAN TX:%x\r\n", CAN_SCPI_S2M_ID + (id * 0x100));
                //                for(uint8_t i = 0; i < 8; ++i) {
                //                    printf("%02X ", tx_data[i]);
                //                }
                //                printf("\r\n");
                CANFD_Send_Extended(CAN_SCPI_S2M_ID + (id * 0x100), tx_data);
                uint16_t waitTime = 200;
                while(can_s2m_ack == 0) {
                    if(waitTime == 0) {
                        retryCount--;
                        goto retry;
                    } else {
                        waitTime--;
                    }
                    osDelay(1);
                }
                can_s2m_ack = 0;
            }
        } else {
            uint8_t tx_data[8] = {0};
            tx_data[0] = 0;
            tx_data[1] = 0;
            CANFD_Send_Extended(CAN_SCPI_S2M_ID + (id * 0x100), tx_data);
        }
    }
    //等待执行完毕
    uint16_t waitTime = 200;
    //    while(can_rx_end == 0) {
    //        if(waitTime == 0) {
    //            break;
    //        }else{
    //            waitTime--;
    //        }
    //        osDelay(1);
    //    }
    return (retryCount != 0) & (waitTime != 0);
}






CAN_Rx_Buffer canRxBuff_S2M = {0};
CAN_Rx_Buffer canRxBuff_M2S = {0};

uint8_t __CAN_SCPI_TransProcess(User_CAN_Rx_Msg_Def *msg)
{
    if(msg == NULL) {
        return 0;
    }
    switch(msg->header.Identifier)
    {
        //设置CAN通讯组
        case CAN_SCPI_GROUP_ID: {
            if(0 == memcmp(&msg->data[2], pfirmwareInfo->uid, sizeof(pfirmwareInfo->uid))) {
                IF_Option_Set_CAN_Groups_ID(msg->data[0]);
            }
        }
        break;
        default : {
            //从机->主机
            if(IF_Power_Get_Hw_ID() == 0)
            {
                if((msg->header.Identifier & CAN_SCPI_S2M_ID) == CAN_SCPI_S2M_ID) {
                    int32_t canid = msg->header.Identifier - CAN_SCPI_S2M_ID;
                    int8_t slaveid = canid / 0x100;
                    //判断组是否正确
                    uint8_t canGroup_ID = IF_Option_Get_CAN_Groups_ID() << 4;
                    if((slaveid & canGroup_ID) == canGroup_ID) {
                        slaveid &= ~canGroup_ID;
                        if(slaveid > 0)
                        {
                            switch(msg->header.RxFrameType) {
                                case FDCAN_REMOTE_FRAME:
                                    can_s2m_ack |= 1;
                                    break;
                                case FDCAN_DATA_FRAME: {
                                    if(msg->data[0] == 0) {
                                        memset(&canRxBuff_M2S, 0, sizeof(canRxBuff_M2S));
                                    }
                                    canRxBuff_M2S.packNo = msg->data[0];
                                    if(canRxBuff_M2S.packNo == 0) {
                                        canRxBuff_M2S.bufferLen = 0;
                                        canRxBuff_M2S.totalCount = msg->data[1];
                                    }
                                    if(canRxBuff_M2S.totalCount >= 6) {
                                        memcpy(&canRxBuff_M2S.buffer[canRxBuff_M2S.packNo * 6], &msg->data[2], 6);
                                        canRxBuff_M2S.bufferLen += 6;
                                        canRxBuff_M2S.totalCount -= 6;
                                    } else {
                                        memcpy(&canRxBuff_M2S.buffer[canRxBuff_M2S.packNo * 6], &msg->data[2], canRxBuff_M2S.totalCount);
                                        canRxBuff_M2S.bufferLen += canRxBuff_M2S.totalCount;
                                        canRxBuff_M2S.totalCount -= canRxBuff_M2S.totalCount;
                                    }
                                    CANFD_Send_ExtRemote(msg->header.Identifier);
                                    //接收完成
                                    if(canRxBuff_M2S.totalCount == 0) {
                                        user_printf(__func__, "RX %d:|%s|\r\n", slaveid, canRxBuff_M2S.buffer);
                                        CAN_RX_RequestQueue_Rec(slaveid, canRxBuff_M2S.buffer, canRxBuff_M2S.bufferLen);
                                        memset(&canRxBuff_M2S, 0, sizeof(canRxBuff_M2S));
                                    }
                                }
                                break;
                            }
                        }
                    }
                    return 1;
                }
            } else {
                //主机->从机
                if((msg->header.Identifier & CAN_SCPI_M2S_ID) == CAN_SCPI_M2S_ID) {
                    int32_t canid = msg->header.Identifier - CAN_SCPI_M2S_ID;
                    int8_t slaveid = canid / 0x100;
                    //判断组是否正确
                    uint8_t canGroup_ID = IF_Option_Get_CAN_Groups_ID() << 4;
                    if((slaveid & canGroup_ID) == canGroup_ID) {
                        slaveid &= ~(canGroup_ID);
                        if((slaveid > 0) && (slaveid == IF_Power_Get_Hw_ID()))
                        {
                            switch(msg->header.RxFrameType) {
                                case FDCAN_REMOTE_FRAME: {
                                    can_m2s_ack |= 1;
                                }
                                break;
                                case FDCAN_DATA_FRAME: {
                                    //                                printf("CAN TX:%6s\r\n", (char *)&msg->data[3]);
                                    //                                for(uint8_t i = 0; i < 8; ++i) {
                                    //                                    printf("%02X ", msg->data[i]);
                                    //                                }
                                    //                                printf("\r\n");
                                    extern uint8_t __ID(void);
                                    if(msg->data[0] == 0) {
                                        memset(&canRxBuff_S2M, 0, sizeof(canRxBuff_S2M));
                                    }
                                    canRxBuff_S2M.packNo = msg->data[0];
                                    if(canRxBuff_S2M.packNo == 0) {
                                        canRxBuff_S2M.bufferLen = 0;
                                        canRxBuff_S2M.totalCount = msg->data[1];
                                    }
                                    if(canRxBuff_S2M.totalCount >= 6) {
                                        memcpy(&canRxBuff_S2M.buffer[canRxBuff_S2M.packNo * 6], &msg->data[2], 6);
                                        canRxBuff_S2M.bufferLen += 6;
                                        canRxBuff_S2M.totalCount -= 6;
                                    } else {
                                        memcpy(&canRxBuff_S2M.buffer[canRxBuff_S2M.packNo * 6], &msg->data[2], canRxBuff_S2M.totalCount);
                                        canRxBuff_S2M.bufferLen += canRxBuff_S2M.totalCount;
                                        canRxBuff_S2M.totalCount -= canRxBuff_S2M.totalCount;
                                    }
                                    CANFD_Send_ExtRemote(msg->header.Identifier);
                                    //接收完成
                                    if(canRxBuff_S2M.totalCount == 0) {
                                        //                                    user_printf(__func__,"TX:|%s|\r\n", canRxBuff_S2M.buffer);
                                        CAN_SCPI_Process(canRxBuff_S2M.buffer, canRxBuff_S2M.bufferLen);
                                        memset(&canRxBuff_S2M, 0, sizeof(canRxBuff_S2M));
                                    }
                                }
                                break;
                            }
                        }
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}


void __CAN_Process_Extended(User_CAN_Rx_Msg_Def *msg)
{
    if(msg == NULL) {
        return ;
    }
    switch(msg->header.Identifier)
    {
        default: {
            switch(IF_Power_Get_Power_Mode())
            {
                case POWER_MODE_CAN_PARALLEL_SLAVE:
                    if((CAN_OUTSET_PARA_REG_OFFSET <= msg->header.Identifier)
                            && (msg->header.Identifier < CAN_OUTSET_PARA_REG_OFFSET + 0x10000))
                    {
                        if(CAN_OutSet_Parallel_Slave_Action(msg->header.Identifier, msg->data) != 0) {
                        }
                        return;
                    }
                    break;
                case POWER_MODE_CAN_SERIES_SLAVE:
                    if((CAN_OUTSET_SER_REG_OFFSET <= msg->header.Identifier)
                            && (msg->header.Identifier < CAN_OUTSET_SER_REG_OFFSET + 0x10000))
                    {
                        if(CAN_OutSet_Series_Slave_Action(msg->header.Identifier, msg->data) != 0) {
                        }
                        return;
                    }
                    break;
                    #if defined(MULT_CHANNEL)
                case POWER_MODE_CAN_SYNERGY_MASTER:
                    if((CAN_OUTSET_SYNERGY_REG_OFFSET <= msg->header.Identifier)
                            && (msg->header.Identifier < CAN_OUTSET_SYNERGY_REG_OFFSET + 0x10000))
                    {
                        if(CAN_OutSet_Synergy_Master_Action(msg->header.Identifier, msg->data) != 0) {
                        }
                        return;
                    }
                    break;
                case POWER_MODE_CAN_SYNERGY_SLAVE:
                    if((CAN_OUTSET_SYNERGY_REG_OFFSET <= msg->header.Identifier)
                            && (msg->header.Identifier < CAN_OUTSET_SYNERGY_REG_OFFSET + 0x10000))
                    {
                        if(CAN_OutSet_Synergy_Slave_Action(msg->header.Identifier, msg->data) != 0) {
                        }
                        return;
                    }
                    break;
                    #endif
            }
            //回读数据处理
            if((CAN_OUTREAD_REG_OFFSET <= msg->header.Identifier)
                    && (msg->header.Identifier < CAN_OUTREAD_REG_OFFSET + 0x10000))
            {
                if(CAN_OutSet_Read_Action(msg->header.Identifier, msg->data) != 0) {
                }
                return;
            }
            #if defined(MULT_CHANNEL)
            //多通道数据处理
            if((CAN_OUTREAD_MULTCH_REG_OFFSET <= msg->header.Identifier)
                    && (msg->header.Identifier < CAN_OUTREAD_MULTCH_REG_OFFSET + 0x10000))
            {
                if(CAN_OutSet_Read_Action_MultCH(msg->header.Identifier, msg->data) != 0) {
                }
                return;
            }
            #endif
            //
            if(__CAN_SCPI_TransProcess(msg) != 0) {
                return;
            }
        }
        break;
    }
}




