#ifndef _MSG_LIST_H
#define _MSG_LIST_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "freertos.h"
#if 0
    #define List_malloc(x)  malloc(x)
    #define List_free(x)    free(x)
    #define List_Printf     printf
#else
    #define List_malloc(x)  pvPortMalloc(x)
    #define List_free(x)    vPortFree(x)
    #define List_Printf     printf
#endif


typedef struct MsgList_Def
{
    struct MsgList_Def *next;  //前一个
    uint8_t *pdata;
    uint16_t len;
} MsgList_Def;

//创建一个列表
MsgList_Def *MsgList_Create(uint8_t *pdata, uint16_t len);

//删除列表里的一个子项
uint16_t MsgList_Delete(MsgList_Def *linkList, uint16_t idx);

//删除列表里所有子项
uint16_t MsgList_DeleteAll(MsgList_Def *linkList);

//获取列表个数
uint16_t MsgList_Count(MsgList_Def *linkList);

//尾部追加
uint16_t MsgList_Append(MsgList_Def *linkList, uint8_t *pdata, uint16_t len);

MsgList_Def *MsgList_At(MsgList_Def *linkList, uint16_t i);
#endif
