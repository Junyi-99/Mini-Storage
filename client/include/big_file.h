#pragma once
#include "../../utils/tcp_socket.hpp"
#include "../etc/config.h"
#include <memory>
#include <stdint.h>
#include <tuple>
#include <vector>

struct Package {
  int32_t package_len;
  MSG_TYPE msg_type;
  char filename[1024];
  int32_t block_len;
  int32_t disk_no;
  // Package()
  // : package_len(sizeof(Package)), msg_type(INIT_STATUS), filename(""),
  // block_len(-1), disk_no(-1) {}
};

// <file_name, fd, offset, real_block_size, disk_no>
using ThreadArg = std::tuple<char *, int32_t, off_t, int32_t, int32_t>;
using ThreadArgPtr = std::shared_ptr<ThreadArg>;
Package *set_package(int32_t package_len, MSG_TYPE msg_type,
                     const char *filename, int32_t block_len, int32_t disk_no);

void do_big_file_upload(int32_t fd, char *file_name, const uint64_t file_size);
void do_big_file_download(char *file_name, const uint64_t file_size);
