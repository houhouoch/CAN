#include "Modbus_master_IO.h"
#include "Modbus_master_Power.h"
#include "usart.h"
#include "Commd.h"

#include "List_Delayer_Interface.h"
#include "BatTester_Interface.h"

uint16_t com_onoffRunTime = 0;
static TickType_t tick[2] = {0};

//设置电源输出
uint8_t Modbus_Update_Power_OnOff(uint16_t onoff) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    tick[0] = HAL_GetTick();
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, onoff);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 0, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    tick[1] = HAL_GetTick();
    com_onoffRunTime = tick[1] - tick[0];
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
//设置输出电压
uint8_t Modbus_Update_VoltageSet(float volSet) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, volSet);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 1, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
//设置输出电流
uint8_t Modbus_Update_CurrentSet(float curSet) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, curSet);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 3, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

//设置输出内阻
uint8_t Modbus_Update_ResInSet(float ResInSet) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, ResInSet);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 5, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}


//设置OVP电压
uint8_t Modbus_Update_OVPSet(float ovpSet) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, ovpSet);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 7, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
//设置OVP电流
uint8_t Modbus_Update_OCPSet(float ocpSet) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, ocpSet);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 9, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

//设置OVP开关
uint8_t Modbus_Update_OVP_OnOff(uint16_t onoff) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, onoff);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 11, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
//设置OCP开关
uint8_t Modbus_Update_OCP_OnOff(uint16_t onoff) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, onoff);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 12, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

//更新斜率模式
uint8_t Modbus_Update_SlewRate_Set(uint16_t slewRate) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, slewRate);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 13, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

//更新电压斜率
uint8_t Modbus_Update_VSR_UP_Set(float VSR) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, VSR);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 14, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
uint8_t Modbus_Update_VSR_DOWN_Set(float VSR) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, VSR);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 16, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
//更新电流斜率
uint8_t Modbus_Update_ISR_UP_Set(float ISR) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, ISR);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 18, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
uint8_t Modbus_Update_ISR_DOWN_Set(float ISR) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, ISR);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 20, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}



//更新电源模式
uint8_t Modbus_Update_PowerMode_Set(uint16_t mode) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, mode);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 22, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}



//过压保护延迟
uint8_t Modbus_Update_OVP_delayTime(int16_t value) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, value);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 28, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
//过流保护延迟
uint8_t Modbus_Update_OCP_delayTime(int16_t value) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, value);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 29, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

//过温保护阈值
uint8_t Modbus_Update_OTP_Temperature(int16_t value) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, value);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 30, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}


//协同开机延迟
uint8_t Modbus_Update_ON_delayTime(int16_t value) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, value);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 31, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
//协同关机延迟
uint8_t Modbus_Update_OFF_delayTime(int16_t value) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, value);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 32, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}




//更新最大风扇
uint8_t Modbus_Update_MaxFan_Set(uint16_t maxFAN) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, maxFAN);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 23, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

//远端补偿电压
uint8_t Modbus_Update_SenseVolt_Set(float value) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, value);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 24, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}


//泄放负载
uint8_t Modbus_Update_eLoad_Set(int16_t value) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, value);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 26, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
//掉电检测开关
uint8_t Modbus_Update_AC_Down_Set(int16_t value) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, value);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET + 27, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

/*
 * @brief       uint8_t Modbus_Update_OutSet(void)
 * @details     更新所有输出设置
 */
uint8_t Modbus_Update_OutSet(void) {
    uint8_t buf[128] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.onoff);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.volt_set);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.curr_set);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.ResIn_set);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.ovp_set);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.ocp_set);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.ovp_onoff);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.ocp_onoff);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.SlewRate_Mode);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.VSR_Up);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.VSR_Down);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.ISR_Up);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.ISR_Down);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.powerMode);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.maxFan);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.senseVolt);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.eLoad);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.ac_down);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.ovp_delayTime);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.ocp_delayTime);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.otp_Temperature);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.on_delayTime);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.off_delayTime);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}


uint8_t Modbus_Update_OutSet_temp(void) {
    uint8_t buf[128] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.onoff);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.volt_set);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.curr_set);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.ResIn_set);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.ovp_set);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.ocp_set);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.ovp_onoff);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.ocp_onoff);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.SlewRate_Mode);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.VSR_Up);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.VSR_Down);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.ISR_Up);
        MB_WRITE_FLOAT(pbuf, PowerInfo.outSet.ISR_Down);
        MB_WRITE_INT16(pbuf, PowerInfo.outSet.powerMode);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}


/*
 * @brief       uint8_t Modbus_Read_PowerState(void)
 * @details     获取当前电源状态
 */
uint8_t Modbus_Read_PowerState(void) {
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_STATE_OFFSET, 12);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                //
                State_Def state = {0};
                MB_READ_INT64(pdata, state.byte);
                PowerInfo.State.byte = state.byte;
                //
                State_Def event = {0};
                MB_READ_INT64(pdata, event.byte);
                PowerInfo.Event.byte = event.byte;
                //运行时长
                uint64_t runTime_ms = 0;
                MB_READ_INT64(pdata, runTime_ms);
                PowerInfo.Read.runTime_ms = runTime_ms;
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

uint8_t Modbus_Clean_PowerState(uint64_t status) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT64(pbuf, status);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_STATE_OFFSET + 0, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

/*
 * @brief       uint8_t Modbus_Read_PowerOut(void)
 * @details     获取当前电源输出回读
 */
uint8_t Modbus_Read_PowerOut(void) {
    uint8_t result = MB_OK;
    uint16_t crc = 0;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_READ_OFFSET, 9);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                MB_READ_INT16(pdata, crc);
                crc = crc;
                float volt_read = 0;
                MB_READ_FLOAT(pdata, volt_read);
                PowerInfo.Read.volt_read = volt_read;
                float volt_drv_read = 0;
                MB_READ_FLOAT(pdata, volt_drv_read);
                PowerInfo.Read.volt_drv_read = volt_drv_read;
                float curr_read = 0;
                MB_READ_FLOAT(pdata, curr_read);
                PowerInfo.Read.curr_read = curr_read;
                float power_read = 0;
                MB_READ_FLOAT(pdata, power_read);
                PowerInfo.Read.power_read = power_read;
                //
                PowerInfo.StateBar.bits.Sample_Ready = 1;
                __SCPI_Sample_Release();
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}




uint8_t Modbus_Read_PowerOutSet(void) {
    if((BatTester_IsRunning() == 0) && (ListOut_IsRunning() == 0)) { return MB_OK; }
    uint8_t result = MB_OK;
    uint16_t crc = 0;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_SET_OFFSET, 5);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                MB_READ_INT16(pdata, crc);
                crc = crc;
                float volt_read = 0;
                MB_READ_FLOAT(pdata, volt_read);
                PowerInfo.outSet.volt_set = volt_read;
                float curr_read = 0;
                MB_READ_FLOAT(pdata, curr_read);
                PowerInfo.outSet.curr_set = curr_read;
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

/*
 * @brief       uint8_t Modbus_Read_Temperature(void)
 * @details     获取当前电源温度
 */
uint8_t Modbus_Read_Temperature(void) {
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_TEMP_OFFSET, 6);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                //温度
                MB_READ_INT16(pdata, PowerInfo.Temperature.temp_ext_1);
                MB_READ_INT16(pdata, PowerInfo.Temperature.temp_ext_2);
                MB_READ_INT16(pdata, PowerInfo.Temperature.temp_ext_3);
                MB_READ_INT16(pdata, PowerInfo.Temperature.temp_ext_4);
                MB_READ_INT16(pdata, PowerInfo.Temperature.temp_ext_5);
                MB_READ_INT16(pdata, PowerInfo.Temperature.temp_pcb);
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}


/*
 * @brief       uint8_t Modbus_Read_Temperature(void)
 * @details     获取当前模组信息
 */
uint8_t Modbus_Read_Model_Data(void) {
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_MODEL_OFFSET, 16);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                //温度
                MB_READ_FLOAT(pdata, PowerInfo.modelData[1].chDrvVoltage);
                MB_READ_FLOAT(pdata, PowerInfo.modelData[1].chCurrent);
                MB_READ_FLOAT(pdata, PowerInfo.modelData[2].chDrvVoltage);
                MB_READ_FLOAT(pdata, PowerInfo.modelData[2].chCurrent);
                MB_READ_FLOAT(pdata, PowerInfo.modelData[3].chDrvVoltage);
                MB_READ_FLOAT(pdata, PowerInfo.modelData[3].chCurrent);
                MB_READ_FLOAT(pdata, PowerInfo.modelData[4].chDrvVoltage);
                MB_READ_FLOAT(pdata, PowerInfo.modelData[4].chCurrent);
                float current = IF_Power_Read_Curr();
                for(uint8_t i = 1; i <= PowerInfo.Read.powerCount; ++i) {
                    current -= PowerInfo.modelData[i].chCurrent;
                }
                PowerInfo.modelData[0].chCurrent = current;
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}



/*
 * @brief       uint8_t Modbus_Read_ModelInfo(void)
 * @details     读取模组信息
 */
uint8_t Modbus_Read_ModelInfo(void) {
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_INFO_OFFSET, 43);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                MB_READ_FLOAT(pdata, PowerInfo.Model.maxVolt);
                MB_READ_FLOAT(pdata, PowerInfo.Model.maxCurr);
                MB_READ_FLOAT(pdata, PowerInfo.Model.maxPower);
                #if defined(MULT_CHANNEL)
                #else
                strcpy((char *)PowerInfo.Model.modelName, (const char *)pdata);
                #endif
                pdata += sizeof(PowerInfo.Model.modelName);
                //
                strcpy((char *)PowerInfo.about.power_version, (const char *)pdata);
                pdata += sizeof(PowerInfo.about.power_version);
                uint16_t powerCount = 0, powerType = 0;
                MB_READ_INT16(pdata, powerCount);
                MB_READ_INT16(pdata, powerType);
                PowerInfo.Read.powerCount = powerCount;
                PowerInfo.Read.powerType = powerType;
                //默认远端补偿电压
                MB_READ_FLOAT(pdata, PowerInfo.Model.default_SenseVolt);
                //最大输出内阻
                MB_READ_FLOAT(pdata, PowerInfo.Model.maxResIn);
                //最小电压
                MB_READ_FLOAT(pdata, PowerInfo.Model.minVolt);
                //最小电流
                MB_READ_FLOAT(pdata, PowerInfo.Model.minCurr);
                //默认保护温度
                MB_READ_INT16(pdata, PowerInfo.Model.defaultTemp);
                //PowerInfo.Model.minVolt = 0.1;
            }
        }
    }
    COM_Mutex_Unlock();
    return result;
}




uint8_t Modbus_Power_OutPut_Sync(void) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        int16_t onoff = 1;
        MB_WRITE_INT16(pbuf, onoff);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_STATE_OFFSET + 12, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}




/*
 * @brief       校准用
 * @details
 */
uint8_t Modbus_Calib_Voltage(float voltage) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, voltage);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 0, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

uint8_t Modbus_Calib_Current(float current) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, current);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 2, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
uint8_t Modbus_Calib_Delete_Volt(void) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, 1);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 4, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
uint8_t Modbus_Calib_Delete_Curr(void) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, 1);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 5, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}





uint8_t Modbus_Calib_Voltage_Drv(float voltage) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, voltage);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 6, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
uint8_t Modbus_Calib_Delete_Volt_Drv(void) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, 1);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 8, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

#include "MsgBox.h"
void __Calib_Result_Process(uint8_t result)
{
    switch(result) {
        case CALIB_RES_OK:
            MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Calib_Completed));
            break;
        case CALIB_RES_CLOSE:
            MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Calib_Close));
            break;
        case CALIB_RES_NULLVALUE:
            MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Calib_NullValue));
            break;
        case CALIB_RES_IDLE:
            break;
    }
}

uint8_t Modbus_Calib_Read_Count(void) {
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 9, 6);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                int16_t voltCalib_Count = 0;
                MB_READ_INT16(pdata, voltCalib_Count);
                int16_t voltDrvCalib_Count = 0;
                MB_READ_INT16(pdata, voltDrvCalib_Count);
                int16_t currCalib_Count = 0;
                MB_READ_INT16(pdata, currCalib_Count);
                int16_t voltCalib_state = 0;
                MB_READ_INT16(pdata, voltCalib_state);
                int16_t currCalib_state = 0;
                MB_READ_INT16(pdata, currCalib_state);
                int16_t calibMode = 0;
                MB_READ_INT16(pdata, calibMode);
                PowerInfo.Read.voltCalib_Count = voltCalib_Count;
                PowerInfo.Read.voltDrvCalib_Count = voltDrvCalib_Count;
                PowerInfo.Read.currCalib_Count = currCalib_Count;
                //校准结果
                PowerInfo.Read.voltCalib_state = voltCalib_state;
                __Calib_Result_Process(voltCalib_state);
                PowerInfo.Read.currCalib_state = currCalib_state;
                __Calib_Result_Process(currCalib_state);
                //校准模式
                PowerInfo.workMode = calibMode;
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}



uint8_t Modbus_Calib_Mode(int16_t cailbMode) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, cailbMode);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 14, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

uint8_t Modbus_Calib_ZeroCurr(int16_t cailbMode) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, cailbMode);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 15, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}


#include "MsgBox.h"
uint8_t Modbus_Read_Calib_ZeroCurr(void) {
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 15, 1);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                int16_t calibZeroCurr = 0;
                MB_READ_INT16(pdata, calibZeroCurr);
                if(calibZeroCurr == 1) {
                    MsgBox_Printf(MSG_TYPE_TIPS, NULL, MultStr_GetS(&mStr_Calib_ZeroCurr_Completed));
                    PowerInfo.StateBar.bits.zeroCurr = 0;
                }
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}


uint8_t Modbus_Set_ZeroCurr_AutoEn(int16_t autoEn) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, autoEn);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DEBUG_OFFSET + 16, pbuf - buf, buf);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_WRITE)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}
