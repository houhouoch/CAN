#include "Task_Manager.h"

#include "Interface.h"
#include "Modbus_master_IO.h"
#include "Modbus_master_Power.h"
#include "Modbus_master_List.h"
#include "Modbus_master_Delayer.h"
#include "fdcan.h"


uint8_t IF_Power_Get_Hw_Channels(void) {
    return PowerInfo.Read.powerChannels;
}
uint8_t IF_Power_Get_Hw_ID(void) {
    return PowerInfo.Read.powerID;
}



void IF_Display_Go_to_Page(uint8_t pageName)
{
    __Interface_Lock();
    {
        extern void UI_Switch_Page(uint8_t page);
        UI_Switch_Page(pageName);
    }
    __Interface_Unlock();
}


uint64_t IF_Power_Get_RunTime(void)
{
    return PowerInfo.Read.runTime_ms;
}

void IF_Power_Set_RunTime(uint64_t runTime_ms)
{
    PowerInfo.Read.runTime_ms = runTime_ms;
}


/*
 * @brief
 * @details     斜率模式
 */
uint8_t IF_Power_Get_SlewRate_Mode(void)
{
    return PowerInfo.outSet.SlewRate_Mode;
}

void IF_Power_Set_SlewRate_Mode(uint8_t mode)
{
    __Interface_Lock();
    {
        //        if(IF_Power_Get_SlewRate_Mode() == mode){
        //            __Interface_Unlock();
        //            return;
        //        }
        RANGE(mode, SRMODE_MIN, SRMODE_MAX);
        PowerInfo.outSet.SlewRate_Mode = mode;
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.canUpdata_event |= 1;
    }
    __Interface_Unlock();
}

/*
 * @brief
 * @details     电压斜率设置
 */
void IF_Power_Set_VSR_Up(float value)
{
    __Interface_Lock();
    {
        RANGE(value, 0.01f, PowerInfo.Model.maxVolt * 2);
        //        if(IF_Power_Get_VSR_Up() == value){
        //            __Interface_Unlock();
        //            return;
        //        }
        PowerInfo.outSet.VSR_Up = value;
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.canUpdata_event |= 1;
    }
    __Interface_Unlock();
}
float IF_Power_Get_VSR_Up(void)
{
    return PowerInfo.outSet.VSR_Up;
}
void IF_Power_Set_VSR_Down(float value)
{
    __Interface_Lock();
    {
        RANGE(value, 0.01f, PowerInfo.Model.maxVolt * 2);
        //        if(IF_Power_Get_VSR_Down() == value){
        //            __Interface_Unlock();
        //            return;
        //        }
        PowerInfo.outSet.VSR_Down = value;
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.canUpdata_event |= 1;
    }
    __Interface_Unlock();
}
float IF_Power_Get_VSR_Down(void)
{
    return PowerInfo.outSet.VSR_Down;
}

/*
 * @brief
 * @details     电流斜率设置
 */
void IF_Power_Set_ISR_Up(float value)
{
    __Interface_Lock();
    {
        RANGE(value, 0.01f, PowerInfo.Model.maxCurr * 2);
        //        if(IF_Power_Get_ISR_Up() == value){
        //            __Interface_Unlock();
        //            return;
        //        }
        PowerInfo.outSet.ISR_Up = value;
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.canUpdata_event |= 1;
    }
    __Interface_Unlock();
}
float IF_Power_Get_ISR_Up(void)
{
    return PowerInfo.outSet.ISR_Up;
}
void IF_Power_Set_ISR_Down(float value)
{
    __Interface_Lock();
    {
        RANGE(value, 0.01f, PowerInfo.Model.maxCurr * 2);
        //        if(IF_Power_Get_ISR_Down() == value){
        //            __Interface_Unlock();
        //            return;
        //        }
        PowerInfo.outSet.ISR_Down = value;
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.canUpdata_event |= 1;
    }
    __Interface_Unlock();
}
float IF_Power_Get_ISR_Down(void)
{
    return PowerInfo.outSet.ISR_Down;
}



/*
 * @brief
 * @details     电源内阻
 */
void IF_Power_Set_ResIn(float value)
{
    __Interface_Lock();
    {
        RANGE(value, 0, PowerInfo.Model.maxResIn);
        //        if(IF_Power_Get_ResIn() == value){
        //            __Interface_Unlock();
        //            return;
        //        }
        PowerInfo.outSet.ResIn_set = value;
        if(PowerInfo.outSet.ResIn_set < 0)
        {
            PowerInfo.outSet.ResIn_set = 0;
        }
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.canUpdata_event |= 1;
    }
    __Interface_Unlock();
}
float IF_Power_Get_ResIn(void)
{
    return PowerInfo.outSet.ResIn_set;
}




/*
 * @brief
 * @details     OVP过压保护设置
 */
void IF_Power_Set_OVP(float value)
{
    __Interface_Lock();
    {
        float setVol = value;
        RANGE(setVol, PowerInfo.Model.minVolt, PowerInfo.Model.maxVolt);
        //        if(IF_Power_Get_OVP() == setVol){
        //            __Interface_Unlock();
        //            return;
        //        }
        PowerInfo.outSet.ovp_set = setVol;
        IF_Power_Set_OutVolt(IF_Power_Get_OutVolt());
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.canUpdata_event |= 1;
    }
    __Interface_Unlock();
}
float IF_Power_Get_OVP(void)
{
    return PowerInfo.outSet.ovp_set;
}
void IF_Power_Set_OVP_onoff(uint8_t onoff)
{
    __Interface_Lock();
    {
        //        if(IF_Power_Get_OVP_onoff() == onoff){
        //            __Interface_Unlock();
        //            return;
        //        }
        PowerInfo.outSet.ovp_onoff = onoff;
        if(onoff)
        {
            IF_Power_Set_OutVolt(IF_Power_Get_OutVolt());
        }
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.canUpdata_event |= 1;
    }
    __Interface_Unlock();
}
uint8_t IF_Power_Get_OVP_onoff(void)
{
    return PowerInfo.outSet.ovp_onoff;
}

/*
 * @brief
 * @details     OCP过流保护设置
 */
void IF_Power_Set_OCP(float value)
{
    __Interface_Lock();
    {
        float setCurr = value;
        RANGE(setCurr, PowerInfo.Model.minCurr, PowerInfo.Model.maxCurr);
        //        if(IF_Power_Get_OCP() == setCurr){
        //            __Interface_Unlock();
        //            return;
        //        }
        PowerInfo.outSet.ocp_set = setCurr;
        IF_Power_Set_OutCurr(IF_Power_Get_OutCurr());
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.canUpdata_event |= 1;
    }
    __Interface_Unlock();
}
float IF_Power_Get_OCP(void)
{
    return PowerInfo.outSet.ocp_set;
}
void IF_Power_Set_OCP_onoff(uint8_t onoff)
{
    __Interface_Lock();
    {
        //        if(IF_Power_Get_OCP_onoff() == onoff){
        //            __Interface_Unlock();
        //            return;
        //        }
        PowerInfo.outSet.ocp_onoff = onoff;
        if(onoff)
        {
            IF_Power_Set_OutCurr(IF_Power_Get_OutCurr());
        }
        PowerInfo.Read.setchannged |= 1;
        PowerInfo.Read.canUpdata_event |= 1;
    }
    __Interface_Unlock();
}
uint8_t IF_Power_Get_OCP_onoff(void)
{
    return PowerInfo.outSet.ocp_onoff;
}


/*
 * @brief
 * @details     输出电压设置
 */
float IF_Power_Get_OutVolt(void)
{
    return PowerInfo.outSet.volt_set;
}
void IF_Power_Set_OutVolt(float value)
{
    __Interface_Lock();
    {
        float setVol = value;
        #ifdef __OVP_LIMIT
        RANGE(setVol, PowerInfo.Model.minVolt, (IF_Power_Get_OVP_onoff() ? IF_Power_Get_OVP() : PowerInfo.Model.maxVolt));
        #endif
        RANGE(setVol, PowerInfo.Model.minVolt, (PowerInfo.Model.maxVolt));
        //        if(IF_Power_Get_OutVolt() == setVol){
        //            __Interface_Unlock();
        //            return ;
        //        }
        PowerInfo.outSet.volt_set = setVol;
        if(IF_Power_Get_Power_Mode() != POWER_MODE_EXT_V) {
            PowerInfo.Read.setchannged |= 1;
        }
        PowerInfo.Read.canUpdata_event |= 1;
        if(osThreadGetId() != CAN_Rx_Task_ThreadID) {
            PowerInfo.Read.syncRequest = 1;
        } else {
        }
        #if 0
        switch(IF_Power_Get_Power_Mode())
        {
            case POWER_MODE_CAN_PARALLEL_MASTER:
            case POWER_MODE_CAN_SERIES_MASTER:
            {
                CANFD_Send_Extended(CAN_VOL_WRITE_MASTER, (uint8_t *)&PowerInfo.outSet.volt_set);
            }
            break;
        }
        #endif
        //更新电流
        IF_Power_Set_OutCurr(PowerInfo.outSet.curr_set);
    }
    __Interface_Unlock();
}


/*
 * @brief
 * @details     输出电流设置
 */
float IF_Power_Get_OutCurr(void)
{
    return PowerInfo.outSet.curr_set;
}
void IF_Power_Set_OutCurr(float value)
{
    __Interface_Lock();
    {
        float setCur_max = PowerInfo.Model.maxPower / (PowerInfo.outSet.volt_set + 0.00001f);
        setCur_max = MAX(0, setCur_max);
        setCur_max = MIN(setCur_max, PowerInfo.Model.maxCurr);
        #ifdef __OCP_LIMIT
        setCur_max = MIN(setCur_max, (IF_Power_Get_OCP_onoff() ? IF_Power_Get_OCP() : PowerInfo.Model.maxCurr));
        #endif
        setCur_max = MIN(setCur_max, PowerInfo.Model.maxCurr);
        float setCurr = value;
        RANGE(setCurr, PowerInfo.Model.minCurr, setCur_max);
        //        if(IF_Power_Get_OutCurr() == setCurr){
        //            __Interface_Unlock();
        //            return ;
        //        }
        PowerInfo.outSet.curr_set = setCurr;
        if(IF_Power_Get_Power_Mode() != POWER_MODE_EXT_V) {
            PowerInfo.Read.setchannged |= 1;
        }
        PowerInfo.Read.canUpdata_event |= 1;
        if(osThreadGetId() != CAN_Rx_Task_ThreadID) {
            PowerInfo.Read.syncRequest = 1;
        } else {
        }
        #if 0
        switch(IF_Power_Get_Power_Mode())
        {
            case POWER_MODE_CAN_PARALLEL_MASTER:
            case POWER_MODE_CAN_SERIES_MASTER:
            {
                CANFD_Send_Extended(CAN_CURR_WRITE_MASTER, (uint8_t *)&PowerInfo.outSet.curr_set);
            }
            break;
        }
        #endif
    }
    __Interface_Unlock();
}

/*
 * @brief
 * @details     电源输出开关
 */
uint8_t IF_Power_Get_On_Off_Now(void)
{
    return PowerInfo.State.bits.onoff;
}

uint8_t IF_Power_Get_On_Off(void)
{
    return PowerInfo.outSet.onoff;
}

void IF_Power_Set_On_Off(uint8_t onoff)
{
    __Interface_Lock();
    {
        //        if(IF_Power_Get_On_Off() == onoff){
        //            __Interface_Unlock();
        //            return;
        //        }
        PowerInfo.outSet.onoff = onoff;
        if(IF_Power_Get_Power_Mode() != POWER_MODE_EXT_V) {
            PowerInfo.Read.setchannged |= 1;
        }
        PowerInfo.Read.canUpdata_event |= 1;
        if(osThreadGetId() != CAN_Rx_Task_ThreadID) {
            PowerInfo.Read.syncRequest = 1;
        } else {
        }
    }
    __Interface_Unlock();
}



uint8_t IF_Power_Get_OTP_TRIPed(void)
{
    return PowerInfo.State.bits.otp;
}
uint8_t IF_Power_Get_OVP_TRIPed(void)
{
    return PowerInfo.State.bits.ovp;
}
void IF_Power_OVP_Clean(void)
{
    PowerInfo.State.bits.ovp = 0;
    //
    State_Def status_temp = {0};
    status_temp.bits.ovp = 1;
    Modbus_Clean_PowerState(status_temp.byte);
}

uint8_t IF_Power_Get_OCP_TRIPed(void)
{
    return PowerInfo.State.bits.ocp;
}
void IF_Power_OCP_Clean(void)
{
    PowerInfo.State.bits.ocp = 0;
    //
    State_Def status_temp = {0};
    status_temp.bits.ocp = 1;
    Modbus_Clean_PowerState(status_temp.byte);
}



/*
 * @brief
 * @details     输出回读状态
 */
double IF_Power_Read_Volt(void)
{
    int32_t volt_read_int = PowerInfo.Read.volt_read * 10000;
    PowerInfo.Read.volt_read = (float)volt_read_int / 10000.0f;
    return PowerInfo.Read.volt_read;
}


double IF_Power_Read_Volt_Drv(void)
{
    int32_t volt_read_int = PowerInfo.Read.volt_drv_read * 10000;
    PowerInfo.Read.volt_drv_read = (float)volt_read_int / 10000.0f;
    return PowerInfo.Read.volt_drv_read;
}

double IF_Power_Read_Curr(void)
{
    int32_t curr_read_int = PowerInfo.Read.curr_read * 10000;
    PowerInfo.Read.curr_read = (float)curr_read_int / 10000.0f;
    return PowerInfo.Read.curr_read;
}

double IF_Power_Read_Power(void)
{
    PowerInfo.Read.power_read = PowerInfo.Read.volt_read * PowerInfo.Read.curr_read;
    return PowerInfo.Read.power_read;
}

int32_t IF_Power_Read_State(void)
{
    return PowerInfo.State.byte;
}

int32_t IF_Power_Read_Event(void)
{
    return PowerInfo.Event.byte;
}

uint8_t IF_Power_Read_CCCV(void)
{
    return PowerInfo.State.bits.cc;
}


/*
 * @brief
 * @details     校准用
 */
void IF_Power_Calib_ZeroCurrent(uint8_t zeroCurrent)
{
    if(zeroCurrent != 0) {
        PowerInfo.StateBar.bits.zeroCurr = 1;
        Modbus_Calib_ZeroCurr(1);
    } else {
        Modbus_Calib_ZeroCurr(0);
    }
}

void IF_Power_Calib_Voltage(double ture_vol)
{
    Modbus_Calib_Voltage(ture_vol);
    Modbus_Calib_Read_Count();
}
void IF_Power_Calib_Voltage_Drv(double ture_vol)
{
    Modbus_Calib_Voltage_Drv(ture_vol);
}
void IF_Power_Calib_Current(double true_amp)
{
    Modbus_Calib_Current(true_amp);
    Modbus_Calib_Read_Count();
}

void IF_Power_Calib_Delete(const char *str)
{
    if(0 != strstr(str, "VOLT_DRV"))
    {
        Modbus_Calib_Delete_Volt_Drv();
    }
    else
        if(0 != strstr(str, "VOLT"))
        {
            Modbus_Calib_Delete_Volt();
            Modbus_Calib_Delete_Volt_Drv();
        }
        else
            if(0 != strstr(str, "CURR"))
            {
                Modbus_Calib_Delete_Curr();
            }
            else
                if(0 != strstr(str, "ALL"))
                {
                    Modbus_Calib_Delete_Volt();
                    Modbus_Calib_Delete_Volt_Drv();
                    Modbus_Calib_Delete_Curr();
                }
}





void IF_System_Set_Lock(uint8_t lock)
{
    PowerInfo.outSet.lock = (lock != 0);
}

uint8_t IF_System_Get_Lock(void)
{
    return (PowerInfo.outSet.lock != 0);
}



uint16_t IF_Power_Get_Max_Temperture(void)
{
    uint16_t maxTemp = 0;
    switch(PowerInfo.Read.powerCount)
    {
        case MODEL_COUNT_1:
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_1);
            break;
        case MODEL_COUNT_2:
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_1);
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_2);
            break;
        case MODEL_COUNT_3:
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_1);
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_2);
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_3);
            break;
        case MODEL_COUNT_4:
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_1);
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_2);
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_3);
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_4);
            break;
        case MODEL_COUNT_5:
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_1);
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_2);
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_3);
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_4);
            maxTemp = MAX(maxTemp, PowerInfo.Temperature.temp_ext_5);
            break;
    }
    return maxTemp;
}



#include "lwip.h"
/*
 * @brief
 * @details     接口状态
 */
void IF_State_LAN_Connect(uint8_t connect)
{
    __Interface_Lock();
    {
        PowerInfo.StateBar.bits.LAN_Linked = (connect != 0);
        if(connect == 0)
        {
            PowerInfo.StateBar.bits.LAN_10M = 0;
            PowerInfo.StateBar.bits.LAN_100M = 0;
        }
    }
    __Interface_Unlock();
    if(connect) {
        user_printf(__func__, "Ethif LAN Connected ! %#x", __LINE__);
    } else {
        user_printf(__func__, "Ethif LAN Disonnected ! %#x", __LINE__);
    }
}
void IF_State_USB_Device_Connect(uint8_t connect)
{
    PowerInfo.StateBar.bits.USB_Device = (connect != 0);
}

#include "fatfs.h"
#include "listout.h"
#include "delayer.h"
#include "battester.h"
void IF_State_USB_Host_Connect(uint8_t connect)
{
    {
        if(PowerInfo.StateBar.bits.USB_Host != (connect != 0))
        {
            DIR tempDir = {0};
            if(f_opendir(&tempDir, LISTOUT_FILE_USB_PATH) != FR_OK) {
                f_mkdir(LISTOUT_FILE_USB_PATH);
            }
            f_closedir(&tempDir);
            if(f_opendir(&tempDir, DELAYER_FILE_USB_PATH) != FR_OK) {
                f_mkdir(DELAYER_FILE_USB_PATH);
            }
            f_closedir(&tempDir);
            if(f_opendir(&tempDir, BAT_TESTER_FILE_USB_PATH) != FR_OK) {
                f_mkdir(BAT_TESTER_FILE_USB_PATH);
            }
            f_closedir(&tempDir);
        }
        PowerInfo.StateBar.bits.USB_Host = (connect != 0);
    }
}