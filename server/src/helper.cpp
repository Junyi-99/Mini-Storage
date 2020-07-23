//
// Created by uohou on 2020/7/21.
//


#include "../include/helper.h"

int epoll_register(int events_, int epoll_fd_, int fd_) {
    struct epoll_event event{};
    event.data.fd = fd_;
    event.events = events_;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd_, &event)) {
        perror("epoll add failed in accept()\n");
        exit(1);
    }
    return 0;
}

void set_unblock(int fd) {
    int opts = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, opts | O_NONBLOCK); // 设置为非阻塞
}