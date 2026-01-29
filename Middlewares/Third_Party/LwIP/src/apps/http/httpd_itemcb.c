#include "Commd.h"
#include "MultString.h"
#include "Interface.h"
#include "option_Interface.h"
#include "usbd_desc_tmc.h"
#include "CJson.h"


enum {
    __HTTPD_READ,
    __HTTPD_WRITE,
};





#include "scpi/scpi.h"
#include "scpi-def.h"

static size_t httpd_SCPI_Write(scpi_t *context, const char *data, size_t len);
static scpi_result_t httpd_SCPI_Flush(scpi_t *context);
static int httpd_SCPI_Error(scpi_t *context, int_fast16_t err);
static scpi_result_t httpd_SCPI_Control(scpi_t *context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val);
static scpi_result_t httpd_SCPI_Reset(scpi_t *context);

typedef struct {
    uint8_t initflag;
    //
    uint64_t webId;
    //
    char result_buffer[128];
    //
    scpi_t scpi_context;
    scpi_interface_t scpi_http_interface;
    //
    char scpi_output_buffer[SCPI_INPUT_BUFFER_LENGTH];
    uint16_t scpi_output_bufferLength;
} Httpd_SCPI_Def;

static Httpd_SCPI_Def httpd_scpi = {
    .scpi_http_interface = {
        .error = httpd_SCPI_Error,
        .write = httpd_SCPI_Write,
        .control = httpd_SCPI_Control,
        .flush = httpd_SCPI_Flush,
        .reset = httpd_SCPI_Reset,
    },
};

static size_t httpd_SCPI_Write(scpi_t *context, const char *data, size_t len) {
    memcpy(&httpd_scpi.scpi_output_buffer[httpd_scpi.scpi_output_bufferLength], data, len);
    httpd_scpi.scpi_output_bufferLength += len;
    return len;
}
static scpi_result_t httpd_SCPI_Flush(scpi_t *context) {
    return SCPI_RES_OK;
}
static int httpd_SCPI_Error(scpi_t *context, int_fast16_t err) {
    return 0;
}
static scpi_result_t httpd_SCPI_Control(scpi_t *context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    return SCPI_RES_OK;
}
static scpi_result_t httpd_SCPI_Reset(scpi_t *context) {
    return SCPI_RES_OK;
}


static void SCPI_Http_Init(void)
{
    SCPI_Init(&httpd_scpi.scpi_context,
              scpi_commands,
              &httpd_scpi.scpi_http_interface,
              scpi_units_def,
              SCPI_IDN1, SCPI_IDN2,
              SCPI_IDN3, SCPI_IDN4,
              httpd_scpi.scpi_context.user.scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
              httpd_scpi.scpi_context.user.scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
    httpd_scpi.scpi_context.user.interface = INTERFACE_WEB;
    //每次开机上电,触发一次开机事件POWERON
    SCPI_RegSetBits(&httpd_scpi.scpi_context, SCPI_REG_ESR, ESR_PON); //Power ON event(10)
}


static void __CGI_SCPI(char *str, uint8_t rw) {
    if(httpd_scpi.initflag == 0) {
        httpd_scpi.initflag = 1;
        SCPI_Http_Init();
    }
    switch(rw) {
        case __HTTPD_READ: {
            memcpy(str, httpd_scpi.scpi_output_buffer, httpd_scpi.scpi_output_bufferLength);
            //
            memset(httpd_scpi.scpi_output_buffer, 0, httpd_scpi.scpi_output_bufferLength);
            httpd_scpi.scpi_output_bufferLength = 0;
            //            user_printf("->%s", str);
        }
        break;
        case __HTTPD_WRITE: {
            memset(httpd_scpi.scpi_output_buffer, 0, httpd_scpi.scpi_output_bufferLength);
            httpd_scpi.scpi_output_bufferLength = 0;
            //
            uint8_t result = SCPI_Input(&httpd_scpi.scpi_context, str, strlen(str));
            if(TRUE == result) {
            }
            //            user_printf("<-%s", str);
        }
        break;
    }
}


static void __CGI_Login_out(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
        } break;
        case __HTTPD_WRITE: {
        } break;
    }
}

static void __CGI_Result(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%s", httpd_scpi.result_buffer);
            memset(httpd_scpi.result_buffer, 0, sizeof(httpd_scpi.result_buffer));
        }
        break;
    }
}







static void __Net_HostName(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%s", PowerInfo.webSet.mdnsName);
        }
        break;
    }
}
static void __Net_DHCP(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%s", (IF_Option_NetWork_Get_DHCP_Now() != 0) ? "true" : "false");
        }
        break;
        case __HTTPD_WRITE: {
            if(0 == strcasecmp(str, "true")) {
                IF_Option_NetWork_Set_DHCP(1);
            } else {
                IF_Option_NetWork_Set_DHCP(0);
            }
            //            user_printf("dhcp = %s \r\n", str);
        }
        break;
    }
}

static void __Net_IPv4(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%d.%d.%d.%d", IF_Option_NetWork_Get_IP_Now(0), IF_Option_NetWork_Get_IP_Now(1),
                    IF_Option_NetWork_Get_IP_Now(2), IF_Option_NetWork_Get_IP_Now(3));
        }
        break;
        case __HTTPD_WRITE: {
            int ip[4] = {0};
            if(IF_Option_NetWork_Get_DHCP() == 0) {
                if(4 == sscanf(str, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3])) {
                    IF_Option_NetWork_Set_IP(0, ip[0]);
                    IF_Option_NetWork_Set_IP(1, ip[1]);
                    IF_Option_NetWork_Set_IP(2, ip[2]);
                    IF_Option_NetWork_Set_IP(3, ip[3]);
                }
            }
            //            user_printf("IPv4 = %s \r\n", str);
        }
        break;
    }
}
static void __Net_NetMask(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%d.%d.%d.%d", IF_Option_NetWork_Get_NetMask_Now(0), IF_Option_NetWork_Get_NetMask_Now(1),
                    IF_Option_NetWork_Get_NetMask_Now(2), IF_Option_NetWork_Get_NetMask_Now(3));
        }
        break;
        case __HTTPD_WRITE: {
            int netMask[4] = {0};
            if(IF_Option_NetWork_Get_DHCP() == 0) {
                if(4 == sscanf(str, "%d.%d.%d.%d", &netMask[0], &netMask[1], &netMask[2], &netMask[3])) {
                    IF_Option_NetWork_Set_NetMask(0, netMask[0]);
                    IF_Option_NetWork_Set_NetMask(1, netMask[1]);
                    IF_Option_NetWork_Set_NetMask(2, netMask[2]);
                    IF_Option_NetWork_Set_NetMask(3, netMask[3]);
                }
            }
            //            user_printf("netMask = %s \r\n", str);
        }
        break;
    }
}
static void __Net_GateWay(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%d.%d.%d.%d", IF_Option_NetWork_Get_Gateway_Now(0), IF_Option_NetWork_Get_Gateway_Now(1),
                    IF_Option_NetWork_Get_Gateway_Now(2), IF_Option_NetWork_Get_Gateway_Now(3));
        }
        break;
        case __HTTPD_WRITE: {
            int gateway[4] = {0};
            if(IF_Option_NetWork_Get_DHCP() == 0) {
                if(4 == sscanf(str, "%d.%d.%d.%d", &gateway[0], &gateway[1], &gateway[2], &gateway[3])) {
                    IF_Option_NetWork_Set_Gateway(0, gateway[0]);
                    IF_Option_NetWork_Set_Gateway(1, gateway[1]);
                    IF_Option_NetWork_Set_Gateway(2, gateway[2]);
                    IF_Option_NetWork_Set_Gateway(3, gateway[3]);
                }
            }
            //            user_printf("gateway = %s \r\n", str);
        }
        break;
    }
}


static void __Net_MAC(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%02X-%02X-%02X-%02X-%02X-%02X",
                    PowerInfo.outSet.mac[0], PowerInfo.outSet.mac[1],
                    PowerInfo.outSet.mac[2], PowerInfo.outSet.mac[3],
                    PowerInfo.outSet.mac[4], PowerInfo.outSet.mac[5]);
        }
        break;
    }
}

static void __visa_LAN_Protocol(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "TCPIP0::%d.%d.%d.%d::INSTR", IF_Option_NetWork_Get_IP_Now(0), IF_Option_NetWork_Get_IP_Now(1),
                    IF_Option_NetWork_Get_IP_Now(2), IF_Option_NetWork_Get_IP_Now(3));
        }
        break;
    }
}

static void __visa_Socket_Protocol(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "TCPIP0::%d.%d.%d.%d::5025::SOCKET", IF_Option_NetWork_Get_IP_Now(0), IF_Option_NetWork_Get_IP_Now(1),
                    IF_Option_NetWork_Get_IP_Now(2), IF_Option_NetWork_Get_IP_Now(3));
        }
        break;
    }
}

static void __visa_USB_Protocol(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "USB0::0x%04x::0x%04x::%s::INSTR", USBD_VID, USBD_PID_FS, IF_Option_Read_SN());
        }
        break;
    }
}



static void __About_Model(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ:
            strcpy(str, PowerInfo.about.model_name);
            break;
    }
}

static void __About_Manufacturer(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ:
            strcpy(str, PowerInfo.about.company);
            break;
    }
}

static void __About_SerialNum(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ:
            strcpy(str, PowerInfo.about.serialNumber);
            break;
    }
}

static void __About_FirmVersion(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ:
            strcpy(str, PowerInfo.about.lcd_version);
            break;
    }
}

static void __About_PowerVersion(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ:
            strcpy(str, PowerInfo.about.power_version);
            break;
    }
}

static void __About_Description(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ:
            sprintf(str, "%s %s %s_%s", USBD_MANUFACTURER_STRING,
                    USBD_CONFIGURATION_STRING_FS, PowerInfo.about.model_name, PowerInfo.about.serialNumber);
            break;
    }
}




#if 1

static void __Web_Control(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_WRITE: {
            IF_System_Set_Remote(0 == strcmp(str, "true"));
        }
        break;
    }
}

static void __Web_SwitchPage(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_WRITE: {
            typedef struct {
                const char *str;
                uint8_t page;
            } temp_def;
            const temp_def temp[] = {
                {"home",        PAGE_HOME},
                {"list",        PAGE_LIST},
                {"delayer",     PAGE_DELAY},
                {"outoption",   PAGE_OUTOPTION},
                {"menu",        PAGE_MENU},
            };
            for(uint8_t j = 0; j < ARRAY_COUNT(temp); ++j) {
                if(strcmp(temp[j].str, str) == 0) {
                    //                    IF_Display_Go_to_Page(temp[j].page);
                    break;
                }
            }
        }
        break;
    }
}

static void __PowerModel(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%s", PowerInfo.about.model_name);
        }
        break;
    }
}

static void __Current_Volt(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            __Voltage_Display_Fmt(str, IF_Power_Read_Volt());
        }
        break;
    }
}



static void __Current_Curr(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            __Current_Display_Fmt(str, IF_Power_Read_Curr());
        }
        break;
    }
}

static void __Current_Power(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            __Power_Display_Fmt(str, IF_Power_Read_Power());
        }
        break;
    }
}

static void __Current_Dvm(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            __Voltage_Display_Fmt(str, IF_Power_Read_Volt_Drv());
        }
        break;
    }
}

static void __Current_State(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            if(IF_Power_Get_OTP_TRIPed() != 0) {
                sprintf(str, "%s", "OTP");
            } else
                if(IF_Power_Get_OVP_TRIPed() != 0) {
                    sprintf(str, "%s", "OVP");
                } else
                    if(IF_Power_Get_OCP_TRIPed() != 0) {
                        sprintf(str, "%s", "OCP");
                    } else
                        if(IF_Power_Get_On_Off_Now() != 0) {
                            sprintf(str, "%s", (IF_Power_Read_CCCV() != 0) ? "CC" : "CV");
                        } else {
                            sprintf(str, "%s", "OFF");
                        }
        }
        break;
    }
}

static void __OutVolt(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, __Get_Voltage_Set_Fmt(), IF_Power_Get_OutVolt());
        }
        break;
        case __HTTPD_WRITE: {
            float param = 0;
            if(sscanf(str, "%f", &param) == 1) {
                IF_Power_Set_OutVolt(param);
            }
        }
        break;
    }
}

static void __OutCurr(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, __Get_Current_Set_Fmt(), IF_Power_Get_OutCurr());
        }
        break;
        case __HTTPD_WRITE: {
            float param = 0;
            if(sscanf(str, "%f", &param) == 1) {
                IF_Power_Set_OutCurr(param);
            }
        }
        break;
    }
}

static void __Out_TimeOff_OnOff(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            //sprintf(str, "%s", IF_Power_Get_TimeOff_onoff() ? "ON" : "OFF");
        } break;
        case __HTTPD_WRITE: {
            //IF_Power_Set_TimeOff_onoff(0 == strcmp(str, "ON"));
        } break;
    }
}

static void __Out_TimeOff(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            //sprintf(str, TIME_DISP_FORM, IF_Power_Get_TimeOff());
        } break;
        case __HTTPD_WRITE: {
            float param = 0;
            if(sscanf(str, "%f", &param) == 1) {
                //IF_Power_Set_TimeOff(param);
            }
        } break;
    }
}

static void __Out_OVP(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%s", IF_Power_Get_OVP_onoff() ? "ON" : "OFF");
        }
        break;
        case __HTTPD_WRITE: {
            //过压保护开关
            IF_Power_Set_OVP_onoff(0 == strcmp(str, "ON"));
        }
        break;
    }
}

static void __Out_OVP_Value(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, __Get_Voltage_Set_Fmt(), IF_Power_Get_OVP());
        }
        break;
        case __HTTPD_WRITE: {
            float param = 0;
            if(sscanf(str, "%f", &param) == 1) {
                IF_Power_Set_OVP(param);
            }
        }
        break;
    }
}

static void __Out_OCP(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%s", IF_Power_Get_OCP_onoff() ? "ON" : "OFF");
        }
        break;
        case __HTTPD_WRITE: {
            //过流保护开关
            IF_Power_Set_OCP_onoff(0 == strcmp(str, "ON"));
        }
        break;
    }
}

static void __Out_OCP_Value(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, __Get_Current_Set_Fmt(), IF_Power_Get_OCP());
        }
        break;
        case __HTTPD_WRITE: {
            float param = 0;
            if(sscanf(str, "%f", &param) == 1) {
                IF_Power_Set_OCP(param);
            }
        }
        break;
    }
}

static void __Out_OnOff(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%s", IF_Power_Get_On_Off_Now() ? "ON" : "OFF");
        }
        break;
        case __HTTPD_WRITE: {
            IF_Power_OVP_Clean();
            IF_Power_OCP_Clean();
            IF_Power_Set_On_Off(0 == strcmp(str, "on"));
        }
        break;
    }
}

static void __Out_Volt_Max(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%f", VOL_MAX);
        }
        break;
        case __HTTPD_WRITE: {
        } break;
    }
}

static void __Out_Curr_Max(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%f", CUR_MAX);
        }
        break;
        case __HTTPD_WRITE: {
        } break;
    }
}

static void __Out_Power_Max(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, "%f", POWER_MAX);
        }
        break;
        case __HTTPD_WRITE: {
        } break;
    }
}



static void __CGI_Read(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            cJSON *cjson_return = cJSON_CreateObject();
            if(cjson_return != NULL) {
                char string[32] = {0};
                __Voltage_Display_Fmt(string, IF_Power_Read_Volt());
                cJSON_AddStringToObject(cjson_return, "voltage", string);
                __Current_Display_Fmt(string, IF_Power_Read_Curr());
                cJSON_AddStringToObject(cjson_return, "current", string);
                __Power_Display_Fmt(string, IF_Power_Read_Power());
                cJSON_AddStringToObject(cjson_return, "power", string);
                const char *fmt = "";
                //
                fmt = __Get_Voltage_Set_Fmt();
                sprintf(string, fmt, IF_Power_Get_OutVolt());
                cJSON_AddStringToObject(cjson_return, "voltageSet", string);
                fmt = __Get_Current_Set_Fmt();
                sprintf(string, fmt, IF_Power_Get_OutCurr());
                cJSON_AddStringToObject(cjson_return, "currentSet", string);
                //
                fmt = __Get_Voltage_Set_Fmt();
                sprintf(string, fmt, PowerInfo.Model.maxVolt);
                cJSON_AddStringToObject(cjson_return, "voltageSetMax", string);
                fmt = __Get_Current_Set_Fmt();
                sprintf(string, fmt, PowerInfo.Model.maxCurr);
                cJSON_AddStringToObject(cjson_return, "currentSetMax", string);
                //
                fmt = __Get_Voltage_Set_Fmt();
                sprintf(string, fmt, IF_Power_Get_OVP());
                cJSON_AddStringToObject(cjson_return, "ovpSet", string);
                fmt = __Get_Current_Set_Fmt();
                sprintf(string, fmt, IF_Power_Get_OCP());
                cJSON_AddStringToObject(cjson_return, "ocpSet", string);
                cJSON_AddStringToObject(cjson_return, "ovpOnSet", IF_Power_Get_OVP_onoff() ? "ON" : "OFF");
                cJSON_AddStringToObject(cjson_return, "ocpOnSet", IF_Power_Get_OCP_onoff() ? "ON" : "OFF");
                cJSON_AddStringToObject(cjson_return, "outSet", IF_Power_Get_On_Off_Now() ? "ON" : "OFF");
                if(IF_Power_Get_OTP_TRIPed() != 0) {
                    sprintf(string, "%s", "OTP");
                } else
                    if(IF_Power_Get_OVP_TRIPed() != 0) {
                        sprintf(string, "%s", "OVP");
                    } else
                        if(IF_Power_Get_OCP_TRIPed() != 0) {
                            sprintf(string, "%s", "OCP");
                        } else
                            if(IF_Power_Get_On_Off_Now() != 0) {
                                sprintf(string, "%s", (IF_Power_Read_CCCV() != 0) ? "CC" : "CV");
                            } else {
                                sprintf(string, "%s", "OFF");
                            }
                cJSON_AddStringToObject(cjson_return, "powerStatus", string);
                char *stringReturn = cJSON_PrintUnformatted(cjson_return);
                if(stringReturn != NULL) {
                    sprintf(str, "%s", stringReturn);
                    cJSON_free(stringReturn);
                }
            }
            cJSON_Delete(cjson_return);
        }
        break;
    }
}


#if 1
#include "CJson.h"
typedef struct {
    struct {
        int idx;
        int count;
        cJSON *json;
    } tran;
    struct {
        cJSON *json;
    } rece;
} Json_Info;

#include "ListOut.h"
#include "Delayer.h"
#include "List_Delayer_Interface.h"

static Json_Info listJson_Info = {0};
static Json_Info delayerJson_Info = {0};

//List表格特定数据写入JSON
static void List_to_JSON(void)
{
    if(listJson_Info.tran.json == NULL) {
        listJson_Info.tran.json = cJSON_CreateArray();
    }
    if(listJson_Info.tran.count <= 0) {return ;}
    for(uint32_t i = 0; i < listJson_Info.tran.count; ++i)
    {
        uint32_t idx = listJson_Info.tran.idx + i;
        if(idx >= LIST_MAX_COUNT) {
            break;
        }
        cJSON *newItem = cJSON_CreateObject();
        if(newItem != NULL) {
            cJSON_AddItemToObject(newItem, "No", cJSON_CreateNumber(idx));
            cJSON_AddItemToObject(newItem, "Vol", cJSON_CreateNumber(listoutInfo.set.points[idx].volt));
            cJSON_AddItemToObject(newItem, "Cur", cJSON_CreateNumber(listoutInfo.set.points[idx].curr));
            cJSON_AddItemToObject(newItem, "keepT", cJSON_CreateNumber(listoutInfo.set.points[idx].time));
            cJSON_AddItemToArray(listJson_Info.tran.json, newItem);
        }
    }
}

//将JSON数据写入List表格
static void JSON_to_List(const char *string)
{
    if(listJson_Info.rece.json != NULL) {
        cJSON_Delete(listJson_Info.rece.json);
        listJson_Info.rece.json = NULL;
    }
    listJson_Info.rece.json = cJSON_Parse(string);
    if(listJson_Info.rece.json == NULL) {
        return ;
    }
    //数组
    uint16_t jsonItemCount = cJSON_GetArraySize(listJson_Info.rece.json);
    for(uint16_t i = 0; i < jsonItemCount; ++i)
    {
        //对象
        cJSON *jsonitem = cJSON_GetArrayItem(listJson_Info.rece.json, i);
        if(jsonitem != NULL)
        {
            int16_t no = -1;
            float vol = 0;
            float amp = 0;
            float keepT = 0;
            uint16_t jsonValCount = cJSON_GetArraySize(jsonitem);
            for(uint16_t j = 0; j < jsonValCount; ++j)
            {
                //对象数组
                cJSON *jsonVal = cJSON_GetArrayItem(jsonitem, j);
                if(jsonVal != NULL)
                {
                    if(0 == strcmp(jsonVal->string, "No")) {
                        no = jsonVal->valuedouble;
                    } else
                        if(0 == strcmp(jsonVal->string, "Vol")) {
                            vol = jsonVal->valuedouble;
                        } else
                            if(0 == strcmp(jsonVal->string, "Cur")) {
                                amp = jsonVal->valuedouble;
                            } else
                                if(0 == strcmp(jsonVal->string, "keepT")) {
                                    keepT = jsonVal->valuedouble;
                                }
                }
            }
            if(no != -1) {
                IF_List_Set_Voltage(no, vol);
                IF_List_Set_Current(no, amp);
                IF_List_Set_kTime(no, keepT);
            }
        }
    }
    if(listJson_Info.rece.json != NULL) {
        cJSON_Delete(listJson_Info.rece.json);
        listJson_Info.rece.json = NULL;
    }
}



//Delayer表格特定数据写入JSON
static void Delayer_to_JSON(void)
{
    if(delayerJson_Info.tran.json == NULL) {
        delayerJson_Info.tran.json = cJSON_CreateArray();
    }
    if(delayerJson_Info.tran.count <= 0) {return ;}
    for(uint32_t i = 0; i < delayerJson_Info.tran.count; ++i)
    {
        uint32_t idx = delayerJson_Info.tran.idx + i;
        if(idx >= DELAYER_MAX_COUNT) {
            break;
        }
        cJSON *newItem = cJSON_CreateObject();
        if(newItem != NULL) {
            cJSON_AddItemToObject(newItem, "No", cJSON_CreateNumber(idx));
            cJSON_AddItemToObject(newItem, "State", cJSON_CreateString(delayerInfo.set.points[idx].out ? "ON" : "OFF"));
            cJSON_AddItemToObject(newItem, "delayT", cJSON_CreateNumber(delayerInfo.set.points[idx].time));
            cJSON_AddItemToArray(delayerJson_Info.tran.json, newItem);
        }
    }
}

//将JSON数据写入List表格
static void JSON_to_Delayer(const char *string)
{
    if(delayerJson_Info.rece.json != NULL) {
        cJSON_Delete(delayerJson_Info.rece.json);
        delayerJson_Info.rece.json = NULL;
    }
    delayerJson_Info.rece.json = cJSON_Parse(string);
    if(delayerJson_Info.rece.json == NULL) {
        return ;
    }
    //数组
    uint16_t jsonItemCount = cJSON_GetArraySize(delayerJson_Info.rece.json);
    for(uint16_t i = 0; i < jsonItemCount; ++i)
    {
        //对象
        cJSON *jsonitem = cJSON_GetArrayItem(delayerJson_Info.rece.json, i);
        if(jsonitem != NULL)
        {
            int16_t no = -1;
            uint8_t state = 0;
            float delayT = 0;
            uint16_t jsonValCount = cJSON_GetArraySize(jsonitem);
            for(uint16_t j = 0; j < jsonValCount; ++j)
            {
                //对象数组
                cJSON *jsonVal = cJSON_GetArrayItem(jsonitem, j);
                if(jsonVal != NULL)
                {
                    if(0 == strcmp(jsonVal->string, "No")) {
                        no = jsonVal->valuedouble;
                    } else
                        if(0 == strcmp(jsonVal->string, "State")) {
                            state = (0 == strcmp("ON", jsonVal->valuestring));
                        } else
                            if(0 == strcmp(jsonVal->string, "delayT")) {
                                delayT = jsonVal->valuedouble;
                            }
                    if(no != -1) {
                        IF_Delayer_Set_Out(no, state);
                        IF_Delayer_Set_kTime(no, delayT);
                    }
                }
            }
        }
    }
    if(delayerJson_Info.rece.json != NULL) {
        cJSON_Delete(delayerJson_Info.rece.json);
        delayerJson_Info.rece.json = NULL;
    }
}




static void _List_JSON(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            if(listJson_Info.tran.json != NULL) {
                char *jsonStr = cJSON_PrintUnformatted(listJson_Info.tran.json);
                if(jsonStr != NULL) {
                    sprintf(str, "%s", jsonStr);
                    cJSON_Delete(listJson_Info.tran.json);
                    listJson_Info.tran.json = NULL;
                    cJSON_free(jsonStr);
                }
            }
        }
        break;
        case __HTTPD_WRITE: {
            JSON_to_List(str);
        }
        break;
    }
}

static void _List_Current_Groups(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, POINT_DISP_FORM, IF_List_Get_CurGroups());
        }
        break;
    }
}

static void _List_Current_Cycled(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, POINT_DISP_FORM, IF_List_Get_CurCycled());
        }
        break;
    }
}

static void _List_Current_State(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            switch(listoutInfo.now.state) {
                case LIST_STATE_NULL:
                case LIST_STATE_STOP:
                    sprintf(str, "%s", MultStr_GetS(&mStr_State_Stopped));
                    break;
                case LIST_STATE_PAUSE:
                    sprintf(str, "%s", MultStr_GetS(&mStr_State_Paused));
                    break;
                case LIST_STATE_RUNNING:
                    sprintf(str, "%s", MultStr_GetS(&mStr_State_Running));
                    break;
                case LIST_STATE_COMPLETED:
                    sprintf(str, "%s", MultStr_GetS(&mStr_State_Completed));
                    break;
            }
        }
        break;
    }
}

//
static void _List_Start(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, POINT_DISP_FORM, IF_List_Get_Start());
        }
        break;
        case __HTTPD_WRITE: {
            int param = 0;
            if(sscanf(str, "%d", &param) == 1) {
                IF_List_Set_Start(param);
            }
        }
        break;
    }
}

static void _List_Count(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, POINT_DISP_FORM, IF_List_Get_Groups());
        }
        break;
        case __HTTPD_WRITE: {
            int param = 0;
            if(sscanf(str, "%d", &param) == 1) {
                IF_List_Set_Groups(param);
            }
        }
        break;
    }
}

static void _List_Cycle(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, CYCLES_DISP_FORM, IF_List_Get_Cycles());
        }
        break;
        case __HTTPD_WRITE: {
            int param = 0;
            if(sscanf(str, "%d", &param) == 1) {
                IF_List_Set_Cycles(param);
            }
        }
        break;
    }
}

static void _List_EndState(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            switch(IF_List_Get_EndState()) {
                case LIST_ENDSTATE_ON:
                    sprintf(str, "%s", "ON");
                    break;
                case LIST_ENDSTATE_OFF:
                    sprintf(str, "%s", "OFF");
                    break;
            }
        }
        break;
        case __HTTPD_WRITE: {
            if(0 == strcmp(str, "ON")) {
                IF_List_Set_EndState(LIST_ENDSTATE_ON);
            } else
                if(0 == strcmp(str, "OFF")) {
                    IF_List_Set_EndState(LIST_ENDSTATE_OFF);
                }
        }
        break;
    }
}

static void _List_MaxCount(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, POINT_DISP_FORM, LIST_MAX_COUNT);
        }
        break;
    }
}

static void _List_Onoff(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            if((IF_List_Get_State() != LIST_STATE_STOP)
                    && (IF_List_Get_State() != LIST_STATE_NULL)) {
                sprintf(str, "%s", "off");
            } else {
                sprintf(str, "%s", "on");
            }
        }
        break;
        case __HTTPD_WRITE: {
            if(0 == strcmp(str, "on")) {
                IF_List_Set_State(LIST_STATE_RUNNING);
            } else {
                IF_List_Set_State(LIST_STATE_STOP);
            }
        }
        break;
    }
}











static void _Delayer_JSON(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            if(delayerJson_Info.tran.json != NULL) {
                char *jsonStr = cJSON_PrintUnformatted(delayerJson_Info.tran.json);
                if(jsonStr != NULL) {
                    sprintf(str, "%s", jsonStr);
                    cJSON_Delete(delayerJson_Info.tran.json);
                    delayerJson_Info.tran.json = NULL;
                    cJSON_free(jsonStr);
                }
            }
        }
        break;
        case __HTTPD_WRITE: {
            JSON_to_Delayer(str);
        }
        break;
    }
}

static void _Delayer_Current_Groups(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, POINT_DISP_FORM, IF_Delayer_Get_CurGroups());
        }
        break;
    }
}

static void _Delayer_Current_Cycled(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, POINT_DISP_FORM, IF_Delayer_Get_CurCycled());
        }
        break;
    }
}

static void _Delayer_Current_State(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            switch(delayerInfo.now.state) {
                case DELAYER_STATE_NULL:
                case DELAYER_STATE_STOP:
                    sprintf(str, "%s", MultStr_GetS(&mStr_State_Stopped));
                    break;
                case DELAYER_STATE_FAILED:
                    sprintf(str, "%s", MultStr_GetS(&mStr_State_Failed));
                    break;
                case DELAYER_STATE_RUNNING:
                    sprintf(str, "%s", MultStr_GetS(&mStr_State_Running));
                    break;
                case DELAYER_STATE_COMPLETED:
                    sprintf(str, "%s", MultStr_GetS(&mStr_State_Completed));
                    break;
            }
        }
        break;
        case __HTTPD_WRITE: {
            if(0 == strcmp(str, "on")) {
                IF_Delayer_Set_State(DELAYER_STATE_RUNNING);
            } else {
                IF_Delayer_Set_State(DELAYER_STATE_STOP);
            }
        }
        break;
    }
}

//
static void _Delayer_Start(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, POINT_DISP_FORM, IF_Delayer_Get_Start());
        }
        break;
        case __HTTPD_WRITE: {
            int param = 0;
            if(sscanf(str, "%d", &param) == 1) {
                IF_Delayer_Set_Start(param);
            }
        }
        break;
    }
}

static void _Delayer_Count(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, POINT_DISP_FORM, IF_Delayer_Get_Groups());
        }
        break;
        case __HTTPD_WRITE: {
            int param = 0;
            if(sscanf(str, "%d", &param) == 1) {
                IF_Delayer_Set_Groups(param);
            }
        }
        break;
    }
}

static void _Delayer_Cycle(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, CYCLES_DISP_FORM, IF_Delayer_Get_Cycles());
        }
        break;
        case __HTTPD_WRITE: {
            int param = 0;
            if(sscanf(str, "%d", &param) == 1) {
                IF_Delayer_Set_Cycles(param);
            }
        }
        break;
    }
}

static void _Delayer_EndState(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            switch(IF_Delayer_Get_EndState()) {
                case DELAYER_ENDSTATE_ON:
                    sprintf(str, "%s", "ON");
                    break;
                case DELAYER_ENDSTATE_OFF:
                    sprintf(str, "%s", "OFF");
                    break;
            }
        }
        break;
        case __HTTPD_WRITE: {
            if(0 == strcmp(str, "ON")) {
                IF_Delayer_Set_EndState(DELAYER_ENDSTATE_ON);
            } else
                if(0 == strcmp(str, "OFF")) {
                    IF_Delayer_Set_EndState(DELAYER_ENDSTATE_OFF);
                }
        }
        break;
    }
}

static void _Delayer_MaxCount(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            sprintf(str, POINT_DISP_FORM, DELAYER_MAX_COUNT);
        }
        break;
    }
}


static void _Delayer_Onoff(char *str, uint8_t rw) {
    switch(rw) {
        case __HTTPD_READ: {
            if(IF_Delayer_Get_State() == DELAYER_STATE_RUNNING) {
                sprintf(str, "%s", "on");
            } else {
                sprintf(str, "%s", "off");
            }
        }
        break;
        case __HTTPD_WRITE: {
            if(0 == strcmp(str, "on")) {
                IF_Delayer_Set_State(DELAYER_STATE_RUNNING);
            } else {
                IF_Delayer_Set_State(DELAYER_STATE_STOP);
            }
        }
        break;
    }
}


#endif


#endif
