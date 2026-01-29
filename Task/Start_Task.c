#include "Start_Task.h"


#include "ff.h"
#include "usb_host.h"
#include "fatfs.h"

#include "GPIO_Device.h"
#include "msgBox.h"



//任务
const osThreadAttr_t Normal_attributes = {
    .name = "Normal_Task",
    .stack_size = 1024 * 8,
    .priority = (osPriority_t) osPriorityNormal,
};
static void Normal_Task(void *argument);

/*
 * @brief       osThreadId_t Normal_Task_Create(void)
 * @details     创建普通任务
 */
osThreadId_t Normal_Task_Create(void)
{
    osThreadId_t hreadId = osThreadNew(Normal_Task, (void *)&Normal_attributes, &Normal_attributes);
    return hreadId;
}

#include "rpc_server.h"
#include "TCP_SCPI_Task.h"

void __USB_Host_Process(void);

extern void MX_USB_HOST_Init(void);
extern void MX_LWIP_Init(void);

#include "fdcan.h"
#include "httpd.h"
#include "dhcp.h"
#include "lwip/apps/lwiperf.h"
extern struct netif gnetif;

void lwiperf_report(void *arg, enum lwiperf_report_type report_type,
                    const ip_addr_t *local_addr, u16_t local_port, const ip_addr_t *remote_addr, u16_t remote_port,
                    u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(local_addr);
    LWIP_UNUSED_ARG(local_port);
    LWIP_PLATFORM_DIAG(("iperf report: type=%d, remote: %s:%d, total bytes: %"U32_F", duration in ms: %"U32_F", kbits/s: %"U32_F"\r\n",
                        (int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port, bytes_transferred, ms_duration, bandwidth_kbitpsec));
}

#if LWIP_MDNS_RESPONDER
static void mdns_example_report(struct netif *netif, u8_t result)
{
    LWIP_PLATFORM_DIAG(("mdns status[netif %d]: %d\r\n", netif->num, result));
}

static char SrvTxtBuf[100];
#define MDNS_ADD_SRV_TXT(svr, ...)                          \
    memset(SrvTxtBuf,0,sizeof(SrvTxtBuf));                  \
    snprintf(SrvTxtBuf, sizeof(SrvTxtBuf), __VA_ARGS__);    \
    {err_t res = mdns_resp_add_service_txtitem(svr, SrvTxtBuf, strlen(SrvTxtBuf));   \
        LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return);}

static void _http_srv_txt(struct mdns_service *service, void *txt_userdata)
{
    MDNS_ADD_SRV_TXT(service, "txtvers=1");
    MDNS_ADD_SRV_TXT(service, "path=/");
};

static void _scpi_raw_srv_txt(struct mdns_service *service, void *txt_userdata)
{
    MDNS_ADD_SRV_TXT(service, "txtvers=1");
    MDNS_ADD_SRV_TXT(service, "Manufacturer=%s", PowerInfo.about.company);
    MDNS_ADD_SRV_TXT(service, "Model=%s", PowerInfo.Model.modelName);
    MDNS_ADD_SRV_TXT(service, "SerialNumber=%s", PowerInfo.about.serialNumber);
    MDNS_ADD_SRV_TXT(service, "FirmwareVersion=%s", PowerInfo.about.lcd_version);
};

static void _vxi11_srv_txt(struct mdns_service *service, void *txt_userdata)
{
    MDNS_ADD_SRV_TXT(service, "txtvers=1");
    MDNS_ADD_SRV_TXT(service, "Manufacturer=%s", PowerInfo.about.company);
};

void mdns_service_callback(struct netif *netif, uint8_t enable) {
    if(enable) {
    } else {
    }
}

void lwip_netif_status_callback(struct netif *netif)
{
    static uint8_t mdns_initFlag = 0;
    user_printf(__func__, "lwip netif status channged! %s %x", (netif->flags & NETIF_FLAG_UP) ? "UP" : "DOWN", netif->ip_addr.addr);
    if((netif->flags & NETIF_FLAG_UP) != 0) {
        if(0 != netif->ip_addr.addr) {
            static uint8_t task_initFlag = 0;
            if(task_initFlag == 0) {
                task_initFlag = 1;
                //HTTPD
                extern void httpd_cgi_init(void);
                extern void httpd_ssi_init(void);
                #if (LWIP_HTTPD_CGI == 1)
                httpd_cgi_init();
                #endif
                httpd_ssi_init();
                httpd_init();
                
                #if LWIP_MDNS_RESPONDER
                mdns_init();
                #endif
                //压力测试
                lwiperf_start_tcp_server_default(lwiperf_report, NULL);
                //        //SCPI TCP SOCKET任务
                //        TCP_SCPI_Task_Create();
                //RPC Server 任务
                RPC_Server_Task_Create();
#include "TCP_Server_Raw.h"
                tcp_echoserver_init(IF_Option_Get_SCPI_Port(), NULL);
#include "TCP_Raw.h"
                tcp_raw_server_init(111, NULL);
                #if 0
                osThreadId_t VNC_Task_Create(void);
                VNC_Task_Create();
                #endif
            }
            //根据SN号命名MDNS
            char mdnsName[32] = {0};
            char *serialNumber_str = "Unknow";
            if(strlen(PowerInfo.about.serialNumber) == 13) {
                serialNumber_str = &PowerInfo.about.serialNumber[13 - 7];
            }
            strcpy(mdnsName, PowerInfo.about.model_name);
            sprintf(PowerInfo.webSet.mdnsName, "%s_%s", mdnsName, serialNumber_str);
            mdns_resp_rename_netif(&gnetif, PowerInfo.webSet.mdnsName);
        }
    } else {
    }
}

static void mdns_report(struct netif *netif, u8_t result)
{
    if(result == MDNS_PROBING_SUCCESSFUL) {
        LWIP_PLATFORM_DIAG(("mdns status[netif %d]: MDNS_PROBING_SUCCESSFUL\n", netif->num));
    } else
        if(result == MDNS_PROBING_SUCCESSFUL) {
            LWIP_PLATFORM_DIAG(("mdns status[netif %d]: MDNS_PROBING_CONFLICT \n", netif->num));
        } else {
            LWIP_PLATFORM_DIAG(("mdns status[netif %d]: %d \n", netif->num, result));
        }
}

void mdns_init(void)
{
    mdns_resp_register_name_result_cb(mdns_report);
    mdns_resp_init();
    mdns_resp_add_netif(&gnetif, PowerInfo.Model.modelName, 3600);
    mdns_resp_add_service(&gnetif, "http web Server", "_http", DNSSD_PROTO_TCP, 80, 3600, _http_srv_txt, NULL);
    mdns_resp_add_service(&gnetif, "VXI-11 Server", "_vxi-11", DNSSD_PROTO_TCP, 111, 3600, _vxi11_srv_txt, NULL);
    mdns_resp_add_service(&gnetif, "Raw SCPI", "_scpi-raw", DNSSD_PROTO_TCP, 5025, 3600, _scpi_raw_srv_txt, NULL);
}
#endif

extern const char *debugFileName;
static void Normal_Task(void *argument)
{
    osDelay(1000);
    #if defined(MULT_CHANNEL)
    if((__isMultChannel() == false) || (IF_Power_Get_Hw_ID() == 0)) {
    } else {
        osThreadTerminate(osThreadGetId());
    }
    #endif
    /* init code for USB_HOST */
    #ifdef NETWORK_ON
    /* init code for LWIP */
    MX_LWIP_Init();
    #endif
    osThreadTerminate(osThreadGetId());
    TickType_t lastTick = xTaskGetTickCount();
    for(;;) {
        //        WDG_Task_Feed(WDG_FLAG_NORMAL);
        vTaskDelayUntil(&lastTick, 100);
        //        void __Monitor_File_Process(uint16_t tick);
        //        __Monitor_File_Process(100);
    }
}


const char *debugFileName = "Flash:monitor.txt";

void __Monitor_File_Process(uint16_t tick)
{
    static FIL myFile;
    static int16_t cycleTime = 0;
    static uint8_t state = 0;
    enum {
        __STATE_OPEN = 0,
        __STATE_WRITE,
        __STATE_CLOSE,
    };
    switch(state) {
        case __STATE_OPEN : {
            if(PowerInfo.StateBar.bits.USB_Host == 0) { break; }
            if(f_open(&myFile, debugFileName, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
                memset(&myFile, 0, sizeof(myFile));
                break;
            } else {
                f_lseek(&myFile, f_size(&myFile));
                state = __STATE_WRITE;
            }
        }
        break;
        case __STATE_WRITE : {
            if(cycleTime > 0) {
                cycleTime -= tick;
            } else {
                cycleTime = 1000;
                FRESULT fres = FR_OK;
                char printf_buf[128] = {0};
                UINT byteswitten = 0;
                uint16_t count = sprintf(printf_buf, "%d ms:\t%3s\t%7.3fV,\t%7.3fV,\t%7.3fA,\t%7.3fW\r\n",
                                         (uint32_t)IF_Power_Get_RunTime(), (IF_Power_Get_On_Off_Now() == 0) ? "OFF" : (IF_Power_Read_CCCV() ? "CC" : "CV"),
                                         IF_Power_Read_Volt(), IF_Power_Read_Volt_Drv(), IF_Power_Read_Curr(), IF_Power_Read_Power());
                fres = f_write(&myFile, printf_buf, count, &byteswitten);
                if(fres != FR_OK) {
                    state = __STATE_CLOSE;
                } else {
                    f_sync(&myFile);
                }
            }
        }
        break;
        case __STATE_CLOSE : {
            f_close(&myFile);
            memset(&myFile, 0, sizeof(myFile));
            state = __STATE_OPEN;
        }
        break;
    }
}






void __USB_Host_Process(void)
{
    extern ApplicationTypeDef Appli_state;
    extern char USBHPath[4]; // USBH logical drive path
    static FATFS FatfsUDisk; // File system object for USB disk logical drive
    switch(Appli_state)
    {
        case APPLICATION_START: {
        } break;
        case APPLICATION_READY:
            if(f_mount(&FatfsUDisk, (TCHAR const *)USBHPath, 1) != FR_OK) {
            } else {
                printf("挂载成功!\r\n");
                IF_State_USB_Host_Connect(1);
                #if defined(FLASH_DEBUG)
                if(__copy_debugFile(debugFileName) == FR_OK) {
                    f_unlink(debugFileName);
                    MsgBox_Printf(MSG_TYPE_WARRING, NULL, "Debug file Copyed!");
                }
                #endif
                if(guider_ui.list_manager_cont != NULL) {
                    lv_event_send_refresh_recursive(guider_ui.list_manager_cont);
                }
                if(guider_ui.delayer_manager_cont != NULL) {
                    lv_event_send_refresh_recursive(guider_ui.delayer_manager_cont);
                }
            }
            Appli_state = APPLICATION_IDLE;
            break;
        case APPLICATION_DISCONNECT:
            f_mount(NULL, (TCHAR const *)USBHPath, 1);
            Appli_state = APPLICATION_IDLE;
            IF_State_USB_Host_Connect(0);
            if(guider_ui.list_manager_cont != NULL) {
                lv_event_send_refresh_recursive(guider_ui.list_manager_cont);
            }
            if(guider_ui.delayer_manager_cont != NULL) {
                lv_event_send_refresh_recursive(guider_ui.delayer_manager_cont);
            }
            break;
        default:
            break;
    }
}



#include "Interface.h"
#include "Task_Manager.h"
char text[512] = {0};
void Debug_Test(void) {
    if(PowerInfo.State.bits.acFault != 0)
    { return ; }
    //
    static FIL myFile;
    //若消息队列有数据,则保存
    if(Debug_MsgQueue_Length() > 0)
    {
        //打开文件
        if(f_open(&myFile, debugFileName, FA_OPEN_APPEND | FA_WRITE) == FR_OK) {
            if(f_size(&myFile) == 0) {
                if(strlen(PowerInfo.about.serialNumber) > 0) {
                    f_printf(&myFile, "SN Serial:%s\r\n", PowerInfo.about.serialNumber);
                } else {
                    f_printf(&myFile, "SN Serial:%s\r\n", "Unknow");
                }
            }
            uint8_t writeCount = 10;
            do {
                Msg_Def thisMsg = {0};
                if(0 != Debug_MsgQueue_Rec(&thisMsg))
                {
                    extern size_t xFreeBytesRemaining;
                    extern size_t xMinimumEverFreeBytesRemaining;
                    extern size_t xNumberOfSuccessfulAllocations;
                    extern size_t xNumberOfSuccessfulFrees;
                    extern size_t xNumberOfMaxBytes;
                    UINT bwrite = 0;
                    if(thisMsg.data.msg != NULL) {
                        //运行时间  线程名称:函数名  输出电压  输出电流  DEBUG信息
                        memset(text, 0, 512);
                        uint16_t printed = snprintf(text, 512 - 1, "%lld:%s:%s--%0.2fV/%0.2fA--%.2f--%.2f--%d:%s\r\n",
                                                    thisMsg.runTime_ms, thisMsg.threadName, thisMsg.funcName,
                                                    //输出电压电流
                                                    IF_Power_Read_Volt(), IF_Power_Read_Curr(),
                                                    //lwip内存信息
                                                    100.0f * (lwip_stats.mem.max + 1) / (lwip_stats.mem.avail + 1), //要+1 否则未初始化为0/0出BUG
                                                    //rtos内存信息
                                                    100.0f * xMinimumEverFreeBytesRemaining / xNumberOfMaxBytes,
                                                    //lwip tcp连接数
                                                    lwip_stats.memp[1]->used,
                                                    thisMsg.data.msg);
                        f_write(&myFile, text, printed, &bwrite);
                        //释放内存
                        vPortFree(thisMsg.data.msg);
                        thisMsg.data.msg = NULL;
                    }
                } else {
                    break;
                }
                if(writeCount > 0) { writeCount--; }
                else    { break; }
            } while(Debug_MsgQueue_Length() > 0);
            f_close(&myFile);
        }
        //        //获取行数
        //        int linesCount = 0;
        //        linesCount = f_linesCount(debugFileName);
        //        if(linesCount > 2000) {
        //            f_delete_first_n_lines(debugFileName, 500);
        //            //                MsgBox_Printf(MSG_TYPE_STATEBAR, NULL, "%d lines",linesCount);
        //        }
    }
}



//#ifdef NETWORK_ON
#if 0
#include "LCD.h"
extern uint16_t FrameBuffer[LCD_H][LCD_W];

typedef struct {
    uint8_t bit_per_pixel;
    uint8_t depth;
    uint8_t big_endian;
    uint8_t true_color_flag;
    uint16_t red_max;
    uint16_t green_max;
    uint16_t blue_max;
    uint8_t red_shift;
    uint8_t green_shift;
    uint8_t blue_shift;
    uint8_t padding[3];
} Pixel_Format_Def;

typedef struct {
    uint16_t width;
    uint16_t height;
    Pixel_Format_Def pixelFormat;
    uint32_t nameLen;
    const char *name;
} Server_Framebuf_Param_Def;

typedef struct {
    uint8_t padding[3];
    Pixel_Format_Def pixelFormat;
} Client_Framebuf_Set_Def;

typedef struct {
    uint8_t msgType;        //消息类型,默认[5]
    uint8_t button_mask;
    uint16_t xpos;
    uint16_t ypos;
} Client_PointerEvent_Def;

typedef struct {
    uint8_t msgType;        //消息类型,默认[4]
    uint8_t down_flag;
    uint16_t padding;
    uint32_t key;
} Client_KeyEvent_Def;

typedef struct {
    uint8_t msgType;        //消息类型,默认[3]
    uint8_t incremental;    //增量更新
    uint16_t xpos;
    uint16_t ypos;
    uint16_t width;
    uint16_t height;
} Client_FrameRequest_Def;


typedef struct {
    uint8_t msgType;
    uint8_t padding;
    uint16_t number_of_rect;
    //后续追加像素
} Server_Update_Def;

typedef struct {
    uint16_t xpos;
    uint16_t ypos;
    uint16_t width;
    uint16_t height;
    int32_t encoding_type;
} Rect_Def;

Server_Update_Def serverupdate = {
    .msgType = 0,
    .padding = 0,
};
#define LITTLE_TO_BIG(A,B) do{ uint16_t C = B;A = 0;A |= ((uint16_t)C>>8)&0xff;  A |= ((uint16_t)C&0xff)<<8;}while(0);


const osThreadAttr_t VNC_attributes = {
    .name = "VNC_Task",
    .stack_size = 1024 * 1,
    .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t vnc_threadID;
static void VNC_Task(void *argument);


/*
 * @brief       osThreadId_t VNC_Task_Create(void)
 * @details     创建VNC任务
 */
osThreadId_t VNC_Task_Create(void)
{
    vnc_threadID = osThreadNew(VNC_Task, (void *)&VNC_attributes, &VNC_attributes);
    return vnc_threadID;
}

RPC_Client rpcClient = {0};
void VNC_Task(void *argument)
{
    osDelay(5000);
    int sockfd = -1;
again:
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        close(sockfd);
        osDelay(100);
        goto again;
    }
    //
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(5900);
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        close(sockfd);
        osDelay(100);
        goto again;
    }
    //监听
    if(listen(sockfd, 1) == -1) {
        close(sockfd);
        osDelay(100);
        goto again;
    }
    //循环等到每个连接接入
    while(1) {
        socklen_t sin_size = sizeof(rpcClient.link.tcp.client_addr);
        //等待新的连接接入(阻塞)
        rpcClient.socket = accept(sockfd, (struct sockaddr *)&rpcClient.link.tcp.client_addr, &sin_size);
        if(rpcClient.socket >= 0) {
#define VNC_VERSION "RFB 003.008\n"       //security
            enum {
                VNC_SECURITY_TYPE_INVALID = 0,
                VNC_SECURITY_TYPE_NONE = 1,
                VNC_SECURITY_TYPE_AUTHENTICATION = 2,
            };
            uint8_t server_buf_1[] = {VNC_VERSION};
            uint8_t server_buf_2[] = {0x01, VNC_SECURITY_TYPE_NONE};
            uint8_t server_buf_3[] = {0x00, 0x00, 0x00, 0x00};
            uint8_t server_buf_4[] = {0x01, 0x40, 0x00, 0xF0, 0x10, 0x10, 0x01, 0x01
                                      , 0x00, 0x1F, 0x00, 0x3F, 0x00, 0x1F, 0x0B, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x2A, 0x49, 0x44, 0x4E, 0x3F
                                     };
            uint8_t state = 0;
            while(1)
            {
                static Client_PointerEvent_Def pointerEvent = {0};
                static Client_FrameRequest_Def requeset = {0};
                uint8_t errorFlag = 0;
                enum {
                    VNC_STATE_INIT_S_VERSION = 0,
                    VNC_STATE_SECURITY_S_SUPPORTED = 1,
                    VNC_STATE_S_ANTHEN_RESULT = 2,
                    VNC_STATE_S_FRAME_PARAMETERS = 3,
                    //VNC_STATE_C_UPDATE_REQUEST = 4,
                    VNC_STATE_S_UPDATE_FRAMEBUFFER = 4,
                };
                switch(state)
                {
                    case VNC_STATE_INIT_S_VERSION:
                        lwip_write(rpcClient.socket, server_buf_1, sizeof(server_buf_1) - 1);
                        osDelay(50);
                        state++;
                        break;
                    case VNC_STATE_SECURITY_S_SUPPORTED:
                        lwip_write(rpcClient.socket, server_buf_2, sizeof(server_buf_2));
                        osDelay(50);
                        state++;
                        break;
                    case VNC_STATE_S_ANTHEN_RESULT:
                        lwip_write(rpcClient.socket, server_buf_3, sizeof(server_buf_3));
                        osDelay(50);
                        state++;
                        break;
                    case VNC_STATE_S_FRAME_PARAMETERS:
                        lwip_write(rpcClient.socket, server_buf_4, sizeof(server_buf_4));
                        osDelay(50);
                        state++;
                        break;
                        #if 0
                    case VNC_STATE_C_UPDATE_REQUEST:
                        break;
                        #endif
                    case VNC_STATE_S_UPDATE_FRAMEBUFFER: {
                        #if 1
                        //memcpy(serverupdate.rect.data,(void*)&FrameBuffer[y][0],LCD_W*2);
                        //                        if(requeset.incremental == 0){
                        //                        if( osOK ==  osEventFlagsWait(vnc_threadID,0xffffffff,osFlagsWaitAll,osWaitForever))
                        {
                            LITTLE_TO_BIG(serverupdate.number_of_rect, 1);
                            lwip_write(rpcClient.socket, &serverupdate, sizeof(serverupdate));
                            Rect_Def rect = {0};
                            LITTLE_TO_BIG(rect.width, LCD_W);
                            LITTLE_TO_BIG(rect.height, LCD_H);
                            LITTLE_TO_BIG(rect.ypos, 0);
                            lwip_write(rpcClient.socket, &rect, sizeof(rect));
                            for(uint16_t y = 0; y < LCD_H; y += 1) {
                                lwip_write(rpcClient.socket, FrameBuffer[y], sizeof(FrameBuffer[y]) * 1);
                            }
                            //                        }else{
                            //                            LITTLE_TO_BIG(serverupdate.number_of_rect,(320/20));
                            //                            lwip_write(client.socket, &serverupdate, sizeof(serverupdate));
                            //                            for(uint16_t y=0;y<LCD_H;y+=20){
                            //                                Rect_Def rect = {0};
                            //                                LITTLE_TO_BIG(rect.width,LCD_W);
                            //                                LITTLE_TO_BIG(rect.height,20);
                            //                                LITTLE_TO_BIG(rect.ypos,y);
                            //                                lwip_write(client.socket, &rect, sizeof(rect));
                            //                                lwip_write(client.socket, FrameBuffer[y], sizeof(FrameBuffer[y])*20);
                            //                                osDelay(1);
                            //                            }
                            //                        }
                        }
                        //requeset.incremental = 0;
                        //state = VNC_STATE_C_UPDATE_REQUEST;
                        #endif
                    }
                    break;
                }
                rpcClient.rece.len = lwip_recv(rpcClient.socket, rpcClient.rece.buffer, VXI_BUF_SIZE, 0);
                if(rpcClient.rece.len >= 0) {
                    //                    if((client.rece.buffer[0] == 3) && (client.rece.len == sizeof(Client_FrameRequest_Def))) {
                    //                        memcpy(&requeset, client.rece.buffer, sizeof(Client_FrameRequest_Def));
                    //                        LITTLE_TO_BIG(requeset.xpos, requeset.xpos);
                    //                        LITTLE_TO_BIG(requeset.ypos, requeset.ypos);
                    //                        LITTLE_TO_BIG(requeset.width, requeset.width);
                    //                        LITTLE_TO_BIG(requeset.height, requeset.height);
                    //                    } else if((client.rece.buffer[0] == 5) && (client.rece.len == sizeof(pointerEvent))) {
                    //                        memcpy(&pointerEvent, client.rece.buffer, sizeof(pointerEvent));
                    //                        LITTLE_TO_BIG(pointerEvent.xpos, pointerEvent.xpos);
                    //                        LITTLE_TO_BIG(pointerEvent.ypos, pointerEvent.ypos);
                    //                     }
                    //state = VNC_STATE_S_UPDATE_FRAMEBUFFER;
                    memset(rpcClient.rece.buffer, 0, rpcClient.rece.len);
                    rpcClient.rece.len = 0 ;
                } else {
                    errorFlag = 1;
                }
                if(errorFlag != 0) {
                    break;
                }
                rpcClient.stackRemain = uxTaskGetStackHighWaterMark(NULL);
            }
            //断开连接
            if(rpcClient.socket >= 0) {
                closesocket(rpcClient.socket);
                rpcClient.socket = -1;
            }
        }
        osDelay(50);
    }
}
#endif
