#ifndef __MODBUS_MASTER_POWER_H_
#define __MODBUS_MASTER_POWER_H_

#include <stdint.h>
#include <string.h>


//设置电源输出
uint8_t Modbus_Update_Power_OnOff(uint16_t onoff);
//设置输出电压
uint8_t Modbus_Update_VoltageSet(float volSet);
//设置输出电流
uint8_t Modbus_Update_CurrentSet(float curSet);
//设置输出内阻
uint8_t Modbus_Update_ResInSet(float ResInSet);
//设置OVP电压
uint8_t Modbus_Update_OVPSet(float ovpSet);
//设置OVP电流
uint8_t Modbus_Update_OCPSet(float ocpSet);
//设置OVP开关
uint8_t Modbus_Update_OVP_OnOff(uint16_t onoff);
//设置OCP开关
uint8_t Modbus_Update_OCP_OnOff(uint16_t onoff);
//更新斜率模式
uint8_t Modbus_Update_SlewRate_Set(uint16_t slewRate);

//更新电压斜率
uint8_t Modbus_Update_VSR_UP_Set(float VSR);
uint8_t Modbus_Update_VSR_DOWN_Set(float VSR);
//更新电流斜率
uint8_t Modbus_Update_ISR_UP_Set(float ISR);
uint8_t Modbus_Update_ISR_DOWN_Set(float ISR);

//更新电源模式
uint8_t Modbus_Update_PowerMode_Set(uint16_t mode);
//过压保护延迟
uint8_t Modbus_Update_OVP_delayTime(int16_t value);
//过流保护延迟
uint8_t Modbus_Update_OCP_delayTime(int16_t value);
//过温保护阈值
uint8_t Modbus_Update_OTP_Temperature(int16_t value);

//协同开机延迟
uint8_t Modbus_Update_ON_delayTime(int16_t value);
//协同关机延迟
uint8_t Modbus_Update_OFF_delayTime(int16_t value);

//更新最大风扇
uint8_t Modbus_Update_MaxFan_Set(uint16_t maxFAN);
//远端补偿电压
uint8_t Modbus_Update_SenseVolt_Set(float value);
//泄放负载
uint8_t Modbus_Update_eLoad_Set(int16_t value);
//掉电检测开关
uint8_t Modbus_Update_AC_Down_Set(int16_t value);

//更新所有输出设置
uint8_t Modbus_Update_OutSet(void);
uint8_t Modbus_Update_OutSet_temp(void);

uint8_t Modbus_Read_PowerState(void);
uint8_t Modbus_Clean_PowerState(uint64_t status);

uint8_t Modbus_Read_PowerOut(void);
uint8_t Modbus_Read_PowerOutSet(void);
uint8_t Modbus_Read_Temperature(void);

uint8_t Modbus_Read_Model_Data(void);
uint8_t Modbus_Read_ModelInfo(void);

uint8_t Modbus_Power_OutPut_Sync(void);

uint8_t Modbus_Calib_Voltage(float voltage);
uint8_t Modbus_Calib_Current(float current);
uint8_t Modbus_Calib_Delete_Volt(void);
uint8_t Modbus_Calib_Delete_Curr(void);

uint8_t Modbus_Calib_Voltage_Drv(float voltage);
uint8_t Modbus_Calib_Delete_Volt_Drv(void);
uint8_t Modbus_Calib_Read_Count(void);

uint8_t Modbus_Calib_Mode(int16_t cailbMode);


uint8_t Modbus_Calib_ZeroCurr(int16_t cailbMode);
uint8_t Modbus_Read_Calib_ZeroCurr(void) ;

uint8_t Modbus_Set_ZeroCurr_AutoEn(int16_t cailbMode);
#endif
