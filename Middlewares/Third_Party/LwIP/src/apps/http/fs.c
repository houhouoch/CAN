/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/apps/httpd_opts.h"
#include "lwip/def.h"
#include "lwip/apps/fs.h"
#include <string.h>


#include HTTPD_FSDATA_FILE

/*-----------------------------------------------------------------------------------*/

#if LWIP_HTTPD_CUSTOM_FILES
    int fs_open_custom(struct fs_file *file, const char *name);
    void fs_close_custom(struct fs_file *file);
    #if LWIP_HTTPD_FS_ASYNC_READ
        u8_t fs_canread_custom(struct fs_file *file);
        u8_t fs_wait_read_custom(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg);
        int fs_read_async_custom(struct fs_file *file, char *buffer, int count, fs_wait_cb callback_fn, void *callback_arg);
    #else /* LWIP_HTTPD_FS_ASYNC_READ */
        int fs_read_custom(struct fs_file *file, char *buffer, int count);
    #endif /* LWIP_HTTPD_FS_ASYNC_READ */
#endif /* LWIP_HTTPD_CUSTOM_FILES */


#include <ff.h>
#include <lwip/apps/fs.h>
#include <lwip/mem.h>
#include <string.h>
#include <UserMemory.h>
#include "bmpImage.h"

int fs_open_custom(struct fs_file *file, const char *name)
{
    char path[260];
    int i, size;
    int ret = 0;
    void *mem;
    FIL *fp;
    FRESULT fr;
    UINT br;
    if(strcmp(name, "/Screen.bmp") == 0) {
        file->flags = 1;
        file->data = (void *)&bmpFile; // 要显示的网页内容
        file->pextension = NULL; // http连接关闭时要在fs_close_custom()函数中释放的内存块
        file->len = sizeof(bmpFile); // 要显示的网页内容的长度
        file->index = sizeof(bmpFile); // 要显示的网页内容的长度
        ret = 1;
        return 1;
    }
    // name为要访问的网页URL, 是相对路径, 将其转换成绝对路径
    snprintf(path, sizeof(path), "USB:%s", name);
    for(i = 0; path[i] != '\0'; i++)
    {
        if(path[i] == '/')
        { path[i] = '\\'; }
    }
    if(path[i - 1] == '\\' && sizeof(path) - i >= 10 + 1)
    { strcat(path, "index.html"); }
    fp = UserMemory_Malloc(sizeof(FIL));
    if(fp != NULL)
    {
        fr = f_open(fp, path, FA_READ); // 打开文件
        if(fr == FR_OK)
        {
            size = f_size(fp); // 获取文件大小
            mem = UserMemory_Malloc(size);
            if(mem != NULL)
            {
                f_read(fp, mem, size, &br); // 一次性读完整个文件
                file->flags = 1;
                file->data = mem; // 要显示的网页内容
                file->pextension = mem; // http连接关闭时要在fs_close_custom()函数中释放的内存块
                file->len = size; // 要显示的网页内容的长度
                file->index = size; // 要显示的网页内容的长度
                printf("%s(0x%p): %s\n", __func__, file->pextension, path);
                ret = 1;
            }
            f_close(fp); // 关闭文件
        }
        UserMemory_Free(fp);
    }
    // 函数返回1表示网页文件存在, 返回0表示网页文件不存在
    // 网页文件不存在时httpd会去加载lwip-2.1.3/apps/http/makefsdata里面的默认网页
    return ret;
}

void fs_close_custom(struct fs_file *file)
{
    printf("%s(0x%p)\n", __func__, file->pextension);
    if(file->pextension != NULL)
    {
        UserMemory_Free(file->pextension);
        file->pextension = NULL;
    }
}


/*-----------------------------------------------------------------------------------*/
err_t
fs_open(struct fs_file *file, const char *name)
{
    const struct fsdata_file *f;
    if((file == NULL) || (name == NULL)) {
        return ERR_ARG;
    }
    #if LWIP_HTTPD_CUSTOM_FILES
    if(fs_open_custom(file, name)) {
        file->is_custom_file = 1;
        return ERR_OK;
    }
    file->is_custom_file = 0;
    #endif /* LWIP_HTTPD_CUSTOM_FILES */
    for(f = FS_ROOT; f != NULL; f = f->next) {
        if(!strcmp(name, (const char *)f->name)) {
            file->data = (const char *)f->data;
            file->len = f->len;
            file->index = f->len;
            file->pextension = NULL;
            file->flags = f->flags;
            #if HTTPD_PRECALCULATED_CHECKSUM
            file->chksum_count = f->chksum_count;
            file->chksum = f->chksum;
            #endif /* HTTPD_PRECALCULATED_CHECKSUM */
            #if LWIP_HTTPD_FILE_STATE
            file->state = fs_state_init(file, name);
            #endif /* #if LWIP_HTTPD_FILE_STATE */
            return ERR_OK;
        }
    }
    /* file not found */
    return ERR_VAL;
}

/*-----------------------------------------------------------------------------------*/
void
fs_close(struct fs_file *file)
{
    #if LWIP_HTTPD_CUSTOM_FILES
    if(file->is_custom_file) {
        fs_close_custom(file);
    }
    #endif /* LWIP_HTTPD_CUSTOM_FILES */
    #if LWIP_HTTPD_FILE_STATE
    fs_state_free(file, file->state);
    #endif /* #if LWIP_HTTPD_FILE_STATE */
    LWIP_UNUSED_ARG(file);
}
/*-----------------------------------------------------------------------------------*/
#if LWIP_HTTPD_DYNAMIC_FILE_READ
#if LWIP_HTTPD_FS_ASYNC_READ
    int
    fs_read_async(struct fs_file *file, char *buffer, int count, fs_wait_cb callback_fn, void *callback_arg)
#else /* LWIP_HTTPD_FS_ASYNC_READ */
    int
    fs_read(struct fs_file *file, char *buffer, int count)
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
{
    int read;
    if(file->index == file->len) {
        return FS_READ_EOF;
    }
    #if LWIP_HTTPD_FS_ASYNC_READ
    LWIP_UNUSED_ARG(callback_fn);
    LWIP_UNUSED_ARG(callback_arg);
    #endif /* LWIP_HTTPD_FS_ASYNC_READ */
    #if LWIP_HTTPD_CUSTOM_FILES
    if(file->is_custom_file) {
        #if LWIP_HTTPD_FS_ASYNC_READ
        return fs_read_async_custom(file, buffer, count, callback_fn, callback_arg);
        #else /* LWIP_HTTPD_FS_ASYNC_READ */
        return fs_read_custom(file, buffer, count);
        #endif /* LWIP_HTTPD_FS_ASYNC_READ */
    }
    #endif /* LWIP_HTTPD_CUSTOM_FILES */
    read = file->len - file->index;
    if(read > count) {
        read = count;
    }
    MEMCPY(buffer, (file->data + file->index), read);
    file->index += read;
    return (read);
}
#endif /* LWIP_HTTPD_DYNAMIC_FILE_READ */
/*-----------------------------------------------------------------------------------*/
#if LWIP_HTTPD_FS_ASYNC_READ
int
fs_is_file_ready(struct fs_file *file, fs_wait_cb callback_fn, void *callback_arg)
{
    if(file != NULL) {
        #if LWIP_HTTPD_FS_ASYNC_READ
        #if LWIP_HTTPD_CUSTOM_FILES
        if(!fs_canread_custom(file)) {
            if(fs_wait_read_custom(file, callback_fn, callback_arg)) {
                return 0;
            }
        }
        #else /* LWIP_HTTPD_CUSTOM_FILES */
        LWIP_UNUSED_ARG(callback_fn);
        LWIP_UNUSED_ARG(callback_arg);
        #endif /* LWIP_HTTPD_CUSTOM_FILES */
        #endif /* LWIP_HTTPD_FS_ASYNC_READ */
    }
    return 1;
}
#endif /* LWIP_HTTPD_FS_ASYNC_READ */
/*-----------------------------------------------------------------------------------*/
int
fs_bytes_left(struct fs_file *file)
{
    return file->len - file->index;
}
