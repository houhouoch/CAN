#include "bmpImage.h"

BmpFile bmpFile = {
    0
};
void bmp_file_init(void)
{
    const uint32_t width = LCD_W;
    const uint32_t height = LCD_H;
#define RGB565_BITS 16
    uint32_t data_size = width * height * (RGB565_BITS / 8);
    /*initialize bmp structs*/
    bmpFile.file_head.bfType = 0x4d42;//文件类型，0x4D42也就是字符'BM'
    bmpFile.file_head.bfSize = sizeof(bmpFile.file_head) + sizeof(bmpFile.info_head) + sizeof(RGBQUAD) * 3 + data_size;
    bmpFile.file_head.bfReserved1 = bmpFile.file_head.bfReserved2 = 0;
    bmpFile.file_head.bfOffBits = sizeof(bmpFile.file_head) + sizeof(bmpFile.info_head) + sizeof(RGBQUAD) * 3;
    bmpFile.info_head.biSize = sizeof(bmpFile.info_head);
    bmpFile.info_head.biWidth = width;              //指定图像的宽度，单位是像素
    bmpFile.info_head.biHeight = -height;            //指定图像的高度，单位是像素
    bmpFile.info_head.biPlanes = 1;                 //目标设备的级别，必须是1
    bmpFile.info_head.biBitCount = RGB565_BITS;     //表示用到颜色时用到的位数 16位表示高彩色图
    bmpFile.info_head.biCompression = 3;            //BI_BITFIELDS;//位域存放方式，根据调色板掩码知道RGB占bit数
    bmpFile.info_head.biSizeImage = data_size;      //指定实际位图所占字节数
    bmpFile.info_head.biXPelsPerMeter = 0;          //水平分辨率，用象素/米表示。
    bmpFile.info_head.biYPelsPerMeter = 0;          //垂直分辨率，用象素/米表示
    bmpFile.info_head.biClrUsed = 0;                //位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）
    bmpFile.info_head.biClrImportant = 0;           //说明对图象显示有重要影响的颜色索引的数目，0表示所有颜色都重要
    //RGB565格式掩码
    bmpFile.rgb_quad[0].rgbBlue = 0;
    bmpFile.rgb_quad[0].rgbGreen = 0xF8;
    bmpFile.rgb_quad[0].rgbRed = 0;
    bmpFile.rgb_quad[0].rgbReserved = 0;
    bmpFile.rgb_quad[1].rgbBlue = 0xE0;
    bmpFile.rgb_quad[1].rgbGreen = 0x07;
    bmpFile.rgb_quad[1].rgbRed = 0;
    bmpFile.rgb_quad[1].rgbReserved = 0;
    bmpFile.rgb_quad[2].rgbBlue = 0x1F;
    bmpFile.rgb_quad[2].rgbGreen = 0;
    bmpFile.rgb_quad[2].rgbRed = 0;
    bmpFile.rgb_quad[2].rgbReserved = 0;
}






/**
 * 自动生成不重复的文件名
 * @param path 基础路径，例如 "0:/LOGS/"
 * @param base_name 基础文件名，例如 "data"
 * @param ext 文件扩展名，例如 "txt"
 * @param new_name 生成的新文件名缓冲区
 * @param max_length 缓冲区最大长度
 * @return 成功返回FR_OK，否则返回错误码
 */
FRESULT generate_unique_filename(const char *path, const char *base_name, const char *ext,
                                 char *new_name, size_t max_length)
{
    FRESULT res;
    FILINFO fno;
    char temp_name[256];
    int index = 0;
    // 先尝试基础文件名
    snprintf(temp_name, sizeof(temp_name), "%s%s.%s", path, base_name, ext);
    // 检查文件是否存在
    res = f_stat(temp_name, &fno);
    // 如果文件不存在，直接使用基础文件名
    if(res == FR_NO_FILE) {
        strncpy(new_name, temp_name, max_length);
        return FR_OK;
    }
    // 如果文件已存在，尝试添加序号
    while(1) {
        // 生成带序号的文件名，格式为：base_name_001.ext
        snprintf(temp_name, sizeof(temp_name), "%s%s_%03d.%s", path, base_name, index, ext);
        // 检查文件是否存在
        res = f_stat(temp_name, &fno);
        // 如果文件不存在，使用此文件名
        if(res == FR_NO_FILE) {
            strncpy(new_name, temp_name, max_length);
            return FR_OK;
        }
        // 增加序号，继续尝试
        index++;
        // 防止无限循环，设置最大尝试次数
        if(index > 999) {
            return FR_EXIST; // 达到最大尝试次数
        }
    }
}

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
                       char *created_name, size_t max_name_length)
{
    FRESULT res;
    FIL file;
    char filename[256];
    UINT bytes_written;
    // 生成唯一文件名
    res = generate_unique_filename(path, base_name, ext, filename, sizeof(filename));
    if(res != FR_OK) {
        return res;
    }
    // 保存文件名（如果需要）
    if(created_name && max_name_length > 0) {
        strncpy(created_name, filename, max_name_length);
    }
    // 创建并写入文件
    res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(res != FR_OK) {
        return res;
    }
    uint8_t *pdata = (uint8_t *)&bmpFile;
    for(uint32_t i = 0; i < sizeof(bmpFile);)
    {
        UINT byteswitten = 0;
        if((sizeof(bmpFile) - i) > 4096) {
            f_write(&file, &pdata[i], 4096, &byteswitten);
            i += 4096;
        } else {
            uint16_t writecount = (sizeof(bmpFile) - i);
            f_write(&file, &pdata[i], writecount, &byteswitten);
            i += writecount;
        }
        f_sync(&file);
    }
    f_close(&file);
    return res;
}

int32_t bmp_file_saveScreen(const char *file_path)
{
    char actual_filename[256];
    return save_file_safe("USB:/", "ScreenShot", "bmp",
                          actual_filename, sizeof(actual_filename));
}




FRESULT __copy_debugFile(const char *src_path)
{
    if(src_path == NULL) while(1);
    FRESULT res;
    FIL file;
    char filename[256];
    UINT bytes_written;
    // 生成唯一文件名
    res = generate_unique_filename("USB:/", "DebugFile", "txt", filename, sizeof(filename));
    if(res != FR_OK) {
        return res;
    }
    return f_copy(src_path, filename);
}



/*
 * 功能：复制文件
 * 参数：
 *   src_path - 源文件路径
 *   dst_path - 目标文件路径
 * 返回值：
 *   FR_OK - 成功
 *   其他值 - 错误代码（参考FATFS定义）
 */
FRESULT f_copy(const TCHAR *src_path, const TCHAR *dst_path)
{
    FIL src_file, dst_file;  // 文件对象
    FRESULT res;             // 结果代码
    BYTE buffer[256];        // 数据缓冲区
    UINT bytes_read, bytes_written;  // 读写字节数
    // 打开源文件（只读模式）
    res = f_open(&src_file, src_path, FA_READ);
    if(res != FR_OK) {
        return res;  // 源文件打开失败
    }
    // 创建并打开目标文件（只写模式，如果存在则覆盖）
    res = f_open(&dst_file, dst_path, FA_WRITE | FA_CREATE_ALWAYS);
    if(res != FR_OK) {
        f_close(&src_file);  // 关闭已打开的源文件
        return res;          // 目标文件创建失败
    }
    // 循环读写数据
    do {
        // 从源文件读取数据
        res = f_read(&src_file, buffer, sizeof(buffer), &bytes_read);
        if(res != FR_OK || bytes_read == 0) {
            break;  // 读取错误或已到文件末尾
        }
        // 将读取的数据写入目标文件
        res = f_write(&dst_file, buffer, bytes_read, &bytes_written);
        if(res != FR_OK || bytes_written != bytes_read) {
            break;  // 写入错误或写入字节数不匹配
        }
    } while(1);
    // 检查是否是正常结束
    if(res == FR_OK && bytes_read == 0) {
        res = FR_OK;  // 复制成功
    } else {
        // 复制过程中出错，删除目标文件
        f_close(&dst_file);
        f_unlink(dst_path);
    }
    // 关闭文件
    f_close(&src_file);
    f_close(&dst_file);
    return res;
}

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
                             void (*progress)(int))
{
    FIL src_file, dst_file;
    FRESULT res;
    BYTE buffer[1024];
    UINT bytes_read, bytes_written;
    FSIZE_t total_size, copied_size = 0;
    int last_progress = -1;
    // 打开源文件
    res = f_open(&src_file, src_path, FA_READ);
    if(res != FR_OK) { return res; }
    // 获取文件总大小
    total_size = f_size(&src_file);
    if(total_size == 0) {
        f_close(&src_file);
        // 处理空文件
        res = f_open(&dst_file, dst_path, FA_WRITE | FA_CREATE_ALWAYS);
        if(res == FR_OK) { f_close(&dst_file); }
        if(progress) { progress(100); }
        return res;
    }
    // 创建目标文件
    res = f_open(&dst_file, dst_path, FA_WRITE | FA_CREATE_ALWAYS);
    if(res != FR_OK) {
        f_close(&src_file);
        return res;
    }
    // 循环复制
    do {
        res = f_read(&src_file, buffer, sizeof(buffer), &bytes_read);
        if(res != FR_OK || bytes_read == 0) { break; }
        res = f_write(&dst_file, buffer, bytes_read, &bytes_written);
        if(res != FR_OK || bytes_written != bytes_read) { break; }
        // 更新进度
        copied_size += bytes_written;
        if(progress) {
            int p = (int)((copied_size * 100) / total_size);
            if(p != last_progress) {
                progress(p);
                last_progress = p;
            }
        }
    } while(1);
    // 确保进度达到100%
    if(res == FR_OK && progress) {
        progress(100);
    } else {
        // 出错处理
        f_close(&dst_file);
        f_unlink(dst_path);
    }
    // 关闭文件
    f_close(&src_file);
    f_close(&dst_file);
    return res;
}

/*
 * 辅助函数：将字节转换为人类可读的单位（KB/MB/GB）
 * 参数：
 *   bytes - 字节数
 *   unit - 输出单位（0=KB, 1=MB, 2=GB）
 * 返回值：
 *   转换后的数值
 */
double f_bytes_to_unit(uint64_t bytes, int unit) {
    double val = bytes;
    switch(unit) {
        case FS_UNIT_KB:
            val /= 1024;
            break;
        case FS_UNIT_MB:
            val /= 1024 * 1024;
            break;
        case FS_UNIT_GB:
            val /= 1024 * 1024 * 1024;
            break;
    }
    return val;
}

// 使用示例
uint8_t f_check_disk_space(const char *disk, float *total, float *free, uint8_t unit) {
    FATFS *fs;
    DWORD free_clusters, total_clusters;
    FRESULT res;
    uint8_t present = 0;
    // 1. 获取文件系统信息（关键：通过FATFS标准函数获取簇信息）
    res = f_getfree(disk, &free_clusters, &fs);
    if(res != FR_OK) {
        return present;
    }
    // 2. 计算总容量和空闲容量（字节数）
    total_clusters = fs->n_fatent - 2; // 总簇数 = FAT表项数 - 2（保留簇）
    uint64_t total_bytes = (uint64_t)total_clusters * fs->csize * _MAX_SS; // 总容量=总簇数×每簇扇区数×扇区大小
    uint64_t free_bytes = (uint64_t)free_clusters * fs->csize * _MAX_SS;  // 空闲容量=空闲簇数×每簇扇区数×扇区大小
    // 3. 单位转换
    *total = f_bytes_to_unit(total_bytes, unit);
    *free = f_bytes_to_unit(free_bytes, unit);
    present = (*free * 100) / *total;
    return present;
}





int f_linesCount(const TCHAR *filename) {
    FIL file;
    FRESULT res;
    UINT bytes_read;
    BYTE buffer[512];
    int lines = 0;
    int i;
    // 尝试打开文件
    res = f_open(&file, filename, FA_READ);
    if(res != FR_OK) {
        return -1;  // 文件不存在或打开失败
    }
    // 读取文件并计数换行符
    do {
        res = f_read(&file, buffer, sizeof(buffer), &bytes_read);
        if(res != FR_OK) { break; }
        for(i = 0; i < bytes_read; i++) {
            if(buffer[i] == '\n') {
                lines++;
            }
        }
    } while(bytes_read > 0);
    f_close(&file);
    return lines;
}




/*
 * 功能：快速删除文件前N行
 * 参数：
 *   path - 文件路径
 *   n - 要删除的行数（若文件行数<=n，结果为清空文件）
 * 返回值：
 *   FR_OK - 成功
 *   其他 - 错误码
 */
FRESULT f_delete_first_n_lines(const TCHAR *path, uint32_t n) {
#define TEMP_FILE "FLASH:_temp.tmp"  // 临时文件路径（与原文件同目录）
#define BUFFER_SIZE 512       // 缓冲区大小（8KB，可根据内存调整）
    FIL src_file, dst_file;
    FRESULT res;
    UINT br, bw;
    BYTE buffer[BUFFER_SIZE];
    uint32_t lines_skipped = 0;  // 已跳过的行数
    uint32_t pos = 0;            // 当前缓冲区处理位置
    uint32_t total_read = 0;     // 累计读取字节数
    uint8_t in_buffer = 0;       // 缓冲区中是否有未处理数据
    // 1. 打开源文件（只读）和临时文件（创建+写入）
    res = f_open(&src_file, path, FA_READ);
    if(res != FR_OK) { return res; }
    res = f_open(&dst_file, TEMP_FILE, FA_WRITE | FA_CREATE_ALWAYS);
    if(res != FR_OK) {
        f_close(&src_file);
        return res;
    }
    // 2. 循环读取源文件，跳过前N行，剩余内容写入临时文件
    do {
        // 若缓冲区中无数据，读取新数据
        if(!in_buffer) {
            res = f_read(&src_file, buffer, BUFFER_SIZE, &br);
            if(res != FR_OK || br == 0) { break; }   // 读取完毕或出错
            total_read = br;
            pos = 0;
            in_buffer = 1;
        }
        // 3. 在缓冲区中查找换行符，统计并跳过前N行
        while(pos < total_read) {
            if(buffer[pos] == '\n') {   // 遇到换行符，行数+1
                lines_skipped++;
                pos++;  // 跳过换行符本身
                // 若已跳过N行，剩余内容全部写入临时文件
                if(lines_skipped >= n) {
                    // 写入当前缓冲区剩余数据
                    uint32_t remaining = total_read - pos;
                    if(remaining > 0) {
                        res = f_write(&dst_file, &buffer[pos], remaining, &bw);
                        if(res != FR_OK || bw != remaining) { goto error; }
                    }
                    // 写入后续所有数据（直接大块复制，无需逐行检查）
                    do {
                        res = f_read(&src_file, buffer, BUFFER_SIZE, &br);
                        if(res != FR_OK) { goto error; }
                        if(br == 0) { break; }
                        res = f_write(&dst_file, buffer, br, &bw);
                        if(res != FR_OK || bw != br) { goto error; }
                    } while(1);
                    in_buffer = 0;  // 标记缓冲区已处理完毕
                    goto copy_done;  // 跳转至后续处理
                }
            } else {
                pos++;  // 未到换行符，继续移动指针
            }
        }
        // 若缓冲区处理完仍未跳过N行，说明当前缓冲区无完整换行符，继续读下一块
        in_buffer = 0;
    } while(1);
copy_done:
    // 4. 处理特殊情况：文件行数 <= N（此时临时文件应为空）
    if(lines_skipped < n) {
        // 截断临时文件（确保为空）
        f_lseek(&dst_file, 0);
        f_truncate(&dst_file);
    }
    // 5. 关闭文件，替换原文件
    f_close(&src_file);
    f_close(&dst_file);
    // 删除原文件，重命名临时文件
    f_unlink(path);
    res = f_rename(TEMP_FILE, path);
    return res;
error:
    // 出错时清理临时文件
    f_close(&src_file);
    f_close(&dst_file);
    f_unlink(TEMP_FILE);
    return res;
}
