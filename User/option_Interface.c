#include "option_Interface.h"
#include "Modbus_master_IO.h"
#include "Modbus_master_Power.h"
#include "Modbus_master_List.h"
#include "Modbus_master_Delayer.h"
#include "fdcan.h"
#include "msgbox.h"


void IF_Debug_Set_ErrorFlag(uint32_t errorFlag)
{
    PowerInfo.debugInfo.errorFlag |= errorFlag;
    PowerInfo.Read.setchannged |= 1;
}
uint32_t IF_Debug_Get_ErrorFlag(void)
{
    uint32_t errorFlag = PowerInfo.debugInfo.errorFlag;
    PowerInfo.debugInfo.errorFlag = 0;
    PowerInfo.Read.setchannged |= 1;
    return errorFlag;
}


void IF_Option_Set_localCookieID(uint32_t localCookieID)
{
    __Interface_Lock();
    {
        PowerInfo.outSet.localCookieID = localCookieID;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
}

uint32_t IF_Option_Get_localCookieID(void) {
    return PowerInfo.outSet.localCookieID;
}



int32_t IF_Option_Get_SCPI_Port(void)
{
    return PowerInfo.outSet.scpiPort;
}

void IF_Option_Set_SCPI_Port(int32_t scpiPort)
{
    __Interface_Lock();
    {
        RANGE(scpiPort, 0, 0xffff);
        PowerInfo.outSet.scpiPort = scpiPort;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
}


uint32_t IF_System_Get_Mem_Max(void)
{
    return xPortGetMaxHeapSize();
}
uint32_t IF_System_Get_Mem_Used(void)
{
    return (xPortGetMaxHeapSize() - xPortGetFreeHeapSize());
}
uint32_t IF_System_Get_Mem_Used_Max(void)
{
    return (xPortGetMaxHeapSize() - xPortGetMinimumEverFreeHeapSize());
}


int16_t IF_Power_Get_UpdateRate(void)
{
    return PowerInfo.outSet.updateRate;
}

void IF_Power_Set_UpdateRate(int16_t updateRate)
{
    __Interface_Lock();
    {
        RANGE(updateRate, 50, 1000);
        PowerInfo.outSet.updateRate = updateRate;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
}


void IF_Power_Set_OVP_delayTime(int16_t value)
{
    __Interface_Lock();
    {
        value = MAX(value, 10);
        value = MIN(value, 10000);
        PowerInfo.outSet.ovp_delayTime = value;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Update_OVP_delayTime(value);
}
int16_t IF_Power_Get_OVP_delayTime(void)
{
    return PowerInfo.outSet.ovp_delayTime;
}
void IF_Power_Set_OCP_delayTime(int16_t value)
{
    __Interface_Lock();
    {
        value = MAX(value, 10);
        value = MIN(value, 10000);
        PowerInfo.outSet.ocp_delayTime = value;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Update_OCP_delayTime(value);
}
int16_t IF_Power_Get_OCP_delayTime(void)
{
    return PowerInfo.outSet.ocp_delayTime;
}

void IF_Power_Set_OTP_Temperature(int16_t value)
{
    __Interface_Lock();
    {
        PowerInfo.outSet.otp_Temperature = value;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Update_OTP_Temperature(value);
}
int16_t IF_Power_Get_OTP_Temperature(void)
{
    return PowerInfo.outSet.otp_Temperature;
}

void IF_Power_Set_ON_delayTime(int16_t value)
{
    __Interface_Lock();
    {
        value = MAX(value, 0);
        value = MIN(value, 10000);
        PowerInfo.outSet.on_delayTime = value;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Update_ON_delayTime(value);
}
int16_t IF_Power_Get_ON_delayTime(void)
{
    return PowerInfo.outSet.on_delayTime;
}

void IF_Power_Set_OFF_delayTime(int16_t value)
{
    __Interface_Lock();
    {
        value = MAX(value, 0);
        value = MIN(value, 10000);
        PowerInfo.outSet.off_delayTime = value;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Update_OFF_delayTime(value);
}
int16_t IF_Power_Get_OFF_delayTime(void)
{
    return PowerInfo.outSet.off_delayTime;
}





void IF_System_Set_Remote(uint8_t remote)
{
    __Interface_Lock();
    {
        static uint8_t lockFlag = 0;
        if(remote != 0)
        {
            //若指令前已经锁定了,则不解锁
            if(IF_System_Get_Lock() != 0)
            {
                lockFlag = 1;
            }
            IF_System_Set_Lock(1);
        }
        else
        {
            //恢复本地控制:若先前没锁定,则解锁
            if(lockFlag == 0)
            {
                IF_System_Set_Lock(0);
            }
            lockFlag = 0;
        }
        PowerInfo.StateBar.bits.Remote = (remote != 0);
    }
    __Interface_Unlock();
}

uint8_t IF_System_Get_Remote(void)
{
    return (PowerInfo.StateBar.bits.Remote != 0);
}


/*
 * @brief
 * @details     远端补偿设置
 */
void IF_Power_Set_Sense_Volt(float value)
{
    __Interface_Lock();
    {
        if(value < 0)
        {
            value = 0;
        }
        PowerInfo.outSet.senseVolt = value;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Update_SenseVolt_Set(value);
}
float IF_Power_Get_Sense_Volt(void)
{
    return PowerInfo.outSet.senseVolt;
}



/*
 * @brief
 * @details     电源模式
 */
uint8_t IF_Power_Get_Power_Mode(void)
{
    return PowerInfo.outSet.powerMode;
}

void IF_Power_Set_Power_Mode(uint8_t mode)
{
    switch(PowerInfo.Read.powerType) {
        case MODEL_800_2: {
            switch(mode) {
                //800V不允许串并联
                case POWER_MODE_CAN_SERIES_MASTER:
                case POWER_MODE_CAN_SERIES_SLAVE: {
                    MsgBox_Printf(MSG_TYPE_TIPS, NULL, MultStr_GetS(&mStr_800V_NotSeries));
                    return;
                }
                break;
            }
        }
        break;
    }
    __Interface_Lock();
    {
        RANGE(mode, POWER_MODE_MIN, POWER_MODE_MAX);
        PowerInfo.outSet.powerMode = mode;
        IF_Option_Set_Slave_ID(PowerInfo.outSet.slave_ID);
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Update_PowerMode_Set(mode);
}




/*
 * @brief
 * @details     CAN通讯组
 */
int8_t IF_Option_Get_CAN_Groups_ID(void)
{
    return PowerInfo.outSet.canGroup;
}

void IF_Option_Set_CAN_Groups_ID(int8_t id)
{
    __Interface_Lock();
    {
        RANGE(id, 0, POWER_CAN_GROUP_ID_MAX);
        PowerInfo.outSet.canGroup = id;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
}

/*
 * @brief
 * @details     CAN通讯ID
 */
int8_t IF_Option_Get_Slave_ID(void)
{
    return PowerInfo.outSet.slave_ID;
}

void IF_Option_Set_Slave_ID(int8_t id)
{
    __Interface_Lock();
    {
        switch(IF_Power_Get_Power_Mode()) {
            //串联模式下,任何机型最多2台
            case POWER_MODE_CAN_SERIES_MASTER:
            case POWER_MODE_CAN_SERIES_SLAVE: {
                RANGE(id, 1, 1);
            }
            break;
            case POWER_MODE_CAN_SYNERGY_MASTER:
            case POWER_MODE_CAN_SYNERGY_SLAVE: {
                RANGE(id, 1, POWER_CAN_SYNERGY_ID_MAX);
            }
            break;
            //并联模式下,除5并机2kw的机型为2台,其他机型为5台
            case POWER_MODE_CAN_PARALLEL_MASTER:
            case POWER_MODE_CAN_PARALLEL_SLAVE: {
                if(PowerInfo.Model.maxPower > 2000.0f) {
                    RANGE(id, 1, 1);
                } else {
                    RANGE(id, 1, POWER_CAN_ID_MAX);
                }
            }
            break;
        }
        PowerInfo.outSet.slave_ID = id;
        PowerInfo.Read.setchannged |= 1;
        #if 0
        uint8_t idIDLE_flag = 1;
        if(PowerInfo.outSet.slave_ID != id) {
            if(id < ARRAY_COUNT(PowerInfo.Read.SlaveInfo)) {
                for(uint8_t i = 0; i < ARRAY_COUNT(PowerInfo.Read.SlaveInfo); ++i) {
                    if(PowerInfo.Read.SlaveInfo[id].data_ready != 0) {
                        idIDLE_flag = 0;
                        break;
                    }
                }
            }
            if(idIDLE_flag != 0) {
                PowerInfo.outSet.slave_ID = id;
                PowerInfo.Read.setchannged |= 1;
            } else {
                MsgBox_Printf(MSG_TYPE_TIPS, NULL, MultStr_GetS(&mStr_CAN_ID_ErroMsg), id);
            }
        }
        #endif
    }
    __Interface_Unlock();
}




/*
 * @brief
 * @details     风扇控制
 */
int8_t IF_Power_Get_MaxFan(void)
{
    return PowerInfo.outSet.maxFan;
}

void IF_Power_Set_MaxFan(int8_t maxFan)
{
    __Interface_Lock();
    {
        RANGE(maxFan, 10, 100);
        PowerInfo.outSet.maxFan = maxFan;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Update_MaxFan_Set(maxFan);
}



//假负载
uint8_t IF_Power_Get_eLoad(void)
{
    return PowerInfo.outSet.eLoad;
}
void IF_Power_Set_eLoad(uint8_t eLoad)
{
    __Interface_Lock();
    {
        PowerInfo.outSet.eLoad = eLoad;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Update_eLoad_Set(eLoad);
}

//上电输出
uint8_t IF_Power_Get_Power_out(void)
{
    return PowerInfo.outSet.power_on;
}
void IF_Power_Set_Power_out(uint8_t enable)
{
    __Interface_Lock();
    {
        PowerInfo.outSet.power_on = enable;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
}


//上电输出
uint8_t IF_Power_Get_Power_Down(void)
{
    return PowerInfo.outSet.ac_down;
}
void IF_Power_Set_Power_Down(uint8_t enable)
{
    __Interface_Lock();
    {
        PowerInfo.outSet.ac_down = enable;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Update_AC_Down_Set(enable);
}

//用户组
int8_t IF_Option_Get_Groups(void)
{
    return PowerInfo.outSet.userGroup;
}
void IF_Option_Set_Groups(int8_t groups)
{
    __Interface_Lock();
    {
        RANGE(groups, 0, (GROUPS_COUNT - 1));
        PowerInfo.outSet.userGroup = groups;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
}

//背光设置
uint8_t IF_Option_Get_Brightness(void)
{
    return PowerInfo.outSet.brightness;
}
void IF_Option_Set_Brightness(uint8_t brightness)
{
    __Interface_Lock();
    {
        RANGE(brightness, 20, 100);
        PowerInfo.outSet.brightness = brightness;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, brightness);
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
}

#include "Beep_Device.h"
//蜂鸣器设置
uint8_t IF_Option_Get_Beeper(void)
{
    return PowerInfo.outSet.beeper_en;
}
void IF_Option_Set_Beeper(uint8_t beeper_en)
{
    __Interface_Lock();
    {
        PowerInfo.outSet.beeper_en = beeper_en;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
}


//远程控制锁
uint8_t IF_Option_Get_RemoteLock(void)
{
    return PowerInfo.outSet.remoteLock;
}
void IF_Option_Set_RemoteLock(uint8_t en)
{
    __Interface_Lock();
    {
        PowerInfo.outSet.remoteLock = en;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
}

//语言设置
int8_t IF_Option_Get_language(void)
{
    return PowerInfo.outSet.language;
}
void IF_Option_Set_language(int8_t language)
{
    __Interface_Lock();
    {
        RANGE(language, LANGUAGE_MIN, LANGUAGE_MAX);
        PowerInfo.outSet.language = language;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
}




void IF_Option_Write_SN(const char *str)
{
    __Interface_Lock();
    {
        //        if(strlen(str) < (sizeof(PowerInfo.about.serialNumber) - 1))
        if(strlen(str) == 13)
        {
            strcpy(PowerInfo.about.serialNumber, str);
            __Save_About();
        }
    }
    __Interface_Unlock();
}
const char *IF_Option_Read_SN(void)
{
    __Interface_Lock();
    {
        __Load_About();
    }
    __Interface_Unlock();
    return PowerInfo.about.serialNumber;
}




const char *IF_Option_Read_ModelName(void)
{
    return PowerInfo.about.model_name;
}


#include "lwip.h"
#include "lwip/init.h"
#include "lwip/netif.h"

//网络设置:DHCP
uint8_t IF_Option_NetWork_Get_DHCP(void)
{
    return PowerInfo.Read.dhcp_en;
}
uint8_t IF_Option_NetWork_Get_DHCP_Now(void)
{
    return PowerInfo.outSet.dhcp_en;
}
void IF_Option_NetWork_Set_DHCP(uint8_t dhcp_en)
{
    __Interface_Lock();
    {
        PowerInfo.Read.dhcp_en = dhcp_en;
    }
    __Interface_Unlock();
}
//网络设置:IP
uint8_t IF_Option_NetWork_Get_IP(uint8_t idx)
{
    if(IF_Option_NetWork_Get_DHCP() == 0)
    {
        return PowerInfo.Read.ip[idx];
    }
    else
    {
        static uint8_t ip_now[4] = {0};
        extern struct netif gnetif;
        ip_now[0] = (gnetif.ip_addr.addr >> 0) & 0xff;
        ip_now[1] = (gnetif.ip_addr.addr >> 8) & 0xff;
        ip_now[2] = (gnetif.ip_addr.addr >> 16) & 0xff;
        ip_now[3] = (gnetif.ip_addr.addr >> 24) & 0xff;
        return ip_now[idx];
    }
}
uint8_t IF_Option_NetWork_Get_IP_Now(uint8_t idx)
{
    if(IF_Option_NetWork_Get_DHCP() == 0)
    {
        return PowerInfo.outSet.ip[idx];
    }
    else
    {
        static uint8_t ip_now[4] = {0};
        extern struct netif gnetif;
        ip_now[0] = (gnetif.ip_addr.addr >> 0) & 0xff;
        ip_now[1] = (gnetif.ip_addr.addr >> 8) & 0xff;
        ip_now[2] = (gnetif.ip_addr.addr >> 16) & 0xff;
        ip_now[3] = (gnetif.ip_addr.addr >> 24) & 0xff;
        return ip_now[idx];
    }
}
void IF_Option_NetWork_Set_IP(uint8_t idx, uint8_t value)
{
    __Interface_Lock();
    {
        PowerInfo.Read.ip[idx] = value;
    }
    __Interface_Unlock();
}


uint8_t IF_Option_NetWork_Get_Gateway(uint8_t idx)
{
    if(IF_Option_NetWork_Get_DHCP() == 0)
    {
        return PowerInfo.Read.gatewate[idx];
    }
    else
    {
        static uint8_t gateway_now[4] = {0};
        extern struct netif gnetif;
        gateway_now[0] = (gnetif.gw.addr >> 0) & 0xff;
        gateway_now[1] = (gnetif.gw.addr >> 8) & 0xff;
        gateway_now[2] = (gnetif.gw.addr >> 16) & 0xff;
        gateway_now[3] = (gnetif.gw.addr >> 24) & 0xff;
        return gateway_now[idx];
    }
}
uint8_t IF_Option_NetWork_Get_Gateway_Now(uint8_t idx)
{
    if(IF_Option_NetWork_Get_DHCP() == 0)
    {
        return PowerInfo.outSet.gatewate[idx];
    }
    else
    {
        static uint8_t gateway_now[4] = {0};
        extern struct netif gnetif;
        gateway_now[0] = (gnetif.gw.addr >> 0) & 0xff;
        gateway_now[1] = (gnetif.gw.addr >> 8) & 0xff;
        gateway_now[2] = (gnetif.gw.addr >> 16) & 0xff;
        gateway_now[3] = (gnetif.gw.addr >> 24) & 0xff;
        return gateway_now[idx];
    }
}
void IF_Option_NetWork_Set_Gateway(uint8_t idx, uint8_t value)
{
    __Interface_Lock();
    {
        PowerInfo.Read.gatewate[idx] = value;
    }
    __Interface_Unlock();
}



uint8_t IF_Option_NetWork_Get_NetMask(uint8_t idx)
{
    if(IF_Option_NetWork_Get_DHCP() == 0)
    {
        return PowerInfo.Read.netmask[idx];
    }
    else
    {
        static uint8_t netmask_now[4] = {0};
        extern struct netif gnetif;
        netmask_now[0] = (gnetif.netmask.addr >> 0) & 0xff;
        netmask_now[1] = (gnetif.netmask.addr >> 8) & 0xff;
        netmask_now[2] = (gnetif.netmask.addr >> 16) & 0xff;
        netmask_now[3] = (gnetif.netmask.addr >> 24) & 0xff;
        return netmask_now[idx];
    }
}
uint8_t IF_Option_NetWork_Get_NetMask_Now(uint8_t idx)
{
    if(IF_Option_NetWork_Get_DHCP() == 0)
    {
        return PowerInfo.outSet.netmask[idx];
    }
    else
    {
        static uint8_t netmask_now[4] = {0};
        extern struct netif gnetif;
        netmask_now[0] = (gnetif.netmask.addr >> 0) & 0xff;
        netmask_now[1] = (gnetif.netmask.addr >> 8) & 0xff;
        netmask_now[2] = (gnetif.netmask.addr >> 16) & 0xff;
        netmask_now[3] = (gnetif.netmask.addr >> 24) & 0xff;
        return netmask_now[idx];
    }
}
void IF_Option_NetWork_Set_NetMask(uint8_t idx, uint8_t value)
{
    __Interface_Lock();
    {
        PowerInfo.Read.netmask[idx] = value;
    }
    __Interface_Unlock();
}


void IF_Option_NetWork_Set_Apply(void)
{
    __Interface_Lock();
    {
        PowerInfo.outSet.dhcp_en = PowerInfo.Read.dhcp_en;
        memcpy(PowerInfo.outSet.ip, PowerInfo.Read.ip, sizeof(PowerInfo.outSet.ip));
        memcpy(PowerInfo.outSet.gatewate, PowerInfo.Read.gatewate, sizeof(PowerInfo.outSet.gatewate));
        memcpy(PowerInfo.outSet.netmask, PowerInfo.Read.netmask, sizeof(PowerInfo.outSet.netmask));
        PowerInfo.Read.netApply = 1;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    extern void netif_set_dhcp_callback(void *arg);
    extern void netif_up_callback(void *arg);
    extern void netif_down_callback(void *arg);
    extern struct netif gnetif;
    tcpip_callback(netif_down_callback, &gnetif);
    osDelay(200);
    tcpip_callback(netif_set_dhcp_callback, &gnetif);
    osDelay(200);
    tcpip_callback(netif_up_callback, &gnetif);
}


int32_t IF_Option_NetWork_Get_WebPasswd(void) {
    return PowerInfo.webSet.userPasswd_uint32;
}

void IF_Option_NetWork_Set_WebPasswd(int32_t passwd)
{
    __Interface_Lock();
    {
        RANGE(passwd, 0, 999999);
        PowerInfo.webSet.userPasswd_uint32 = passwd;
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.webchannged |= 1;
    }
    __Interface_Unlock();
}

#include "MsgBox.h"
void IF_Option_Input_Passwd(uint32_t passwd)
{
    __Interface_Lock();
    {
        switch(passwd)
        {
            //校准密码
            case 1218: {
                Modbus_Calib_Mode(1);
                MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_Enter_CalibMode_Tips));
            }
            break;
        }
    }
    __Interface_Unlock();
}


void IF_Option_Restore(void)
{
    __Interface_Lock();
    {
        __Setting_Load_Default();
        __Save_Setting();
        #if 0
        __Save_List();
        __Save_Delayer();
        #endif
        __Save_About();
    }
    __Interface_Unlock();
    Modbus_Update_OutSet();
}



//上电电流自动清零
uint8_t IF_Option_Get_ZeroCurrent_AutoEn(void)
{
    return PowerInfo.outSet.zeroCurrent_autoEn;
}
void IF_Option_Set_ZeroCurrent_AutoEn(uint8_t en)
{
    __Interface_Lock();
    {
        PowerInfo.outSet.zeroCurrent_autoEn = en;
        PowerInfo.Read.setchannged |= 1;
    }
    __Interface_Unlock();
    Modbus_Set_ZeroCurr_AutoEn(en);
}


#if 0
//上电电流自动清零
uint8_t IF_Option_Get_Debug_En(void)
{
    return PowerInfo.outSet.debugEn;
}
void IF_Option_Set_Debug_En(uint8_t en)
{
    PowerInfo.outSet.debugEn = en;
    PowerInfo.Read.setchannged |= 1;
}
#endif




uint8_t IF_Preset_Load(uint8_t userGroup, uint8_t idx)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    IF_Power_Set_OVP(PowerInfo.group.Preset[userGroup][idx].ovpSet);
    IF_Power_Set_OCP(PowerInfo.group.Preset[userGroup][idx].ocpSet);
    IF_Power_Set_OVP_onoff(PowerInfo.group.Preset[userGroup][idx].ovpOnoff);
    IF_Power_Set_OCP_onoff(PowerInfo.group.Preset[userGroup][idx].ocpOnoff);
    IF_Power_Set_OutVolt(PowerInfo.group.Preset[userGroup][idx].voltSet);
    IF_Power_Set_OutCurr(PowerInfo.group.Preset[userGroup][idx].currSet);
    return 1;
}
uint8_t IF_Preset_Save(uint8_t userGroup, uint8_t idx)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    PowerInfo.group.Preset[userGroup][idx].ovpSet = IF_Power_Get_OVP();
    PowerInfo.group.Preset[userGroup][idx].ocpSet = IF_Power_Get_OCP();
    PowerInfo.group.Preset[userGroup][idx].ovpOnoff = IF_Power_Get_OVP_onoff();
    PowerInfo.group.Preset[userGroup][idx].ocpOnoff = IF_Power_Get_OCP_onoff();
    PowerInfo.group.Preset[userGroup][idx].voltSet = IF_Power_Get_OutVolt();
    PowerInfo.group.Preset[userGroup][idx].currSet = IF_Power_Get_OutCurr();
    PowerInfo.Read.setchannged = 1;
    PowerInfo.Read.groupChannged = 1;
    return 1;
}
//预设值电压
uint8_t IF_Preset_Set_Voltage(uint8_t userGroup, uint8_t idx, float voltage)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    PowerInfo.group.Preset[userGroup][idx].voltSet = voltage;
    PowerInfo.Read.setchannged = 1;
    PowerInfo.Read.groupChannged = 1;
    return 1;
}
float IF_Preset_Get_Voltage(uint8_t userGroup, uint8_t idx)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    return PowerInfo.group.Preset[userGroup][idx].voltSet;
}

//预设值电流
uint8_t IF_Preset_Set_Current(uint8_t userGroup, uint8_t idx, float current)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    PowerInfo.group.Preset[userGroup][idx].currSet = current;
    PowerInfo.Read.setchannged = 1;
    PowerInfo.Read.groupChannged = 1;
    return 1;
}
float IF_Preset_Get_Current(uint8_t userGroup, uint8_t idx)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    return PowerInfo.group.Preset[userGroup][idx].currSet;
}

//预设值OVP
uint8_t IF_Preset_Set_OVP(uint8_t userGroup, uint8_t idx, float ovp)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    PowerInfo.group.Preset[userGroup][idx].ovpSet = ovp;
    PowerInfo.Read.setchannged = 1;
    PowerInfo.Read.groupChannged = 1;
    return 1;
}
float IF_Preset_Get_OVP(uint8_t userGroup, uint8_t idx)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    return PowerInfo.group.Preset[userGroup][idx].ovpSet;
}
uint8_t IF_Preset_Set_OVP_onoff(uint8_t userGroup, uint8_t idx, uint8_t onoff)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    PowerInfo.group.Preset[userGroup][idx].ovpOnoff = onoff;
    PowerInfo.Read.setchannged = 1;
    PowerInfo.Read.groupChannged = 1;
    return 1;
}
uint8_t IF_Preset_Get_OVP_onoff(uint8_t userGroup, uint8_t idx)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    return PowerInfo.group.Preset[userGroup][idx].ovpOnoff;
}

//预设值OCP
uint8_t IF_Preset_Set_OCP(uint8_t userGroup, uint8_t idx, float ocp)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    PowerInfo.group.Preset[userGroup][idx].ocpSet = ocp;
    PowerInfo.Read.setchannged = 1;
    PowerInfo.Read.groupChannged = 1;
    return 1;
}
uint8_t IF_Preset_Get_OCP(uint8_t userGroup, uint8_t idx)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    return PowerInfo.group.Preset[userGroup][idx].ocpSet;
}
uint8_t IF_Preset_Set_OCP_onoff(uint8_t userGroup, uint8_t idx, uint8_t onoff)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    PowerInfo.group.Preset[userGroup][idx].ocpOnoff = onoff;
    PowerInfo.Read.setchannged = 1;
    PowerInfo.Read.groupChannged = 1;
    return 1;
}
uint8_t IF_Preset_Get_OCP_onoff(uint8_t userGroup, uint8_t idx)
{
    if(userGroup >= GROUPS_COUNT) { return 0; }
    if(idx >= ARRAY_COUNT(PowerInfo.group.Preset[userGroup])) { return 0; }
    return PowerInfo.group.Preset[userGroup][idx].ocpOnoff;
}

