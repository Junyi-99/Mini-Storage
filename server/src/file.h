//
// Created by Junyi Hou on 7/21/20.
//

#ifndef FUCK_STORAGE_SERVER_DISK_H
#define FUCK_STORAGE_SERVER_DISK_H

// 挂载磁盘
// 申请空间
// mmap 映射
// 写文件到指定磁盘

#include <cstdint>

// 获取文件基本信息（文件名、大小、多少个块等等）
void *disk_info(int disk_id, const char *filename) {

}

// 磁盘编号，文件名
void *disk_read(int disk_id, const char *filename) {

}

// 写文件块数据
/*
 * open
 * truncate
 * fsync
 * mmap
 *
 *
 * */
// 磁盘编号 [0,12), 文件名，要写的数据缓冲区，要写多少字节
int disk_write(int disk_id, const char *filename, void *buffer, uint64_t length);

// 挂载磁盘（防止服务器磁盘没有挂载）
int disk_mount(int disk_id);

#endif //FUCK_STORAGE_SERVER_DISK_H
