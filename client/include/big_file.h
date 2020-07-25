#pragma once

#include "../../etc/config.hpp"
#include "../utils//tcp_socket.hpp"
#include <memory>
#include <stdint.h>
#include <tuple>
#include <vector>

// <file_name, fd, offset, real_block_size, disk_no>
using ThreadArg = std::tuple<char *, int64_t, off_t, int64_t, int32_t>;
using ThreadArgPtr = std::shared_ptr<ThreadArg>;

void do_big_file_upload(int32_t fd, char *file_name, const uint64_t file_size);

void do_big_file_download(char *file_name, const uint64_t file_size);
