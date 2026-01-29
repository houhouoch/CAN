#ifndef _SETTING_H_
#define _SETTING_H_

#include "Commd.h"


enum {
    MODEL_40_40 = 0,
    MODEL_80_20,
    MODEL_160_10,
    MODEL_250_6_5,
    MODEL_800_2,
    MODEL_UNKNOW,
};

enum {
    MODEL_COUNT_1 = 0,
    MODEL_COUNT_2,
    MODEL_COUNT_3,
    MODEL_COUNT_4,
    MODEL_COUNT_5,
    MODEL_COUNT_UNKNOW,
};


enum {
    MODEL_ID_0 = 0,
    MODEL_ID_1 = 1,
    MODEL_ID_2,
    MODEL_ID_3,
    MODEL_ID_4,
    MODEL_ID_5,
    MODEL_ID_UNKNOW,
};

enum {
    MODEL_CHANNELS_1 = 0,
    MODEL_CHANNELS_2,
    MODEL_CHANNELS_3,
    MODEL_CHANNELS_4,
    MODEL_CHANNELS_5,
    MODEL_CHANNELS_UNKNOW,
};


enum {
    FAN_MODEL_OK = 0,
    FAN_MODEL_1,
    FAN_MODEL_2,
    FAN_MODEL_3,
    FAN_MODEL_4,
    FAN_MODEL_5,
    FAN_MODEL_ERR,
};



typedef struct {
    //
    float minVolt;
    float maxVolt;
    float minCurr;
    float maxCurr;
    float minPower;
    float maxPower;
    //默认远端补偿电压
    float default_SenseVolt;
    //最大输出内阻
    float maxResIn;
    //最大温度
    int16_t defaultTemp;
    //模拟转换系数
    float volt_Gain;
    float curr_Gain;
    //机型名称
    char modelName[20];
} Model_Info_Def;


enum {
    CALIB_RES_IDLE = 0,
    CALIB_RES_OK = 1,
    CALIB_RES_CLOSE = 2,
    CALIB_RES_NULLVALUE = 3,
};

enum {
    /*
     * CV优先:固定CC电流设置,逐步提升/降低电压设置
     * CC优先:固定CV电压设置,逐步提升/降低电流设置
     */
    SRMODE_CV_HIGHSPEED = 0,
    SRMODE_CC_HIGHSPEED,
    SRMODE_CV_SLEWRATE,
    SRMODE_CC_SLEWRATE,

    SRMODE_MIN = SRMODE_CV_HIGHSPEED,
    SRMODE_MAX = SRMODE_CC_SLEWRATE,
};

enum LANGUAGE_ENUM {
    LANGUAGE_CH = 0,        //简体中文
    LANGUAGE_EN,            //英语

    LANGUAGE_MIN = LANGUAGE_CH,
    LANGUAGE_MAX = LANGUAGE_EN,
};

enum ELOAD_ENUM {
    ELOAD_OFF = 0,       //关闭
    ELOAD_ON,            //常开
    ELOAD_AUTO,          //自动

    ELOAD_MIN = ELOAD_OFF,
    ELOAD_MAX = ELOAD_ON,
};

#ifdef MULT_CHANNEL
    #define __SYNERGY_EN
#endif

enum {
    POWER_MODE_NORMAL = 0,
    POWER_MODE_CAN_PARALLEL_MASTER,
    POWER_MODE_CAN_PARALLEL_SLAVE,
    POWER_MODE_CAN_SERIES_MASTER,
    POWER_MODE_CAN_SERIES_SLAVE,
    POWER_MODE_EXT_V,
    POWER_MODE_CAN_SYNERGY_MASTER,
    POWER_MODE_CAN_SYNERGY_SLAVE,

    #ifdef __SYNERGY_EN
    POWER_MODE_MIN = POWER_MODE_NORMAL,
    POWER_MODE_MAX = POWER_MODE_CAN_SYNERGY_SLAVE,
    #else
    POWER_MODE_MIN = POWER_MODE_NORMAL,
    POWER_MODE_MAX = POWER_MODE_EXT_V,
    #endif
};

#pragma pack (1)
#define GROUPS_COUNT (32)
#define PRESET_COUNT (3)
typedef struct {
    struct {
        float voltSet;
        float currSet;
        float ovpSet;
        float ocpSet;
        uint8_t ovpOnoff;
        uint8_t ocpOnoff;
    } Preset[GROUPS_COUNT][PRESET_COUNT];
    uint8_t reserve[2];     //保留
    uint16_t crc;
} Group_Def;

typedef struct {

    //输出设置
    double volt_set;
    double curr_set;
    double ResIn_set;
    uint8_t onoff;
    //保护设置
    double ovp_set;
    double ocp_set;
    uint8_t ovp_onoff;
    uint8_t ocp_onoff;
    //电源斜率
    uint8_t SlewRate_Mode;
    float VSR_Up;
    float VSR_Down;
    float ISR_Up;
    float ISR_Down;

    uint8_t powerMode;      //电源模式
    uint8_t ac_down;        //掉电检测
    uint8_t power_on;       //上电输出
    uint8_t eLoad;          //泄放负载

    #if 1

    int16_t updateRate;     //回读速度

    int16_t ovp_delayTime;
    int16_t ocp_delayTime;
    int16_t otp_Temperature;
    int16_t on_delayTime;    //延时启动
    int16_t off_delayTime;   //延时关闭
    #endif



    uint8_t lock;
    uint8_t userGroup;          //用户组
    uint8_t slave_ID;           //从机通讯ID(用于外部并机)
    uint8_t brightness;         //亮度
    uint8_t language;           //系统语言
    uint8_t beeper_en;          //蜂鸣器
    uint8_t remoteLock;         //自动上锁
    uint8_t dhcp_en;            //DHCP
    uint8_t ip[4 + 1];          //IP
    uint8_t gatewate[4 + 1];    //网关地址
    uint8_t netmask[4 + 1];     //子网掩码
    uint8_t mac[6 + 1];         //MAC地址
    uint16_t scpiPort;          //Socket SCPI端口号
    //测试
    float senseVolt;            //远端补偿电压
    int8_t maxFan;              //最大风扇占空比(Debug)
    int8_t zeroCurrent_autoEn;  //开机自动清零
    //    int8_t debugEn;             //Debug日志
    //
    uint32_t localCookieID;

    char multCH_ModelName[64];
    uint8_t canGroup;
    uint16_t reserve;     //保留
    uint16_t crc;
} OutSet_Def;

typedef struct {
    char mdnsName[64];
    uint32_t userPasswd_uint32;
    uint16_t crc;
} webSet_Def;

#if 1
enum {
    WORK_MODE_NORMAL = 0,
    WORK_MODE_ENGINEERING = 1,
} ;
#endif

typedef struct {
    uint8_t chip_UID[12 + 1];   //设备唯一ID
    char company[32];
    char serialNumber[32];
    char lcd_version[32];
    char power_version[32];
    char model_name[64];
    uint16_t crc;
} About_Def;

typedef union {
    uint64_t byte;
    struct {
        uint8_t dislinked: 1;      //连接状态
        uint8_t onoff: 1;       //1:输出打开,0:输出关闭
        uint8_t acFault: 1;     //1:AC电源断开
        uint8_t cc: 1;          //1:CC恒流模式

        uint8_t cv: 1;          //1:CV恒压模式
        uint8_t ovp: 1;         //1:过压保护
        uint8_t ocp: 1;         //1:过流保护
        uint8_t otp: 1;         //1:过温保护

        uint8_t opp: 1;         //1:过功保护
        uint8_t font_ocp: 1;    //前面板输出过流(10A)
        uint8_t sense: 1;       //远端补偿
        uint8_t reverseCurr: 1; //外部电流灌入

        uint8_t fan_error: 3;   //1:风扇故障
        uint8_t pfc_error: 3;   //1:PFC故障(不使用)
        uint8_t pfc_hot : 3;    //PFC过热
        uint8_t mos_hot : 3;    //原边MOS过热

        uint8_t noCalib_Volt: 1;    //无校准电压
        uint8_t noCalib_Curr: 1;    //无校准电流
        uint8_t noCalib_Drv : 1;    //无校准DRV电压
        uint8_t eload_onoff : 1;    //泄放电阻开关

        uint8_t model_2_ready: 1;
        uint8_t model_3_ready: 1;
        uint8_t model_4_ready: 1;
        uint8_t model_5_ready: 1;

        uint8_t model_2_error: 1;
        uint8_t model_3_error: 1;
        uint8_t model_4_error: 1;
        uint8_t model_5_error: 1;

        uint8_t test_bit: 1;
        uint8_t outOverVolt_error: 1;     //输出端电压异常
        uint8_t adcDevice_error: 1;         //ADC芯片异常
        uint8_t dacDevice_error: 1;         //DAC芯片异常
    } bits;
} State_Def;


//CAN通讯最大从机数
//串联模式下,任何机型最多2台
//并联模式下,除5并机2kw的机型为2台,其他机型为5台
#define POWER_CAN_ID_MAX  (5-1)
#define POWER_CAN_SYNERGY_ID_MAX  (128-1)
#define POWER_CAN_GROUP_ID_MAX  (16-1)
#define POWER_CAN_MULT_MAX  (5-1)
typedef struct {
    uint8_t onoff;
    float volt_Set;
    float curr_Set;
    //
    uint16_t voltADC;
    uint16_t currADC;
    uint16_t voltDAC;
    uint16_t currDAC;
} Ext_C_Def;

typedef struct {
    int16_t adcDat[3];
    float adcVolt[3];
    //
    uint8_t flag_temp;
    uint8_t flag_count;
    //
    float chDrvVoltage;
    float temperture;
    float chCurrent;
} Model_Data_Def;


typedef struct {
    double volt_read;
    double curr_read;
    double power_read;

    State_Def State;
    State_Def Event;

    uint8_t powerType;  //电源类型
    uint8_t powerMode;

    uint8_t packetNo;
    uint8_t data_ready;
    int16_t timeout;
    uint32_t errCount;
} SlaveInfo_Def;

typedef struct  {
    uint32_t errorFlag;
    char errorTaskName[32];
    uint16_t crc;
} Debug_Def;

typedef struct {
    Model_Data_Def modelData[5];
    Group_Def group;
    webSet_Def webSet;
    OutSet_Def outSet;
    About_Def about;
    Debug_Def debugInfo;
    //    Option_Def option;
    Ext_C_Def extC;
    uint8_t workMode;
    struct {
        uint8_t powerID;
        uint8_t powerChannels;

        uint8_t canUpdata_event;
        uint8_t syncRequest;
        uint8_t setchannged;    //用于保存
        uint8_t groupChannged;  //组参数保存
        uint8_t webchannged;    //网页密码保存
        uint8_t netApply;       //网络设置保存

        uint64_t runTime_ms;
        //输出AD采集
        double volt_adc;
        double curr_sum_adc;
        double curr_front_adc;  //前面板输出电流
        //实际输出
        double volt_read;
        double volt_drv_read;
        double curr_read;
        double power_read;
        #if 0
        //滤波后
        double volt_read;
        double curr_read;
        double power_read;
        #endif

        //        //
        //        SlaveInfo_Def SlaveInfo[((POWER_CAN_SYNERGY_ID_MAX > POWER_CAN_ID_MAX) ? POWER_CAN_SYNERGY_ID_MAX : POWER_CAN_ID_MAX) + 1];
        //多通道机器信息
        SlaveInfo_Def MultSlaveInfo[POWER_CAN_MULT_MAX + 1];
        //串联机器信息
        SlaveInfo_Def SeriesSlaveInfo[POWER_CAN_ID_MAX + 1];
        //并联机器信息
        SlaveInfo_Def ParallelSlaveInfo[POWER_CAN_ID_MAX + 1];
        //协同机器信息
        SlaveInfo_Def SynergySlaveInfo[POWER_CAN_SYNERGY_ID_MAX + 1];

        double volt_read_total;
        double curr_read_total;
        double power_read_total;
        //校准状态
        uint8_t voltCalib_state;
        uint8_t currCalib_state;
        //
        uint8_t powerCount;
        uint8_t powerType;
        //
        uint8_t voltCalib_Count;
        uint8_t voltDrvCalib_Count;
        uint8_t currCalib_Count;
        //
        float voltage_step;
        float current_step;
        //
        uint8_t dhcp_en;            //DHCP
        uint8_t ip[4 + 1];          //IP
        uint8_t gatewate[4 + 1];    //网关地址
        uint8_t netmask[4 + 1];     //子网掩码

        uint32_t cookieID;
    } Read;

    struct {
        int16_t temp_ext_1;     //模组1温度
        int16_t temp_ext_2;     //模组2温度
        int16_t temp_ext_3;     //模组3温度
        int16_t temp_ext_4;     //模组4温度
        int16_t temp_ext_5;     //模组5温度
        int16_t temp_pcb;       //PCB温度
    } Temperature;

    union {
        uint64_t byte;
        struct {
            uint8_t Power_Link: 1;
            uint8_t Sample_Ready: 1;
            uint8_t CAN: 1;
            uint8_t USB_Host: 1;
            uint8_t USB_Device: 1;
            uint8_t LAN_Linked: 1;
            uint8_t LOCK: 1;
            uint8_t Remote: 1;
            uint8_t Web: 1;
            uint8_t Socket: 1;
            uint8_t USB: 1;
            uint8_t LAN_10M: 1;
            uint8_t LAN_100M: 1;
            uint8_t LAN_Tx: 1;
            uint8_t LAN_Rx: 1;
            uint8_t SDRAM: 1;
            uint8_t LAN_DP83848: 1;
            uint8_t LAN_LAN8720: 1;
            uint8_t zeroCurr: 1;

            uint8_t eeprom_setting_lost: 1;
            uint8_t network_init_failed: 1;
            uint8_t flash_init_failed: 1;

        } bits;
    } StateBar;

    State_Def State;
    State_Def Event;

    Model_Info_Def Model;
} PowerInfo_t;

#pragma pack ()

extern PowerInfo_t PowerInfo;

void __Load_Group(void);
void __Save_Group(void);

void __Load_DebugInfo(void);
void __Save_DebugInfo(void);

void __Setting_Load_Default(void);
void __Setting_Load(void);


void __Load_Setting(void);
void __Save_Setting(void);

void __Load_WebSet(void);
void __Save_WebSet(void);

void __Load_About(void);
void __Save_About(void);

#if 0
    void __Load_List(void);
    void __Save_List(void);

    void __Load_Delayer(void);
    void __Save_Delayer(void);
#endif

#endif

