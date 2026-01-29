#include "WDG_Task.h"

osThreadId_t wdg_task_thread;
//任务
const osThreadAttr_t WDG_attributes = {
    .name = "WDG_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityHigh,
};
static void WDG_Task(void *argument);

uint32_t flag = 0;


// 定义 flag 与字符串的映射关系
typedef struct {
    uint32_t flag;        // flag 位掩码
    const char *name;     // 对应的字符串
} FlagNameMap;

// 映射表（与枚举顺序一致，仅包含有效项）
const FlagNameMap g_flagNameMap[] = {
    {WDG_FLAG_COM,        "WDG_FLAG_COM"},
    {WDG_FLAG_GUI,        "WDG_FLAG_GUI"},
    {WDG_FLAG_CAN_IRQ,    "WDG_FLAG_CAN_IRQ"},
    {WDG_FLAG_CAN_RX,     "WDG_FLAG_CAN_RX"},
    {WDG_FLAG_CAN_TX,     "WDG_FLAG_CAN_TX"},
    {WDG_FLAG_USB_TMC,    "WDG_FLAG_USB_TMC"},
};
/**
 * @brief 解析 flag 并打印对应字符串
 * @param flag 输入的 flag 值（支持多 bit 组合）
 */
void printFlagString(uint32_t flag) {
    int hasMatch = 0;  // 标记是否有匹配的 flag
    // 遍历所有有效 flag，检查 bit 是否被置位
    for(size_t i = 0; i < ARRAY_COUNT(g_flagNameMap); i++) {
        if(flag & g_flagNameMap[i].flag) {   // 检测对应 bit 是否为 1
            if(hasMatch) {
                user_printf(__func__, ", "); // 多 flag 时添加分隔符
            }
            user_printf("%s", g_flagNameMap[i].name);
            hasMatch = 1;
        }
    }
    // 无匹配 flag 时的提示
    if(!hasMatch) {
        user_printf(__func__, "无匹配的 WDG 标志（flag 为 0 或无效组合）");
    }
    user_printf(__func__, "\n");
}


void WDG_Task_Feed(uint32_t flag)
{
    osThreadFlagsSet(wdg_task_thread, flag);
}
void WDG_Task_Read(void)
{
    //当所有任务正常时,喂狗
    uint32_t waitFlag = WDG_FLAG_COM | WDG_FLAG_GUI
                        | WDG_FLAG_CAN_IRQ | WDG_FLAG_CAN_TX
                        | WDG_FLAG_CAN_RX
                        | WDG_FLAG_USB_TMC ;
    flag = osThreadFlagsGet();
    if(waitFlag == (waitFlag & flag)) {
        osThreadFlagsClear(0xfffffff);
    } else {
        static uint8_t wdgCount = 0;
        if(wdgCount < 20) {
            wdgCount++;
        } else {
            IF_Debug_Set_ErrorFlag(flag ^ waitFlag);
            printFlagString(flag ^ waitFlag);
        }
    }
    HAL_IWDG_Refresh(&hiwdg1);
    //    if( waitFlag == osThreadFlagsWait(waitFlag,osFlagsWaitAll,5000) ){
    //        osThreadFlagsClear(0xfffffff);
    //        HAL_IWDG_Refresh(&hiwdg1);
    //    }else{
    //        user_printf(__func__,"Watch Dog Timeout!");
    //    }
}






/*
 * @brief       osThreadId_t WDG_Task_Create(void)
 * @details     创建看门狗WDG任务
 */
osThreadId_t WDG_Task_Create(void)
{
    wdg_task_thread = osThreadNew(WDG_Task, (void *)&WDG_attributes, &WDG_attributes);
    return wdg_task_thread;
}

static void WDG_Task(void *argument)
{
    osDelay(5000);
    #ifdef __DWG_EN
    MX_IWDG1_Init();
    __HAL_DBGMCU_FREEZE_IWDG1();
    #else
    osThreadTerminate(osThreadGetId());
    #endif
    for(;;) {
        static TickType_t lastTick = 0;
        vTaskDelayUntil(&lastTick, 1000);
        WDG_Task_Read();
    }
}



