//
// Created by Junyi Hou on 7/21/20.
//

#include "file.h"
// 获取文件基本信息（文件名、大小、多少个块等等）
// 失败返回 -1
uint32_t file_size(const char *filename) {
    /*
         获取记录的文件信息里的 【文件大小】
         单位：字节

         成功返回 字节数
         失败返回 -1
     */
    return 0; // REPLACE YOUR CODE HERE
}

// 磁盘编号，文件名
void *file_get(int disk_id, const char *filename) {

    /*

     找到指定磁盘的指定文件

     mmap映射

     return ( mmap 返回的指针 )

     成功返回 mmap指针
     失败返回 nullptr
     */
    return nullptr;
}

// 设置文件基本信息
void file_set(const char *filename, uint32_t filesize) {
    // 记录文件信息到硬盘，给以后的 file_size() 使用

    // 按照分块策略，在服务器创建好文件
    // truncate 到指定大小
    // fsync 刷新缓冲区


}

// 写文件块数据

// 磁盘编号 [0,12), 文件名，要写的数据缓冲区，要写多少字节
int file_write(int disk_id, const char *filename, void *buffer, uint32_t length) {
    //TODO: 思考题：多个客户端同时写一个文件怎么办？

    /*

     open(指定硬盘的指定文件)
     mmap()
     写入数据

     */
}

// 挂载磁盘（防止服务器磁盘没有挂载）
int disk_mount(int disk_id) {

}