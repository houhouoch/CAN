#ifndef __MODBUS_MASTER_LIST_H_
#define __MODBUS_MASTER_LIST_H_

#include <stdint.h>
#include <string.h>


#if 1
    /*
    * @brief       uint8_t Modbus_Read_List(void)
    * @details     读取List当前状态数据
    */
    uint8_t Modbus_Read_List(void);
    //设置LIST状态
    uint8_t Modbus_List_Set_State(uint16_t onoff);
    //设置LIST起始组
    uint8_t Modbus_List_Set_Start(uint16_t start);
    //设置LIST组数
    uint8_t Modbus_List_Set_Groups(uint16_t groups);
    //设置LIST循环次数
    uint8_t Modbus_List_Set_Cycles(uint16_t cycles);
    //设置LIST终止态
    uint8_t Modbus_List_Set_EndState(uint16_t endstate);

    //设置LIST编号
    uint8_t Modbus_List_Set_Parameter_No(uint16_t param_no);
    //设置List组参数
    uint8_t Modbus_List_Set_Parameter(float volt, float curr, float time);
    //读取List组参数
    uint8_t Modbus_List_Get_Parameter(float *volt, float *curr, float *time);

    uint8_t Modbus_List_UpdateSet(void);
#endif

#endif
