#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define SMALL_FILE_SIZE_MAX 10 * 1024 * 1024
#define ACK_SIZE 8
#define BIG_FILE_MAX_THR 24

#define SERVER_DISK_COUNT 24
#define SINGLE_SERVER_DISK 12

#define SERVER_IP_ADDR_1  "9.100.173.170" // 客户端要连接的第一个ip
#define SERVER_IP_ADDR_2  "9.100.173.189" // 客户端要连接的第二个ip

// #define SERVER_IP_ADDR_1 "127.0.0.1"
// #define SERVER_IP_ADDR_2 "127.0.0.1"


// 服务端相关配置
#define SERVER_PORT 6667           // 监听端口
#define SERVER_MAX_CONNECTION 500  // epoll 设置
#define SERVER_MAX_EVENTS     500  // epoll 设置
#define SERVER_MAX_THREADS    16   // 服务端有多少线程处理文件分块（一个分块一个线程）





// 下方为公共部分

enum MSG_TYPE : int8_t {
    INIT_STATUS = 0,
    FILE_SIZE_REQUEST,   // 向服务器请求文件大小信息
    SMALL_UPLOAD,   // 向服务器发送小文件（文件头里就有小文件的文件大小了）
    SMALL_DOWNLOAD,
    BIG_META,       // 向服务器发送文件大小信息（准备接收大文件）
    BIG_UPLOAD,     // 向服务器发送大文件的文件块
    BIG_DOWNLOAD,
};

#pragma pack(push, 1)

struct Package {
    MSG_TYPE msg_type;
    uint64_t block_len;
    uint32_t disk_no;
    char file_name[256];

    Package(MSG_TYPE tp = INIT_STATUS, uint64_t blen = 0, uint32_t dno = 0,
            char *fname = nullptr)
            : msg_type(tp), block_len(blen), disk_no(dno) {
        // bzero((void *)file_name, sizeof(file_name));
        memset(file_name, 0, sizeof(file_name));
        if (fname) strcpy(file_name, fname);
    }
};

#pragma pack(pop)

const u_int32_t Package_len = sizeof(Package);
