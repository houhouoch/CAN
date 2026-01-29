#include "GUI_Task.h"
#include "BtnTree.h"
#include "Setting.h"
#include "Modbus_master_IO.h"
#include "Modbus_master_Power.h"
#include "Modbus_master_List.h"
#include "Modbus_master_Delayer.h"

#include "Start_Task.h"
#include "COM_Task.h"
#include "GUI_Task.h"
#include "Debug_Task.h"
#include "USB_TMC_Task.h"
#include "CAN_IRQ_Task.h"
#include "CAN_RX_Task.h"
#include "CAN_TX_Task.h"

#include "TCP_SCPI_Task.h"
#include "WDG_Task.h"

#include "List_Delayer_Interface.h"
#include "BatTester_Interface.h"

/*
 * @brief       void Power_Ext_C_Task(struct _lv_task_t *p)
 * @details     外部控制任务
 */
void Power_Ext_C_Task(struct _lv_task_t *p)
{
    //模拟监测输出
#include "dac.h"
    float dac_Volt = 0;
    float dac_Curr = 0;
    dac_Volt = IF_Power_Read_Volt() * 4096 / PowerInfo.Model.maxVolt  / (3.0f / 2.5f); //(float)(IF_Power_Read_Volt() / PowerInfo.Model.maxVolt) * (3.0f ) * (10.0f / 4)* 4096;
    dac_Curr = IF_Power_Read_Curr() * 4096 / PowerInfo.Model.maxCurr  / (3.3f / 2.5f); //(float)(IF_Power_Read_Curr() / PowerInfo.Model.maxCurr) * (3.0f ) * (10.0f / 4)* 4096.0f;
    if(dac_Volt > 4095) { dac_Volt = 4095; }
    if(dac_Curr > 4095) { dac_Curr = 4095; }
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, ((uint16_t)dac_Volt) & 0xfff);
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, ((uint16_t)dac_Curr) & 0xfff);
    //模拟控制输出
    PowerInfo.extC.voltDAC = dac_Volt;
    PowerInfo.extC.currDAC = dac_Curr;
    switch(IF_Power_Get_Power_Mode())
    {
        case POWER_MODE_EXT_V: {
            IF_Power_Set_OutVolt((float)((uint32_t)(PowerInfo.extC.volt_Set * 1000)) / 1000);
            IF_Power_Set_OutCurr((float)((uint32_t)(PowerInfo.extC.curr_Set * 1000)) / 1000);
            IF_Power_Set_On_Off(PowerInfo.extC.onoff);
        }
        break;
    }
}

static void __USB_Host_Task(struct _lv_task_t *p)
{
    void __USB_Host_Process(void);
    __USB_Host_Process();
    //USB Debug 检测异常
    #if defined(FLASH_DEBUG)
    extern void Debug_Test(void);
    Debug_Test();
    #endif
}


static void __Power_On_Task(struct _lv_task_t *p)
{
    //若系统发生看门狗IWDG复位,则弹窗提示用户
    //并且不会继续开启电源输出(若用户开启上电保持功能)
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDG1RST)) {
        __HAL_RCC_CLEAR_RESET_FLAGS();
        IF_Power_Set_On_Off(0);
        MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_System_WatchDog_Error));
    } else
        if(IF_Power_Get_Power_out() != 0) {
            if(PowerInfo.outSet.onoff != 0) {
                if(IF_Power_Get_Power_Mode() == POWER_MODE_NORMAL) {
                    IF_Power_Set_On_Off(PowerInfo.outSet.onoff);
                    MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, MultStr_GetS(&mStr_OutSet_P_Out));
                } else {
                    IF_Power_Set_On_Off(0);
                }
            }
        }
    //多并机连接检测
    #if defined(MULT_CHANNEL)
    for(uint8_t i = 0; i < (IF_Power_Get_Hw_Channels() + 1); ++i) {
        if(PowerInfo.Read.MultSlaveInfo[i].data_ready == 0) {
            MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Channel_Not_Connected), i);
        }
    }
    #else
    switch(PowerInfo.Read.powerCount) {
        case MODEL_COUNT_5:
            if(PowerInfo.State.bits.model_5_ready == 0) {
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Model_5_Not_Connected));
            }
        case MODEL_COUNT_4:
            if(PowerInfo.State.bits.model_4_ready == 0) {
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Model_4_Not_Connected));
            }
        case MODEL_COUNT_3:
            if(PowerInfo.State.bits.model_3_ready == 0) {
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Model_3_Not_Connected));
            }
        case MODEL_COUNT_2:
            if(PowerInfo.State.bits.model_2_ready == 0) {
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Model_2_Not_Connected));
            }
        case MODEL_COUNT_1:
            break;
    }
    #endif
    //系统设置丢失
    if(PowerInfo.StateBar.bits.eeprom_setting_lost != 0) {
        MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_EEPROM_Settting_Lost));
    }
    //网络初始化失败
    if(PowerInfo.StateBar.bits.network_init_failed != 0) {
        MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Network_Init_Failed));
    }
    //片上存储初始化失败
    if(PowerInfo.StateBar.bits.flash_init_failed != 0) {
        MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Disk_Init_Failed));
    }
    if((PowerInfo.State.bits.noCalib_Volt != 0)
            || (PowerInfo.State.bits.noCalib_Drv != 0)
            || (PowerInfo.State.bits.noCalib_Curr != 0))
    {
        //ADC采样异常
        if(PowerInfo.State.bits.adcDevice_error != 0) {
            MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Power_ADC_Failed));
        }
        //DAC输出异常
        if(PowerInfo.State.bits.dacDevice_error != 0) {
            MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Power_DAC_Failed));
        }
    }
    MX_USB_HOST_Init();
    //打开USB端口输出
    gpio_set(&gpio_USB_Host, 1);
    lv_task_create(__USB_Host_Task, 200, LV_TASK_PRIO_MID, NULL);
}


osThreadId_t GUI_Flush_TaskHandle;
const osThreadAttr_t GUI_Flush_Task_attributes = {
    .name = "GUI_Flush_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal + 1,
};

osMutexId_t GUI_Flush_MutexHandle;
const osMutexAttr_t GUI_Flush_Mutex_attributes = {
    .name = "GUI_Flush_Mutex",
    .attr_bits = osMutexRecursive,
};



osThreadId_t Setting_TaskHandle;
const osThreadAttr_t Setting_Task_attributes = {
    .name = "Setting_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};

void Setting_Task(void *argument)
{
    User_Task_t thisTask = {0};
    if(argument != NULL) {
        thisTask.threadId = osThreadGetId();
        thisTask.attributes = (const osThreadAttr_t *)argument;
    }
    //
    static TickType_t tick = 0;
    tick = xTaskGetTickCount();
    while(1) {
        thisTask = thisTask;
        thisTask.startHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        vTaskDelayUntil(&tick, 10 / portTICK_PERIOD_MS);
        if(PowerInfo.State.bits.acFault == 0) {
            static uint8_t saveCount = 0;
            if(saveCount < 1) {
                saveCount++;
            } else {
                saveCount = 0;
                //
                if(PowerInfo.Read.setchannged != 0) {
                    PowerInfo.Read.setchannged = 0;
                    if(PowerInfo.Read.netApply == 1) {
                        PowerInfo.Read.netApply = 0;
                        //                        MsgBox_Printf(MSG_TYPE_TIPS, NULL, MultStr_GetS(&mStr_NetWork_Apply_Tips), 1);
                    }
                    if(PowerInfo.Read.webchannged != 0) {
                        PowerInfo.Read.webchannged = 0;
                        __Save_WebSet();
                    }
                    if(PowerInfo.Read.groupChannged != 0) {
                        PowerInfo.Read.groupChannged = 0;
                        __Save_Group();
                    }
                    __Save_Setting();
                    __Save_DebugInfo();
                    #if defined(MULT_CHANNEL)
#include "CAN_Table.h"
                    if(IF_Power_Get_Hw_ID() == 0)
                    {
                        static uint8_t lastCAN_Groups = 0xff;
                        uint8_t thisCAN_Groups = IF_Option_Get_CAN_Groups_ID();
                        if(lastCAN_Groups != thisCAN_Groups) {
                            lastCAN_Groups = thisCAN_Groups;
                            uint8_t tx_data[8] = {0};
                            tx_data[0] = thisCAN_Groups;
                            memcpy(&tx_data[2], pfirmwareInfo->uid, sizeof(pfirmwareInfo->uid));
                            CANFD_Send_Extended(CAN_SCPI_GROUP_ID, tx_data);
                        }
                    }
                    #endif
                }
            }
        }
    }
}


#include "MsgList.h"

MsgList_Def *msgList = NULL;
void Task_Create(void)
{
    Normal_Task_Create();
    //USB TMC任务
    USB_TMC_Task_Create();
    //CAN任务
    CAN_IRQ_Task_Create();
    CAN_Rx_Task_Create();
    CAN_Tx_Task_Create();
    //看门狗任务
    WDG_Task_Create();
}


int passwd = 0;
static void __Enter_CalibMode(void)
{
    IF_Option_Input_Passwd(passwd);
    passwd = 0;
}

/*
 * @brief       void __Enter_CalibMode_InputCall(void)
 * @details     弹出模式密码弹窗
 */
void __Enter_CalibMode_InputCall(void)
{
    #if 1
    Data_Edit_Def calibEdit = {0};
    calibEdit.title = MultStr_GetS(&mStr_CalibPasswd_Title);
    calibEdit.stepCount = ARRAY_COUNT(Calib_Passwd_step);
    calibEdit.v_int32.step = Calib_Passwd_step;
    calibEdit.v_int32.pdata = &passwd;
    calibEdit.v_int32.min = 0;
    calibEdit.v_int32.max = 99999;
    calibEdit.fmt = PASSWD_CALIB_FORM;
    InputBox_Call(INPUTBOX_DATA_INT32, __Enter_CalibMode, calibEdit);
    #endif
}


#include "W25qxx.h"
#include "fatfs.h"

uint16_t guiRunTime = 0;

#include "elog.h"
void app_elog_init(void)
{
    elog_init();
    elog_start();
}

static void __Firmware_MultCH_Check (void)
{
    #ifdef MULT_CHANNEL
    if(__isMultChannel() == false) {
    #else
    if(__isMultChannel()) {
    #endif
        lv_obj_t *stepLabel = lv_label_create(lv_scr_act(), NULL);
        lv_obj_set_size(stepLabel, 300, 30);
        lv_obj_align(stepLabel, NULL, LV_ALIGN_CENTER, 0, 50);
        lv_label_set_align(stepLabel, LV_LABEL_ALIGN_CENTER);
        lv_label_set_text(stepLabel, "Invild Firmware!!!");
        extern lv_font_t MultFont_20;
        lv_obj_set_style_local_text_font(stepLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &MultFont_20);
        lv_obj_realign(stepLabel);
        static TickType_t tick = 0;
        while(1) {
            lv_task_handler();
            vTaskDelayUntil(&tick, 10 / portTICK_PERIOD_MS);
            static uint16_t time = 0;
            if(time < 50) {
                time++;
                lv_obj_set_style_local_text_color(stepLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xff0000));
            } else
                if(time < 100) {
                    time++;
                    lv_obj_set_style_local_text_color(stepLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffff00));
                }
                else {
                    time = 0;
                }
        }
    }
}



lv_ui guider_ui;
void GUI_Task(void *argument)
{
    app_elog_init();
    #ifdef NO_GUI
    //内部通讯串口任务
    COM_Task_Create();
    //加载系统设置
    void __Setting_Load(void);
    __Setting_Load();
    //开始其他任务
    Task_Create();
    for(;;) {
        WDG_Task_Feed(WDG_FLAG_GUI);
        //        WDG_Task_Feed(WDG_FLAG_GUI);
        //        lv_task_handler();
        static TickType_t tick = 0;
        vTaskDelayUntil(&tick, 10);
    }
    #endif
    //内部通讯串口任务
    osThreadId_t hreadId_COM = COM_Task_Create();
    //校准模式
#include "Key_Device.h"
    uint8_t key = KEY_DEFAULT;
    uint8_t calibMode_request = 0;
    for(uint8_t i = 0; i < 5; ++i) {
        keyScan(&key);
        if(key == KEY_ID_LOCK) {
            calibMode_request = 1;
            break;
        }
        osDelay(10);
    }
    //LCD LVGL_GUI初始化
    lv_init();              //lvgl 系统初始化
    lv_port_disp_init();    //lvgl 显示接口初始化,放在 lv_init()的后面
    GUI_Flush_MutexHandle = osSemaphoreNew(1, 1, NULL);
    extern void GUI_Flush_Task(void *argument);
    GUI_Flush_TaskHandle = osThreadNew(GUI_Flush_Task, NULL, &GUI_Flush_Task_attributes);
    setup_scr_screen(&guider_ui);
    //
    extern char USERPath[4];   /* USER logical drive path */
    extern FATFS USERFatFS;    /* File system object for USER logical drive */
    extern FIL USERFile;       /* File object for USER */
    //存储器
    {
        //W25QXX初始化 & 挂载
        if(0 != W25QXX_Init()) {
            uint8_t buffer[512] = {0};
            //            f_mkfs(USERPath, FM_FAT, 512, buffer, 512);
            while(FR_OK != f_mount(&USERFatFS, USERPath, 1)) {
                f_mkfs(USERPath, FM_FAT, 512, buffer, 512);
                user_printf(__func__, "W25Q格式化\r\n");
            }
        } else {
            PowerInfo.StateBar.bits.flash_init_failed |= 1;
        }
    }
    lv_obj_t *imgLogo = NULL;
    {
        //
        lv_obj_set_hidden(guider_ui.screen_img_UNIT, 1);
        lv_obj_set_hidden(guider_ui.screen_img_JINGHUI, 1);
        #if defined(VER_UNIT)
        lv_obj_set_style_local_bg_color(guider_ui.screen_cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
        lv_obj_set_hidden(guider_ui.screen_img_UNIT, 0);
        imgLogo = guider_ui.screen_img_UNIT;
        #elif defined(VER_JINGHUI)
        lv_obj_set_style_local_bg_color(guider_ui.screen_cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
        lv_obj_set_hidden(guider_ui.screen_img_JINGHUI, 0);
        imgLogo = guider_ui.screen_img_JINGHUI;
        #endif
        const uint16_t animTime = 1000;
        lv_scr_load(guider_ui.screen);
        //开始显示LOGO
        void btnTree_event_handler(lv_obj_t *obj, lv_event_t event);
        lv_obj_set_event_cb(guider_ui.screen, btnTree_event_handler);
        uint16_t waitTime = 0;
        do {
            lv_task_handler();
            if(waitTime > animTime) {
                if(PowerInfo.StateBar.bits.Power_Link != 0)
                { break; }
            } else {
                waitTime += 10;
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (waitTime * 100 / animTime));
            }
            osDelay(5 / portTICK_PERIOD_MS);
            static uint8_t flag = 0;
            flag = !flag;
            TIM3->CCR1 = (flag) ? 100 : 0;
        } while(1);
        //加载系统设置
        void __Setting_Load(void);
        __Setting_Load();
        #ifdef ZEROCURRENT_EN
        if(IF_Option_Get_ZeroCurrent_AutoEn() != 0) {
            IF_Power_Calib_ZeroCurrent(1);
        }
        #endif
        uint8_t temp_onoff = PowerInfo.outSet.onoff;
        PowerInfo.outSet.onoff = 0;
        Modbus_Update_OutSet();
        PowerInfo.outSet.onoff = temp_onoff;
    }
    #if 0
    __Firmware_MultCH_Check();
    #endif
    //    lv_obj_del(guider_ui.screen_img_UNIT);
    //    guider_ui.screen_img_UNIT = NULL;
    //    lv_obj_del(guider_ui.screen_img_JINGHUI);
    //    guider_ui.screen_img_JINGHUI = NULL;
    lv_obj_set_hidden(imgLogo, 1);
    //初始化页面
    //
    setup_scr_stateBar(&guider_ui);
    lv_obj_set_parent(guider_ui.stateBar_cont, guider_ui.screen);
    extern lv_font_t MultFont_16;
    lv_obj_set_style_local_text_font(guider_ui.stateBar_btn_State_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &MultFont_16);
    lv_obj_set_style_local_text_letter_space(guider_ui.stateBar_btn_State_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 1);
    lv_label_set_recolor(guider_ui.stateBar_btn_State_label, 1);
    {
        UI_Tree_Init();
        InputBox_Create();
        MsgBox_Create();
        //        lv_event_send_refresh_recursive(guider_ui.screen);
    }
    #ifndef DEBUG_CALIBMODE
    if(calibMode_request != 0) {
        __Enter_CalibMode_InputCall();
    }
    #else
    IF_Option_Input_Passwd(1218);
    #endif
    osThreadFlagsSet(hreadId_COM, 0xffffffff);
    lv_task_once(lv_task_create(__Power_On_Task, 5000, LV_TASK_PRIO_MID, NULL));
    void User_Task(struct _lv_task_t *p);
    lv_task_create(User_Task, 20, LV_TASK_PRIO_MID, NULL);
    //外部模拟控制任务
    lv_task_create(Power_Ext_C_Task, 50, LV_TASK_PRIO_MID, NULL);
    //    //数据修改保存任务
    Setting_TaskHandle = osThreadNew(Setting_Task, NULL, &Setting_Task_attributes);
    lv_task_enable(1);
    //开始其他任务
    Task_Create();
    lv_port_indev_init();   //lvgl 输入接口初始化,放在 lv_init()的后面
    //
    static TickType_t tick = 0;
    tick = xTaskGetTickCount();
    MX_RTC_Init();
    for(;;) {
        WDG_Task_Feed(WDG_FLAG_GUI);
        TickType_t runtick[2] = {0};
        runtick[0] = HAL_GetTick();
        lv_task_handler();
        runtick[1] = HAL_GetTick();
        guiRunTime = runtick[1] - runtick[0];
        vTaskDelayUntil(&tick, 10 / portTICK_PERIOD_MS);
        //        lv_tick_inc(10);
        #if 0
        static uint32_t time = 0;
        if(time < 200) {
            time++;
        }
        else {
            time = 0;
            osThreadId_t *threadArray = pvPortMalloc(osThreadGetCount() * sizeof(osThreadId_t) +1);
            if(threadArray != NULL) {
                uint32_t count = osThreadEnumerate(threadArray, osThreadGetCount());
                //                USER_DEBUG(__func__,"%16s\tSpace\tPriority\r\n","ID");
                for(uint32_t i = 0; i < count; ++i) {
                    user_printf(__func__, "%16s\t%d\t%d",
                                osThreadGetName(threadArray[i]), osThreadGetPriority(threadArray[i]),
                                osThreadGetStackSpace(threadArray[i]));
                }
                vPortFree(threadArray);
            }
        }
        #endif
    }
}



static void StateBar_Update(void)
{
    if(guider_ui.stateBar_bar == NULL) { while(1) ; }
    lv_bar_set_range(guider_ui.stateBar_bar, 0, 100);
    lv_bar_set_anim_time(guider_ui.stateBar_bar, 30);
    float present = 0;
    if(ListOut_IsRunning()) {
        lv_obj_set_hidden(guider_ui.stateBar_bar, 0);
        present = (listoutInfo.set.points[IF_List_Get_CurGroups()].time - IF_List_Get_RemainTime()) * 100 / listoutInfo.set.points[IF_List_Get_CurGroups()].time;
        lv_bar_set_value(guider_ui.stateBar_bar, present, LV_ANIM_OFF);
    } else
        if(Delayer_IsRunning()) {
            lv_obj_set_hidden(guider_ui.stateBar_bar, 0);
            present = (delayerInfo.set.points[IF_Delayer_Get_CurGroups()].time - IF_Delayer_Get_RemainTime()) * 100 / delayerInfo.set.points[IF_Delayer_Get_CurGroups()].time;
            lv_bar_set_value(guider_ui.stateBar_bar, present, LV_ANIM_OFF);
        } else
            if(BatTester_IsRunning()) {
                lv_obj_set_hidden(guider_ui.stateBar_bar, 0);
                present = (BatTester.set.points[IF_BatTester_Get_CurGroups()].time - IF_BatTester_Get_RemainTime()) * 100 / BatTester.set.points[IF_BatTester_Get_CurGroups()].time;
                lv_bar_set_value(guider_ui.stateBar_bar, present, LV_ANIM_OFF);
            } else {
                lv_obj_set_hidden(guider_ui.stateBar_bar, 1);
                lv_bar_set_value(guider_ui.stateBar_bar, 0, LV_ANIM_OFF);
            }
    static char buff[128] = {0};
    memset(buff, 0, sizeof(buff));
    //
    if(PowerInfo.StateBar.bits.zeroCurr != 0) {
        strcat(buff, MultStr_GetS(&mStr_Calib_ZeroCurr_Running));
        lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
        return;
    }
    //保护提示
    if(PowerInfo.State.bits.fan_error != 0) {
        char fanStr[32] = "";
        if(PowerInfo.State.bits.fan_error != FAN_MODEL_ERR) {
            sprintf(fanStr, MultStr_GetS(&mStr_Power_FAN_Error), PowerInfo.State.bits.fan_error);
        } else {
            sprintf(fanStr, "%s", MultStr_GetS(&mStr_Power_FAN_Error_Unknow));
        }
        strcat(buff, fanStr);
        lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
        return ;
    } else
        if(PowerInfo.State.bits.font_ocp != 0) {
            strcat(buff, MultStr_GetS(&mStr_Power_Font_OCP));
            lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
            return ;
        } else
            if(PowerInfo.State.bits.ovp != 0) {
                strcat(buff, MultStr_GetS(&mStr_Power_OVP));
                lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
                return ;
            } else
                if(PowerInfo.State.bits.ocp != 0) {
                    strcat(buff, MultStr_GetS(&mStr_Power_OCP));
                    lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
                    return ;
                } else
                    if(PowerInfo.State.bits.otp != 0) {
                        strcat(buff, MultStr_GetS(&mStr_Power_OTP));
                        lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
                        return ;
                    } else
                        if(PowerInfo.State.bits.opp != 0) {
                            strcat(buff, MultStr_GetS(&mStr_Power_OPP));
                            lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
                            return ;
                        } else
                            if(PowerInfo.State.bits.sense != 0) {
                                strcat(buff, MultStr_GetS(&mStr_Power_Over_Offset));
                                lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
                                return ;
                            } else
                                if(PowerInfo.State.bits.pfc_hot != 0) {
                                    strcat(buff, MultStr_GetS(&mStr_Power_PFC_Hot));
                                    lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
                                    return ;
                                } else
                                    if(PowerInfo.State.bits.outOverVolt_error != 0) {
                                        strcat(buff, MultStr_GetS(&mStr_Power_OverVolt));
                                        lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
                                        return ;
                                    }
    //    else if(PowerInfo.State.bits.reverseCurr != 0){
    //        strcat(buff,"电流倒灌警告!");
    //        lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
    //        return ;
    //    }
    if(IF_Option_Get_Beeper() != 0) {
        strcat(buff, " ");
    } else {
        strcat(buff, " ");
    }
    //显示电源最高温度
    if(PowerInfo.workMode == WORK_MODE_ENGINEERING) {
        char temp_str[16] = {0};
        sprintf(buff, "%d℃ ", IF_Power_Get_Max_Temperture());
        strcat(buff, temp_str);
    }
    //显示校准模式图标
    if(PowerInfo.workMode == WORK_MODE_ENGINEERING) {
        strcat(buff, "#ff9600 # ");
    }
    if((PowerInfo.StateBar.bits.LAN_Linked != 0)
            && ((PowerInfo.StateBar.bits.LAN_10M) || (PowerInfo.StateBar.bits.LAN_100M))) {
        uint8_t flag = 0;
        static uint8_t LAN_Count = 0;
        if(LAN_Count > 10) {
            if(PowerInfo.StateBar.bits.LAN_Tx | PowerInfo.StateBar.bits.LAN_Rx) {
                PowerInfo.StateBar.bits.LAN_Tx = 0;
                PowerInfo.StateBar.bits.LAN_Rx = 0;
                LAN_Count = 0;
                flag = 0;
            }
        } else {
            if(LAN_Count > 5) {
                flag = 1;
            }
            LAN_Count++;
        }
        if(flag == 1) {
            strcat(buff, "#00FF00 ");
        } else {
            strcat(buff, "#FFFFFF ");
        }
        strcat(buff, "");
        if(PowerInfo.workMode == WORK_MODE_ENGINEERING) {
            if(PowerInfo.StateBar.bits.LAN_10M) {
                strcat(buff, "10M");
            } else
                if(PowerInfo.StateBar.bits.LAN_100M) {
                    strcat(buff, "100M");
                }
        }
        strcat(buff, "# ");
    }
    PowerInfo.StateBar.bits.LOCK = (IF_System_Get_Lock() != 0);
    gpio_set(&gpio_led_Lock, (IF_System_Get_Lock() != 0));
    if(PowerInfo.StateBar.bits.LOCK != 0) {
        strcat(buff, "#FF0000 # ");
    }
    //    if(PowerInfo.StateBar.bits.USB_Device != 0) {
    //        strcat(buff, " ");
    //    }
    if(PowerInfo.StateBar.bits.USB_Host != 0) {
        strcat(buff, " ");
    }
    if(ListOut_IsRunning() != 0) {
        switch(IF_List_Get_State()) {
            case LIST_STATE_RUNNING:
                strcat(buff, "#00FFFF [List]# ");
                break;
            case LIST_STATE_PAUSE:
                strcat(buff, "#FFFF00 [List]# ");
                break;
        }
    } else
        if(BatTester_IsRunning() != 0) {
            switch(IF_BatTester_Get_State()) {
                case BAT_TESTER_STATE_RUNNING:
                    strcat(buff, "#00FFFF [BatTester]# ");
                    break;
                case BAT_TESTER_STATE_PAUSE:
                    strcat(buff, "#FFFF00 [BatTester]# ");
                    break;
                case BAT_TESTER_STATE_FAILED:
                    strcat(buff, "#FF0000 [BatTester]# ");
                    break;
            }
        } else
            if(Delayer_IsRunning() != 0) {
                switch(IF_Delayer_Get_State()) {
                    case DELAYER_STATE_RUNNING:
                        strcat(buff, "#00FFFF [Delayer]# ");
                        break;
                }
            }
    //    //显示当前输出
    //    static char voltage_str[64] = {0};
    //    double voltage_read = IF_Power_Read_Volt();
    //    __Voltage_Display_Fmt(voltage_str, voltage_read);
    //    static char current_str[64] = {0};
    //    double current_read = IF_Power_Read_Curr();
    //    __Current_Display_Fmt(current_str, current_read);
    //    sprintf(buff, "%s#FF8000 %sV#/#00FFFF %sA#", buff, voltage_str, current_str);
    //
    lv_label_set_text_static(guider_ui.stateBar_btn_State_label, buff);
}



#include "Modbus_master_Power.h"
void User_Task(struct _lv_task_t *p)
{
    static uint8_t language_last = 0xff;
    if(language_last != IF_Option_Get_language()) {
        language_last = IF_Option_Get_language();
        lv_event_send_refresh_recursive(guider_ui.screen);
    }
    StateBar_Update();
    if(PowerInfo.StateBar.bits.zeroCurr != 0) {
        Modbus_Read_Calib_ZeroCurr();
    }
    static State_Def lastState;
    if(lastState.byte != PowerInfo.Event.byte) {
        State_Def state = {0};
        {
            state.byte = lastState.byte ^ PowerInfo.Event.byte;
            state.byte &= PowerInfo.Event.byte;
            //异常处理
            if(state.bits.ovp) {
                IF_Power_Set_On_Off(0);
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Power_OVP));
            }
            if(state.bits.ocp) {
                IF_Power_Set_On_Off(0);
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Power_OCP));
            }
            if(state.bits.otp) {
                IF_Power_Set_On_Off(0);
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Power_OTP));
            }
            if(state.bits.font_ocp) {
                IF_Power_Set_On_Off(0);
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Power_Font_OCP));
            }
            if(state.bits.sense) {
                IF_Power_Set_On_Off(0);
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Power_Over_Offset));
            }
            if(state.bits.pfc_hot) {
                IF_Power_Set_On_Off(0);
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Power_PFC_Hot));
            }
            if(state.bits.acFault) {
                IF_Power_Set_On_Off(0);
                MsgBox_Printf(MSG_TYPE_WARRING, NULL, MultStr_GetS(&mStr_Power_AC_Down));
            }
            if(state.bits.fan_error) {
                IF_Power_Set_On_Off(0);
                if(state.bits.fan_error != FAN_MODEL_ERR) {
                    MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Power_FAN_Error), state.bits.fan_error);
                } else {
                    MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Power_FAN_Error_Unknow));
                }
            }
            //校准数据
            if(state.bits.noCalib_Volt) {
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Calib_Vol_Null));
            }
            if(state.bits.noCalib_Drv) {
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Calib_Vol_Null));
            }
            if(state.bits.noCalib_Curr) {
                MsgBox_Printf(MSG_TYPE_ERROR, NULL, MultStr_GetS(&mStr_Calib_Cur_Null));
            }
            if(state.bits.reverseCurr) {
            }
        }
        lastState.byte = PowerInfo.Event.byte;
    }
}



