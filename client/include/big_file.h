#pragma once
#include "../../utils/tcp_socket.hpp"
#include "../etc/config.h"
#include <memory>
#include <stdint.h>
#include <tuple>
#include <vector>

struct Package {
  uint32_t package_len;
  MSG_TYPE msg_type;
  char filename[1024];
  uint32_t block_len;
  uint32_t disk_no;
  // Package()
  // : package_len(sizeof(Package)), msg_type(INIT_STATUS), filename(""),
  // block_len(0), disk_no(0) {}
};

using ThreadArg = std::tuple<char *, uint32_t, off_t, uint32_t, uint32_t>;
using ThreadArgPtr = std::shared_ptr<ThreadArg>;
Package *set_package(uint32_t package_len, MSG_TYPE msg_type,
                     const char *filename, uint32_t block_len,
                     uint32_t disk_no);

void do_big_file_upload(uint32_t fd, char *file_name, const uint64_t file_size);
void do_big_file_download(char *file_name, const uint64_t file_size);
