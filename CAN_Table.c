#include "CAN_Table.h"
#include "MsgBox.h"

typedef enum {
    CAN_RW_TYPE_W = 1,
    CAN_RW_TYPE_R = 2,
    CAN_RW_TYPE_RW = 3,
} eCAN_RW_TYPE_t;

typedef enum {
    CAN_REG_READ,                /*!< Read register values and pass to protocol stack. */
    CAN_REG_WRITE                /*!< Update register values. */
} eCANRegisterMode;

typedef struct {
    uint32_t regAddr;           //寄存器地址
    eCAN_RW_TYPE_t rw_type;     //读写(没用)
    uint8_t slaveID;            //
    void(*callFunction)(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
} CAN_User_Table_t;




uint8_t __ID(void) {
    uint8_t id = 0;
    switch(IF_Power_Get_Power_Mode())
    {
        case POWER_MODE_CAN_PARALLEL_MASTER:
        case POWER_MODE_CAN_SERIES_MASTER:
        case POWER_MODE_CAN_SYNERGY_MASTER:
            id = 0;
            break;
        case POWER_MODE_CAN_PARALLEL_SLAVE:
        case POWER_MODE_CAN_SERIES_SLAVE:
        case POWER_MODE_CAN_SYNERGY_SLAVE:
            id = IF_Option_Get_Slave_ID();
            break;
        default:
            if(__isMultChannel()) {
                id = IF_Power_Get_Hw_ID();
            } else {
                id = 0;
            }
            break;
    }
    return id;
}

//判断当前从设备模式是否与本机模式匹配
uint8_t __CAN_isMode(uint8_t powerMode) {
    switch(IF_Power_Get_Power_Mode()) {
        case POWER_MODE_CAN_PARALLEL_MASTER:
        case POWER_MODE_CAN_PARALLEL_SLAVE: {
            if((powerMode == POWER_MODE_CAN_PARALLEL_MASTER)
                    || (powerMode == POWER_MODE_CAN_PARALLEL_SLAVE)) {
                return 1;
            }
        }
        break;
        case POWER_MODE_CAN_SERIES_MASTER:
        case POWER_MODE_CAN_SERIES_SLAVE: {
            if((powerMode == POWER_MODE_CAN_SERIES_MASTER)
                    || (powerMode == POWER_MODE_CAN_SERIES_SLAVE)) {
                return 1;
            }
        }
        break;
        case POWER_MODE_CAN_SYNERGY_MASTER:
        case POWER_MODE_CAN_SYNERGY_SLAVE: {
            if((powerMode == POWER_MODE_CAN_SYNERGY_MASTER)
                    || (powerMode == POWER_MODE_CAN_SYNERGY_SLAVE)) {
                return 1;
            }
        }
        break;
        case POWER_MODE_NORMAL: {
            return 1;
        }
        break;
    }
    return 0;
}



void CAN_OnOff_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_Voltage_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_Current_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_ResIn_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_OVP_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_OCP_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_OVP_On_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_OCP_On_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_SlewRate_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_VSR_Up_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_VSR_Down_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_ISR_Up_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_ISR_Down_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);

void CAN_Sync_Request_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);

void CAN_Out_Voltage(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_Out_Current(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_Out_Power(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);

void CAN_State_Read(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);
void CAN_Event_Read(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode);


#if 1

/**
  * @brief  并联指令
  * @retval
  */
CAN_User_Table_t CAN_Out_Set_Para_Table[] = {
    {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 0,  .rw_type = CAN_RW_TYPE_RW, //输出开关
        .callFunction = CAN_OnOff_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 1,  .rw_type = CAN_RW_TYPE_RW, //电压设定
        .callFunction = CAN_Voltage_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 2,  .rw_type = CAN_RW_TYPE_RW, //电流设定
        .callFunction = CAN_Current_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 3,  .rw_type = CAN_RW_TYPE_RW, //内阻设定
        .callFunction = CAN_ResIn_Set,
    },
    //保护设定
    {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 4,  .rw_type = CAN_RW_TYPE_RW, //过压保护
        .callFunction = CAN_OVP_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 5,  .rw_type = CAN_RW_TYPE_RW, //过流保护
        .callFunction = CAN_OCP_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 6,  .rw_type = CAN_RW_TYPE_RW, //过压保护开关
        .callFunction = CAN_OVP_On_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 7,  .rw_type = CAN_RW_TYPE_RW, //过流保护开关
        .callFunction = CAN_OCP_On_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 8,  .rw_type = CAN_RW_TYPE_RW, //斜率模式
        .callFunction = CAN_SlewRate_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 9,  .rw_type = CAN_RW_TYPE_RW, //电压上升斜率
        .callFunction = CAN_VSR_Up_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 10,  .rw_type = CAN_RW_TYPE_RW, //电压下降斜率
        .callFunction = CAN_VSR_Down_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 11,  .rw_type = CAN_RW_TYPE_RW, //电流上升斜率
        .callFunction = CAN_ISR_Up_Set,
    }, {
        .regAddr = CAN_OUTSET_PARA_REG_OFFSET + 12,  .rw_type = CAN_RW_TYPE_RW, //电流下降斜率
        .callFunction = CAN_ISR_Down_Set,
    },
};


void CAN_OutSet_Parallel_Master_Update(void) {
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.ParallelSlaveInfo;
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Set_Para_Table); ++i) {
        uint8_t data[8] = {0};
        data[0] = PowerInfo.Read.powerType;
        data[1] = IF_Power_Get_Power_Mode();
        const uint8_t dataBase = 4;
        if(CAN_Out_Set_Para_Table[i].callFunction != NULL) {
            CAN_Out_Set_Para_Table[i].callFunction(pSlaveInfo, __ID(), &data[dataBase], CAN_REG_READ);
            CANFD_Send_Extended(CAN_Out_Set_Para_Table[i].regAddr + ((IF_Power_Get_Power_Mode() << 4) | __ID()) * 0x100, data);
        }
    }
}

uint8_t CAN_OutSet_Parallel_Slave_Action(uint32_t id, uint8_t *pdata) {
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.ParallelSlaveInfo;
    uint8_t result = 0;
    uint8_t deviceID = ((id & 0x0f00) >> 8);
    uint8_t powerMode = ((id & 0xf000) >> 12);
    id &= ~0xff00;
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Set_Para_Table); ++i) {
        const uint8_t dataBase = 4;
        if(CAN_Out_Set_Para_Table[i].callFunction != NULL) {
            if(id == CAN_Out_Set_Para_Table[i].regAddr) {
                if(__CAN_isMode(powerMode)) {
                    CAN_Out_Set_Para_Table[i].callFunction(pSlaveInfo, deviceID, &pdata[dataBase], CAN_REG_WRITE);
                    return 1;
                }
            }
        }
    }
    return 0;
}


#endif


#if 1

/**
  * @brief  串联指令
  * @retval
  */
CAN_User_Table_t CAN_Out_Set_Series_Table[] = {
    {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 0,  .rw_type = CAN_RW_TYPE_RW, //输出开关
        .callFunction = CAN_OnOff_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 1,  .rw_type = CAN_RW_TYPE_RW, //电压设定
        .callFunction = CAN_Voltage_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 2,  .rw_type = CAN_RW_TYPE_RW, //电流设定
        .callFunction = CAN_Current_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 3,  .rw_type = CAN_RW_TYPE_RW, //内阻设定
        .callFunction = CAN_ResIn_Set,
    },
    //保护设定
    {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 4,  .rw_type = CAN_RW_TYPE_RW, //过压保护
        .callFunction = CAN_OVP_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 5,  .rw_type = CAN_RW_TYPE_RW, //过流保护
        .callFunction = CAN_OCP_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 6,  .rw_type = CAN_RW_TYPE_RW, //过压保护开关
        .callFunction = CAN_OVP_On_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 7,  .rw_type = CAN_RW_TYPE_RW, //过流保护开关
        .callFunction = CAN_OCP_On_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 8,  .rw_type = CAN_RW_TYPE_RW, //斜率模式
        .callFunction = CAN_SlewRate_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 9,  .rw_type = CAN_RW_TYPE_RW, //电压上升斜率
        .callFunction = CAN_VSR_Up_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 10,  .rw_type = CAN_RW_TYPE_RW, //电压下降斜率
        .callFunction = CAN_VSR_Down_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 11,  .rw_type = CAN_RW_TYPE_RW, //电流上升斜率
        .callFunction = CAN_ISR_Up_Set,
    }, {
        .regAddr = CAN_OUTSET_SER_REG_OFFSET + 12,  .rw_type = CAN_RW_TYPE_RW, //电流下降斜率
        .callFunction = CAN_ISR_Down_Set,
    },
};



void CAN_OutSet_Series_Master_Update(void) {
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.SeriesSlaveInfo;
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Set_Series_Table); ++i) {
        uint8_t data[8] = {0};
        data[0] = PowerInfo.Read.powerType;
        data[1] = IF_Power_Get_Power_Mode();
        const uint8_t dataBase = 4;
        if(CAN_Out_Set_Series_Table[i].callFunction != NULL) {
            CAN_Out_Set_Series_Table[i].callFunction(pSlaveInfo, __ID(), &data[dataBase], CAN_REG_READ);
            CANFD_Send_Extended(CAN_Out_Set_Series_Table[i].regAddr + ((IF_Power_Get_Power_Mode() << 4) | __ID()) * 0x100, data);
        }
    }
}

uint8_t CAN_OutSet_Series_Slave_Action(uint32_t id, uint8_t *pdata) {
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.SeriesSlaveInfo;
    uint8_t result = 0;
    uint8_t deviceID = ((id & 0x0f00) >> 8);
    uint8_t powerMode = ((id & 0xf000) >> 12);
    id &= ~0xff00;
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Set_Series_Table); ++i) {
        const uint8_t dataBase = 4;
        if(CAN_Out_Set_Series_Table[i].callFunction != NULL) {
            if(id == CAN_Out_Set_Series_Table[i].regAddr) {
                if(__CAN_isMode(powerMode)) {
                    CAN_Out_Set_Series_Table[i].callFunction(pSlaveInfo, deviceID, &pdata[dataBase], CAN_REG_WRITE);
                    return 1;
                }
            }
        }
    }
    return 0;
}

#endif


#if defined(MULT_CHANNEL)

/**
  * @brief  协同主机
  * @retval
  */
CAN_User_Table_t CAN_Out_Set_Synergy_M2S_Table[] = {
    {
        .regAddr = CAN_OUTSET_SYNERGY_REG_OFFSET + 0,  .rw_type = CAN_RW_TYPE_RW, //输出开关
        .callFunction = CAN_OnOff_Set,
    },
};

void CAN_OutSet_Synergy_Master_Update(void) {
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.SynergySlaveInfo;
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Set_Synergy_M2S_Table); ++i) {
        uint8_t data[8] = {0};
        data[0] = PowerInfo.Read.powerType;
        data[1] = IF_Power_Get_Power_Mode();
        const uint8_t dataBase = 4;
        if(CAN_Out_Set_Synergy_M2S_Table[i].callFunction != NULL) {
            CAN_Out_Set_Synergy_M2S_Table[i].callFunction(pSlaveInfo, __ID(), &data[dataBase], CAN_REG_READ);
            CANFD_Send_Extended(CAN_Out_Set_Synergy_M2S_Table[i].regAddr + __ID() * 0x100, data);
        }
    }
}

uint8_t CAN_OutSet_Synergy_Slave_Action(uint32_t id, uint8_t *pdata) {
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.SynergySlaveInfo;
    uint8_t result = 0;
    uint8_t deviceID = ((id & 0xff00) >> 8);
    uint8_t powerMode = pdata[1];
    id &= ~0xff00;
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Set_Synergy_M2S_Table); ++i) {
        const uint8_t dataBase = 4;
        if(CAN_Out_Set_Synergy_M2S_Table[i].callFunction != NULL) {
            if(id == CAN_Out_Set_Synergy_M2S_Table[i].regAddr) {
                if(__CAN_isMode(powerMode)) {
                    CAN_Out_Set_Synergy_M2S_Table[i].callFunction(pSlaveInfo, deviceID, &pdata[dataBase], CAN_REG_WRITE);
                    return 1;
                }
            }
        }
    }
    return 0;
}


CAN_User_Table_t CAN_Out_Set_Synergy_S2M_Table[] = {
    {
        .regAddr = CAN_OUTSET_SYNERGY_REG_OFFSET + 0,  .rw_type = CAN_RW_TYPE_RW, //输出开关
        .callFunction = CAN_OnOff_Set,
    },
    {
        .regAddr = CAN_OUTSET_SYNERGY_REG_OFFSET + 1,  .rw_type = CAN_RW_TYPE_RW, //请求输出开关
        .callFunction = CAN_Sync_Request_Set,
    },
};


void CAN_OutSet_Synergy_Slave_Update(void) {
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.SynergySlaveInfo;
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Set_Synergy_S2M_Table); ++i) {
        uint8_t data[8] = {0};
        data[0] = PowerInfo.Read.powerType;
        data[1] = IF_Power_Get_Power_Mode();
        const uint8_t dataBase = 4;
        if(CAN_Out_Set_Synergy_S2M_Table[i].callFunction != NULL) {
            CAN_Out_Set_Synergy_S2M_Table[i].callFunction(pSlaveInfo, __ID(), &data[dataBase], CAN_REG_READ);
            CANFD_Send_Extended(CAN_Out_Set_Synergy_S2M_Table[i].regAddr + __ID() * 0x100, data);
        }
    }
}

uint8_t CAN_OutSet_Synergy_Master_Action(uint32_t id, uint8_t *pdata) {
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.SynergySlaveInfo;
    uint8_t result = 0;
    uint8_t deviceID = ((id & 0xff00) >> 8);
    uint8_t powerMode = pdata[1];
    id &= ~0xff00;
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Set_Synergy_S2M_Table); ++i) {
        const uint8_t dataBase = 4;
        if(CAN_Out_Set_Synergy_S2M_Table[i].callFunction != NULL) {
            if(id == CAN_Out_Set_Synergy_S2M_Table[i].regAddr) {
                if(__CAN_isMode(powerMode)) {
                    CAN_Out_Set_Synergy_S2M_Table[i].callFunction(pSlaveInfo, deviceID, &pdata[dataBase], CAN_REG_WRITE);
                    return 1;
                }
            }
        }
    }
    return 0;
}

#endif


#if 1

CAN_User_Table_t CAN_Out_Read_Table[] = {
    //回读上报
    {
        .regAddr = CAN_OUTREAD_REG_OFFSET + 0,  .rw_type = CAN_RW_TYPE_RW, //电压
        .callFunction = CAN_Out_Voltage,
    }, {
        .regAddr = CAN_OUTREAD_REG_OFFSET + 1,  .rw_type = CAN_RW_TYPE_RW, //电流
        .callFunction = CAN_Out_Current,
    }, {
        .regAddr = CAN_OUTREAD_REG_OFFSET + 2,  .rw_type = CAN_RW_TYPE_RW, //功率
        .callFunction = CAN_Out_Power,
    },


    {
        .regAddr = CAN_OUTREAD_REG_OFFSET + 3,  .rw_type = CAN_RW_TYPE_RW, //状态
        .callFunction = CAN_State_Read,
    }, {
        .regAddr = CAN_OUTREAD_REG_OFFSET + 4,  .rw_type = CAN_RW_TYPE_RW, //状态
        .callFunction = CAN_Event_Read,
    },
};


//CAN ID = REG | ((POWER_MODE<<4) | SLAVE_ID)*0X100
void CAN_OutSet_Read_Update(void) {
    SlaveInfo_Def *pSlaveInfo = NULL;
    switch(IF_Power_Get_Power_Mode()) {
        case POWER_MODE_CAN_PARALLEL_MASTER:
        case POWER_MODE_CAN_PARALLEL_SLAVE:
            pSlaveInfo = PowerInfo.Read.ParallelSlaveInfo;
            break;
        case POWER_MODE_CAN_SERIES_MASTER:
        case POWER_MODE_CAN_SERIES_SLAVE:
            pSlaveInfo = PowerInfo.Read.SeriesSlaveInfo;
            break;
        default:
            return;
    }
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Read_Table); ++i) {
        uint8_t data[8] = {0};
        pSlaveInfo[__ID()].powerType = PowerInfo.Read.powerType;
        pSlaveInfo[__ID()].powerMode = IF_Power_Get_Power_Mode();
        data[0] = PowerInfo.Read.powerType;
        //        #if defined(__DEBUG_CAN_PACKET_LOST)
        static uint8_t packetNo = 0;
        data[2] = packetNo++;
        //        #endif
        const uint8_t dataBase = 4;
        if(CAN_Out_Read_Table[i].callFunction != NULL) {
            CAN_Out_Read_Table[i].callFunction(pSlaveInfo, __ID(), &data[dataBase], CAN_REG_READ);
            CANFD_Send_Extended(CAN_Out_Read_Table[i].regAddr + ((IF_Power_Get_Power_Mode() << 4) | __ID()) * 0x100, data);
        }
    }
}

uint8_t CAN_OutSet_Read_Action(uint32_t id, uint8_t *pdata) {
    uint8_t result = 0;
    uint8_t deviceID = ((id & 0x0f00) >> 8);
    uint8_t powerMode = ((id & 0xf000) >> 12);
    id &= ~0xff00;
    SlaveInfo_Def *pSlaveInfo = NULL;
    switch(IF_Power_Get_Power_Mode()) {
        case POWER_MODE_CAN_PARALLEL_MASTER:
        case POWER_MODE_CAN_PARALLEL_SLAVE:
            pSlaveInfo = PowerInfo.Read.ParallelSlaveInfo;
            if(deviceID < ARRAY_COUNT(PowerInfo.Read.ParallelSlaveInfo)) {
            } else {
                return 1;
            }
            break;
        case POWER_MODE_CAN_SERIES_MASTER:
        case POWER_MODE_CAN_SERIES_SLAVE:
            pSlaveInfo = PowerInfo.Read.SeriesSlaveInfo;
            if(deviceID < ARRAY_COUNT(PowerInfo.Read.SeriesSlaveInfo)) {
            } else {
                return 1;
            }
            break;
        default:
            return 1;
    }
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Read_Table); ++i) {
        const uint8_t dataBase = 4;
        if(CAN_Out_Read_Table[i].callFunction != NULL) {
            if(id == CAN_Out_Read_Table[i].regAddr) {
                if(__CAN_isMode(powerMode)) {
                    pSlaveInfo[deviceID].powerMode = powerMode;
                    //丢包检测
                    #if defined(__DEBUG_CAN_PACKET_LOST)
                    uint8_t packetNo = pdata[2];
                    uint8_t packetNo_next = pSlaveInfo[deviceID].packetNo + 1;
                    if(packetNo_next != packetNo) {
                        if(pSlaveInfo[deviceID].data_ready != 0) {
                            pSlaveInfo[deviceID].errCount += 1;
                            MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, "CAN Lost Packet %d  %d", pSlaveInfo[deviceID].packetNo, packetNo);
                        }
                    }
                    pSlaveInfo[deviceID].packetNo = packetNo;
                    #endif
                    pSlaveInfo[deviceID].powerType = pdata[0];
                    CAN_Out_Read_Table[i].callFunction(pSlaveInfo, deviceID, &pdata[dataBase], CAN_REG_WRITE);
                }
                return 1;
            }
        }
    }
    return 0;
}

#endif




#if defined(MULT_CHANNEL)

CAN_User_Table_t CAN_Out_Read_Mult_Table[] = {
    //回读上报
    {
        .regAddr = CAN_OUTREAD_MULTCH_REG_OFFSET + 0,  .rw_type = CAN_RW_TYPE_RW, //电压
        .callFunction = CAN_Out_Voltage,
    }, {
        .regAddr = CAN_OUTREAD_MULTCH_REG_OFFSET + 1,  .rw_type = CAN_RW_TYPE_RW, //电流
        .callFunction = CAN_Out_Current,
    }, {
        .regAddr = CAN_OUTREAD_MULTCH_REG_OFFSET + 2,  .rw_type = CAN_RW_TYPE_RW, //功率
        .callFunction = CAN_Out_Power,
    },


    {
        .regAddr = CAN_OUTREAD_MULTCH_REG_OFFSET + 3,  .rw_type = CAN_RW_TYPE_RW, //状态
        .callFunction = CAN_State_Read,
    }, {
        .regAddr = CAN_OUTREAD_MULTCH_REG_OFFSET + 4,  .rw_type = CAN_RW_TYPE_RW, //状态
        .callFunction = CAN_Event_Read,
    },
};


//CAN ID = REG | ((POWER_MODE<<4) | SLAVE_ID)*0X100
void CAN_OutSet_Read_Update_MultCH(void) {
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.MultSlaveInfo;
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Read_Mult_Table); ++i) {
        uint8_t data[8] = {0};
        pSlaveInfo[IF_Power_Get_Hw_ID()].powerType = PowerInfo.Read.powerType;
        pSlaveInfo[IF_Power_Get_Hw_ID()].powerMode = IF_Power_Get_Power_Mode();
        data[0] = PowerInfo.Read.powerType;
        data[1] = IF_Power_Get_Power_Mode();
        //        #if defined(__DEBUG_CAN_PACKET_LOST)
        static uint8_t packetNo = 0;
        data[2] = packetNo++;
        //        #endif
        const uint8_t dataBase = 4;
        if(CAN_Out_Read_Mult_Table[i].callFunction != NULL) {
            CAN_Out_Read_Mult_Table[i].callFunction(pSlaveInfo, IF_Power_Get_Hw_ID(), &data[dataBase], CAN_REG_READ);
            CANFD_Send_Extended(CAN_Out_Read_Mult_Table[i].regAddr + ((IF_Option_Get_CAN_Groups_ID() << 4) | IF_Power_Get_Hw_ID()) * 0x100, data);
        }
    }
}
uint8_t CAN_OutSet_Read_Action_MultCH(uint32_t id, uint8_t *pdata) {
    uint8_t result = 0;
    uint8_t deviceID = ((id & 0x0f00) >> 8);
    uint8_t groupID = ((id & 0xf000) >> 12);
    id &= ~0xff00;
    SlaveInfo_Def *pSlaveInfo = PowerInfo.Read.MultSlaveInfo;
    if(deviceID < ARRAY_COUNT(PowerInfo.Read.MultSlaveInfo)) {
    } else {
        return 1;
    }
    for(uint8_t i = 0; i < ARRAY_COUNT(CAN_Out_Read_Mult_Table); ++i) {
        const uint8_t dataBase = 4;
        if(CAN_Out_Read_Mult_Table[i].callFunction != NULL) {
            if(id == CAN_Out_Read_Mult_Table[i].regAddr) {
                if(groupID == IF_Option_Get_CAN_Groups_ID()) {
                    pSlaveInfo[deviceID].powerMode = pdata[1];
                    #if defined(__DEBUG_CAN_PACKET_LOST)
                    uint8_t packetNo = pdata[2];
                    uint8_t packetNo_next = pSlaveInfo[deviceID].packetNo + 1;
                    if(packetNo_next != packetNo) {
                        pSlaveInfo[deviceID].errCount += 1;
                        MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, "CAN Lost Packet %d  %d", pSlaveInfo[deviceID].packetNo, packetNo);
                    }
                    pSlaveInfo[deviceID].packetNo = packetNo;
                    #endif
                    pSlaveInfo[deviceID].powerType = pdata[0];
                    CAN_Out_Read_Mult_Table[i].callFunction(pSlaveInfo, deviceID, &pdata[dataBase], CAN_REG_WRITE);
                }
                return 1;
            }
        }
    }
    return 0;
}

#endif




#include "Msgbox.h"
#include "MultString.h"


//CAN通讯已连接
static void __CAN_Device_Connected(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID) {
    if(pSlaveInfo == NULL) { return; }
    if(slaveID != __ID()) {
        if(pSlaveInfo[slaveID].timeout <= 0) {
            if(slaveID == 0) {
                MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Master_is_Connected));
            } else {
                MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Slave_is_Connected), slaveID);
            }
        }
    }
    pSlaveInfo[slaveID].timeout = 1000;
    pSlaveInfo[slaveID].data_ready = 1;
}

void CAN_Out_Voltage(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float voltage = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, voltage);
        pSlaveInfo[slaveID].volt_read = voltage;
    } else
        if(eMode  == CAN_REG_READ) {
            voltage = IF_Power_Read_Volt();
            CAN_WRITE_FLOAT(pucRegBuffer, voltage);
            pSlaveInfo[slaveID].volt_read = voltage;
        }
    __CAN_Device_Connected(pSlaveInfo, slaveID);
}

void CAN_Out_Current(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float current = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, current);
        pSlaveInfo[slaveID].curr_read = current;
    } else
        if(eMode  == CAN_REG_READ) {
            current = IF_Power_Read_Curr();
            CAN_WRITE_FLOAT(pucRegBuffer, current);
            pSlaveInfo[slaveID].curr_read = current;
        }
    __CAN_Device_Connected(pSlaveInfo, slaveID);
}

void CAN_Out_Power(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float power = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, power);
        pSlaveInfo[slaveID].power_read = power;
    } else
        if(eMode  == CAN_REG_READ) {
            power = IF_Power_Read_Power();
            CAN_WRITE_FLOAT(pucRegBuffer, power);
            pSlaveInfo[slaveID].power_read = power;
        }
    __CAN_Device_Connected(pSlaveInfo, slaveID);
}



//状态
void CAN_State_Read(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    int32_t state = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_INT32(pucRegBuffer, state);
        pSlaveInfo[slaveID].State.byte = state;
    } else
        if(eMode  == CAN_REG_READ) {
            state = IF_Power_Read_State();
            CAN_WRITE_INT32(pucRegBuffer, state);
            pSlaveInfo[slaveID].State.byte = state;
        }
    __CAN_Device_Connected(pSlaveInfo, slaveID);
}
//事件
void CAN_Event_Read(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    int32_t event = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_INT32(pucRegBuffer, event);
        pSlaveInfo[slaveID].Event.byte = event;
    } else
        if(eMode  == CAN_REG_READ) {
            event = IF_Power_Read_Event();
            CAN_WRITE_INT32(pucRegBuffer, event);
            pSlaveInfo[slaveID].Event.byte = event;
        }
    __CAN_Device_Connected(pSlaveInfo, slaveID);
}


#include "Interface.h"
/*
 * @brief       void CAN_OnOff_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读输出开关
 */
void CAN_OnOff_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    uint16_t onoff = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_INT16(pucRegBuffer, onoff);
        IF_Power_Set_On_Off(onoff);
    } else
        if(eMode  == CAN_REG_READ) {
            onoff = IF_Power_Get_On_Off();
            CAN_WRITE_INT16(pucRegBuffer, onoff);
        }
}


/*
 * @brief       void CAN_Voltage_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读电压设置
 */
void CAN_Voltage_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float volt = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, volt);
        IF_Power_Set_OutVolt(volt);
    } else
        if(eMode  == CAN_REG_READ) {
            volt = IF_Power_Get_OutVolt();
            CAN_WRITE_FLOAT(pucRegBuffer, volt);
        }
}

/*
 * @brief       void CAN_Current_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读电流设置
 */
void CAN_Current_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float curr = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, curr);
        IF_Power_Set_OutCurr(curr);
    } else
        if(eMode  == CAN_REG_READ) {
            curr = IF_Power_Get_OutCurr();
            CAN_WRITE_FLOAT(pucRegBuffer, curr);
        }
}

/*
 * @brief       void CAN_ResIn_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读内阻设置
 */
void CAN_ResIn_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float resIn = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, resIn);
        IF_Power_Set_ResIn(resIn);
    } else
        if(eMode  == CAN_REG_READ) {
            resIn = IF_Power_Get_ResIn();
            CAN_WRITE_FLOAT(pucRegBuffer, resIn);
        }
}

/*
 * @brief       void CAN_OVP_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读OVP设置
 */
void CAN_OVP_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float volt = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, volt);
        IF_Power_Set_OVP(volt);
    } else
        if(eMode  == CAN_REG_READ) {
            volt = IF_Power_Get_OVP();
            CAN_WRITE_FLOAT(pucRegBuffer, volt);
        }
}

/*
 * @brief       void CAN_OCP_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读OCP设置
 */
void CAN_OCP_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float curr = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, curr);
        IF_Power_Set_OCP(curr);
    } else
        if(eMode  == CAN_REG_READ) {
            curr = IF_Power_Get_OCP();
            CAN_WRITE_FLOAT(pucRegBuffer, curr);
        }
}





/*
 * @brief       void CAN_OVP_On_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读OVP开关设置
 */
void CAN_OVP_On_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    uint16_t onoff = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_INT16(pucRegBuffer, onoff);
        IF_Power_Set_OVP_onoff(onoff);
    } else
        if(eMode  == CAN_REG_READ) {
            onoff = IF_Power_Get_OVP_onoff();
            CAN_WRITE_INT16(pucRegBuffer, onoff);
        }
}

/*
 * @brief       void CAN_OCP_On_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读OCP开关设置
 */
void CAN_OCP_On_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    uint16_t onoff = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_INT16(pucRegBuffer, onoff);
        IF_Power_Set_OCP_onoff(onoff);
    } else
        if(eMode  == CAN_REG_READ) {
            onoff = IF_Power_Get_OCP_onoff();
            CAN_WRITE_INT16(pucRegBuffer, onoff);
        }
}


/*
 * @brief       void CAN_SlewRate_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读斜率模式
 */
void CAN_SlewRate_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    uint16_t slewRate = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_INT16(pucRegBuffer, slewRate);
        IF_Power_Set_SlewRate_Mode(slewRate);
    } else
        if(eMode  == CAN_REG_READ) {
            slewRate = IF_Power_Get_SlewRate_Mode();
            CAN_WRITE_INT16(pucRegBuffer, slewRate);
        }
}


/*
 * @brief       void CAN_SlewRate_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读电压上升斜率
 */
void CAN_VSR_Up_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float VSR_Up = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, VSR_Up);
        IF_Power_Set_VSR_Up(VSR_Up);
    } else
        if(eMode  == CAN_REG_READ) {
            VSR_Up = IF_Power_Get_VSR_Up();
            CAN_WRITE_FLOAT(pucRegBuffer, VSR_Up);
        }
}
/*
 * @brief       void CAN_SlewRate_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读电压下降斜率
 */
void CAN_VSR_Down_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float VSR_Down = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, VSR_Down);
        IF_Power_Set_VSR_Down(VSR_Down);
    } else
        if(eMode  == CAN_REG_READ) {
            VSR_Down = IF_Power_Get_VSR_Down();
            CAN_WRITE_FLOAT(pucRegBuffer, VSR_Down);
        }
}


/*
 * @brief       void CAN_SlewRate_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读电流上升斜率
 */
void CAN_ISR_Up_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float ISR_Up = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, ISR_Up);
        IF_Power_Set_ISR_Up(ISR_Up);
    } else
        if(eMode  == CAN_REG_READ) {
            ISR_Up = IF_Power_Get_ISR_Up();
            CAN_WRITE_FLOAT(pucRegBuffer, ISR_Up);
        }
}
/*
 * @brief       void CAN_SlewRate_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读电流下降斜率模式
 */
void CAN_ISR_Down_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    float ISR_Down = 0;
    if(eMode  == CAN_REG_WRITE) {
        CAN_READ_FLOAT(pucRegBuffer, ISR_Down);
        IF_Power_Set_ISR_Down(ISR_Down);
    } else
        if(eMode  == CAN_REG_READ) {
            ISR_Down = IF_Power_Get_ISR_Down();
            CAN_WRITE_FLOAT(pucRegBuffer, ISR_Down);
        }
}



/*
 * @brief       void CAN_Sync_Request_Set(uint8_t * pucRegBuffer,eCANRegisterMode eMode)
 * @details     ModBus写/读同步请求
 */
void CAN_Sync_Request_Set(SlaveInfo_Def *pSlaveInfo, uint8_t slaveID, uint8_t *pucRegBuffer, eCANRegisterMode eMode) {
    if(pSlaveInfo == NULL) { return; }
    if(eMode  == CAN_REG_WRITE) {
        uint16_t onoff = 0;
        CAN_READ_INT16(pucRegBuffer, onoff);
        PowerInfo.Read.syncRequest = onoff;
    } else
        if(eMode  == CAN_REG_READ) {
            uint16_t onoff = PowerInfo.Read.syncRequest;
            CAN_WRITE_INT16(pucRegBuffer, onoff);
        }
}
