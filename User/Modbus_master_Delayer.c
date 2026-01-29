#include "Modbus_master_IO.h"
#include "Modbus_master_Delayer.h"
#include "usart.h"
#include "Commd.h"

#include "List_Delayer_Interface.h"

#define DELAYER
#if defined(DELAYER)
/*
 * @brief       uint8_t Modbus_Read_Delayer(void)
 * @details     读取Delayer当前状态数据
 */
uint8_t Modbus_Read_Delayer(void) {
    uint8_t result = MB_OK;
    uint16_t crc = 0;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 0, 10);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                //
                uint16_t curState = 0;
                float remainTime = 0;
                uint16_t curGroups = 0, endGroups = 0;
                uint32_t remainCycles = 0, curCycled = 0;
                MB_READ_INT16(pdata, curState);
                MB_READ_FLOAT(pdata, remainTime);
                MB_READ_INT16(pdata, curGroups);
                MB_READ_INT16(pdata, endGroups);
                MB_READ_INT32(pdata, remainCycles);
                MB_READ_INT32(pdata, curCycled);
                MB_READ_INT16(pdata, crc);
                delayerInfo.now.state = curState;
                delayerInfo.now.RemainTimes = remainTime;
                delayerInfo.now.curGroup = curGroups;
                delayerInfo.now.endGroup = endGroups;
                delayerInfo.now.RemainCycles = remainCycles;
                delayerInfo.now.curCycled = curCycled;
                delayerInfo.now.crc_now = crc;
                if(crc != delayerInfo.set.crc) {
                    delayerInfo.now.updateRequest = 1;
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

//设置DELAYER状态
uint8_t Modbus_Delayer_Set_State(uint16_t onoff) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, onoff);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 0, pbuf - buf, buf);
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


//设置DELAYER起始组
uint8_t Modbus_Delayer_Set_Start(uint16_t start) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, start);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 10, pbuf - buf, buf);
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
//设置DELAYER组数
uint8_t Modbus_Delayer_Set_Groups(uint16_t groups) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, groups);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 11, pbuf - buf, buf);
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
//设置DELAYER循环次数
uint8_t Modbus_Delayer_Set_Cycles(uint16_t cycles) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, cycles);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 12, pbuf - buf, buf);
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
//设置DELAYER终止态
uint8_t Modbus_Delayer_Set_EndState(uint16_t endstate) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, endstate);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 13, pbuf - buf, buf);
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


//设置Delayer停止条件
uint8_t Modbus_Delayer_Set_StopRule(uint16_t stopRule) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, stopRule);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 14, pbuf - buf, buf);
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
//设置Delayer停止值
uint8_t Modbus_Delayer_Set_StopValue(float stopValue) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, stopValue);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 15, pbuf - buf, buf);
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



//设置DELAYER编号
uint8_t Modbus_Delayer_Set_Parameter_No(uint16_t param_no) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, param_no);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 17, pbuf - buf, buf);
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
//设置Delayer组参数
uint8_t Modbus_Delayer_Set_Parameter(uint8_t out, float time) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        uint16_t out_temp = out;
        MB_WRITE_INT16(pbuf, out_temp);
        MB_WRITE_FLOAT(pbuf, time);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 18, pbuf - buf, buf);
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
//读取Delayer组参数
uint8_t Modbus_Delayer_Get_Parameter(uint8_t *out, float *time) {
    if((out == NULL) || (time == NULL)) {
        return MB_ERROR;
    }
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_DELAYER_OFFSET + 18, 3);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                //
                int16_t temp_out = 0;
                MB_READ_INT16(pdata, temp_out);
                MB_READ_FLOAT(pdata, *time);
                *out = temp_out;
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}



uint8_t Modbus_Delayer_UpdateSet(void)
{
    delayerInfo.set.crc = userCRC_Calculate((uint8_t *)&delayerInfo.set, sizeof(delayerInfo.set) - sizeof(delayerInfo.set.crc));
    //    if(delayerInfo.set.crc != delayerInfo.now.crc_now)
    {
        Modbus_Delayer_Set_State(delayerInfo.enable);
        Modbus_Delayer_Set_Start(delayerInfo.set.start);
        Modbus_Delayer_Set_Groups(delayerInfo.set.groups);
        Modbus_Delayer_Set_Cycles(delayerInfo.set.cycles);
        Modbus_Delayer_Set_EndState(delayerInfo.set.endState);
        Modbus_Delayer_Set_StopRule(delayerInfo.set.stopCfg.stopRule);
        Modbus_Delayer_Set_StopValue(delayerInfo.set.stopCfg.stopValue);
        for(uint16_t i = 0; i < DELAYER_MAX_COUNT; ++i) {
            COM_Mutex_Lock();
            Modbus_Delayer_Set_Parameter_No(i);
            Modbus_Delayer_Set_Parameter(delayerInfo.set.points[i].out, delayerInfo.set.points[i].time);
            COM_Mutex_Unlock();
        }
    }
    delayerInfo.now.updateRequest = 0;
    return MB_OK;
}

#endif

