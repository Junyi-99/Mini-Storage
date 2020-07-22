#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "../include/tcp.h"

int tcp_init(int port, int max_connection) {

    int server_fd;
    struct sockaddr_in server_addr{};

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// 本地任意ip
    server_addr.sin_port = htons(port);

    // 创建 socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error\n");
        return -1;
    }

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt error\n");
        return -1;
    }

    // 绑定端口
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind error\n");
        return -1;
    }

    // 监听
    if (listen(server_fd, max_connection) < 0) {
        perror("listen error\n");
        return -1;
    }

    set_unblock(server_fd);

    return server_fd;
}

int tcp_accept(int epoll_fd, int fd) {

    struct sockaddr_in client_addr{};
    socklen_t client_length;
    int connect_fd;

    client_length = sizeof(client_addr);
    bzero(&client_addr, client_length);

    connect_fd = accept(fd, (struct sockaddr *) &client_addr, &client_length);
    if (connect_fd < 0) {
        perror("BAD ACCEPT\n");
        close(fd);
        return -1;
    }

    //set_unblock(connect_fd);

    // ET 模式 使用 ONESHOT，保证通知线程之后，不再次通知
    epoll_register(EPOLLIN | EPOLLET | EPOLLONESHOT, epoll_fd, connect_fd); // connection_fd 绑定到 epoll
    return connect_fd;
}

int tcp_receive(int client_fd) {
    int ret;
    unsigned char buf[sizeof(Package)] = {0};
    ret = recv(client_fd, buf, sizeof(buf), 0); // 取一个 header 这么大的数据

    if (ret < 0) {
        // 连接被重置
        if (errno == ECONNRESET) {
            close(client_fd);
        } else {
            perror("tcp_receive error");
            close(client_fd);
        }
        return -1;
    } else if (ret == 0) {
        // 客户端关闭连接
        close(client_fd);
        printf("client %d connection closed\n", client_fd);
        return -1;
    }

    Package *p = unpack_header(buf);
    printf("Package Length: %lu\n", p->package_len);
    printf("Message Type:   %d\n", p->msg_type);
    printf("Block Length:   %lu\n", p->block_len);
    printf("Disk No:        %d\n", p->disk_no);
    printf("File Name:      %s\n", p->filename);

    switch ((MSG_TYPE) p->msg_type) {
        case SMALL_UPLOAD: {
            // 客户端上传小文件到服务器
            unsigned char buf2[81920] = {0};
            uint64_t received = 0;

            // 创建 文件 准备边接收边写入
            creat(p->filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            truncate(p->filename, p->block_len);

            int wfd = open(p->filename, O_RDWR,
                           S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (wfd == -1) {
                perror("Cannot open output file\n");
                return -1;
            }


            unsigned char *mp = (unsigned char *) mmap(NULL, p->block_len, PROT_READ | PROT_WRITE, MAP_SHARED, wfd, 0);
            if (mp == MAP_FAILED) {
                close(wfd);
                perror("Error mmapping the file");
                exit(EXIT_FAILURE);
            }

            printf("Receiving data ... \n");

            double last_percent = 0.0f;
            double curr_percent = 0.0f;

            while (received < p->block_len) {
                ret = recv(client_fd, buf2, sizeof(buf2), 0);
                if (ret <= 0) {
                    close(client_fd);
                    break;
                }

                curr_percent = received * 100 / (double) p->block_len;
                if (curr_percent - last_percent > 1) {
                    printf("Progress: %.2f%%\n", last_percent = curr_percent);
                }

                memcpy(mp + received, buf2, ret); // mmap 写法

                received += ret;
                printf("received: %d, total: %lu\n", ret, received);
            }

            printf("Syncing the disk ... \n");
            if (msync(mp, p->block_len, MS_SYNC) == -1) {
                perror("Could not sync the file to disk");
            }

            // Don't forget to free the mmapped memory
            if (munmap(mp, p->block_len) == -1) {
                close(wfd);
                perror("Error un-mmapping the file");
                exit(EXIT_FAILURE);
            }

            //printf("Closing the socket ... \n");
            close(wfd);
            printf("Transfer complete!\n");
            break;
        }
        case SMALL_DOWNLOAD: {
            break;
        }
        case BIG_UPLOAD: {
            break;
        }
        case BIG_DOWNLOAD: {
            break;
        }
    }
    delete p;
    close(client_fd);

    return 0;
}

int tcp_send(int sock_fd, char *buffer, std::size_t length) {

    while (length > 0) {
        int num = send(sock_fd, buffer, length, 0);
        if (num < 0) {
            perror("send error:");
            close(sock_fd);
            return false;
        } else {
            length -= num;
            buffer += num;
        }
    }

    return 0;
}