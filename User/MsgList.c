#include "MsgList.h"

uint16_t debug_msgList_Count = 0;
//创建一个列表
MsgList_Def *MsgList_Create(uint8_t *pdata, uint16_t len)
{
    if(pdata == NULL) {
        return 0;
    }
    MsgList_Def *newLinkList = List_malloc(sizeof(struct MsgList_Def));
    if(newLinkList == NULL) {
        return 0;
    }
    uint8_t *newData = List_malloc(len + 1);
    if(newData == NULL) {
        return 0;
    } else {
        memset(newData, 0, len + 1);
    }
    memset(newLinkList, 0, sizeof(struct MsgList_Def));
    memcpy(newData, pdata, len);
    newLinkList->pdata = newData;
    newLinkList->len = len;
    newLinkList->next = NULL;
    //debug
    debug_msgList_Count++;
    return newLinkList;
}

MsgList_Def *MsgList_At(MsgList_Def *linkList, uint16_t i)
{
    if(linkList == NULL) {
        return NULL;
    }
    MsgList_Def *pnode = linkList->next;
    if(pnode == NULL) {
        return NULL;
    }
    while(pnode != NULL)
    {
        if(i-- == 0) {
            return pnode;
        }
        pnode = pnode->next;
    }
    return pnode;
}

//删除列表里所有子项
uint16_t MsgList_DeleteAll(MsgList_Def *linkList)
{
    if(linkList == NULL) {
        return 0;
    }
    MsgList_Def *pnode = linkList->next;
    linkList->next = NULL;
    if(pnode == NULL) {
        return 0;
    }
    uint16_t count = 0;
    while(1) {
        count++;
        debug_msgList_Count--;
        MsgList_Def *tempnode = pnode;
        pnode = pnode->next;
        {
            //删除节点
            if(tempnode->pdata != NULL) {
                List_free(tempnode->pdata);
                tempnode->pdata = NULL;
            }
            tempnode->len = 0;
            tempnode->next = NULL;
            List_free(tempnode);
        }
        if(pnode == NULL) {
            break;
        }
    }
    return count;
}


//删除列表里的一个子项
uint16_t MsgList_Delete(MsgList_Def *linkList, uint16_t idx)
{
    if(linkList == NULL) {
        return 0;
    }
    MsgList_Def *pnode = linkList->next;
    if(pnode == NULL) {
        return 0;
    }
    while(idx > 0) {
        pnode = pnode->next;
        idx--;
        if(pnode == NULL) { return 0; }
    }
    MsgList_Def *tempnode = pnode;
    {
        //删除节点
        if(tempnode->pdata != NULL) {
            List_free(tempnode->pdata);
            tempnode->pdata = NULL;
        }
        tempnode->len = 0;
        tempnode->next = NULL;
        List_free(tempnode);
    }
    return 1;
}

//获取列表个数
uint16_t MsgList_Count(MsgList_Def *linkList)
{
    if(linkList == NULL) {
        return 0;
    }
    MsgList_Def *pnode = linkList->next;
    if(pnode == NULL) {
        return 0;
    }
    uint16_t count = 0;
    while(1) {
        count++;
        if(pnode->next == NULL) {
            break;
        }
        pnode = pnode->next;
    }
    return count;
}

#include "stdio.h"
//尾部追加
uint16_t MsgList_Append(MsgList_Def *linkList, uint8_t *pdata, uint16_t len)
{
    if(linkList == NULL) {
        return 0;
    }
    if(pdata == NULL) {
        List_Printf("Null Data!\r\n");
        return 0;
    }
    MsgList_Def *node = MsgList_Create(pdata, len);
    if(node == NULL) {
        return 0;
    }
    MsgList_Def *pnode = linkList;
    uint16_t count = 0;
    while(1) {
        count++;
        if(pnode->next == NULL) {
            pnode->next = node;
            break;
        }
        pnode = pnode->next;
    }
    return count;
}