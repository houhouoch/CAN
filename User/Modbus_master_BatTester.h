#ifndef __MODBUS_MASTER_BATTESTER_H_
#define __MODBUS_MASTER_BATTESTER_H_

#include <stdint.h>
#include <string.h>


#if 1
    /*
    * @brief       uint8_t Modbus_Read_BatTester(void)
    * @details     读取BatTester当前状态数据
    */
    uint8_t Modbus_Read_BatTester(void);
    //设置BatTester状态
    uint8_t Modbus_BatTester_Set_State(uint16_t onoff);
    //设置BatTester起始组
    uint8_t Modbus_BatTester_Set_Start(uint16_t start);
    //设置BatTester组数
    uint8_t Modbus_BatTester_Set_Groups(uint16_t groups);
    //设置BatTester循环次数
    uint8_t Modbus_BatTester_Set_Cycles(uint16_t cycles);
    //设置BatTester终止态
    uint8_t Modbus_BatTester_Set_Global_Protect(uint16_t idx, uint16_t logic, uint16_t stop, float value);

    //设置BatTester编号
    uint8_t Modbus_BatTester_Set_Parameter_No(uint16_t param_no);
    //设置BatTester组参数
    uint8_t Modbus_BatTester_Set_Parameter(float volt, float curr, float time);
    //设置BatTester终止态
    uint8_t Modbus_BatTester_Set_Parameter_Protect(uint16_t idx, uint16_t logic, uint16_t stop, float value);

    //读取BatTester组参数
    uint8_t Modbus_BatTester_Get_Parameter(float *volt, float *curr, float *time);

    uint8_t Modbus_BatTester_UpdateSet(void);

    //读取BatTester组异常参数
    uint8_t Modbus_BatTester_Get_ErrParam(void);
#endif

#endif
