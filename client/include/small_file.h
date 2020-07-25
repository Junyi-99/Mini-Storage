#pragma once

#include <stdlib.h>


Package *
set_package(MSG_TYPE msg_type, char filename[256], size_t block_len, unsigned int disk_no);

char *split_filename(char *filename);

void do_small_file_upload(int fd, char *file_name, u_int64_t file_size);

void do_small_file_download(TcpSocket socket_fd, char *file_name, u_int64_t file_size);