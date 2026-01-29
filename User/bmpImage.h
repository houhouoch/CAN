#ifndef _BMP_IMAGE_H_
#define _BMP_IMAGE_H_

#include "stdint.h"
#include <string.h>
#include "Fatfs.h"

#include "LVGL.h"

typedef struct tagBITMAPFILEHEADER {
    WORD  bfType;               //位图文件的类型，在Windows中，此字段的值总为‘BM’(1-2字节）
    DWORD bfSize;               //位图文件的大小，以字节为单位（3-6字节，低位在前)
    WORD  bfReserved1;          //位图文件保留字，必须为0(7-8字节）
    WORD  bfReserved2;          //位图文件保留字，必须为0(9-10字节）
    DWORD bfOffBits;            //位图数据的起始位置，以相对于位图（11-14字节，低位在前）
} __attribute__((packed)) BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;               //本结构所占用字节数（15-18字节）
    LONG  biWidth;              //位图的宽度，以像素为单位（19-22字节）
    LONG  biHeight;             //位图的高度，以像素为单位（23-26字节
    WORD  biPlanes;             //目标设备的级别，必须为1(27-28字节）
    WORD  biBitCount;           //每个像素所需的位数，必须是1（双色），（29-30字节）//4(16色），8(256色）16(高彩色)或24（真彩色）之一
    DWORD biCompression;        //位图压缩类型，必须是0（不压缩），（31-34字节）    //1(BI_RLE8压缩类型）或2(BI_RLE4压缩类型）之一
    DWORD biSizeImage;          //位图的大小(其中包含了为了补齐行数是4的倍数而添加的空字节)，以字节为单位（35-38字节）
    LONG  biXPelsPerMeter;      //位图水平分辨率，像素数（39-42字节）
    LONG  biYPelsPerMeter;      //位图垂直分辨率，像素数（43-46字节)
    DWORD biClrUsed;            //位图实际使用的颜色表中的颜色数（47-50字节）
    DWORD biClrImportant;       //位图显示过程中重要的颜色数（51-54字节）
} __attribute__((packed)) BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} __attribute__((packed)) RGBQUAD;

#include "LCD.h"
typedef struct {
    BITMAPFILEHEADER file_head; //定义文件头
    BITMAPINFOHEADER info_head; //定义信息头
    RGBQUAD rgb_quad[3];        //定义调色板
    uint16_t FrameBuffer[LCD_H][LCD_W];
} BmpFile;

extern BmpFile bmpFile;

void bmp_file_init(void);

/**
 * 安全保存文件，自动处理重名
 * @param path 保存路径，例如 "0:/LOGS/"
 * @param base_name 基础文件名，例如 "data"
 * @param ext 文件扩展名，例如 "txt"
 * @param data 要保存的数据
 * @param size 数据大小（字节）
 * @param created_name 如果不为NULL，返回实际创建的文件名
 * @param max_name_length created_name缓冲区最大长度
 * @return 成功返回FR_OK，否则返回错误码
 */
FRESULT save_file_safe(const char *path, const char *base_name, const char *ext,
                       char *created_name, size_t max_name_length);

int32_t bmp_file_saveScreen(const char *file_path);







/*
 * 功能：复制文件
 * 参数：
 *   src_path - 源文件路径
 *   dst_path - 目标文件路径
 * 返回值：
 *   FR_OK - 成功
 *   其他值 - 错误代码（参考FATFS定义）
 */
FRESULT f_copy(const TCHAR *src_path, const TCHAR *dst_path);
/*
 * 功能：带进度的复制文件
 * 参数：
 *   src_path - 源文件路径
 *   dst_path - 目标文件路径
 *   progress - 进度回调函数(0-100)
 * 返回值：
 *   FR_OK - 成功
 *   其他值 - 错误代码
 */
FRESULT f_copy_with_progress(const TCHAR *src_path, const TCHAR *dst_path,
                             void (*progress)(int));

FRESULT __copy_debugFile(const char *src_path);


enum {
    FS_UNIT_KB = 0,
    FS_UNIT_MB,
    FS_UNIT_GB,
};

uint8_t f_check_disk_space(const char *disk, float *total, float *free, uint8_t unit);
int f_linesCount(const TCHAR *filename);
FRESULT f_delete_first_n_lines(const TCHAR *path, uint32_t n) ;
#endif