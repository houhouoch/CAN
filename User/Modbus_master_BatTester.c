#include "Modbus_master_IO.h"
#include "Modbus_master_BatTester.h"
#include "usart.h"
#include "Commd.h"

#include "BatTester_Interface.h"

#define BATTERY_TESTER
#if defined(BATTERY_TESTER)
/*
 * @brief       uint8_t Modbus_Read_BatTester(void)
 * @details     读取BatTester当前状态数据
 */
uint8_t Modbus_Read_BatTester(void) {
    uint8_t result = MB_OK;
    uint16_t crc = 0;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 0, 10);
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
                BatTester.now.state = curState;
                BatTester.now.RemainTimes = remainTime;
                BatTester.now.curGroup = curGroups;
                BatTester.now.endGroup = endGroups;
                BatTester.now.RemainCycles = remainCycles;
                BatTester.now.curCycled = curCycled;
                BatTester.now.crc_now = crc;
                if(crc != BatTester.set.crc) {
                    BatTester.now.updateRequest = 1;
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

//设置BatTester状态
uint8_t Modbus_BatTester_Set_State(uint16_t onoff) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, onoff);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 0, pbuf - buf, buf);
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


//设置BatTester起始组
uint8_t Modbus_BatTester_Set_Start(uint16_t start) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, start);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 10, pbuf - buf, buf);
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
//设置BatTester组数
uint8_t Modbus_BatTester_Set_Groups(uint16_t groups) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, groups);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 11, pbuf - buf, buf);
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
//设置BatTester循环次数
uint8_t Modbus_BatTester_Set_Cycles(uint16_t cycles) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, cycles);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 12, pbuf - buf, buf);
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
//设置BatTester全局保护
uint8_t Modbus_BatTester_Set_Global_Protect(uint16_t idx, uint16_t logic, uint16_t stop, float value) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, idx);
        MB_WRITE_INT16(pbuf, logic);
        MB_WRITE_INT16(pbuf, stop);
        MB_WRITE_FLOAT(pbuf, value);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 13, pbuf - buf, buf);
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






//设置BatTester编号
uint8_t Modbus_BatTester_Set_Parameter_No(uint16_t param_no) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, param_no);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 18, pbuf - buf, buf);
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
//设置BatTester组参数
uint8_t Modbus_BatTester_Set_Parameter(float volt, float curr, float time) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, volt);
        MB_WRITE_FLOAT(pbuf, curr);
        MB_WRITE_FLOAT(pbuf, time);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 19, pbuf - buf, buf);
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

//读取BatTester组参数
uint8_t Modbus_BatTester_Get_Parameter(float *volt, float *curr, float *time) {
    if((volt == NULL) || (curr == NULL) || (time == NULL)) {
        return MB_ERROR;
    }
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 19, 6);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                //
                MB_READ_FLOAT(pdata, *volt);
                MB_READ_FLOAT(pdata, *curr);
                MB_READ_FLOAT(pdata, *time);
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

//设置BatTester终止态
uint8_t Modbus_BatTester_Set_Parameter_Protect(uint16_t idx, uint16_t logic, uint16_t stop, float value) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, idx);
        MB_WRITE_INT16(pbuf, logic);
        MB_WRITE_INT16(pbuf, stop);
        MB_WRITE_FLOAT(pbuf, value);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 25, pbuf - buf, buf);
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






uint8_t Modbus_BatTester_UpdateSet(void)
{
    BatTester.set.crc = userCRC_Calculate((uint8_t *)&BatTester.set, sizeof(BatTester.set) - sizeof(BatTester.set.crc));
    //    if(BatTester.set.crc != BatTester.now.crc_now)
    {
        Modbus_BatTester_Set_State(BatTester.enable);
        Modbus_BatTester_Set_Start(BatTester.set.start);
        Modbus_BatTester_Set_Groups(BatTester.set.groups);
        Modbus_BatTester_Set_Cycles(BatTester.set.cycles);
        for(uint16_t j = 0; j < ARRAY_COUNT(BatTester.set.stopCfg); ++j) {
            Modbus_BatTester_Set_Global_Protect(j, BatTester.set.stopCfg[j].logic, BatTester.set.stopCfg[j].stop, BatTester.set.stopCfg[j].value);
        }
        for(uint16_t i = 0; i < BAT_TESTER_MAX_COUNT; ++i) {
            COM_Mutex_Lock();
            Modbus_BatTester_Set_Parameter_No(i);
            Modbus_BatTester_Set_Parameter(BatTester.set.points[i].volt, BatTester.set.points[i].curr, BatTester.set.points[i].time);
            for(uint16_t j = 0; j < ARRAY_COUNT(BatTester.set.points[i].stopCfg); ++j) {
                Modbus_BatTester_Set_Parameter_Protect(j, BatTester.set.points[i].stopCfg[j].logic, BatTester.set.points[i].stopCfg[j].stop, BatTester.set.points[i].stopCfg[j].value);
            }
            COM_Mutex_Unlock();
        }
    }
    BatTester.now.updateRequest = 0;
    return MB_OK;
}



//读取BatTester组参数
uint8_t Modbus_BatTester_Get_ErrParam(void) {
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_BATTESTER_OFFSET + 30, 7);
        {
            uint8_t *pdata = Modbus_Receive();
            if((pdata == NULL) || (pdata[1] != MB_CMD_READ)) {
                result = MB_ERROR;
            } else {
                pdata = &pdata[3];
                //
                int16_t errState = 0;
                MB_READ_INT16(pdata, errState);
                MB_READ_FLOAT(pdata, BatTester.now.errVoltage);
                MB_READ_FLOAT(pdata, BatTester.now.errCurrent);
                MB_READ_FLOAT(pdata, BatTester.now.errPower);
                BatTester.now.errState |= errState;
            }
        }
    }
    COM_Mutex_Unlock();
    if(result != MB_OK) {
        errorCount++;
    }
    return result;
}

#endif
