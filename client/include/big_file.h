#ifndef __BIG_FILE_H__
#define __BIG_FILE_H__
#include <stdint.h>

void do_big_file_upload(const uint32_t fd, const char *file_name,
                        const uint64_t file_size);

#endif
