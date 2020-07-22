//
// Created by uohou on 2020/7/21.
//

#ifndef FUCK_STORAGE_SERVER_HELPER_H
#define FUCK_STORAGE_SERVER_HELPER_H

#include <cstdlib>
#include <cstdio>
#include <poll.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define PORT 1234
#define MAX_CONNECTION 500
#define MAX_EVENTS     500
#define MAX_THREADS    2

int epoll_register(int events_, int epoll_fd_, int fd_);

void set_unblock(int fd);

#endif //FUCK_STORAGE_SERVER_HELPER_H
