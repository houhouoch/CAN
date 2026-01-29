#ifndef _OPTION_INTERFACE_H_
#define _OPTION_INTERFACE_H_

#include "Commd.h"
#include "Setting.h"

void IF_Debug_Set_ErrorFlag(uint32_t errorFlag);
uint32_t IF_Debug_Get_ErrorFlag(void);


void IF_Option_Set_localCookieID(uint32_t localCookieID);
uint32_t IF_Option_Get_localCookieID(void);

int32_t IF_Option_Get_SCPI_Port(void);
void IF_Option_Set_SCPI_Port(int32_t scpiPort);

uint32_t IF_System_Get_Mem_Max(void);
uint32_t IF_System_Get_Mem_Used(void);
uint32_t IF_System_Get_Mem_Used_Max(void);
int16_t IF_Power_Get_UpdateRate(void);
void IF_Power_Set_UpdateRate(int16_t updateRate);


void IF_Power_Set_OVP_delayTime(int16_t value);
int16_t IF_Power_Get_OVP_delayTime(void);
void IF_Power_Set_OCP_delayTime(int16_t value);
int16_t IF_Power_Get_OCP_delayTime(void);
void IF_Power_Set_OTP_Temperature(int16_t value);
int16_t IF_Power_Get_OTP_Temperature(void);

void IF_Power_Set_ON_delayTime(int16_t value);
int16_t IF_Power_Get_ON_delayTime(void);
void IF_Power_Set_OFF_delayTime(int16_t value);
int16_t IF_Power_Get_OFF_delayTime(void);


void IF_System_Set_Remote(uint8_t remote);
uint8_t IF_System_Get_Remote(void);



int8_t IF_Power_Get_MaxFan(void);
void IF_Power_Set_MaxFan(int8_t maxFan);

void IF_Power_Set_Sense_Volt(float value);
float IF_Power_Get_Sense_Volt(void);

/*
 * @brief
 * @details     电源模式
 */
uint8_t IF_Power_Get_Power_Mode(void);
void IF_Power_Set_Power_Mode(uint8_t mode);


/*
 * @brief
 * @details     CAN通讯组
 */
int8_t IF_Option_Get_CAN_Groups_ID(void);
void IF_Option_Set_CAN_Groups_ID(int8_t id);
/*
 * @brief
 * @details     CAN通讯ID
 */
int8_t IF_Option_Get_Slave_ID(void);
void IF_Option_Set_Slave_ID(int8_t id);






//上电输出
uint8_t IF_Power_Get_Power_out(void);
void IF_Power_Set_Power_out(uint8_t enable);
//假负载
uint8_t IF_Power_Get_eLoad(void);
void IF_Power_Set_eLoad(uint8_t eLoad);
//AC掉电
uint8_t IF_Power_Get_Power_Down(void);
void IF_Power_Set_Power_Down(uint8_t enable);
//用户组
int8_t IF_Option_Get_Groups(void);
void IF_Option_Set_Groups(int8_t groups);
//背光设置
uint8_t IF_Option_Get_Brightness(void);
void IF_Option_Set_Brightness(uint8_t brightness);
//蜂鸣器设置
uint8_t IF_Option_Get_Beeper(void);
void IF_Option_Set_Beeper(uint8_t beeper_en);
//远程控制锁
uint8_t IF_Option_Get_RemoteLock(void);
void IF_Option_Set_RemoteLock(uint8_t en);
//语言设置
int8_t IF_Option_Get_language(void);
void IF_Option_Set_language(int8_t language);

//SN号
void IF_Option_Write_SN(const char *str);
const char *IF_Option_Read_SN(void);

//网络设置:DHCP
uint8_t IF_Option_NetWork_Get_DHCP(void);
uint8_t IF_Option_NetWork_Get_DHCP_Now(void);
void    IF_Option_NetWork_Set_DHCP(uint8_t dhcp_en);
//网络设置:IP
uint8_t IF_Option_NetWork_Get_IP(uint8_t idx);
uint8_t IF_Option_NetWork_Get_IP_Now(uint8_t idx);
void    IF_Option_NetWork_Set_IP(uint8_t idx, uint8_t value);

uint8_t IF_Option_NetWork_Get_Gateway(uint8_t idx);
uint8_t IF_Option_NetWork_Get_Gateway_Now(uint8_t idx);
void    IF_Option_NetWork_Set_Gateway(uint8_t idx, uint8_t value);

uint8_t IF_Option_NetWork_Get_NetMask(uint8_t idx);
uint8_t IF_Option_NetWork_Get_NetMask_Now(uint8_t idx);
void    IF_Option_NetWork_Set_NetMask(uint8_t idx, uint8_t value);
//网络设置:应用设置
void    IF_Option_NetWork_Set_Apply(void);

//网页密码
int32_t IF_Option_NetWork_Get_WebPasswd(void);
void IF_Option_NetWork_Set_WebPasswd(int32_t passwd);

void IF_Option_Input_Passwd(uint32_t passwd);

void IF_Option_Restore(void);

uint8_t IF_Option_Get_ZeroCurrent_AutoEn(void);
void IF_Option_Set_ZeroCurrent_AutoEn(uint8_t en);

#if 0
    uint8_t IF_Option_Get_Debug_En(void);
    void IF_Option_Set_Debug_En(uint8_t en);
#endif

//预设值
uint8_t IF_Preset_Load(uint8_t userGroup, uint8_t idx);
uint8_t IF_Preset_Save(uint8_t userGroup, uint8_t idx);
//预设值电压
uint8_t IF_Preset_Set_Voltage(uint8_t userGroup, uint8_t idx, float voltage);
float IF_Preset_Get_Voltage(uint8_t userGroup, uint8_t idx);
//预设值电流
uint8_t IF_Preset_Set_Current(uint8_t userGroup, uint8_t idx, float current);
float IF_Preset_Get_Current(uint8_t userGroup, uint8_t idx);

//预设值OVP
uint8_t IF_Preset_Set_OVP(uint8_t userGroup, uint8_t idx, float ovp);
float IF_Preset_Get_OVP(uint8_t userGroup, uint8_t idx);
uint8_t IF_Preset_Set_OVP_onoff(uint8_t userGroup, uint8_t idx, uint8_t onoff);
uint8_t IF_Preset_Get_OVP_onoff(uint8_t userGroup, uint8_t idx);
//预设值OCP
uint8_t IF_Preset_Set_OCP(uint8_t userGroup, uint8_t idx, float ocp);
uint8_t IF_Preset_Get_OCP(uint8_t userGroup, uint8_t idx);
uint8_t IF_Preset_Set_OCP_onoff(uint8_t userGroup, uint8_t idx, uint8_t onoff);
uint8_t IF_Preset_Get_OCP_onoff(uint8_t userGroup, uint8_t idx);
#endif
