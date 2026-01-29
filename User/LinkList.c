#include "LinkList.h"

uint16_t debug_linkList_Count = 0;

//创建一个列表
LinkList_Def *LinkList_Create(const void *device, void *path, void *fileName)
{
    LinkList_Def *newLinkList = LinkList_malloc(sizeof(struct LinkList_Def));
    if(newLinkList == NULL) {
        return 0;
    }
    if(path == NULL) {
        return 0;
    }
    if(fileName == NULL) {
        return 0;
    }
    memset(newLinkList, 0, sizeof(struct LinkList_Def));
    newLinkList->device = device;
    newLinkList->path = path;
    newLinkList->fileName = fileName;
    //    strcpy(newLinkList->path, path);
    //    strcpy(newLinkList->fileName, fileName);
    newLinkList->next = NULL;
    //debug
    debug_linkList_Count++;
    return newLinkList;
}

LinkList_Def *LinkList_At(LinkList_Def *linkList, uint16_t i)
{
    if(linkList == NULL) {
        return NULL;
    }
    LinkList_Def *pnode = linkList->next;
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
uint16_t LinkList_DeleteAll(LinkList_Def *linkList)
{
    if(linkList == NULL) {
        return 0;
    }
    LinkList_Def *pnode = linkList->next;
    linkList->next = NULL;
    if(pnode == NULL) {
        return 0;
    }
    uint16_t count = 0;
    while(1) {
        count++;
        debug_linkList_Count--;
        LinkList_Def *tempnode = pnode;
        pnode = pnode->next;
        {
            //删除节点
            if(tempnode->path != NULL) {
                LinkList_free(tempnode->path);
                tempnode->path = NULL;
            }
            if(tempnode->fileName != NULL) {
                LinkList_free(tempnode->fileName);
                tempnode->fileName = NULL;
            }
            tempnode->device = "";
            tempnode->next = NULL;
            LinkList_free(tempnode);
        }
        if(pnode == NULL) {
            break;
        }
    }
    return count;
}

//获取列表个数
uint16_t LinkList_Count(LinkList_Def *linkList)
{
    if(linkList == NULL) {
        return 0;
    }
    LinkList_Def *pnode = linkList->next;
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
uint16_t LinkList_Append(LinkList_Def *linkList, const void *device, void *path, void *fileName)
{
    if(linkList == NULL) {
        return 0;
    }
    if(fileName == NULL) {
        printf("Null Data!\r\n");
        return 0;
    }
    if(path == NULL) {
        printf("Null Data!\r\n");
        return 0;
    }
    LinkList_Def *node = LinkList_Create(device, path, fileName);
    if(node == NULL) {
        return 0;
    }
    LinkList_Def *pnode = linkList;
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



FRESULT FATFS_find(LinkList_Def *linkList, const char *device, const char *path, const char *Name)
{
    if(linkList == NULL) while(1);
    uint16_t linklist_count = 0;
    uint16_t fileCount = 0;
    DIR dir;
    /*  搜索文件名 */
    FILINFO temp_fno;
    FRESULT fr = f_findfirst(&dir, &temp_fno, path, Name); /* Start to search for photo files */
    while(fr == FR_OK && temp_fno.fname[0]) {          /* Repeat while an item is found */
        //        char fileName_Buf[64] = {0};
        //        char path_Buf[256] = {0};
        //        uint16_t fileName_length = sprintf(fileName_Buf, "%s", temp_fno.fname);              /* Print the object name */
        //        uint16_t path_Length = sprintf(path_Buf, "%s/%s", path, temp_fno.fname);             /* Print the object name */
        //        LinkList_Append(linkList, device, path_Buf, fileName_Buf);
        char fileName_Buf[256] = {0};
        char path_Buf[256] = {0};
        uint16_t fileName_length = snprintf(fileName_Buf, sizeof(fileName_Buf) - 1, "%s", temp_fno.fname) + 1;
        uint16_t path_Length = snprintf(path_Buf, sizeof(path_Buf) - 1, "%s/%s", path, temp_fno.fname) + 1;
        char *fileName = LinkList_malloc(fileName_length);
        char *pathName = LinkList_malloc(path_Length);
        if((fileName != NULL) && (pathName != NULL)) {
            memset(fileName, 0, fileName_length);
            memset(pathName, 0, path_Length);
            //
            strcpy(fileName, fileName_Buf);
            strcpy(pathName, path_Buf);
            if(linklist_count < LINKLIST_MAX_COUNT) {
                linklist_count++;
                LinkList_Append(linkList, device, pathName, fileName);
            }
        }
        fr = f_findnext(&dir, &temp_fno);               /* Search for next item */
        fileCount++;
    }
    f_closedir(&dir);
    return FR_OK;
}
