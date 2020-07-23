//
// Created by uohou on 2020/7/21.
//

#ifndef FUCK_STORAGE_SERVER_WORKER_H
#define FUCK_STORAGE_SERVER_WORKER_H

#include <mutex>
#include <queue>
#include <thread>
#include <cassert>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <condition_variable>

#include "tcp.h"
#include "jobs.h"

void worker_put(int fd);

void *worker_work(void *ptr);

extern std::mutex __mutex;
extern std::queue<int> worker_queue;
extern std::condition_variable worker_condition; // 全局 条件变量


#endif //FUCK_STORAGE_SERVER_WORKER_H
