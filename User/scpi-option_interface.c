#include "scpi-interface.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "gui_guider.h"

#include "Commd.h"

static const scpi_choice_def_t scpi_powerMode_type_def[] = {
    {"Normal", POWER_MODE_NORMAL},   //普通模式
    {"PARAMaster", POWER_MODE_CAN_PARALLEL_MASTER}, //并联主机
    {"PARASlave", POWER_MODE_CAN_PARALLEL_SLAVE},  //并联从机
    {"SERMaster", POWER_MODE_CAN_SERIES_MASTER},    //串联主机
    {"SERSlave", POWER_MODE_CAN_SERIES_SLAVE},      //串联从机
    {"SYNMaster", POWER_MODE_CAN_SYNERGY_MASTER},          //协同主机
    {"SYNSlave", POWER_MODE_CAN_SYNERGY_SLAVE},          //协同从机
    {"EXT_V", POWER_MODE_EXT_V},
    SCPI_CHOICE_LIST_END /* termination of option list */
};
scpi_result_t Cmd_Set_System_Power_Mode(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    int32_t powerMode = 0;
    if(SCPI_ParamChoice(context, scpi_powerMode_type_def, &powerMode, false)) {
        //        IF_Display_Go_to_Page(PAGE_OUTOPTION);
        IF_Power_Set_Power_Mode(powerMode);
    }
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_System_Power_Mode(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *buffer = "";
    if(!SCPI_ChoiceToName(scpi_powerMode_type_def, IF_Power_Get_Power_Mode(), &buffer) != 0) {
        SCPI_ERR_RETURN;
    } else {
        uint16_t length = strlen(buffer);
        SCPI_ResultCharacters(context, buffer, length);
    }
    return SCPI_RES_OK;
}




scpi_result_t Cmd_Set_System_Slave_ID(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = 1;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = POWER_CAN_ID_MAX;
                break;
            default:
                break;
        }
    }
    //IF_Display_Go_to_Page(PAGE_SETTING);
    IF_Option_Set_Slave_ID(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_System_Slave_ID(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%d", IF_Option_Get_Slave_ID());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}





static const scpi_choice_def_t scpi_powerOut_type_def[] = {
    {"OFF", 0},   //上电保持
    {"KEEP", 1},    //上电关闭
    SCPI_CHOICE_LIST_END /* termination of option list */
};
scpi_result_t Cmd_Set_System_Power_Out(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    int32_t powerOut = 0;
    if(SCPI_ParamChoice(context, scpi_powerOut_type_def, &powerOut, false)) {
        IF_Power_Set_Power_out(powerOut);
    }
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_System_Power_Out(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *buffer = "";
    if(!SCPI_ChoiceToName(scpi_powerOut_type_def, IF_Power_Get_Power_out(), &buffer) != 0) {
        SCPI_ERR_RETURN;
    } else {
        uint16_t length = strlen(buffer);
        SCPI_ResultCharacters(context, buffer, length);
    }
    return SCPI_RES_OK;
}




static const scpi_choice_def_t scpi_eload_type_def[] = {
    {"OFF", ELOAD_OFF},     //关闭
    {"ON", ELOAD_ON},       //常开
    {"AUTO", ELOAD_AUTO},   //自动
    SCPI_CHOICE_LIST_END /* termination of option list */
};
scpi_result_t Cmd_Set_Power_eLoad(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    int32_t eload_onoff = 0;
    if(SCPI_ParamChoice(context, scpi_eload_type_def, &eload_onoff, false)) {
        IF_Power_Set_eLoad(eload_onoff);
    }
    //IF_Display_Go_to_Page(PAGE_SETTING);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Power_eLoad(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *eload_str = "";
    if(!SCPI_ChoiceToName(scpi_eload_type_def, IF_Power_Get_eLoad(), &eload_str) != 0) {
        SCPI_ERR_RETURN;
    }
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%s", eload_str);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}






/*
 * @brief
 * @details
 */
scpi_result_t Cmd_Set_OVP_Delay(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = 10;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = 10000;
                break;
            default:
                break;
        }
    }
    //IF_Display_Go_to_Page(PAGE_SETTING);
    IF_Power_Set_OVP_delayTime(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_OVP_Delay(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%d", IF_Power_Get_OVP_delayTime());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}


/*
 * @brief
 * @details
 */
scpi_result_t Cmd_Set_OCP_Delay(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = 10;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = 10000;
                break;
            default:
                break;
        }
    }
    //IF_Display_Go_to_Page(PAGE_SETTING);
    IF_Power_Set_OCP_delayTime(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_OCP_Delay(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%d", IF_Power_Get_OCP_delayTime());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}





//远程控制模式
scpi_result_t Cmd_Set_System_Remote_Mode(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    IF_System_Set_Remote(1);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Set_System_Local_Mode(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    IF_System_Set_Remote(0);
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Set_System_Lock(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    IF_System_Set_Lock(1);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Set_System_UnLock(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    IF_System_Set_Lock(0);
    return SCPI_RES_OK;
}


//用户组
scpi_result_t Cmd_Set_System_Groups(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //IF_Display_Go_to_Page(PAGE_SETTING);
    IF_Option_Set_Groups(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_System_Groups(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%d", IF_Option_Get_Groups());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}




//蜂鸣器
scpi_result_t Cmd_Set_Beeper(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_bool_t param1 = {0};
    if(!SCPI_ParamBool(context, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //IF_Display_Go_to_Page(PAGE_SETTING);
    IF_Option_Set_Beeper(param1);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Beeper(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *beeper_str = "";
    if(!SCPI_ChoiceToName(scpi_bool_def, IF_Option_Get_Beeper(), &beeper_str) != 0) {
        SCPI_ERR_RETURN;
    }
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%s", beeper_str);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_System_Beeper(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    BEEP_ONE();
    return SCPI_RES_OK;
}

//散热器
scpi_result_t Cmd_System_Set_FAN(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param1 = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param1.special != FALSE) {
        switch(param1.content.tag) {
            case SCPI_NUM_MIN:
                param1.content.value = 25;
                break;
            case SCPI_NUM_MAX:
                param1.content.value = 100;
                break;
            default:
                break;
        }
    }
    //IF_Display_Go_to_Page(PAGE_SETTING);
    IF_Power_Set_MaxFan(param1.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_System_Read_FAN(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%03d", IF_Power_Get_MaxFan());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}



//系统语言
static const scpi_choice_def_t scpi_language_type_def[] = {
    {"CH", LANGUAGE_CH},   //简体中文
    {"EN", LANGUAGE_EN}, //英文
    SCPI_CHOICE_LIST_END /* termination of option list */
};
scpi_result_t Cmd_System_Set_Language(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    int32_t language = 0;
    if(SCPI_ParamChoice(context, scpi_language_type_def, &language, false)) {
        //        IF_Display_Go_to_Page(PAGE_OUTOPTION);
        IF_Option_Set_language(language);
    }
    return SCPI_RES_OK;
}
scpi_result_t Cmd_System_Read_Language(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *buffer = "";
    if(!SCPI_ChoiceToName(scpi_language_type_def, IF_Option_Get_language(), &buffer) != 0) {
        SCPI_ERR_RETURN;
    } else {
        uint16_t length = strlen(buffer);
        SCPI_ResultCharacters(context, buffer, length);
    }
    return SCPI_RES_OK;
}

//掉电检测
scpi_result_t Cmd_Set_Power_Down(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_bool_t param1 = {0};
    if(!SCPI_ParamBool(context, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //IF_Display_Go_to_Page(PAGE_SETTING);
    IF_Power_Set_Power_Down(param1);
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Read_Power_Down(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *beeper_str = "";
    if(!SCPI_ChoiceToName(scpi_bool_def, IF_Power_Get_Power_Down(), &beeper_str) != 0) {
        SCPI_ERR_RETURN;
    }
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%s", beeper_str);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}
//屏幕背光
scpi_result_t Cmd_Set_Brightness(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_number_t param_brightness = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_brightness, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(param_brightness.special != FALSE) {
        switch(param_brightness.content.tag) {
            case SCPI_NUM_MIN:
                param_brightness.content.value = 20;
                break;
            case SCPI_NUM_MAX:
                param_brightness.content.value = 100;
                break;
        }
    }
    //IF_Display_Go_to_Page(PAGE_SETTING);
    IF_Option_Set_Brightness(param_brightness.content.value);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_Brightness(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%d", IF_Option_Get_Brightness());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}




//保存网络设置
scpi_result_t Cmd_Set_LAN_Apply(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    //IF_Display_Go_to_Page(PAGE_SETTING);
    //IF_System_LAN_Apply();
    IF_Option_NetWork_Set_Apply();
    return SCPI_RES_OK;
}

//DHCP
scpi_result_t Cmd_Set_LAN_DHCP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    scpi_bool_t param1 = {0};
    if(!SCPI_ParamBool(context, &param1, FALSE)) {
        SCPI_ERR_RETURN;
    }
    //IF_Display_Go_to_Page(PAGE_SETTING);
    IF_Option_NetWork_Set_DHCP(param1);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_LAN_DHCP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    const char *beeper_str = "";
    if(!SCPI_ChoiceToName(scpi_bool_def, IF_Option_NetWork_Get_DHCP_Now(), &beeper_str) != 0) {
        SCPI_ERR_RETURN;
    }
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%s", beeper_str);
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}


//IP地址
scpi_result_t Cmd_Set_LAN_IP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char IP_str[64] = {0};
    size_t IP_len = 0;
    if(!SCPI_ParamCopyText(context, IP_str, sizeof(IP_str) - 1, &IP_len, FALSE)) {
        SCPI_ERR_RETURN;
    }
    int ip_addr[4] = {0};
    uint8_t valibCount = sscanf(IP_str, "%d.%d.%d.%d", &ip_addr[0], &ip_addr[1], &ip_addr[2], &ip_addr[3]);
    if(valibCount == 4) {
        //IF_Display_Go_to_Page(PAGE_SETTING);
        IF_Option_NetWork_Set_IP(0, ip_addr[0]);
        IF_Option_NetWork_Set_IP(1, ip_addr[1]);
        IF_Option_NetWork_Set_IP(2, ip_addr[2]);
        IF_Option_NetWork_Set_IP(3, ip_addr[3]);
    }
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Read_LAN_IP(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[64] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%d.%d.%d.%d",
                          IF_Option_NetWork_Get_IP_Now(0), IF_Option_NetWork_Get_IP_Now(1),
                          IF_Option_NetWork_Get_IP_Now(2), IF_Option_NetWork_Get_IP_Now(3));
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}

//网关
scpi_result_t Cmd_Set_LAN_GateWay(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char GW_str[64] = {0};
    size_t GW_len = 0;
    if(!SCPI_ParamCopyText(context, GW_str, sizeof(GW_str) - 1, &GW_len, FALSE)) {
        SCPI_ERR_RETURN;
    }
    int gateway_addr[4] = {0};
    uint8_t valibCount = sscanf(GW_str, "%d.%d.%d.%d", &gateway_addr[0], &gateway_addr[1], &gateway_addr[2], &gateway_addr[3]);
    if(valibCount == 4) {
        //IF_Display_Go_to_Page(PAGE_SETTING);
        IF_Option_NetWork_Set_Gateway(0, gateway_addr[0]);
        IF_Option_NetWork_Set_Gateway(1, gateway_addr[1]);
        IF_Option_NetWork_Set_Gateway(2, gateway_addr[2]);
        IF_Option_NetWork_Set_Gateway(3, gateway_addr[3]);
    }
    return SCPI_RES_OK;
}

scpi_result_t Cmd_Read_LAN_GateWay(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[64] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%d.%d.%d.%d",
                          IF_Option_NetWork_Get_Gateway_Now(0), IF_Option_NetWork_Get_Gateway_Now(1),
                          IF_Option_NetWork_Get_Gateway_Now(2), IF_Option_NetWork_Get_Gateway_Now(3));
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}

//子网掩码
scpi_result_t Cmd_Set_LAN_NetMask(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char NetMask_str[64] = {0};
    size_t NetMask_len = 0;
    if(!SCPI_ParamCopyText(context, NetMask_str, sizeof(NetMask_str) - 1, &NetMask_len, FALSE)) {
        SCPI_ERR_RETURN;
    }
    int netmask_addr[4] = {0};
    uint8_t valibCount = sscanf(NetMask_str, "%d.%d.%d.%d", &netmask_addr[0], &netmask_addr[1], &netmask_addr[2], &netmask_addr[3]);
    if(valibCount == 4) {
        //IF_Display_Go_to_Page(PAGE_SETTING);
        IF_Option_NetWork_Set_NetMask(0, netmask_addr[0]);
        IF_Option_NetWork_Set_NetMask(1, netmask_addr[1]);
        IF_Option_NetWork_Set_NetMask(2, netmask_addr[2]);
        IF_Option_NetWork_Set_NetMask(3, netmask_addr[3]);
    }
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_LAN_NetMask(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    //
    char buffer[64] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%d.%d.%d.%d",
                          IF_Option_NetWork_Get_NetMask_Now(0), IF_Option_NetWork_Get_NetMask_Now(1),
                          IF_Option_NetWork_Get_NetMask_Now(2), IF_Option_NetWork_Get_NetMask_Now(3));
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}


/*
 * @brief       scpi_result_t Cmd_Set_SN(scpi_t *context)
 * @details     写入SN号
 */
scpi_result_t Cmd_Set_SN(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    char SN_str[20] = {0};
    size_t SN_len = 0;
    if(!SCPI_ParamCopyText(context, SN_str, sizeof(SN_str) - 1, &SN_len, FALSE)) {
        SCPI_ERR_RETURN;
    }
    if(strlen(SN_str) != 13) {
        SCPI_ERR_RETURN;
    }
    #if defined(MULT_CHANNEL)
    //将SN号发送给下属的子模块
    if(IF_Power_Get_Hw_ID() == 0) {
        for(uint8_t i = 1; i < (IF_Power_Get_Hw_Channels() + 1); ++i) {
            uint8_t id = i;
            __CAN_SCPI_Lock();
            {
                char cmdBuffer[64] = {0};
                strncat(cmdBuffer, context->param_list.cmd_raw.data, context->param_list.cmd_raw.length);
                strcat(cmdBuffer, " ");
                strncat(cmdBuffer, context->param_list.lex_state.buffer, context->param_list.lex_state.len);
                if(NULL == strstr(cmdBuffer, "\n")) {
                    strcat(cmdBuffer, "\r\n");
                }
                //
                uint8_t returnBuf[256] = {0};
                uint16_t reeturnBufLen = 0;
                CAN_Rx_RequestQueue_t requestQueue = {0};
                requestQueue.canId = id;
                requestQueue.pdata = returnBuf;
                requestQueue.len = &reeturnBufLen;
                requestQueue.threadID = osThreadGetId();
                CAN_RX_RequestQueue_Send(&requestQueue);
                //
                uint8_t canGroup_ID = IF_Option_Get_CAN_Groups_ID();
                id |= (canGroup_ID << 4);
                user_printf(__func__, "发送指令 %2x:|%s|\r\n", id, cmdBuffer);
                extern uint8_t __CAN_Send_MultPack_M2S(uint8_t id, const uint8_t *pdata, uint8_t len);
                __CAN_Send_MultPack_M2S(id, (const uint8_t *)cmdBuffer, strlen(cmdBuffer));
                uint8_t scpi_result = 0, scpi_cmderror = 0;
                //等待回复
                if(true == CAN_RX_RequestQueue_Wait(500)) {
                } else {
                    user_printf(__func__, "接收超时\r\n");
                }
            }
            __CAN_SCPI_Unlock();
        }
    }
    #endif
    //写入SN
    IF_Option_Write_SN(SN_str);
    __Save_About();
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Read_SN(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    __Load_About();
    char buffer[32] = "";
    int length = snprintf(buffer, sizeof(buffer) - 1, "%s", IF_Option_Read_SN());
    SCPI_ResultCharacters(context, buffer, length);
    return SCPI_RES_OK;
}
scpi_result_t Cmd_Input_Password(scpi_t *context) {
    __MULT_SCPI_ADDR_CHECK();
    scpi_number_t param_passwd = {0};
    if(!SCPI_ParamNumber(context, scpi_special_numbers_def, &param_passwd, FALSE)) {
        SCPI_ERR_RETURN;
    }
    IF_Option_Input_Passwd((uint32_t)param_passwd.content.value);
    return SCPI_RES_OK;
}

