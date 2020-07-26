#include <ctime>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <signal.h>

#include "../include/ThreadPool.h"
#include "../include/tcp.h"
#include "../include/worker.h"

ThreadPool pool(SERVER_MAX_THREADS);


void sig_handler(int signal) {

    if (signal == SIGALRM) {

        std::cout << "Jobs in queue: " << worker_queue.size() << std::endl;

        alarm(5);
    }
}

/*
 * 主线程 epoll 处理用户连接
 * accept 之后，开启线程处理文件 上传/下载 请求
 * */
int main(int argc, const char *argv[]) {
    signal(SIGALRM, sig_handler);
    alarm(5);

    // result = pool.enqueue([](int answer, int c) { return answer; }, 42, 32);
    //std::cout << result.get() << std::endl;
    int colorList[] = {196, 197, 198, 199, 200, 201, 165, 129, 93, 57, 21, 27, 33, 39, 45, 51, 50, 49, 48, 47};

    printf("服务器正在做无用的彩虹打印工作，请稍后\n");
    float progress = 0.0;
    while (progress < 100.0) {

        printf("%3d %% [", (int) progress);

        for (int i = 0; i < (int) progress; ++i) {
            printf("\x1b[48;5;%dm \x1b[0m", colorList[(int) i / 5]);
        }
        printf("]\r");
        std::cout.flush();

        progress += 1; // for demonstration only
        //printf("Progress: %.2f%%  \e[?25l\n", progress);

        usleep(10 * 1000);
    }
    printf("\n");


    int server_fd;
    if ((server_fd = tcp_init(SERVER_PORT, SERVER_MAX_CONNECTION)) < 0) {
        return 1;
    }

    printf("Server Running on 0.0.0.0:%d\n", SERVER_PORT);

    // 创建 worker 线程
    pthread_t threads[SERVER_MAX_THREADS] = {0};
    for (auto &thread : threads) {
        if (pthread_create(&thread, nullptr, worker_work, nullptr) > 0) {
            perror("pthread_create");
        }
    }

    // ========== 准备好接收客户端连接 ==========

    // 创建一个 epoll
    int epoll_fd;
    if ((epoll_fd = epoll_create(SERVER_MAX_EVENTS)) < 0) {
        perror("epoll create failed\n");
        return 1;
    }

    // 注册 server_fd 到 epoll，这里不能使用 ONESHOT 否则会丢失客户端数据
    epoll_register(EPOLLIN, epoll_fd, server_fd);
    while (true) {
        struct epoll_event events[SERVER_MAX_EVENTS];

        // epoll_wait [错误]返回 -1 ，[超时]返回 0 ，[正常]返回获取到的事件数量
        int ret = epoll_wait(epoll_fd, events, SERVER_MAX_EVENTS, -1);

        if (ret < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("epoll wait error\n");
                break;
            }
        }

        // 遍历获取到的 epoll 事件
        for (int i = 0; i < ret; i++) {

            auto fd = (int) events[i].data.fd;
            auto ev = (uint32_t) events[i].events;

            if ((ev & EPOLLERR) ||
                (ev & EPOLLHUP) ||
                !(ev & EPOLLIN)) {
                perror("epoll event error\n");
                close(events[i].data.fd);
                continue;
            } else if ((fd == server_fd) && (ev & EPOLLIN)) {
                tcp_accept(epoll_fd, fd);
            } else if (ev & EPOLLIN) {
                // 有 客户端 的 fd 收到 数据
                worker_put(fd);
            } else {

            }
        }
    }

    close(epoll_fd);
    close(server_fd);
    return 0;
}

