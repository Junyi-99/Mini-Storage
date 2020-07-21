#include "../etc/config.h"
#include "../include/small_file.h"

void do_small_file_upload(int fd, char *file_name, size_t file_size) {
  /* Calculate send to which disk(server) */
  unsigned int disk_no = my_hash(file_name);
  printf("disk_no: %d\n", disk_no);
}