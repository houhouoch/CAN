#include "TCP_SCPI_Task.h"

#include "lwip/opt.h"

#include <lwip/sockets.h>
#include <lwip/tcp.h>
#include "lwip/api.h"
#include "lwip/sys.h"

//任务
const osThreadAttr_t SCPI_TCP_attributes = {
    .name = "SCPI_TCP_Task",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) TCPIP_THREAD_PRIO,
};

static void TCP_SCPI_Task(void *argument);


/*
 * @brief       osThreadId_t TCP_SCPI_Task_Create(void)
 * @details     创建SCPI TCP SOCKET任务
 */
osThreadId_t TCP_SCPI_Task_Create(void)
{
    osThreadId_t hreadId = osThreadNew(TCP_SCPI_Task, (void *)&SCPI_TCP_attributes, &SCPI_TCP_attributes);
    return hreadId;
}


#include "scpi/scpi.h"
#include "scpi-def.h"

static size_t SCPI_TCP_Write(scpi_t *context, const char *data, size_t len);
static scpi_result_t SCPI_TCP_Flush(scpi_t *context);

static int SCPI_TCP_Error(scpi_t *context, int_fast16_t err) {
    (void) context;
    return 0;
}
static scpi_result_t SCPI_TCP_Control(scpi_t *context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    (void) context;
    return SCPI_RES_OK;
}
static scpi_result_t SCPI_TCP_Reset(scpi_t *context) {
    (void) context;
    return SCPI_RES_OK;
}

static scpi_interface_t scpi_tcp_interface = {
    .error = SCPI_TCP_Error,
    .write = SCPI_TCP_Write,
    .control = SCPI_TCP_Control,
    .flush = SCPI_TCP_Flush,
    .reset = SCPI_TCP_Reset,
};


typedef struct{
    scpi_t scpi_tcp_context;
    int socket;
}TCP_SCPI_Table_t;

#define TCP_SCPI_MAX_CONNECTS  5
TCP_SCPI_Table_t   TCP_SCPI_Table[TCP_SCPI_MAX_CONNECTS] = {0};

void __TCP_SCPI_Table_Init(void){
    __Interface_Lock();
    for(uint8_t i=0;i<ARRAY_COUNT(TCP_SCPI_Table);++i){
        TCP_SCPI_Table[i].socket = -1;
    }
    __Interface_Unlock();
}

TCP_SCPI_Table_t* __TCP_SCPI_Table_Get_New(void){
    __Interface_Lock();
    for(uint8_t i=0;i<ARRAY_COUNT(TCP_SCPI_Table);++i){
        if(TCP_SCPI_Table[i].socket == -1){
            __Interface_Unlock();
            return &TCP_SCPI_Table[i];
        }
    }
    __Interface_Unlock();
    return NULL;
}

void __TCP_SCPI_Table_Delete_By_Socket(int socket){
    if(socket <= 0) return ;
    __Interface_Lock();
    for(uint8_t i=0;i<ARRAY_COUNT(TCP_SCPI_Table);++i){
        if(TCP_SCPI_Table[i].socket == socket){
            TCP_SCPI_Table[i].socket = -1;
            memset(&TCP_SCPI_Table[i].scpi_tcp_context,0,sizeof(TCP_SCPI_Table[i].scpi_tcp_context));
        }
    }
    __Interface_Unlock();
}


/*
 * @brief       void TCP_SCPI_Init(void)
 * @details     TCP SCPI初始化
 */
static void TCP_SCPI_Init(scpi_t* pSCPI)
{
    if(pSCPI == NULL) while(1);
    SCPI_Init(pSCPI, scpi_commands,
              &scpi_tcp_interface, scpi_units_def,
              SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
              pSCPI->user.scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
              pSCPI->user.scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
    pSCPI->user.interface = INTERFACE_SOCKET;
    //每次开机上电,触发一次开机事件POWERON
    SCPI_RegSetBits(pSCPI, SCPI_REG_ESR, ESR_PON); //Power ON event(10)
}




static scpi_result_t SCPI_TCP_Flush(scpi_t *context) {
    (void) context;
    lwip_write(context->user.socketidx, context->user.txBuf, context->user.txBufLen);
    memset(context->user.txBuf, 0, context->user.txBufLen);
    context->user.txBufLen = 0;
    return SCPI_RES_OK;
}

static size_t SCPI_TCP_Write(scpi_t *context, const char *data, size_t len) {
    (void) context;
    memcpy(&context->user.txBuf[context->user.txBufLen], data, len);
    context->user.txBufLen += len;
    return len;
}





void clientTask(void *argument)
{
    TCP_SCPI_Table_t table = *(TCP_SCPI_Table_t*)argument;
    #define RECV_DATA (SCPI_INPUT_BUFFER_LENGTH)
    uint8_t recv_data[RECV_DATA + 1] = {0};
    int recv_data_len = 0;
    __IO int clientsock = table.scpi_tcp_context.user.socketidx;
    
//    int so_keepalive_val = 1;   //使能心跳机制
//    int tcp_keepalive_idle = 3; //发送心跳空闲周期 S
//    int tcp_keepalive_intvl = 3;//发送心跳间隔 S
//    int tcp_keepalive_cnt = 3;  //重发次数
//    int tcp_nodelay = 1;    //延迟并打包发送
//    setsockopt(clientsock, SOL_SOCKET, SO_KEEPALIVE, (void *)&so_keepalive_val, sizeof(so_keepalive_val));
//    setsockopt(clientsock, IPPROTO_TCP, TCP_KEEPALIVE, &so_keepalive_val, sizeof(int));
//    setsockopt(clientsock, IPPROTO_TCP, TCP_KEEPIDLE, &tcp_keepalive_idle, sizeof(int));
//    setsockopt(clientsock, IPPROTO_TCP, TCP_KEEPINTVL, &tcp_keepalive_intvl, sizeof(int));
//    setsockopt(clientsock, IPPROTO_TCP, TCP_KEEPCNT, &tcp_keepalive_cnt, sizeof(int));
    //    setsockopt(clientsock, IPPROTO_TCP, TCP_NODELAY, (void *) &tcp_nodelay, sizeof(int));
    TickType_t delayTick = xTaskGetTickCount();
    while(1)
    {
        //        vTaskDelayUntil(&delayTick, 10 / portTICK_PERIOD_MS);
        //
        recv_data_len = lwip_recv(clientsock, recv_data, RECV_DATA, 0);
        //刷新SCPI状态
        SCPI_Core_Reg_Update(&table.scpi_tcp_context);
        if(recv_data_len > 0) {
            uint8_t result = SCPI_Input(&table.scpi_tcp_context, (const char *)recv_data, recv_data_len);
            if(TRUE == result) {
                if(table.scpi_tcp_context.input_count != 0) {
                    if(IF_Option_Get_RemoteLock() != 0) {
                        if(IF_System_Get_Lock() == 0) {
                            IF_System_Set_Lock(1);
                        }
                    }
                }
            }
        } else {
            break;
        }
    }
    if(clientsock >= 0) {
        __TCP_SCPI_Table_Delete_By_Socket(clientsock);
        closesocket(clientsock);
        clientsock = -1;
        PowerInfo.StateBar.bits.Socket = 0;
    }
    
    osThreadExit();
}




static void TCP_SCPI_Task(void *argument)
{
    User_Task_t thisTask = {0};
    if(argument != NULL) {
        thisTask.threadId = osThreadGetId();
        thisTask.attributes = (const osThreadAttr_t *)argument;
    }
    while((netif_default == NULL) || (ip4_addr_isany_val(*netif_ip4_addr(netif_default)))) {
        osDelay(10);
    }
    
    //
    int sockfd = -1;
again:
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        close(sockfd);
        osDelay(100);
        goto again;
    }
    //设置端口号5025 (tcp_scpi_raw)
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(IF_Option_Get_SCPI_Port());
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
    /* Infinite loop */
    __TCP_SCPI_Table_Init();
    while(1) {
        thisTask = thisTask;
        thisTask.startHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        struct sockaddr_in client_addr;
        socklen_t sin_size = sizeof(struct sockaddr_in);
        //等待新的连接接入(阻塞)
        int newsock = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
//        if(PowerInfo.StateBar.bits.Socket == 0) {
//            clientsock = newsock;
//            PowerInfo.StateBar.bits.Socket = 1;
            //        clientTask(NULL);
            
            
            TCP_SCPI_Table_t *table = __TCP_SCPI_Table_Get_New();
            if(table != NULL){
                TCP_SCPI_Init(&table->scpi_tcp_context);
                table->socket = newsock;
                table->scpi_tcp_context.user.socketidx = newsock;
                osThreadNew(clientTask, (void *)table ,&SCPI_TCP_attributes);
            }else{
                user_printf(__func__,"TCP SCPI Max Connects");
                closesocket(newsock);
            }
//        } else {
//            closesocket(newsock);
//        }
    }
}



