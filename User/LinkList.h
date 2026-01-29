#ifndef _LINK_LIST_H
#define _LINK_LIST_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "freertos.h"


#define LINKLIST_MAX_COUNT (20)

#if 0
    #define LinkList_malloc(x)  malloc(x)
    #define LinkList_free(x)    free(x)
    #define LinkList_Printf     printf
#else
    #define LinkList_malloc(x)  pvPortMalloc(x)
    #define LinkList_free(x)    vPortFree(x)
    #define LinkList_Printf     printf
#endif

typedef struct LinkList_Def
{
    struct LinkList_Def *next;  //前一个
    const char *device;
    char *path;
    char *fileName;
} LinkList_Def;

//创建一个列表
LinkList_Def *LinkList_Create(const void *device, void *path, void *fileName);

//删除列表里所有子项
uint16_t LinkList_DeleteAll(LinkList_Def *linkList);

//获取列表个数
uint16_t LinkList_Count(LinkList_Def *linkList);

//尾部追加
uint16_t LinkList_Append(LinkList_Def *linkList, const void *device, void *path, void *fileName);

LinkList_Def *LinkList_At(LinkList_Def *linkList, uint16_t i);

#include "FATFS.h"
FRESULT FATFS_find(LinkList_Def *linkList, const char *device, const char *path, const char *Name);

#endif
