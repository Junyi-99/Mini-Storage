#include "../etc/config.h"
#include "../include/big_file.h"
#include "../include/small_file.h"
#include <iostream>

int main(int argc, char *argv[]) {
  int fd = open(argv[1], O_RDONLY);
  struct stat file_state;
  fstat(fd, &file_state);

  uint64_t file_size = file_state.st_size;
  std::cout << "file_size: " << file_size << std::endl;

  if (file_size > SMALL_FILE_SIZE_MAX) {
    do_big_file_upload(fd, argv[1], file_size);
  } else {
    do_small_file_upload(fd, argv[1], file_size);
  }
  // printf("%s   %d \n", argv[1], file_size);
  close(fd);
  exit(0);
}
