#include "../etc/config.h"
#include "../include/big_file.h"
#include "../include/small_file.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  // TODO: 向server请求文件大小
  if (file_size > SMALL_FILE_SIZE_MAX) {
    do_big_file_download(argv[1], file_size);
  } else {
    do_small_file_download(argv[1], file_size);
  }
  // printf("%s   %d \n",argv[1], file_size);
  close(fd);
  exit(0);
}
