#include "Setting.h"

//冗余量
#define MODEL_REDUND (1.100f)


PowerInfo_t PowerInfo = {
    .Read = {
        .powerCount = MODEL_COUNT_UNKNOW,
        .powerType = MODEL_UNKNOW,
        .powerID = MODEL_ID_UNKNOW,
        .powerChannels = MODEL_CHANNELS_UNKNOW,
    }
};

void __Setting_Load_Default(void)
{
    //加载默认输出设置
    PowerInfo.outSet.onoff = 0;
    PowerInfo.outSet.volt_set = 5;
    PowerInfo.outSet.curr_set = 1;
    PowerInfo.outSet.ResIn_set = 0;
    PowerInfo.outSet.ovp_set = PowerInfo.Model.maxVolt;
    PowerInfo.outSet.ocp_set = PowerInfo.Model.maxCurr;
    PowerInfo.outSet.ovp_onoff = 1;
    PowerInfo.outSet.ocp_onoff = 1;
    PowerInfo.outSet.SlewRate_Mode = SRMODE_CV_HIGHSPEED;
    PowerInfo.outSet.VSR_Up = PowerInfo.Model.maxVolt / 0.5f;
    PowerInfo.outSet.VSR_Down = PowerInfo.Model.maxVolt / 0.5f;
    PowerInfo.outSet.ISR_Up = PowerInfo.Model.maxCurr / 0.5f;
    PowerInfo.outSet.ISR_Down = PowerInfo.Model.maxCurr / 0.5f;
    //
    PowerInfo.outSet.powerMode = POWER_MODE_NORMAL;
    PowerInfo.outSet.power_on = 0;
    if((LANGUAGE_MIN <= PowerInfo.outSet.language)
            && (PowerInfo.outSet.language <= LANGUAGE_MAX)) {
    } else {
        PowerInfo.outSet.language = LANGUAGE_CH;
    }
    PowerInfo.outSet.beeper_en = 1;         //蜂鸣器提示音
    PowerInfo.outSet.ac_down = 1;           //市电检测
    PowerInfo.outSet.remoteLock = 0;        //远程控制自动锁:OFF
    //
    #if defined(MULT_CHANNEL)
    if(IF_Power_Get_Hw_ID() != 0xff) {
        PowerInfo.outSet.slave_ID = IF_Power_Get_Hw_ID();          //从机ID
    } else {
        PowerInfo.outSet.slave_ID = 1;          //从机ID
    }
    #else
    PowerInfo.outSet.slave_ID = 1;          //从机ID
    #endif
    PowerInfo.outSet.maxFan = 0;            //风扇转速
    PowerInfo.outSet.senseVolt = PowerInfo.Model.default_SenseVolt;      //远端补偿电压
    PowerInfo.outSet.eLoad = ELOAD_ON;    //泄放负载开关
    PowerInfo.outSet.brightness = 100;      //亮度
    PowerInfo.outSet.on_delayTime = 0;     //延时启动
    PowerInfo.outSet.off_delayTime = 0;    //延时关闭
    PowerInfo.outSet.updateRate = 100;      //回读速度
    PowerInfo.outSet.ovp_delayTime = 20;
    PowerInfo.outSet.ocp_delayTime = 20;
    PowerInfo.outSet.otp_Temperature = PowerInfo.Model.defaultTemp;
    PowerInfo.outSet.localCookieID = 0xffffffff;
    //MAC地址
    PowerInfo.outSet.mac[0] = 0x00;
    PowerInfo.outSet.mac[1] = 0x80;
    PowerInfo.outSet.mac[2] = 0xE1;
    PowerInfo.outSet.mac[3] = (*(uint8_t *)(UID_BASE + 0)) + (*(uint8_t *)(UID_BASE + 1)) + (*(uint8_t *)(UID_BASE + 2)) + (*(uint8_t *)(UID_BASE + 3));
    PowerInfo.outSet.mac[4] = (*(uint8_t *)(UID_BASE + 4)) + (*(uint8_t *)(UID_BASE + 5)) + (*(uint8_t *)(UID_BASE + 6)) + (*(uint8_t *)(UID_BASE + 7));
    PowerInfo.outSet.mac[5] = (*(uint8_t *)(UID_BASE + 8)) + (*(uint8_t *)(UID_BASE + 9)) + (*(uint8_t *)(UID_BASE + 10)) + (*(uint8_t *)(UID_BASE + 11));
    PowerInfo.outSet.dhcp_en = 1;           //DHCP服务器:默认开启
    PowerInfo.Read.dhcp_en = PowerInfo.outSet.dhcp_en;
    //IP
    PowerInfo.outSet.ip[0] = 192;
    PowerInfo.outSet.ip[1] = 168;
    PowerInfo.outSet.ip[2] = 6;
    PowerInfo.outSet.ip[3] = 5;
    memcpy(PowerInfo.Read.ip, PowerInfo.outSet.ip, sizeof(PowerInfo.Read.ip));
    //子网掩码
    PowerInfo.outSet.netmask[0] = 255;
    PowerInfo.outSet.netmask[1] = 255;
    PowerInfo.outSet.netmask[2] = 255;
    PowerInfo.outSet.netmask[3] = 0;
    memcpy(PowerInfo.Read.netmask, PowerInfo.outSet.netmask, sizeof(PowerInfo.Read.netmask));
    //网关
    PowerInfo.outSet.gatewate[0] = 192;
    PowerInfo.outSet.gatewate[1] = 168;
    PowerInfo.outSet.gatewate[2] = 1;
    PowerInfo.outSet.gatewate[3] = 1;
    memcpy(PowerInfo.Read.gatewate, PowerInfo.outSet.gatewate, sizeof(PowerInfo.Read.gatewate));
    //网页
    memset(&PowerInfo.webSet, 0, sizeof(webSet_Def));
    //SOCKET SCPI
    PowerInfo.outSet.scpiPort = 5025;
    ////    PowerInfo.outSet.debugEn = 1;
    //生成6位数密码
    do {
        HAL_RNG_GenerateRandomNumber(&hrng, &PowerInfo.webSet.userPasswd_uint32);
        PowerInfo.webSet.userPasswd_uint32 %= 1000000;
    } while(PowerInfo.webSet.userPasswd_uint32 < 99999);
    //
    PowerInfo.outSet.zeroCurrent_autoEn = 0;
    PowerInfo.outSet.reserve = sizeof(PowerInfo.outSet);
}

#include "ListOut.h"
#include "Delayer.h"
#include "I2C_Device.h"
#include "EEPROM_Device.h"
#include "Interface.h"
#include "Modbus_master_IO.h"
#include "Modbus_master_List.h"
#include "Modbus_master_Delayer.h"
#include "Modbus_master_Power.h"

#define EEPROM_ABOUT_ADDR   (0x0)
#define EEPROM_OUTSET_ADDR  (0x100)
#define EEPROM_WEBSET_ADDR  (0x200)
#define EEPROM_PRESET_ARRD  (0x300) //0x800 2048
#define EEPROM_DEBUG_ARRD   (0xB00)
//#define EEPROM_LIST_ADDR    (0x400)
//#define EEPROM_DELAYER_ADDR (0x1000)



void __Load_Group(void)
{
    Group_Def group = {0};
    EEPROM_Read(&EEPROM, EEPROM_PRESET_ARRD, (uint8_t *)&group, sizeof(Group_Def));
    if(userCRC_Calculate((uint8_t *)&group, sizeof(Group_Def)) == 0) {
        memcpy(&PowerInfo.group, &group, sizeof(Group_Def));
    } else {
        memset(&PowerInfo.group, 0, sizeof(Group_Def));
    }
}
void __Save_Group(void)
{
    Group_Def group = {0};
    uint16_t crc = PowerInfo.group.crc;
    PowerInfo.group.crc = userCRC_Calculate((uint8_t *)&PowerInfo.group, sizeof(Group_Def) - sizeof(PowerInfo.group.crc));
    if(crc != PowerInfo.group.crc) {
        EEPROM_Write(&EEPROM, EEPROM_PRESET_ARRD, (uint8_t *)&PowerInfo.group, sizeof(Group_Def));
    }
}



void __Load_DebugInfo(void)
{
    Debug_Def debugInfo = {0};
    EEPROM_Read(&EEPROM, EEPROM_DEBUG_ARRD, (uint8_t *)&debugInfo, sizeof(Debug_Def));
    if(userCRC_Calculate((uint8_t *)&debugInfo, sizeof(Debug_Def)) == 0) {
        memcpy(&PowerInfo.debugInfo, &debugInfo, sizeof(Debug_Def));
    } else {
        memset(&PowerInfo.debugInfo, 0, sizeof(Debug_Def));
    }
}
void __Save_DebugInfo(void)
{
    Debug_Def debugInfo = {0};
    uint16_t crc = PowerInfo.debugInfo.crc;
    PowerInfo.debugInfo.crc = userCRC_Calculate((uint8_t *)&PowerInfo.debugInfo, sizeof(Debug_Def) - sizeof(PowerInfo.debugInfo.crc));
    if(crc != PowerInfo.debugInfo.crc) {
        EEPROM_Write(&EEPROM, EEPROM_DEBUG_ARRD, (uint8_t *)&PowerInfo.debugInfo, sizeof(Debug_Def));
    }
}



void __Setting_Load(void)
{
    //配置eeprom
    EEPROM_Config(&EEPROM, 0xA0, &i2c1_device);
    //加载网络设置
    __Load_WebSet();
    //加载关于本机
    __Load_About();
    //加载用户设置
    __Load_Setting();
    //加载用户组设置
    __Load_Group();
    //加载异常信息
    __Load_DebugInfo();
}

static OutSet_Def outSetTemp = {0};
void __Load_Setting(void)
{
    EEPROM_Read(&EEPROM, EEPROM_OUTSET_ADDR, (uint8_t *)&outSetTemp, sizeof(OutSet_Def));
    if((outSetTemp.reserve == sizeof(outSetTemp))
            && (userCRC_Calculate((uint8_t *)&outSetTemp, sizeof(OutSet_Def)) == 0)) {
        osKernelLock();
        {
            memcpy(&PowerInfo.outSet, &outSetTemp, sizeof(OutSet_Def));
        }
        osKernelUnlock();
    } else {
        PowerInfo.StateBar.bits.eeprom_setting_lost |= 1;
        osKernelLock();
        {
            void __Setting_Load_Default(void);
            __Setting_Load_Default();
        }
        osKernelUnlock();
    }
    //
    PowerInfo.Read.dhcp_en = PowerInfo.outSet.dhcp_en;
    memcpy(PowerInfo.Read.ip, PowerInfo.outSet.ip, sizeof(PowerInfo.Read.ip));
    memcpy(PowerInfo.Read.gatewate, PowerInfo.outSet.gatewate, sizeof(PowerInfo.Read.gatewate));
    memcpy(PowerInfo.Read.netmask, PowerInfo.outSet.netmask, sizeof(PowerInfo.Read.netmask));
    //若上电保持 & 上次
    if(PowerInfo.outSet.power_on == 0) {
        IF_Power_Set_On_Off(0);
    }
    Modbus_Update_OutSet();
}


void __Save_Setting(void)
{
    memset(&outSetTemp, 0, sizeof(OutSet_Def));
    osKernelLock();
    {
        memcpy(&outSetTemp, &PowerInfo.outSet, sizeof(OutSet_Def));
    }
    osKernelUnlock();
    uint16_t crc = outSetTemp.crc;
    outSetTemp.crc = userCRC_Calculate((uint8_t *)&outSetTemp, sizeof(OutSet_Def) - sizeof(outSetTemp.crc));
    if(crc != outSetTemp.crc) {
        EEPROM_Write(&EEPROM, EEPROM_OUTSET_ADDR, (uint8_t *)&outSetTemp, sizeof(OutSet_Def));
    }
}


webSet_Def temp_webSet = {0};
void __Load_WebSet(void)
{
    EEPROM_Read(&EEPROM, EEPROM_WEBSET_ADDR, (uint8_t *)&temp_webSet, sizeof(webSet_Def));
    osKernelLock();
    {
        if(userCRC_Calculate((uint8_t *)&temp_webSet, sizeof(webSet_Def)) == 0) {
            memcpy(&PowerInfo.webSet, &temp_webSet, sizeof(webSet_Def));
        } else {
            memset(&PowerInfo.webSet, 0, sizeof(webSet_Def));
            //生成6位数密码
            do {
                HAL_RNG_GenerateRandomNumber(&hrng, &PowerInfo.webSet.userPasswd_uint32);
                PowerInfo.webSet.userPasswd_uint32 %= 1000000;
            } while(PowerInfo.webSet.userPasswd_uint32 < 99999);
        }
    }
    osKernelUnlock();
    __Save_WebSet();
}
void __Save_WebSet(void)
{
    osKernelLock();
    {
        memcpy(&temp_webSet, &PowerInfo.webSet, sizeof(webSet_Def));
    }
    osKernelUnlock();
    //
    uint16_t crc = temp_webSet.crc;
    temp_webSet.crc = userCRC_Calculate((uint8_t *)&temp_webSet, sizeof(webSet_Def) - sizeof(temp_webSet.crc));
    if(crc != temp_webSet.crc) {
        EEPROM_Write(&EEPROM, EEPROM_WEBSET_ADDR, (uint8_t *)&temp_webSet, sizeof(webSet_Def));
    }
}



About_Def temp_about = {0};
void __Load_About(void)
{
    EEPROM_Read(&EEPROM, EEPROM_ABOUT_ADDR, (uint8_t *)&temp_about, sizeof(About_Def));
    osKernelLock();
    {
        if(userCRC_Calculate((uint8_t *)&temp_about, sizeof(About_Def)) == 0) {
            memcpy(&PowerInfo.about, &temp_about, sizeof(About_Def));
        } else {
            memset(&PowerInfo.about, 0, sizeof(About_Def));
        }
        strcpy(PowerInfo.about.company, POWER_COMPANY);
        strcpy(PowerInfo.about.lcd_version, POWER_LCD_VERSION);
        #ifdef USE_SDRAM
        PowerInfo.StateBar.bits.SDRAM = 1;
        #endif
        //写入设备ID
        for(uint8_t i = 0; i < 12; ++i) {
            PowerInfo.about.chip_UID[i] = (*(uint8_t *)(UID_BASE + i));
        }
    }
    osKernelUnlock();
    __Save_About();
}
void __Save_About(void)
{
    osKernelLock();
    {
        memcpy(&temp_about, &PowerInfo.about, sizeof(About_Def));
    }
    osKernelUnlock();
    //
    uint16_t crc = temp_about.crc;
    temp_about.crc = userCRC_Calculate((uint8_t *)&temp_about, sizeof(About_Def) - sizeof(temp_about.crc));
    if(crc != temp_about.crc) {
        EEPROM_Write(&EEPROM, EEPROM_ABOUT_ADDR, (uint8_t *)&temp_about, sizeof(About_Def));
    }
}


#if 0
void __Load_List(void)
{
    ListOut_InfoDef list = {0};
    EEPROM_Read(&EEPROM, EEPROM_LIST_ADDR, (uint8_t *)&list.set, sizeof(list.set));
    if(userCRC_Calculate((uint8_t *)&list.set, sizeof(list.set)) == 0) {
        memcpy(&listoutInfo.set, &list.set, sizeof(list.set));
        Modbus_List_UpdateSet();
    } else {
    }
}
void __Save_List(void)
{
    uint16_t crc = listoutInfo.set.crc;
    listoutInfo.set.crc = userCRC_Calculate((uint8_t *)&listoutInfo.set, sizeof(listoutInfo.set) - sizeof(listoutInfo.set.crc));
    if(crc != listoutInfo.set.crc) {
        EEPROM_Write(&EEPROM, EEPROM_LIST_ADDR, (uint8_t *)&listoutInfo.set, sizeof(listoutInfo.set));
    }
}


void __Load_Delayer(void)
{
    Delayer_InfoDef delayer = {0};
    EEPROM_Read(&EEPROM, EEPROM_DELAYER_ADDR, (uint8_t *)&delayer.set, sizeof(delayer.set));
    if(userCRC_Calculate((uint8_t *)&delayer.set, sizeof(delayer.set)) == 0) {
        memcpy(&delayerInfo.set, &delayer.set, sizeof(delayer.set));
        Modbus_Delayer_UpdateSet();
    } else {
    }
}
void __Save_Delayer(void)
{
    uint16_t crc = delayerInfo.set.crc;
    delayerInfo.set.crc = userCRC_Calculate((uint8_t *)&delayerInfo.set, sizeof(delayerInfo.set) - sizeof(delayerInfo.set.crc));
    if(crc != delayerInfo.set.crc) {
        EEPROM_Write(&EEPROM, EEPROM_DELAYER_ADDR, (uint8_t *)&delayerInfo.set, sizeof(delayerInfo.set));
    }
}
#endif