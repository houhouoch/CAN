#ifndef __MODBUS_MASTER_DELAYER_H_
#define __MODBUS_MASTER_DELAYER_H_

#include <stdint.h>
#include <string.h>


#if 1
    /*
    * @brief       uint8_t Modbus_Read_Delayer(void)
    * @details     读取Delayer当前状态数据
    */
    uint8_t Modbus_Read_Delayer(void);
    //设置Delayer状态
    uint8_t Modbus_Delayer_Set_State(uint16_t onoff);
    //设置Delayer起始组
    uint8_t Modbus_Delayer_Set_Start(uint16_t start);
    //设置Delayer组数
    uint8_t Modbus_Delayer_Set_Groups(uint16_t groups);
    //设置Delayer循环次数
    uint8_t Modbus_Delayer_Set_Cycles(uint16_t cycles);
    //设置Delayer终止态
    uint8_t Modbus_Delayer_Set_EndState(uint16_t endstate);
    //设置Delayer停止条件
    uint8_t Modbus_Delayer_Set_StopRule(uint16_t stopRule);
    //设置Delayer停止值
    uint8_t Modbus_Delayer_Set_StopValue(float stopValue);


    //设置Delayer编号
    uint8_t Modbus_Delayer_Set_Parameter_No(uint16_t param_no);
    //设置Delayer组参数
    uint8_t Modbus_Delayer_Set_Parameter(uint8_t out, float time);
    //读取Delayer组参数
    uint8_t Modbus_Delayer_Get_Parameter(uint8_t *out, float *time);

    uint8_t Modbus_Delayer_UpdateSet(void);
#endif

#endif
