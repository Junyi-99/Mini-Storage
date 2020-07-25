#include "../../etc/config.hpp"
#include "../include/tcp.h"
#include <assert.h>

/*
  SDBMHash function to calculate send to which disk(server)
  @param str  input filepath
  @return num 
*/
unsigned int my_hash(char *str) {
    unsigned int hash = 0;

    while (*str) {
        // equivalent to: hash = 65599*hash + (*str++);
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
    }

    return (hash & SERVER_DISK_COUNT);
}

Package *
set_package(MSG_TYPE msg_type, char filename[256], size_t block_len, unsigned int disk_no) {
    Package *package = new Package;
    package->msg_type = msg_type;
    strcpy(package->file_name, filename);
    package->block_len = block_len;
    package->disk_no = disk_no;
    return package;
}

char* split_filename(char* filename) {
    uint32_t filename_begin = 0;
    uint32_t str_index;
    for(str_index = 0; *(filename + str_index) != '\0'; ++str_index)
        if (*(filename + str_index) == '/')
            filename_begin = str_index + 1;
    assert (filename_begin < str_index);
    char *filename_split = new char[256];
    memcpy(filename_split, filename + filename_begin, str_index - filename_begin);
    return filename_split;
}

void do_small_file_upload(int fd, char *file_name, size_t file_size) {
    /* Calculate send to which disk(server) */
    unsigned int disk_no = my_hash(file_name);
    // printf("send file: %s\n", file_name);

    /* Connect to server(single connection) */
    const char *ip_addr = (disk_no > SINGLE_SERVER_DISK) ?
                          SERVER_IP_ADDR_1 : SERVER_IP_ADDR_2;
    int connfd = tcp_connect(ip_addr);

    /* Send head */
    Package *package = set_package(SMALL_UPLOAD, file_name, file_size, disk_no);
    ssize_t send_size = write(connfd, (void *) package, Package_len);
    if (send_size != Package_len) {
        perror("Send error!");
        close(connfd);
        close(fd);
        exit(0);
    }
    delete package;

    /* Send file */
    ssize_t total_sent = 0;

    off_t offset = 0;
    while (offset < file_size) {
        send_size = sendfile(connfd, fd, &offset, file_size);
        total_sent += send_size;
        printf("bytes sent: %zd, total: %zd\n", send_size, total_sent);
        if (send_size < 0) {
            perror("sendfile error");
            close(connfd);
            close(fd);
            exit(0);
        }
    }


    if (total_sent != file_size) {
        perror("Send error! total_sent != file_size");
        close(connfd);
        close(fd);
        exit(0);
    }

    /* Get ack signal & close connection */
    char readBuffer[ACK_SIZE];
    ssize_t read_size = recv(connfd, readBuffer, ACK_SIZE, 0);
    if (read_size == 0) {
        printf("Transfer complete!\n");
    } else {
        printf("UNKNOWN MESSAGE FROM SERVER! Length: %zd\n", read_size);
    }

    close(connfd);
    close(fd);
    exit(0);
}