//
// Created by xingyuyang on 2020/7/23.
//

#include "../../etc/config.hpp"
#include "../include/tcp.h"
#include "../utils/tcp_socket.hpp"


int do_small_file_download(TcpSocket socket_fd, char *file_name, u_int64_t file_size) {
    // 创建文件，truncate 到指定大小
    int flag = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    creat(file_name, flag);
    truncate(file_name, file_size);

    int wfd = open(file_name, O_RDWR, flag);
    if (wfd == -1) {
        perror("Cannot open output file\n");
        return -1;
    }

    unsigned int disk_no = my_hash(file_name); // 上传时指定了diskno下载也需要
    const char *ip_addr = (disk_no > SINGLE_SERVER_DISK) ?
                          SERVER_IP_ADDR_1 : SERVER_IP_ADDR_2;

    printf("Request file ... \n");
    socket_fd.Socket();
    socket_fd.Connect(ip_addr, SERVER_PORT);

    Package package(SMALL_DOWNLOAD, 0, disk_no, file_name);
    socket_fd.Send(&package, sizeof(Package));


    printf("Downloading data ... \n");

    uint64_t ret = 0;
    uint64_t received = 0;
    double last_percent = 0.0f;
    double curr_percent = 0.0f;
    char buff[81920] = {0};
    while (received < file_size) {
        ret = socket_fd.Recv(buff, sizeof(buff));
        if (ret <= 0) {
            perror("receive error");
            break;
        }

        curr_percent = (double) received * 100 / file_size;
        if (curr_percent - last_percent > 5) {
            printf("Progress: %.2f%%\n", last_percent = curr_percent);
        }

        write(wfd, buff, ret);
        received += ret;
    }

    close(wfd);
    printf("Transfer complete!\n");
    return 0;
}
