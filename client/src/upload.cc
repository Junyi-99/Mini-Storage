#include "../../etc/config.hpp"
#include "../include/big_file.h"
#include "../include/small_file.h"
#include <iostream>

int main(int argc, char *argv[]) {
  char *filename = split_filename(argv[1]);
  int fd = open(filename, O_RDONLY);
  struct stat file_state;
  fstat(fd, &file_state);

  uint64_t file_size = file_state.st_size;
  std::cout << "file_size: " << file_size << std::endl;

  if (file_size > SMALL_FILE_SIZE_MAX) {
    do_big_file_upload(fd, filename, file_size);
  } else {
    do_small_file_upload(fd, filename, file_size);
  }
<<<<<<< HEAD
  delete filename;
=======
>>>>>>> 9a994cd6b73872836cca57f9d19c0d70a9ce13eb
  close(fd);
  return 0;
}
