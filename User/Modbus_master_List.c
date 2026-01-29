#include "Modbus_master_IO.h"
#include "Modbus_master_List.h"
#include "usart.h"
#include "Commd.h"

#include "List_Delayer_Interface.h"

#define LIST
#if defined(LIST)
/*
 * @brief       uint8_t Modbus_Read_List(void)
 * @details     读取List当前状态数据
 */
uint8_t Modbus_Read_List(void) {
    uint8_t result = MB_OK;
    uint16_t crc = 0;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_LIST_OFFSET + 0, 10);
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
                listoutInfo.now.state = curState;
                listoutInfo.now.RemainTimes = remainTime;
                listoutInfo.now.curGroup = curGroups;
                listoutInfo.now.endGroup = endGroups;
                listoutInfo.now.RemainCycles = remainCycles;
                listoutInfo.now.curCycled = curCycled;
                listoutInfo.now.crc_now = crc;
                if(crc != listoutInfo.set.crc) {
                    listoutInfo.now.updateRequest = 1;
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

//设置LIST状态
uint8_t Modbus_List_Set_State(uint16_t onoff) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, onoff);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_LIST_OFFSET + 0, pbuf - buf, buf);
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


//设置LIST起始组
uint8_t Modbus_List_Set_Start(uint16_t start) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, start);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_LIST_OFFSET + 10, pbuf - buf, buf);
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
//设置LIST组数
uint8_t Modbus_List_Set_Groups(uint16_t groups) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, groups);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_LIST_OFFSET + 11, pbuf - buf, buf);
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
//设置LIST循环次数
uint8_t Modbus_List_Set_Cycles(uint16_t cycles) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, cycles);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_LIST_OFFSET + 12, pbuf - buf, buf);
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
//设置LIST终止态
uint8_t Modbus_List_Set_EndState(uint16_t endstate) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, endstate);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_LIST_OFFSET + 13, pbuf - buf, buf);
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






//设置LIST编号
uint8_t Modbus_List_Set_Parameter_No(uint16_t param_no) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_INT16(pbuf, param_no);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_LIST_OFFSET + 14, pbuf - buf, buf);
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
//设置List组参数
uint8_t Modbus_List_Set_Parameter(float volt, float curr, float time) {
    uint8_t buf[64] = {0};
    uint8_t *pbuf = buf;
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        MB_WRITE_FLOAT(pbuf, volt);
        MB_WRITE_FLOAT(pbuf, curr);
        MB_WRITE_FLOAT(pbuf, time);
        __Creat_Write_Pack(MB_SLAVE_ADDR, MB_REG_LIST_OFFSET + 15, pbuf - buf, buf);
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
//读取List组参数
uint8_t Modbus_List_Get_Parameter(float *volt, float *curr, float *time) {
    if((volt == NULL) || (curr == NULL) || (time == NULL)) {
        return MB_ERROR;
    }
    uint8_t result = MB_OK;
    COM_Mutex_Lock();
    {
        __Creat_Read_Pack(MB_SLAVE_ADDR, MB_REG_LIST_OFFSET + 15, 6);
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



uint8_t Modbus_List_UpdateSet(void)
{
    listoutInfo.set.crc = userCRC_Calculate((uint8_t *)&listoutInfo.set, sizeof(listoutInfo.set) - sizeof(listoutInfo.set.crc));
    //    if(listoutInfo.set.crc != listoutInfo.now.crc_now)
    {
        Modbus_List_Set_State(listoutInfo.enable);
        Modbus_List_Set_Start(listoutInfo.set.start);
        Modbus_List_Set_Groups(listoutInfo.set.groups);
        Modbus_List_Set_Cycles(listoutInfo.set.cycles);
        Modbus_List_Set_EndState(listoutInfo.set.endState);
        for(uint16_t i = 0; i < LIST_MAX_COUNT; ++i) {
            COM_Mutex_Lock();
            Modbus_List_Set_Parameter_No(i);
            Modbus_List_Set_Parameter(listoutInfo.set.points[i].volt, listoutInfo.set.points[i].curr, listoutInfo.set.points[i].time);
            COM_Mutex_Unlock();
        }
    }
    listoutInfo.now.updateRequest = 0;
    return MB_OK;
}

#endif
