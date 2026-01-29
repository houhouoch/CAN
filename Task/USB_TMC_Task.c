#include "USB_TMC_Task.h"
#include "Setting.h"

//任务
const osThreadAttr_t USB_TMC_attributes = {
    .name = "USB_TMC_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal + 1,
};

//信号量
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
osStaticSemaphoreDef_t USB_TMC_IRQControlBlock;
const osSemaphoreAttr_t USB_TMC_IRQ_attributes = {
    .name = "USB_TMC_IRQ",
    .cb_mem = &USB_TMC_IRQControlBlock,
    .cb_size = sizeof(USB_TMC_IRQControlBlock),
};
static osSemaphoreId_t USB_TMC_Semaph = NULL;



#include "Interface.h"
#include "scpi/scpi.h"
#include "scpi-def.h"
static scpi_t scpi_usb_context;



void USB_TMC_SemaphrCallback(void)
{
    if(USB_TMC_Semaph != NULL) {
        osSemaphoreRelease(USB_TMC_Semaph);
    }
}


static void USB_TMC_Task(void *argument);
/*
 * @brief       osThreadId_t USB_TMC_Task_Create(void)
 * @details     创建USB TMC任务
 */
osThreadId_t USB_TMC_Task_Create(void)
{
    //初始化USB TMC
    TMC_USB_DEVICE_Init();
    //创建信号量
    USB_TMC_Semaph = osSemaphoreNew(5, 0, &USB_TMC_IRQ_attributes);
    osThreadId_t hreadId = osThreadNew(USB_TMC_Task, (void *)&USB_TMC_attributes, &USB_TMC_attributes);
    return hreadId;
}


void SCPI_USB_Init(void);
void SCPI_USB_Tick(void);
void USB_TMC_Task(void *argument)
{
    User_Task_t thisTask = {0};
    if(argument != NULL) {
        thisTask.threadId = osThreadGetId();
        thisTask.attributes = (const osThreadAttr_t *)argument;
    }
    //
    SCPI_USB_Init();
    for(;;) {
        WDG_Task_Feed(WDG_FLAG_USB_TMC);
        thisTask = thisTask;
        thisTask.startHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        //接收到数据后将数据
        if(osSemaphoreAcquire(USB_TMC_Semaph, 1000) == osOK)
        {
            SCPI_USB_Tick();
            osDelay(2);
        }
    }
}





extern char *SCPI_GetCmd_TMC(scpi_t *context, uint32_t *pLen);
extern void SCPI_FreeCmd_TMC(scpi_t *context, const char *pbuf);

void SCPI_USB_Tick(void)
{
    //SCPI内核寄存器更新
    SCPI_Core_Reg_Update(&scpi_usb_context);
    static uint32_t CmdLen = 0;
    char *pCmd = SCPI_GetCmd_TMC(&scpi_usb_context, &CmdLen);
    if((CmdLen > 0) && (pCmd != NULL)) {
        //USER_DEBUG(__func__,"<-%s\r\n", pCmd);
        uint8_t result = SCPI_Input(&scpi_usb_context, pCmd, CmdLen);
        if(TRUE == result) {
            //解析成功
            if(scpi_usb_context.input_count != 0) {
                if(IF_Option_Get_RemoteLock() != 0) {
                    if(IF_System_Get_Lock() == 0) {
                        IF_System_Set_Lock(1);
                    }
                }
            }
        }
        SCPI_FreeCmd_TMC(&scpi_usb_context, pCmd);
    }
}

static size_t SCPI_Write(scpi_t *context, const char *data, size_t len) {
    if(context == NULL) { return 0; }
    if(data == NULL) { return 0; }
    // 修复：使用 <= 而不是 <
    if((context->user.txBufLen + len) <= sizeof(context->user.txBuf)) {
        memcpy(&context->user.txBuf[context->user.txBufLen], data, len);
        context->user.txBufLen += len;
        return len;
    } else {
        // 缓冲区不足，尝试刷新
        if(context->interface->flush(context) == SCPI_RES_OK) {
            // 刷新后再次尝试
            if((context->user.txBufLen + len) <= sizeof(context->user.txBuf)) {
                memcpy(&context->user.txBuf[context->user.txBufLen], data, len);
                context->user.txBufLen += len;
                return len;
            }
        }
        return 0;
    }
    //USER_DEBUG(__func__,"->%s\r\n", data);
    return len;
}
static scpi_result_t SCPI_Flush(scpi_t *context) {
    if(context == NULL) { return SCPI_RES_ERR; }
    if(context->user.txBufLen > 0) {
        extern size_t SCPI_Write_TMC(scpi_t *context, const char *data, size_t len);
        SCPI_Write_TMC(context, (const char *)context->user.txBuf, context->user.txBufLen);
        extern scpi_result_t SCPI_Flush_TMC(scpi_t *context);
        SCPI_Flush_TMC(context);
        context->user.txBufLen = 0;
    }
    return SCPI_RES_OK;
}
static int SCPI_Error(scpi_t *context, int_fast16_t err) {
    return 0;
}
static scpi_result_t SCPI_Control(scpi_t *context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    return SCPI_RES_OK;
}
static scpi_result_t SCPI_Reset(scpi_t *context) {
    return SCPI_RES_OK;
}



static scpi_interface_t scpi_usb_interface = {
    .error = SCPI_Error,
    .write = SCPI_Write,
    .control = SCPI_Control,
    .flush = SCPI_Flush,
    .reset = SCPI_Reset,
};

/*
 * @brief       void SCPI_USB_Init(void)
 * @details     串口SCPI初始化
 */
void SCPI_USB_Init(void)
{
    SCPI_Init(&scpi_usb_context,
              scpi_commands,
              &scpi_usb_interface,
              scpi_units_def,
              SCPI_IDN1, SCPI_IDN2,
              SCPI_IDN3, SCPI_IDN4,
              scpi_usb_context.user.scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
              scpi_usb_context.user.scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
    scpi_usb_context.user.interface = INTERFACE_USB;
    //每次开机上电,触发一次开机事件POWERON
    SCPI_RegSetBits(&scpi_usb_context, SCPI_REG_ESR, ESR_PON); //Power ON event(10)
}




