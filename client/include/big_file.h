#pragma once
#include "../../utils/tcp_socket.hpp"
#include "../etc/config.hpp"
#include <memory>
#include <stdint.h>
#include <tuple>
#include <vector>

// <file_name, fd, offset, real_block_size, disk_no>
using ThreadArg = std::tuple<char *, int64_t, off_t, int64_t, int32_t>;
using ThreadArgPtr = std::shared_ptr<ThreadArg>;

Package *set_package(int64_t package_len, MSG_TYPE msg_type,
                     const char *filename, int64_t block_len, int32_t disk_no);

void do_big_file_upload(int32_t fd, char *file_name, const uint64_t file_size);

void do_big_file_download(char *file_name, const uint64_t file_size);
