#ifndef __SMALL_FILE_H__
#define __SMALL_FILE_H__

#include <stdlib.h>

/*
  SDBMHash function to calculate send to which disk(server)
  @param str  input filepath
  @return num 
*/
unsigned int hash(char *str);

void do_small_file_upload(int fd, char *file_name, size_t file_size);


#endif
