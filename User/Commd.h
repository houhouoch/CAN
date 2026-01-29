#ifndef __COMMD_H_
#define __COMMD_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "adc.h"
#include "tim.h"
#include "Setting.h"
#include "Interface.h"
#include "option_Interface.h"
#include "elog.h"

#include "usb_host.h"

#include "Task_Manager.h"


////打开串口调试信息输出
#define UART_DEBUG
////开机默认进入校准模式
//#define DEBUG_CALIBMODE
//保存Debug数据到Flash
//#define FLASH_DEBUG

////CAN通讯丢包检测
//#define __DEBUG_CAN_PACKET_LOST

#define __LIST_EN
#define __DELAYER_EN
#define __MENU_EN
#define __PRESET_EN

#if defined(FLASH_DEBUG) | defined(UART_DEBUG)
#else
//打开看门狗
#define __DWG_EN
#endif

//#define __BAT_TESTER_EN


#define __OVP_LIMIT
#define __OCP_LIMIT


//#define ZEROCURRENT_EN      //自动电流清零

////打开波形图
//#define __POWER_CHART_EN
//#define __POWER_CHART_POINT_COUNT (100)

#if defined(VER_UNIT)
    #define POWER_COMPANY   "Unitrend"
#elif defined(VER_JINGHUI)
    #define POWER_COMPANY   "JINGHUI"
#endif

#if defined(FLASH_DEBUG) | defined(UART_DEBUG)
    #define VERSION_ARG_NAME  "_Debug"
#else
    #define VERSION_ARG_NAME  ""
#endif

#ifndef __POWER_800V
    #define POWER_LCD_VERSION   "1.04.0108"VERSION_ARG_NAME
#else
    #define POWER_LCD_VERSION   "1.04.0108_800V"VERSION_ARG_NAME
#endif


#define ARRAY_COUNT(X) (sizeof(X)/sizeof(X[0]))
#ifndef MIN
    #define MIN(A,B)       (A<B)?A:B
#endif

#ifndef MAX
    #define MAX(A,B)       (A>B)?A:B
#endif

#include "Beep_Device.h"
#include "Interface.h"
#include "Option_Interface.h"
#ifndef RANGE
#define RANGE(A,MIN,MAX)    \
    do{                         \
        if(A > MAX) {A = MAX;if(IF_Power_Get_Power_Mode() != POWER_MODE_EXT_V)BEEP_WARRING();}  \
        if(A < MIN) {A = MIN;if(IF_Power_Get_Power_Mode() != POWER_MODE_EXT_V)BEEP_WARRING();}  \
    }while(0)
#endif

//调试
#define USER_DEBUG   user_printf






#include "gui_guider.h"

extern const int scpiPort_step[4];

#define VSR_SET_FORM    "%0.2f"
#define ISR_SET_FORM    "%0.2f"
extern const float VSR_step[5];
extern const float ASR_step[5];

extern const float Sense_Volt_step[2];
#define SENSE_VOLT_FORM     "%2.1f"
extern const float Temperature_step[2];
extern const int delayTime_step[3];

const float *__Get_Voltage_Set_Step(int *step_len);
const char *__Get_Voltage_Set_Fmt(void);

const float *__Get_Current_Set_Step(int *step_len);
const char *__Get_Current_Set_Fmt(void);

const float *__Get_Power_Set_Step(int *step_len);
const char *__Get_Power_Set_Fmt(void);


extern const float Rin_step[4];
#define RES_IN_SET_FORM "%6.3f"
#define POWER_SET_FORM  "%6.3f"
extern const float Calib_V_step[6];
extern const float Calib_A_step[6];
extern const int Calib_Passwd_step[5];
#define VOL_CALIB_FORM  "%08.4f"
#define CUR_CALIB_FORM  "%08.4f"
#define PASSWD_CALIB_FORM  "%05d"

extern const int point_step[3];
extern const int cycle_step[4];
extern const float time_step_HH[5];
extern const float time_step_H[5];
extern const float time_step_M[5];
extern const float time_step_L[5];

//时间
const float *__Get_Time_Set_Step(float value, int *step_len);
const char *__Get_Time_Set_Fmt(float value);

#define POINT_DISP_FORM     "%3d"
#define CYCLES_DISP_FORM    "%5d"
#define TIME_DISP_FORM      "%6.3f"
#define TIME_DISP_FORM_HH   "%6.0f"
#define TIME_DISP_FORM_H    "%6.1f"
#define TIME_DISP_FORM_M    "%6.2f"
#define TIME_DISP_FORM_L    "%6.3f"

extern const float Value_step[5];

extern const uint16_t UpdateRate_step[3];
#define UPDATE_RATE_DISP_FORM    "%5d"

extern const uint32_t passwd_step[6];

#define VOL_MIN             (PowerInfo.Model.minVolt)
#define VOL_MAX             (PowerInfo.Model.maxVolt)
#define CUR_MIN             (PowerInfo.Model.minCurr)
#define CUR_MAX             (PowerInfo.Model.maxCurr)
#define POWER_MIN           (PowerInfo.Model.minPower)
#define POWER_MAX           (PowerInfo.Model.maxPower)

#define VSR_MIN             (PowerInfo.Model.minVolt)
#define VSR_MAX             (PowerInfo.Model.maxVolt*10)
#define ISR_MIN             (PowerInfo.Model.minCurr)
#define ISR_MAX             (PowerInfo.Model.maxCurr*10)

#define CYCLE_MIN           (0)
#define CYCLE_MAX           (99999)

#define LIST_VOL_SET_FORM    "%6.3f"
#define LIST_CUR_SET_FORM    "%6.3f"

#define TIME_MIN           (0.1f)
#define TIME_MAX           (99999.9f)


void __Voltage_Display_Fmt(char *pbuf, float volt);
void __Current_Display_Fmt(char *pbuf, float curr);
void __Power_Display_Fmt(char *pbuf, float power);


enum {
    PAGE_NULL = 0,
    PAGE_HOME,
    PAGE_HOME2,
    PAGE_LIST,
    PAGE_LIST_TEMP,
    PAGE_LIST_MANAGER,

    PAGE_DELAY,
    PAGE_DELAY_TEMP,
    PAGE_DELAY_MANAGER,

    PAGE_PRESET,
    PAGE_MENU,
    PAGE_MONITOR,
    PAGE_OUTOPTION,
    PAGE_SETTING,
    PAGE_GROUP,
    PAGE_BATTESTER,
    //
    PAGE_MIN = PAGE_HOME,
    PAGE_MAX = PAGE_BATTESTER,
};





/*
 * @brief       void label_setSelNum(lv_obj_t* label,uint16_t numpos,const char* text)
 * @details     Label选择特定数字(用于设置参数)
 */
void label_setSelNum(lv_obj_t *label, uint16_t numpos, const char *text);
/*
 * @brief       void label_setSelStr(lv_obj_t* label,const char* str,const char* text)
 * @details     Label选择特定字符(用于设置参数)
 */
void label_setSelStr(lv_obj_t *label, const char *str, const char *text);
/*
 * @brief       void label_setSelIdx(lv_obj_t* label,const char* str,int idx)
 * @details     Label选择特定索引
 */
void label_setSelIdx(lv_obj_t *label, const char *str, int idx);

void UI_paramBtn_LoadStyle(lv_obj_t *button);
void UI_editBtn_LoadStyle(lv_obj_t *button);


void GUI_KeySend(lv_obj_t *obj, lv_key_t key);





#ifdef MULT_CHANNEL

    /*
    * @brief       uint8_t __Get_Model_Hw_ID(float ADVolt)
    * @details     输入ADC电压,计算硬件ID
    */
    uint8_t __Get_Model_Hw_ID(float ADVolt);

    uint8_t __Get_Model_Hw_Channels(float ADVolt);
#endif

/*
 * @brief       uint16_t userGetIntTemp(uint16_t adc_data)
 * @details     输入ADC转换值,计算片内温度
 */
float __Get_Chip_Temp(uint16_t adcValue);

/*
 * @brief       uint16_t userCRC_Calculate(uint8_t *pucFrame, uint16_t usLen)
 * @details     计算CRC
 */
uint16_t userCRC_Calculate(uint8_t *pucFrame, uint16_t usLen);

#endif
