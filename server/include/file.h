//
// Created by Junyi Hou on 7/21/20.
//

#ifndef FUCK_STORAGE_SERVER_FILE_H
#define FUCK_STORAGE_SERVER_FILE_H

#include <cstdint>

// 获取文件大小
// 失败返回 -1
uint32_t file_size(const char *filename);

// 获取文件【读】指针
// 成功返回 mmap映射的地址，失败返回 nullptr
void *file_get(int disk_id, const char *filename);

// 设置文件基本信息
void file_set(const char *filename, uint32_t filesize);

// 磁盘编号 [0,12), 文件名，要写的数据缓冲区，要写多少字节
int file_write(int disk_id, const char *filename, void *buffer, uint32_t length);

// 挂载磁盘（防止服务器磁盘没有挂载）
int disk_mount(int disk_id);

#endif //FUCK_STORAGE_SERVER_FILE_H
