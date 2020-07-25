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

#define SERVER_DISK_COUNT 24
#define SINGLE_SERVER_DISK 12

// #define SERVER_IP_ADDR_1  "9.100.173.189"
// #define SERVER_IP_ADDR_2  "9.100.173.189"

#define SERVER_IP_ADDR_1 "127.0.0.1"
#define SERVER_IP_ADDR_2 "127.0.0.1"

#define SERVER_PORT 6667

#define BIG_FILE_UPLOAD_BLOCK_NUM 24
#define BIG_FILE_DOWNLOAD_THR_NUM 24

enum MSG_TYPE : int8_t {
  INIT_STATUS = 0,
  SMALL_UPLOAD,
  SMALL_DOWNLOAD,
  BIG_UPLOAD,
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
    if(fname) strcpy(file_name, fname);
  }
};
#pragma pack(pop)

const u_int32_t Package_len = sizeof(Package);