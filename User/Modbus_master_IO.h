#ifndef __MODBUS_MASTER_IO_H_
#define __MODBUS_MASTER_IO_H_

#include <stdint.h>
#include <string.h>


/*
 * @brief
 * @details     寄存地址偏移
 */
#define MB_REG_STATE_OFFSET     (0x0000)
#define MB_REG_READ_OFFSET      (0x0040)
#define MB_REG_TEMP_OFFSET      (0x0080)
#define MB_REG_MODEL_OFFSET     (0x0090)
#define MB_REG_SET_OFFSET       (0x0100)
#define MB_REG_LIST_OFFSET      (0x0200)
#define MB_REG_DELAYER_OFFSET   (0x0300)
#define MB_REG_SETTING_OFFSET   (0x0400)
#define MB_REG_BATTESTER_OFFSET (0x0500)

#define MB_REG_INFO_OFFSET      (0x0600)
#define MB_REG_DEBUG_OFFSET     (0x1000)

#define MB_SLAVE_ADDR (1)



enum {
    MB_OK = 0,
    MB_ERROR = 1,
};

enum {
    MB_CMD_WRITE = 0x10,
    MB_CMD_READ = 0x03,
};

enum {
    STATE_SEND = 0,
    STATE_RECV = 1,
};

/*
 * @brief       uint32_t Modbus_Get_ErrorCount(void)
 * @details     获取通讯错误次数
 */
extern uint32_t errorCount;
void Modbus_Clean_ErrorCount(void);
uint32_t Modbus_Get_ErrorCount(void);

void __Creat_Write_Pack(uint8_t addr, uint16_t reg, uint8_t byteCount, uint8_t *pdata);
void __Creat_Read_Pack(uint8_t addr, uint16_t reg, uint16_t regCount);

void COM_Mutex_Lock(void);
void COM_Mutex_Unlock(void);

uint8_t *Modbus_Receive(void);
void Modbus_Clean_Receive(void);

typedef union {
    uint64_t data_u64;
    uint32_t data_u32;
    uint16_t data_u16;
    double data_double;
    float data_float;
    uint8_t pdata[8];
} temp_data_t;


#define MB_WRITE_INT16(PDATA,val_u16)       \
    {                                       \
        temp_data_t temp = {0};             \
        temp.data_u16 = val_u16;            \
        PDATA[0] = temp.pdata[1];           \
        PDATA[1] = temp.pdata[0];           \
        PDATA += 2;                         \
    }

#define MB_READ_INT16(PDATA,val_u16)        \
    {                                       \
        temp_data_t temp = {0};             \
        temp.pdata[1] = PDATA[0];           \
        temp.pdata[0] = PDATA[1];           \
        val_u16 = temp.data_u16;            \
        PDATA += 2;                         \
    }

#define MB_WRITE_INT32(PDATA,val_u32)       \
    {                                       \
        temp_data_t temp = {0};             \
        temp.data_u32 = val_u32;            \
        PDATA[0] = temp.pdata[3];           \
        PDATA[1] = temp.pdata[2];           \
        PDATA[2] = temp.pdata[1];           \
        PDATA[3] = temp.pdata[0];           \
        PDATA += 4;                         \
    }

#define MB_READ_INT32(PDATA,val_u32)        \
    {                                       \
        temp_data_t temp = {0};             \
        temp.pdata[3] = PDATA[0];           \
        temp.pdata[2] = PDATA[1];           \
        temp.pdata[1] = PDATA[2];           \
        temp.pdata[0] = PDATA[3];           \
        val_u32 = temp.data_u32;            \
        PDATA += 4;                         \
    }

#define MB_WRITE_INT64(PDATA,val_u64)       \
    {                                       \
        temp_data_t temp = {0};             \
        temp.data_u64 = val_u64;            \
        PDATA[0] = temp.pdata[7];           \
        PDATA[1] = temp.pdata[6];           \
        PDATA[2] = temp.pdata[5];           \
        PDATA[3] = temp.pdata[4];           \
        PDATA[4] = temp.pdata[3];           \
        PDATA[5] = temp.pdata[2];           \
        PDATA[6] = temp.pdata[1];           \
        PDATA[7] = temp.pdata[0];           \
        PDATA += 8;                         \
    }

#define MB_READ_INT64(PDATA,val_u64)        \
    {                                       \
        temp_data_t temp = {0};             \
        temp.pdata[7] = PDATA[0];           \
        temp.pdata[6] = PDATA[1];           \
        temp.pdata[5] = PDATA[2];           \
        temp.pdata[4] = PDATA[3];           \
        temp.pdata[3] = PDATA[4];           \
        temp.pdata[2] = PDATA[5];           \
        temp.pdata[1] = PDATA[6];           \
        temp.pdata[0] = PDATA[7];           \
        val_u64 = temp.data_u64;            \
        PDATA += 8;                         \
    }

#define MB_WRITE_DOUBLE(PDATA,val_double)   \
    {                                       \
        temp_data_t temp = {0};             \
        temp.data_double = val_double;      \
        PDATA[0] = temp.pdata[7];           \
        PDATA[1] = temp.pdata[6];           \
        PDATA[2] = temp.pdata[5];           \
        PDATA[3] = temp.pdata[4];           \
        PDATA[4] = temp.pdata[3];           \
        PDATA[5] = temp.pdata[2];           \
        PDATA[6] = temp.pdata[1];           \
        PDATA[7] = temp.pdata[0];           \
        PDATA += 8;                         \
    }

#define MB_READ_DOUBLE(PDATA,val_double)    \
    {                                       \
        temp_data_t temp = {0};             \
        temp.pdata[7] = PDATA[0];           \
        temp.pdata[6] = PDATA[1];           \
        temp.pdata[5] = PDATA[2];           \
        temp.pdata[4] = PDATA[3];           \
        temp.pdata[3] = PDATA[4];           \
        temp.pdata[2] = PDATA[5];           \
        temp.pdata[1] = PDATA[6];           \
        temp.pdata[0] = PDATA[7];           \
        val_double = temp.data_double;      \
        PDATA += 8;                         \
    }

#define MB_WRITE_FLOAT(PDATA,val_float)     \
    {                                       \
        temp_data_t temp = {0};             \
        temp.data_float = val_float;        \
        PDATA[0] = temp.pdata[3];           \
        PDATA[1] = temp.pdata[2];           \
        PDATA[2] = temp.pdata[1];           \
        PDATA[3] = temp.pdata[0];           \
        PDATA += 4;                         \
    }

#define MB_READ_FLOAT(PDATA,val_float)      \
    {                                       \
        temp_data_t temp = {0};             \
        temp.pdata[3] = PDATA[0];           \
        temp.pdata[2] = PDATA[1];           \
        temp.pdata[1] = PDATA[2];           \
        temp.pdata[0] = PDATA[3];           \
        val_float = temp.data_float;        \
        PDATA += 4;                         \
    }



#endif
