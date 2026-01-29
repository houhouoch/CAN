#include "Modbus_master_IO.h"
#include "usart.h"
#include "Commd.h"


uint8_t *Modbus_Receive(void);
void Modbus_Clean_Receive(void);

uint8_t __txBuf[128] = {0};
uint16_t __txCount = 0;
static void __txClean(void)
{
    memset(__txBuf, 0, sizeof(__txBuf));
    __txCount = 0;
}

static uint16_t __GetCRC(uint8_t *pdata, uint16_t count)
{
    uint16_t crc = 0;
    crc = userCRC_Calculate(pdata, count);
    return crc;
}

osSemaphoreId PowerComSemaphore = NULL;
void __onUSART5_TxCplt(void) {
    if(PowerComSemaphore != 0) {
        osSemaphoreRelease(PowerComSemaphore);
    }
}

uint8_t buffer[512] = {0};
static void __SendPack(uint8_t *pdata, uint16_t count)
{
    Modbus_Clean_Receive();
    #if 0
    HAL_UART_Transmit(&huart5, pdata, count, 100);
    #else
    memcpy(buffer, pdata, count);
    if(PowerComSemaphore == NULL) {
        PowerComSemaphore = osSemaphoreNew(1, 1, NULL);
    }
    osSemaphoreAcquire(PowerComSemaphore, 0);
    HAL_UART_Transmit_DMA(&huart5, buffer, count);
    osSemaphoreAcquire(PowerComSemaphore, 500);
    //while(huart5.gState != HAL_UART_STATE_READY) {  }
    #endif
}


void __Creat_Write_Pack(uint8_t addr, uint16_t reg, uint8_t byteCount, uint8_t *pdata)
{
    __txClean();
    __txBuf[__txCount++] = addr;
    __txBuf[__txCount++] = MB_CMD_WRITE;
    __txBuf[__txCount++] = (reg >> 8) & 0xff;
    __txBuf[__txCount++] = reg & 0xff;
    uint16_t regCount = byteCount / 2;
    __txBuf[__txCount++] = (regCount >> 8) & 0xff;
    __txBuf[__txCount++] = regCount & 0xff;
    __txBuf[__txCount++] = byteCount;
    for(uint8_t i = 0; i < byteCount; ++i) {
        __txBuf[__txCount++] = pdata[i];
    }
    //计算CRC
    uint16_t crc = __GetCRC(__txBuf, __txCount);
    __txBuf[__txCount++] = crc & 0xff;
    __txBuf[__txCount++] = (crc >> 8) & 0xff;
    __SendPack(__txBuf, __txCount);
}

void __Creat_Read_Pack(uint8_t addr, uint16_t reg, uint16_t regCount)
{
    __txClean();
    __txBuf[__txCount++] = addr;
    __txBuf[__txCount++] = MB_CMD_READ;
    __txBuf[__txCount++] = (reg >> 8) & 0xff;
    __txBuf[__txCount++] = reg & 0xff;
    __txBuf[__txCount++] = (regCount >> 8) & 0xff;
    __txBuf[__txCount++] = regCount & 0xff;
    //计算CRC
    uint16_t crc = __GetCRC(__txBuf, __txCount);
    __txBuf[__txCount++] = crc & 0xff;
    __txBuf[__txCount++] = (crc >> 8) & 0xff;
    __SendPack(__txBuf, __txCount);
}





/*
 * @brief       uint32_t Modbus_Get_ErrorCount(void)
 * @details     获取通讯错误次数
 */
uint32_t errorCount = 0;
void Modbus_Clean_ErrorCount(void) {
    errorCount = 0;
}
uint32_t Modbus_Get_ErrorCount(void) {
    return errorCount;
}


