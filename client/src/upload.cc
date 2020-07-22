#include "../etc/config.h"
#include "../include/big_file.h"
#include "../include/small_file.h"
#include <stdio.h>
#include <glob.h>

int main(int argc, char *argv[]) {
    int fd = open(argv[1], O_RDONLY);
    struct stat file_state;
    fstat(fd, &file_state);
    size_t file_size = file_state.st_size;
    if (file_size > SMALL_FILE_SIZE_MAX) {
        printf("big file \n");
        do_big_file_upload(fd, argv[1], file_size);
    } else {
        printf("small file\n");
        do_small_file_upload(fd, argv[1], file_size);
    }
    // printf("%s   %d \n",argv[1], file_size);
    close(fd);
    exit(0);
}
