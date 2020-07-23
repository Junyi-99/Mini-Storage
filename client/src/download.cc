#include "../etc/config.h"
#include "../include/big_file.h"
#include "../include/small_file.h"
#include "../include/tcp.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    // TODO: 文件名切分，去除路径
    char *filename = split_filename(argv[1]);
    // TODO: 向server请求文件大小
    int connfd = tcp_connect(SERVER_IP_ADDR_1);
    Package *package = set_package(sizeof(Package), INIT_STATUS, filename, 0, 0);
    delete filename;

    ssize_t send_size = send(connfd, (void *) package, package->package_len, 0);
    if (send_size != package->package_len) {
        perror("Send error!");
        close(connfd);
        exit(0);
    }
    ssize_t read_size = recv(connfd, (void*)package, ACK_SIZE, 0);

    if (package->block_len > SMALL_FILE_SIZE_MAX) {
        do_big_file_download(package->filename, package->block_len);
    } else {
        do_small_file_download(connfd, package->filename, package->block_len);
    }
    delete package;
    exit(0);
}
