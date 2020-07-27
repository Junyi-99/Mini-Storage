#include "../../etc/config.hpp"
#include "../include/big_file.h"
#include "../include/small_file.h"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 0;
    }

    char *filename = split_filename(argv[1]);
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    struct stat64 file_state;
    fstat64(fd, &file_state);

    uint64_t file_size = file_state.st_size;
    std::cout << "file_size: " << file_size << std::endl;

    if (file_size > SMALL_FILE_SIZE_MAX) {
        do_big_file_upload(fd, filename, file_size);
    } else {
        do_small_file_upload(fd, filename, file_size);
    }
    delete filename;
    close(fd);
    return 0;
}
