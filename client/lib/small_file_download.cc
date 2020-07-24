//
// Created by xingyuyang on 2020/7/23.
//

#include "../etc/config.h"
#include "../include/tcp.h"

int do_small_file_download(int socket_fd, char *file_name, u_int64_t file_size) {
  // 创建文件，truncate 到指定大小
  int flag = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  creat(file_name, flag);
  truncate(file_name, file_size);

  int wfd = open(file_name, O_RDWR, flag);
  if (wfd == -1) {
    perror("Cannot open output file\n");
    return -1;
  }

  printf("Downloading data ... \n");

  uint64_t ret = 0;
  uint64_t received = 0;

  auto *buff = new unsigned char[81920];
  while (received < file_size) {
    ret = tcp_receive(socket_fd, buff, sizeof(buff));
    write(wfd, buff, ret);
    received += ret;
  }

  close(wfd);
  printf("Transfer complete!\n");
  return 0;
}