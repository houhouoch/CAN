#ifndef __CAN_TABLE__
#define __CAN_TABLE__

#include "commd.h"
#include "stdint.h"



#define CAN_OUTSET_PARA_REG_OFFSET      0x10000
#define CAN_OUTSET_SER_REG_OFFSET       0x20000
#define CAN_OUTSET_SYNERGY_REG_OFFSET   0x30000

#define CAN_OUTREAD_REG_OFFSET          0xA0000
#define CAN_OUTREAD_MULTCH_REG_OFFSET   0xB0000

enum {
    CAN_SCPI_M2S_ID           = 0xC0000,  //CAN_SCPI_M2S_ID + (id*0x100)
    CAN_SCPI_S2M_ID           = 0xC0001,  //CAN_SCPI_S2M_ID + (id*0x100)
    CAN_SCPI_GROUP_ID         = 0xC0002,  //
};

#if 1
typedef union {
    uint64_t data_u64;
    uint32_t data_u32;
    uint16_t data_u16;
    double data_double;
    float data_float;
    uint8_t pdata[8];
} temp_data_t;


#define CAN_WRITE_INT16(PDATA,val_u16)       \
    {                                       \
        temp_data_t temp = {0};             \
        temp.data_u16 = val_u16;            \
        PDATA[0] = temp.pdata[1];           \
        PDATA[1] = temp.pdata[0];           \
        PDATA += 2;                         \
    }

#define CAN_READ_INT16(PDATA,val_u16)        \
    {                                       \
        temp_data_t temp = {0};             \
        temp.pdata[1] = PDATA[0];           \
        temp.pdata[0] = PDATA[1];           \
        val_u16 = temp.data_u16;            \
        PDATA += 2;                         \
    }

#define CAN_WRITE_INT32(PDATA,val_u32)       \
    {                                       \
        temp_data_t temp = {0};             \
        temp.data_u32 = val_u32;            \
        PDATA[0] = temp.pdata[3];           \
        PDATA[1] = temp.pdata[2];           \
        PDATA[2] = temp.pdata[1];           \
        PDATA[3] = temp.pdata[0];           \
        PDATA += 4;                         \
    }

#define CAN_READ_INT32(PDATA,val_u32)        \
    {                                       \
        temp_data_t temp = {0};             \
        temp.pdata[3] = PDATA[0];           \
        temp.pdata[2] = PDATA[1];           \
        temp.pdata[1] = PDATA[2];           \
        temp.pdata[0] = PDATA[3];           \
        val_u32 = temp.data_u32;            \
        PDATA += 4;                         \
    }

#define CAN_WRITE_INT64(PDATA,val_u64)       \
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

#define CAN_READ_INT64(PDATA,val_u64)        \
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

#define CAN_WRITE_DOUBLE(PDATA,val_double)   \
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

#define CAN_READ_DOUBLE(PDATA,val_double)    \
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

#define CAN_WRITE_FLOAT(PDATA,val_float)     \
    {                                       \
        temp_data_t temp = {0};             \
        temp.data_float = val_float;        \
        PDATA[0] = temp.pdata[3];           \
        PDATA[1] = temp.pdata[2];           \
        PDATA[2] = temp.pdata[1];           \
        PDATA[3] = temp.pdata[0];           \
        PDATA += 4;                         \
    }

#define CAN_READ_FLOAT(PDATA,val_float)      \
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

uint8_t __CAN_isMode(uint8_t powerMode);


void CAN_OutSet_Parallel_Master_Update(void);
uint8_t CAN_OutSet_Parallel_Slave_Action(uint32_t id, uint8_t *pdata);

void CAN_OutSet_Series_Master_Update(void);
uint8_t CAN_OutSet_Series_Slave_Action(uint32_t id, uint8_t *pdata);

#if defined(MULT_CHANNEL)
    void CAN_OutSet_Synergy_Master_Update(void);
    uint8_t CAN_OutSet_Synergy_Slave_Action(uint32_t id, uint8_t *pdata);

    void CAN_OutSet_Synergy_Slave_Update(void);
    uint8_t CAN_OutSet_Synergy_Master_Action(uint32_t id, uint8_t *pdata);
#endif

void CAN_OutSet_Read_Update(void);
uint8_t CAN_OutSet_Read_Action(uint32_t id, uint8_t *pdata);

#if defined(MULT_CHANNEL)
    void CAN_OutSet_Read_Update_MultCH(void);
    uint8_t CAN_OutSet_Read_Action_MultCH(uint32_t id, uint8_t *pdata);
#endif

#endif
