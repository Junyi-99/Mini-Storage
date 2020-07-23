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
#include "../etc/config.h"

int tcp_connect(const char *ip_addr);

int tcp_receive(int client_fd, void *buf, size_t n);

int tcp_send(int sock_fd, char *buffer, size_t length);

#endif //FUCK_STORAGE_SERVER_TCP_H
