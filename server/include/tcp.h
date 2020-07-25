//
// Created by uohou on 2020/7/21.
//

#ifndef FUCK_STORAGE_SERVER_TCP_H
#define FUCK_STORAGE_SERVER_TCP_H

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "helper.h"

int tcp_init(int port, int max_connection);

int tcp_accept(int epoll_fd, int fd);

int tcp_receive(int client_fd, void *buf, int64_t n);

int tcp_send(int sock_fd, char *buffer, int64_t length);

int tcp_sendfile(int sock_fd, int file_fd, off64_t *offset, int64_t bytes_need_to_send);

#endif //FUCK_STORAGE_SERVER_TCP_H
