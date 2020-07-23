//
// Created by uohou on 2020/7/21.
//


#include "../include/worker.h"


std::mutex __mutex;
std::queue<int> worker_queue;
std::condition_variable worker_condition; // 全局 条件变量

// 新的处理任务
void worker_put(int fd) {
    {
        __mutex.lock();
        worker_queue.push(fd);
        worker_condition.notify_one();
        __mutex.unlock();
    }
}

// 循环从队列里取出任务，解析包头，判断类型，然后交给 job.handler 处理
void *worker_work(void *ptr) {
    while (true) {
        std::unique_lock<std::mutex> lock(__mutex);
        while (worker_queue.empty()) {
            worker_condition.wait(lock);
        }
        assert(!worker_queue.empty());

        int job_fd = worker_queue.front();
        worker_queue.pop();
        lock.unlock();
        // ================= ↑ 取出一个任务 ↑ =================

        printf("processing fd: %d\n", job_fd);

        // 接收数据
        unsigned char buffer[sizeof(Package)] = {0};
        if (tcp_receive(job_fd, buffer, sizeof(Package)) < 0) {
            continue;
        }

        // 解析包
        Package *p = unpack_header(buffer);
        printf("==============================================\n");
        printf("Package Length: %lu\n", p->package_len);
        printf("Message Type:   %d\n", p->msg_type);
        printf("Block Length:   %lu\n", p->block_len);
        printf("Disk No:        %d\n", p->disk_no);
        printf("File Name:      %s\n", p->filename);
        printf("==============================================\n");

        // 根据控制码调用相关的处理函数
        // 包括继续接收后续的包，也由 handler 进行处理
        for (auto &job : jobs) {
            if (job.type == (MSG_TYPE) p->msg_type) {
                job.handler(job_fd, p);
            }
        }

        delete p;
        close(job_fd);
        // 任务执行完毕，关闭 job_fd
        // 任务 = 包头 + payload
    }
    return nullptr;
}

