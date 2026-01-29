#include "Debug_Task.h"

//任务
const osThreadAttr_t Debug_attributes = {
    .name = "Debug_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};

//信号量
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
osStaticSemaphoreDef_t Debug_Recv_IRQControlBlock;
const osSemaphoreAttr_t Debug_Recv_IRQ_attributes = {
    .name = "Debug_Recv_IRQ",
    .cb_mem = &Debug_Recv_IRQControlBlock,
    .cb_size = sizeof(Debug_Recv_IRQControlBlock),
};
static osSemaphoreId_t Debug_Recv_Semaph = {0};
static void COM_Debug_Task(void *argument);



//
#define UART_BUFFER_LENGTH 256
uint8_t USART1_Rx_DMA_Buffer[UART_BUFFER_LENGTH] = {0};
static uint8_t USART1_Rx_Buffer[UART_BUFFER_LENGTH] = {0};
static uint8_t USART1_Rx_Buffer_Length = 0;

void __onUSART1_RxCplt(void)
{
    //停止DMA
    HAL_UART_DMAStop(&huart1);
    //接收了多长的数据
    uint16_t dma_rx_len = UART_BUFFER_LENGTH - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
    if(dma_rx_len > 0) {
        if(Debug_Recv_Semaph != NULL) {
            memcpy(USART1_Rx_Buffer, USART1_Rx_DMA_Buffer, dma_rx_len);
            USART1_Rx_Buffer_Length = dma_rx_len;
            //任务处理
            osSemaphoreRelease(Debug_Recv_Semaph);
        }
        memset(USART1_Rx_DMA_Buffer, 0, sizeof(USART1_Rx_DMA_Buffer));
    }
    //再次接收
    HAL_UART_Receive_DMA(&huart1, USART1_Rx_DMA_Buffer, UART_BUFFER_LENGTH);
}

/*
 * @brief       osThreadId_t Debug_Task_Create(void)
 * @details     创建调试串口通讯任务
 */
osThreadId_t Debug_Task_Create(void)
{
    Debug_Recv_Semaph = osSemaphoreNew(5, 0, &Debug_Recv_IRQ_attributes);
    osThreadId_t hreadId = osThreadNew(COM_Debug_Task, (void *)&Debug_attributes, &Debug_attributes);
    return hreadId;
}







static void COM_Debug_Task(void *argument)
{
    User_Task_t thisTask = {0};
    if(argument != NULL) {
        thisTask.threadId = osThreadGetId();
        thisTask.attributes = (const osThreadAttr_t *)argument;
    }
    //开始DMA接收
    __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_IDLE);
    __HAL_UART_CLEAR_IT(&huart1, UART_IT_IDLE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1, USART1_Rx_DMA_Buffer, UART_BUFFER_LENGTH);
    //#include "scpi_uart.h"
    //    UART_SCPI_Init();
    /* Infinite loop */
    while(1)
    {
        thisTask = thisTask;
        thisTask.startHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        //接收到数据后将数据
        if(osSemaphoreAcquire(Debug_Recv_Semaph, 0) == osOK)
        {
            if(USART1_Rx_Buffer_Length > 0) {
                //将数据存入ModBus缓存区
                uint8_t *pbuffer = USART1_Rx_Buffer;
                uint16_t buffer_length = USART1_Rx_Buffer_Length;
                //清空接收缓存区
                USART1_Rx_Buffer_Length = 0;
                memset(USART1_Rx_Buffer, 0, sizeof(USART1_Rx_Buffer));
            }
        }
    }
}



