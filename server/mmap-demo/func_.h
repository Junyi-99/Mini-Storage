#ifndef __FUNC__
#define __FUNC__

#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <thread>


void mmapcopy(int src_fd, size_t src_len, int dst_fd, off_t offset = 0);

void mergeFile(int src_fd_1, int src_fd_2, size_t src_len_sum, const char *output);


#endif // !__MMAP_TEST__