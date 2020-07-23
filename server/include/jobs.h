//
// Created by uohou on 2020/7/23.
//

#ifndef FUCK_STORAGE_JOBS_H
#define FUCK_STORAGE_JOBS_H


#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <condition_variable>

#include "tcp.h"
#include "package.h"

#define ADDJOB(type, handler) {type, handler}
#define MAX_JOBS 5 // 与 MSG_TYPE 有关

typedef int (*HandlerFunc)(int socket_fd, Package *p);

struct job {
    MSG_TYPE type;
    HandlerFunc handler;
};

extern struct job jobs[MAX_JOBS];

// Handler Function Definitions ↓
// 不用管 socket_fd 和 Package *p 的释放，外面会帮你释放的
// 别自己手动释放

int job_small_upload_mmap(int socket_fd, Package *p);

int job_small_upload_write(int socket_fd, Package *p);

int job_small_download(int socket_fd, Package *p);

int job_big_upload(int socket_fd, Package *p);

int job_big_download(int socket_fd, Package *p);

int job_init_status(int socket_fd, Package *p);


#endif //FUCK_STORAGE_JOBS_H
