#include "CAN_SCPI.h"


static osMutexId_t SCPI_CAN_MutexHandle;
static const osMutexAttr_t SCPI_CAN_Mutex_attributes = {
    .name = "SCPI_CAN_Mutex",
    .attr_bits = osMutexRecursive,
};



static int SCPI_CAN_Error(scpi_t *context, int_fast16_t err) {
    (void) context;
    return 0;
}
static scpi_result_t SCPI_CAN_Control(scpi_t *context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    (void) context;
    return SCPI_RES_OK;
}
static scpi_result_t SCPI_CAN_Reset(scpi_t *context) {
    (void) context;
    return SCPI_RES_OK;
}

static size_t SCPI_CAN_Write(scpi_t *context, const char *data, size_t len) {
    memcpy(&context->user.txBuf[context->user.txBufLen], data, len);
    context->user.txBufLen += len;
    return len;
}

static scpi_result_t SCPI_CAN_Flush(scpi_t *context) {
    user_printf(__func__, "flush:%d |%s|", context->user.txBufLen, context->user.txBuf);
    //发送到对应的 CAN组
    uint8_t canGroup_ID = IF_Option_Get_CAN_Groups_ID();
    uint8_t canid = (canGroup_ID << 4) | IF_Power_Get_Hw_ID();
    __CAN_Send_MultPack_S2M(canid, context->user.txBuf, context->user.txBufLen);
    memset(context->user.txBuf, 0, context->user.txBufLen);
    context->user.txBufLen = 0;
    return SCPI_RES_OK;
}


static scpi_interface_t scpi_can_interface = {
    .error = SCPI_CAN_Error,
    .write = SCPI_CAN_Write,
    .control = SCPI_CAN_Control,
    .flush = SCPI_CAN_Flush,
    .reset = SCPI_CAN_Reset,
};
static scpi_t scpi_can_context;

/*
 * @brief       void CAN_SCPI_Init(void)
 * @details     CAN SCPI初始化
 */
void CAN_SCPI_Init(void)
{
    SCPI_Init(&scpi_can_context, scpi_commands,
              &scpi_can_interface, scpi_units_def,
              SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
              scpi_can_context.user.scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
              scpi_can_context.user.scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
    scpi_can_context.user.interface = INTERFACE_CAN;
    //每次开机上电,触发一次开机事件POWERON
    SCPI_RegSetBits(&scpi_can_context, SCPI_REG_ESR, ESR_PON); //Power ON event(10)
}
uint8_t scpi_can_initFlag = 0;

void CAN_SCPI_Process(const uint8_t *pdata, uint16_t len)
{
    if(scpi_can_initFlag == 0) {
        scpi_can_initFlag = 1;
        CAN_SCPI_Init();
    }
    //    uint8_t result = SCPI_Input(&scpi_can_context, (const char *)pdata, len);
    if(SCPI_Input(&scpi_can_context, (const char *)pdata, len) == TRUE) {
        if(scpi_can_context.input_count != 0) {
            if(IF_Option_Get_RemoteLock() != 0) {
                if(IF_System_Get_Lock() == 0) {
                    IF_System_Set_Lock(1);
                }
            }
        }
    }
}
