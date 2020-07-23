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
            return nullptr;
        }

        // 解析包
        Package *p = unpack_header(buffer);
        printf("Package Length: %lu\n", p->package_len);
        printf("Message Type:   %d\n", p->msg_type);
        printf("Block Length:   %lu\n", p->block_len);
        printf("Disk No:        %d\n", p->disk_no);
        printf("File Name:      %s\n", p->filename);

        // 判断控制码
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

                }


//            unsigned char *mp = (unsigned char *) mmap(NULL, p->block_len, PROT_READ | PROT_WRITE, MAP_SHARED, wfd, 0);
//            if (mp == MAP_FAILED) {
//                close(wfd);
//                perror("Error mmapping the file");
//                exit(EXIT_FAILURE);
//            }

                printf("Receiving data ... \n");


                double last_percent = 0.0f;
                double curr_percent = 0.0f;
                int ret;
                while (received < p->block_len) {
                    ret = tcp_receive(job_fd, buf2, sizeof(buf2));

                    curr_percent = (double) received * 100 / p->block_len;
                    if (curr_percent - last_percent > 1) {
                        printf("Progress: %.2f%%\n", last_percent = curr_percent);
                    }

                    //memcpy(mp + received, buf2, ret); // mmap 写法
                    write(wfd, buf2, ret);
                    received += ret;
                    printf("received: %d, total: %lu\n", ret, received);
                }

//            printf("Syncing the disk ... \n");
//            if (msync(mp, p->block_len, MS_SYNC) == -1) {
//                perror("Could not sync the file to disk");
//            }
//
//            // Don't forget to free the mmapped memory
//            if (munmap(mp, p->block_len) == -1) {
//                close(wfd);
//                perror("Error un-mmapping the file");
//                exit(EXIT_FAILURE);
//            }

                //printf("Closing the socket ... \n");
                close(wfd);
                close(job_fd);
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
        close(job_fd); // 任务执行完毕，关闭 job_fd
    }
    return nullptr;
}

