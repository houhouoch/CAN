#include "lwip/debug.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "fs.h"
#include <string.h>
#include <stdlib.h>


#include "Commd.h"
#include "MultString.h"
#include "Interface.h"
#include "option_Interface.h"
#include "usbd_desc_tmc.h"

#include "httpd_itemcb.c"






typedef struct {
    const char *tag;
    //显示
    const MultStr_Def *readMultS;
    void (*rwCb)(char *str, uint8_t rw);
} Http_Tag_Item_Def;



Http_Tag_Item_Def Http_Tag_Table[] = {
    {
        .tag = "cgi_login_out",
        .rwCb = __CGI_Login_out,
    }, {
        .tag = "cgi_scpi",
        .rwCb = __CGI_SCPI,
    }, {
        .tag = "cgi_result",
        .rwCb = __CGI_Result,
    }, {
        .tag = "cgi_read",
        .rwCb = __CGI_Read,
    },

    //
    {
        .tag = "aboutModel",
        .rwCb = __About_Model,
    }, {
        .tag = "aboutSerialNum",
        .rwCb = __About_SerialNum,
    }, {
        .tag = "aboutManufacturer",
        .rwCb = __About_Manufacturer,
    }, {
        .tag = "aboutFirmVersion",
        .rwCb = __About_FirmVersion,
    }, {
        .tag = "aboutPowerVersion",
        .rwCb = __About_PowerVersion,
    }, {
        .tag = "aboutDescription",
        .rwCb = __About_Description,
    },


    {
        .tag = "netHostName",
        .rwCb = __Net_HostName,
    }, {
        .tag = "netDHCP",
        .rwCb = __Net_DHCP,
    }, {
        .tag = "netIPv4",
        .rwCb = __Net_IPv4,
    }, {
        .tag = "netNetMask",
        .rwCb = __Net_NetMask,
    }, {
        .tag = "netGateWay",
        .rwCb = __Net_GateWay,
    }, {
        .tag = "netMAC",
        .rwCb = __Net_MAC,
    }, {
        .tag = "visaLANProtocol",
        .rwCb = __visa_LAN_Protocol,
    }, {
        .tag = "visaSocketProtocol",
        .rwCb = __visa_Socket_Protocol,
    }, {
        .tag = "visaUSBProtocol",
        .rwCb = __visa_USB_Protocol,
    },



    #if 1
    //标签
    {
        .tag = "lb_Power_Model",
        .rwCb = __PowerModel,
    }, {
        .tag = "MfrUrl",
        .readMultS = &mStr_MfrUrl,
    }, {
        .tag = "lb_Home_Title",
        .readMultS = &mStr_Home,
    }, {
        .tag = "lb_Home_OutVol",
        .readMultS = &mStr_home_OutVol,
    }, {
        .tag = "lb_Home_OutCur",
        .readMultS = &mStr_home_OutCur,
    }, {
        .tag = "lb_Home_OutPower",
        .readMultS = &mStr_home_OutPower,
    }, {
        .tag = "lb_Home_OutState",
        .readMultS = &mStr_home_OutState,
    }, {
        .tag = "lb_Home_Set",
        .readMultS = &mStr_home_Set,
    }, {
        .tag = "lb_Home_Limit",
        .readMultS = &mStr_home_Limit,
    }, {
        .tag = "lb_Home_OnOff",
        .readMultS = &mStr_home_Out_OnOff,
    },

    //List
    {
        .tag = "lb_List_Title",
        .readMultS = &mStr_List,
    }, {
        .tag = "lb_List_curState",
        .readMultS = &mStr_list_lab_Status,
    }, {
        .tag = "lb_List_curIdx",
        .readMultS = &mStr_list_lab_nowIdx,
    }, {
        .tag = "lb_List_curLoop",
        .readMultS = &mStr_list_lab_Loop,
    }, {
        .tag = "lb_List_Start",
        .readMultS = &mStr_list_lab_Start,
    }, {
        .tag = "lb_List_Count",
        .readMultS = &mStr_list_lab_Groups,
    }, {
        .tag = "lb_List_Cycle",
        .readMultS = &mStr_list_lab_Cycles,
    }, {
        .tag = "lb_List_endState",
        .readMultS = &mStr_list_lab_EndState,
    }, {
        .tag = "lb_List_OnOff",
        .readMultS = &mStr_list_lab_ONOFF,
    }, {
        .tag = "lb_List_No",
        .readMultS = &mStr_list_No,
    }, {
        .tag = "lb_List_Voltage",
        .readMultS = &mStr_list_Voltage,
    }, {
        .tag = "lb_List_Current",
        .readMultS = &mStr_list_Current,
    }, {
        .tag = "lb_List_Time",
        .readMultS = &mStr_list_keepT,
    },





    //Delayer
    {
        .tag = "lb_Delayer_Title",
        .readMultS = &mStr_Delayer,
    }, {
        .tag = "lb_Delayer_curState",
        .readMultS = &mStr_delayer_lab_Status,
    }, {
        .tag = "lb_Delayer_curIdx",
        .readMultS = &mStr_delayer_lab_nowIdx,
    }, {
        .tag = "lb_Delayer_curLoop",
        .readMultS = &mStr_delayer_lab_Loop,
    }, {
        .tag = "lb_Delayer_Start",
        .readMultS = &mStr_delayer_lab_Start,
    }, {
        .tag = "lb_Delayer_Count",
        .readMultS = &mStr_delayer_lab_Groups,
    }, {
        .tag = "lb_Delayer_Cycle",
        .readMultS = &mStr_delayer_lab_Cycles,
    }, {
        .tag = "lb_Delayer_endState",
        .readMultS = &mStr_delayer_lab_EndState,
    }, {
        .tag = "lb_Delayer_OnOff",
        .readMultS = &mStr_delayer_lab_ONOFF,
    }, {
        .tag = "lb_Delayer_Stop",
        .readMultS = &mStr_delayer_lab_stopRule,
    }, {
        .tag = "lb_Delayer_No",
        .readMultS = &mStr_delayer_No,
    }, {
        .tag = "lb_Delayer_OutState",
        .readMultS = &mStr_delayer_State,
    }, {
        .tag = "lb_Delayer_Time",
        .readMultS = &mStr_delayer_dT,
    },





    //通用
    {
        .tag = "lb_btn_Operation",
        .readMultS = &mStr_Web_Operation,
    }, {
        .tag = "lb_btn_Sumbit",
        .readMultS = &mStr_Web_Submit,
    }, {
        .tag = "lb_btn_SumbitAll",
        .readMultS = &mStr_Web_SubmitAll,
    }, {
        .tag = "lb_btn_Refresh",
        .readMultS = &mStr_Web_Refresh,
    }, {
        .tag = "lb_btn_Next",
        .readMultS = &mStr_Web_Next,
    }, {
        .tag = "lb_btn_Previous",
        .readMultS = &mStr_Web_Previous,
    }, {
        .tag = "webControl",
        .rwCb = __Web_Control,
    }, {
        .tag = "webSwitchPage",
        .rwCb = __Web_SwitchPage,
    },




    //参数
    {
        .tag = "curVol",
        .rwCb = __Current_Volt,
    }, {
        .tag = "curCur",
        .rwCb = __Current_Curr,
    }, {
        .tag = "curWat",
        .rwCb = __Current_Power,
    }, {
        .tag = "curDvm",
        .rwCb = __Current_Dvm,
    }, {
        .tag = "curState",
        .rwCb = __Current_State,
    }, {
        .tag = "setVol",
        .rwCb = __OutVolt,
    }, {
        .tag = "setCur",
        .rwCb = __OutCurr,
    }, {
        .tag = "setTimeOFF_onoff",
        .rwCb = __Out_TimeOff_OnOff,
    }, {
        .tag = "setTimeOFF",
        .rwCb = __Out_TimeOff,
    }, {
        .tag = "setOVP_on",
        .rwCb = __Out_OVP,
    }, {
        .tag = "setOVP",
        .rwCb = __Out_OVP_Value,
    }, {
        .tag = "setOCP_on",
        .rwCb = __Out_OCP,
    }, {
        .tag = "setOCP",
        .rwCb = __Out_OCP_Value,
    }, {
        .tag = "setOnoff",
        .rwCb = __Out_OnOff,
    }, {
        .tag = "maxVol",
        .rwCb = __Out_Volt_Max,
    }, {
        .tag = "maxCur",
        .rwCb = __Out_Curr_Max,
    }, {
        .tag = "maxWat",
        .rwCb = __Out_Power_Max,
    },


    //List
    {
        .tag = "listJson",
        .rwCb = _List_JSON,
    }, {
        .tag = "listcurIdx",
        .rwCb = _List_Current_Groups,
    }, {
        .tag = "listcurLoop",
        .rwCb = _List_Current_Cycled,
    }, {
        .tag = "listcurState",
        .rwCb = _List_Current_State,
    }, {
        .tag = "listStart",
        .rwCb = _List_Start,
    }, {
        .tag = "listCount",
        .rwCb = _List_Count,
    }, {
        .tag = "listCycle",
        .rwCb = _List_Cycle,
    }, {
        .tag = "listEndState",
        .rwCb = _List_EndState,
    }, {
        .tag = "listmaxCount",
        .rwCb = _List_MaxCount,
    }, {
        .tag = "listonoff",
        .rwCb = _List_Onoff,
    },



    //Delayer
    {
        .tag = "delayerJson",
        .rwCb = _Delayer_JSON,
    }, {
        .tag = "delayercurIdx",
        .rwCb = _Delayer_Current_Groups,
    }, {
        .tag = "delayercurLoop",
        .rwCb = _Delayer_Current_Cycled,
    }, {
        .tag = "delayercurState",
        .rwCb = _Delayer_Current_State,
    }, {
        .tag = "delayerStart",
        .rwCb = _Delayer_Start,
    }, {
        .tag = "delayerCount",
        .rwCb = _Delayer_Count,
    }, {
        .tag = "delayerCycle",
        .rwCb = _Delayer_Cycle,
    }, {
        .tag = "delayerEndState",
        .rwCb = _Delayer_EndState,
    }, {
        .tag = "delayermaxCount",
        .rwCb = _Delayer_MaxCount,
    }, {
        .tag = "delayeronoff",
        .rwCb = _Delayer_Onoff,
    },
    #endif
};

static const char *ppcTAGs[sizeof(Http_Tag_Table) / sizeof(Http_Tag_Table[0]) + 1] = {0};


//SSI的Handler句柄
static uint16_t SSIHandler(int i, char *pcInsert, int iInsertLen)
{
    if(i < ARRAY_COUNT(Http_Tag_Table)) {
        if(Http_Tag_Table[i].readMultS != NULL) {
            snprintf(pcInsert, iInsertLen, "%s", MultStr_GetS(Http_Tag_Table[i].readMultS));
        } else
            if(Http_Tag_Table[i].rwCb != NULL) {
                Http_Tag_Table[i].rwCb(pcInsert, __HTTPD_READ);
            }
    }
    return strlen(pcInsert);
}

//SSI句柄初始化
void httpd_ssi_init(void)
{
    uint16_t tagCount = 0;
    for(uint16_t i = 0; i < ARRAY_COUNT(Http_Tag_Table); ++i) {
        if(i < ARRAY_COUNT(ppcTAGs)) {
            tagCount++;
            ppcTAGs[i] = Http_Tag_Table[i].tag;
        }
    }
    //配置SSI句柄
    http_set_ssi_handler((tSSIHandler)SSIHandler, ppcTAGs, (sizeof(ppcTAGs) / sizeof(ppcTAGs[0])));
}




/* -------------------------------------------------------------------------- */

#if (LWIP_HTTPD_CGI == 1)


void Web_InputCookie(uint32_t cookieID) {
    PowerInfo.Read.cookieID = cookieID;
}

void Web_CleanCookie(void) {
    IF_Option_Set_localCookieID(0);
}

/**
  * @brief
  * @retval Web端口输入SCPI指令
  */
const char *SCPI_Write_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
        for(uint16_t i = 0; i < iNumParams; ++i)
        {
            for(uint16_t idx = 0; idx < ARRAY_COUNT(Http_Tag_Table); ++idx) {
                if(0 == strcmp(pcParam[i], Http_Tag_Table[idx].tag)) {
                    if(Http_Tag_Table[idx].rwCb != NULL) {
                        Http_Tag_Table[idx].rwCb(pcValue[i], __HTTPD_WRITE);
                    }
                    break;
                }
            }
        }
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"OK\"}");
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
    }
    return "/cgi_result.shtml";
}
const char *SCPI_Read_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    return "/cgi_scpi.shtml";
}





/**
  * @brief
  * @retval 用户登录
  */
const char *SCPI_Login_in_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    if(iNumParams <= 0) {
    } else {
        for(uint16_t i = 0; i < iNumParams; ++i) {
            if(0 == strcmp(pcParam[i], "pass")) {
                uint32_t inputPasswd = 0;
                sscanf(pcValue[i], "%d", &inputPasswd);
                uint32_t webPasswd = IF_Option_NetWork_Get_WebPasswd();
                //对比密码
                if(inputPasswd == webPasswd) {
                    //随机生成Cookie ID
                    uint32_t cookiedID = 0;
                    HAL_RNG_GenerateRandomNumber(&hrng, &cookiedID);
                    IF_Option_Set_localCookieID(cookiedID);
                    sprintf(httpd_scpi.result_buffer, "{ \"status\":\"OK\",\"code\":\"%#X\"}", IF_Option_Get_localCookieID());
                } else {
                    sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
                }
            }
        }
    }
    return "/cgi_result.shtml";
}
const char *SCPI_Login_out_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    Web_CleanCookie();
    sprintf(httpd_scpi.result_buffer, "{ \"status\":\"OK\"}");
    return "/cgi_result.shtml";
}



const char *NetWork_Set_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
        for(uint16_t i = 0; i < iNumParams; ++i)
        {
            for(uint16_t idx = 0; idx < ARRAY_COUNT(Http_Tag_Table); ++idx) {
                if(0 == strcmp(pcParam[i], Http_Tag_Table[idx].tag)) {
                    if(Http_Tag_Table[idx].rwCb != NULL) {
                        Http_Tag_Table[idx].rwCb(pcValue[i], __HTTPD_WRITE);
                    }
                    break;
                }
            }
        }
        if(iNumParams > 0) {
            IF_Option_NetWork_Set_Apply();
        }
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"OK\"}");
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
    }
    return "/cgi_result.shtml";
}

//修改密码
const char *Web_PasswdChange_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    uint8_t passwdTrue = 0;
    uint32_t passwd_new = 0;
    for(uint16_t i = 0; i < iNumParams; ++i)
    {
        if(0 == strcmp(pcParam[i], "oldPasswd")) {
            uint32_t passwd_old = 0;
            if(1 == sscanf(pcValue[i], "%d", &passwd_old)) {
                if(passwd_old == IF_Option_NetWork_Get_WebPasswd()) {
                    passwdTrue |= (1 << 0);
                }
            }
        } else
            if(0 == strcmp(pcParam[i], "newPasswd")) {
                if(1 == sscanf(pcValue[i], "%d", &passwd_new)) {
                    passwdTrue |= (1 << 1);
                }
            }
    }
    if(passwdTrue == 3) {
        IF_Option_NetWork_Set_WebPasswd(passwd_new);
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"OK\"}");
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\"}");
    }
    return "/cgi_result.shtml";
}


#if (LWIP_HTTPD_CGI == 1)
const char *POWER_READ_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    return "/powerRead.shtml";
}


const char *OUTPUT_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    //
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    //
    if(iNumParams <= 0) { return "/404.html"; }
    for(uint16_t i = 0; i < iNumParams; ++i)
    {
        for(uint16_t idx = 0; idx < ARRAY_COUNT(Http_Tag_Table); ++idx) {
            if(0 == strcmp(pcParam[i], Http_Tag_Table[idx].tag)) {
                if(Http_Tag_Table[idx].rwCb != NULL) {
                    Http_Tag_Table[idx].rwCb(pcValue[i], __HTTPD_WRITE);
                }
                break;
            }
        }
    }
    return "/powerRead.shtml";
}

const char *ONOFF_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    //
    if(iNumParams <= 0) {
        IF_Power_Set_On_Off(0);
        return "/powerRead.shtml";
    }
    for(uint16_t i = 0; i < iNumParams; ++i)
    {
        for(uint16_t idx = 0; idx < ARRAY_COUNT(Http_Tag_Table); ++idx) {
            if(0 == strcmp(pcParam[i], Http_Tag_Table[idx].tag)) {
                if(Http_Tag_Table[idx].rwCb != NULL) {
                    Http_Tag_Table[idx].rwCb(pcValue[i], __HTTPD_WRITE);
                }
                break;
            }
        }
    }
    return "/powerRead.shtml";
}


const char *WebControl_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    //
    for(uint16_t i = 0; i < iNumParams; ++i)
    {
        for(uint16_t idx = 0; idx < ARRAY_COUNT(Http_Tag_Table); ++idx) {
            if(0 == strcmp(pcParam[i], Http_Tag_Table[idx].tag)) {
                if(Http_Tag_Table[idx].rwCb != NULL) {
                    Http_Tag_Table[idx].rwCb(pcValue[i], __HTTPD_WRITE);
                }
                break;
            }
        }
    }
    return "/index.shtml";
}
#endif

const char *PowerRead_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    return "/cgi_read.shtml";
}

const char *ListRead_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    return "/ListState.shtml";
}
const char *DelayerRead_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    return "/DelayerState.shtml";
}

const char *ListData_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    if(iNumParams > 0)
    {
        for(uint16_t i = 0; i < iNumParams; ++i)
        {
            printf("%s %s : \r\n%s \r\n", "参数", pcParam[i], pcValue[i]);
            //请求获取数据
            if(0 == strcmp(pcParam[i], "listRequestIdx")) {
                int param = 0;
                if(sscanf(pcValue[i], "%d", &param) == 1) {
                    listJson_Info.tran.idx = param;
                } else {
                    listJson_Info.tran.idx = 0;
                }
            } else
                if(0 == strcmp(pcParam[i], "listRequestCount")) {
                    int param = 0;
                    if(sscanf(pcValue[i], "%d", &param) == 1) {
                        listJson_Info.tran.count = param;
                        List_to_JSON();
                    }
                }
        }
    }
    return "/ListData.shtml";
}

const char *ListOut_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    for(uint16_t i = 0; i < iNumParams; ++i)
    {
        for(uint16_t idx = 0; idx < ARRAY_COUNT(Http_Tag_Table); ++idx) {
            if(0 == strcmp(pcParam[i], Http_Tag_Table[idx].tag)) {
                if(Http_Tag_Table[idx].rwCb != NULL) {
                    Http_Tag_Table[idx].rwCb(pcValue[i], __HTTPD_WRITE);
                }
                break;
            }
        }
    }
    return "/ListPage.shtml";
}

const char *ListOnoff_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    if(iNumParams <= 0) {
        IF_List_Set_State(LIST_STATE_STOP);
        return "/ListPage.shtml";
    }
    for(uint16_t i = 0; i < iNumParams; ++i)
    {
        for(uint16_t idx = 0; idx < ARRAY_COUNT(Http_Tag_Table); ++idx) {
            if(0 == strcmp(pcParam[i], Http_Tag_Table[idx].tag)) {
                if(Http_Tag_Table[idx].rwCb != NULL) {
                    Http_Tag_Table[idx].rwCb(pcValue[i], __HTTPD_WRITE);
                }
                break;
            }
        }
    }
    return "/ListPage.shtml";
}








const char *DelayerData_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    if(iNumParams > 0)
    {
        for(uint16_t i = 0; i < iNumParams; ++i)
        {
            printf("%s %s : \r\n%s \r\n", "参数", pcParam[i], pcValue[i]);
            //请求获取数据
            if(0 == strcmp(pcParam[i], "delayerRequestIdx")) {
                int param = 0;
                if(sscanf(pcValue[i], "%d", &param) == 1) {
                    delayerJson_Info.tran.idx = param;
                } else {
                    delayerJson_Info.tran.idx = 0;
                }
            } else
                if(0 == strcmp(pcParam[i], "delayerRequestCount")) {
                    int param = 0;
                    if(sscanf(pcValue[i], "%d", &param) == 1) {
                        delayerJson_Info.tran.count = param;
                        Delayer_to_JSON();
                    }
                }
        }
    }
    return "/DelayerData.shtml";
}

const char *DelayerOut_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    for(uint16_t i = 0; i < iNumParams; ++i)
    {
        for(uint16_t idx = 0; idx < ARRAY_COUNT(Http_Tag_Table); ++idx) {
            if(0 == strcmp(pcParam[i], Http_Tag_Table[idx].tag)) {
                if(Http_Tag_Table[idx].rwCb != NULL) {
                    Http_Tag_Table[idx].rwCb(pcValue[i], __HTTPD_WRITE);
                }
                break;
            }
        }
    }
    return "/DelayerPage.shtml";
}

const char *DelayerOnoff_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if((IF_Option_Get_localCookieID() != 0) && (PowerInfo.Read.cookieID == IF_Option_Get_localCookieID())) {
    } else {
        sprintf(httpd_scpi.result_buffer, "{ \"status\":\"ERR\",\"code\":\"%s\"}", "AUTH");
        return "/cgi_result.shtml";
    }
    if(iNumParams <= 0) {
        IF_Delayer_Set_State(DELAYER_STATE_STOP);
        return "/DelayerPage.shtml";
    }
    for(uint16_t i = 0; i < iNumParams; ++i)
    {
        for(uint16_t idx = 0; idx < ARRAY_COUNT(Http_Tag_Table); ++idx) {
            if(0 == strcmp(pcParam[i], Http_Tag_Table[idx].tag)) {
                if(Http_Tag_Table[idx].rwCb != NULL) {
                    Http_Tag_Table[idx].rwCb(pcValue[i], __HTTPD_WRITE);
                }
                break;
            }
        }
    }
    return "/DelayerPage.shtml";
}

static const tCGI ppcURLs[] = //cgi程序
{
    {"/cgi/login",     SCPI_Login_in_Handler},
    {"/cgi/logout",     SCPI_Login_out_Handler},

    {"/cgi/scpi/write", SCPI_Write_CGI_Handler},
    {"/cgi/scpi/read",  SCPI_Read_CGI_Handler},

    {"/cgi/setlan",  NetWork_Set_CGI_Handler},
    {"/cgi/changepasswd",  Web_PasswdChange_CGI_Handler},

    #if 1
    //PowerOut
    {"/output.cgi",     OUTPUT_CGI_Handler},
    {"/onoff.cgi",      ONOFF_CGI_Handler},
    {"/powerread.cgi",     POWER_READ_CGI_Handler},
    //
    {"/webControl.cgi", WebControl_CGI_Handler},    //网页控制
    #endif


    #if 1
    //List
    {"/listread.cgi",   ListRead_CGI_Handler},
    {"/listdata.cgi",   ListData_CGI_Handler},
    {"/listout.cgi",    ListOut_CGI_Handler},           //返回List组数据
    {"/listonoff.cgi",  ListOnoff_CGI_Handler},         //返回List当前状态{当前点,已循环,状态}
    //Delayer
    {"/delayerread.cgi",    DelayerRead_CGI_Handler},
    {"/delayerdata.cgi",    DelayerData_CGI_Handler},
    {"/delayerout.cgi",     DelayerOut_CGI_Handler},    //返回Delayer组数据
    {"/delayeronoff.cgi",   DelayerOnoff_CGI_Handler},  //返回Delayer当前状态{当前点,已循环,状态}

    #endif
    //实验
    {"/cgi-bin/get_power_readings.cgi", PowerRead_CGI_Handler},
};




//CGI句柄初始化
void httpd_cgi_init(void)
{
    //配置CGI句柄
    http_set_cgi_handlers(ppcURLs, (sizeof(ppcURLs) / sizeof(ppcURLs[0])));
}

#endif





