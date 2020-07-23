#pragma once
#pragma pack(1)
#include "../../utils/tcp_socket.hpp"
#include "../etc/config.h"
#include <memory>
#include <stdint.h>
#include <string.h>
#include <tuple>
#include <vector>

struct Package {
  MSG_TYPE msg_type;
  uint64_t block_len;
  uint32_t disk_no;
  char file_name[256];

  Package(MSG_TYPE tp = INIT_STATUS, uint64_t blen = 0, uint32_t dno = 0)
      : msg_type(tp), block_len(blen), disk_no(0) {
    bzero((void *)file_name, sizeof(file_name));
  }
};

// <file_name, fd, offset, real_block_size, disk_no>
using ThreadArg = std::tuple<char *, int64_t, off_t, int64_t, int32_t>;
using ThreadArgPtr = std::shared_ptr<ThreadArg>;

Package *set_package(int64_t package_len, MSG_TYPE msg_type,
                     const char *filename, int64_t block_len, int32_t disk_no);

void do_big_file_upload(int32_t fd, char *file_name, const uint64_t file_size);

void do_big_file_download(char *file_name, const uint64_t file_size);
