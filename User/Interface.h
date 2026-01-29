#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "Commd.h"
#include "Setting.h"


uint8_t IF_Power_Get_Hw_Channels(void);
uint8_t IF_Power_Get_Hw_ID(void);


void IF_Display_Go_to_Page(uint8_t pageName);

uint64_t IF_Power_Get_RunTime(void);
void IF_Power_Set_RunTime(uint64_t runTime_ms);



/*
 * @brief
 * @details     斜率模式
 */
uint8_t IF_Power_Get_SlewRate_Mode(void);
void IF_Power_Set_SlewRate_Mode(uint8_t mode);


/*
 * @brief
 * @details     电源内阻
 */
void IF_Power_Set_ResIn(float value);
float IF_Power_Get_ResIn(void);

/*
 * @brief
 * @details     电压斜率设置
 */
void IF_Power_Set_VSR_Up(float value);
float IF_Power_Get_VSR_Up(void);
void IF_Power_Set_VSR_Down(float value);
float IF_Power_Get_VSR_Down(void);

/*
 * @brief
 * @details     电流斜率设置
 */
void IF_Power_Set_ISR_Up(float value);
float IF_Power_Get_ISR_Up(void);
void IF_Power_Set_ISR_Down(float value);
float IF_Power_Get_ISR_Down(void);


/*
 * @brief
 * @details     OVP过压保护设置
 */
void IF_Power_Set_OVP(float value);
float IF_Power_Get_OVP(void);
void IF_Power_Set_OVP_onoff(uint8_t onoff);
uint8_t IF_Power_Get_OVP_onoff(void);

/*
 * @brief
 * @details     OCP过流保护设置
 */
void IF_Power_Set_OCP(float value);
float IF_Power_Get_OCP(void);
void IF_Power_Set_OCP_onoff(uint8_t onoff);
uint8_t IF_Power_Get_OCP_onoff(void);

/*
 * @brief
 * @details     输出电压设置
 */
float IF_Power_Get_OutVolt(void);
void IF_Power_Set_OutVolt(float value);

/*
 * @brief
 * @details     输出电流设置
 */
float IF_Power_Get_OutCurr(void);
void IF_Power_Set_OutCurr(float value);

/*
 * @brief
 * @details     电源输出开关
 */
uint8_t IF_Power_Get_On_Off_Now(void);
uint8_t IF_Power_Get_On_Off(void);
void IF_Power_Set_On_Off(uint8_t onoff);

/*
 * @brief
 * @details     输出回读状态
 */
uint8_t IF_Power_Get_OTP_TRIPed(void);
uint8_t IF_Power_Get_OVP_TRIPed(void);
void IF_Power_OVP_Clean(void);
uint8_t IF_Power_Get_OCP_TRIPed(void);
void IF_Power_OCP_Clean(void);
double IF_Power_Read_Volt(void);
double IF_Power_Read_Volt_Drv(void);
double IF_Power_Read_Curr(void);
double IF_Power_Read_Power(void);

int32_t IF_Power_Read_State(void);
int32_t IF_Power_Read_Event(void);

uint8_t IF_Power_Read_CCCV(void);



/*
 * @brief
 * @details     校准用
 */
void IF_Power_Calib_ZeroCurrent(uint8_t zeroCurrent);
void IF_Power_Calib_Voltage(double ture_vol);
void IF_Power_Calib_Voltage_Drv(double ture_vol);
void IF_Power_Calib_Current(double true_amp);
void IF_Power_Calib_Delete(const char *str);


void IF_System_Set_Lock(uint8_t lock);
uint8_t IF_System_Get_Lock(void);

uint16_t IF_Power_Get_Max_Temperture(void);



void IF_State_LAN_Connect(uint8_t connect);
void IF_State_USB_Device_Connect(uint8_t connect);
void IF_State_USB_Host_Connect(uint8_t connect);
#endif
