#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <string.h>
#include <stdio.h>

#define SMALL_FILE_SIZE_MAX 10 * 1024 * 1024
#define ACK_SIZE            8

#define SERVER_DISK_COUNT   24
#define SINGLE_SERVER_DISK  12

#define SERVER_IP_ADDR_1  "9.100.173.189"
#define SERVER_IP_ADDR_2  "9.100.173.189"

//#define SERVER_IP_ADDR_1  "9.135.10.111"
//#define SERVER_IP_ADDR_2  "9.135.10.111"

#define SERVER_PORT     6667

enum MSG_TYPE {
    SMALL_UPLOAD = 0, SMALL_DOWNLOAD, BIG_UPLOAD, BIG_DOWNLOAD
};

#endif
