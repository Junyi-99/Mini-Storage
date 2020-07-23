//
// Created by uohou on 2020/7/23.
//

#include "jobs.h"


struct job jobs[MAX_JOBS] = {
        ADDJOB(MSG_TYPE::SMALL_UPLOAD, job_small_upload_mmap),
        ADDJOB(MSG_TYPE::SMALL_DOWNLOAD, job_small_download),
        ADDJOB(MSG_TYPE::BIG_UPLOAD, job_big_upload),
        ADDJOB(MSG_TYPE::BIG_DOWNLOAD, job_big_download),
        ADDJOB(MSG_TYPE::INIT_STAUS, job_init_status),
};

int job_small_upload_mmap(int socket_fd, Package *p) {

    // 创建文件，truncate 到指定大小
    int flag = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    creat(p->filename, flag);
    truncate(p->filename, p->block_len);

    int wfd = open(p->filename, O_RDWR, flag);
    if (wfd == -1) {
        perror("Cannot open output file\n");
        return -1;
    }

    auto *mp = (unsigned char *) mmap(nullptr, p->block_len, PROT_READ | PROT_WRITE, MAP_SHARED, wfd, 0);
    if (mp == MAP_FAILED) {
        close(wfd);
        perror("Error memory mapping the file");
        return -1;
    }

    printf("Receiving data ... \n");

    uint64_t ret = 0;
    uint64_t received = 0;
    double last_percent = 0.0f;
    double curr_percent = 0.0f;

    auto *buff = new unsigned char[81920];
    while (received < p->block_len) {
        ret = tcp_receive(socket_fd, buff, sizeof(buff));

        curr_percent = (double) received * 100 / p->block_len;
        if (curr_percent - last_percent > 5) {
            printf("Progress: %.2f%%\n", last_percent = curr_percent);
        }

        memcpy(mp + received, buff, ret); // mmap 写法
        received += ret;
    }
    delete[] buff;

    printf("Syncing the disk ... \n");
    if (msync(mp, p->block_len, MS_SYNC) == -1) {
        perror("Could not sync the file to disk");
    }

    printf("Unmapping the file ... \n");
    if (munmap(mp, p->block_len) == -1) {
        close(wfd);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }

    close(wfd);
    printf("Transfer complete!\n");
    return 0;
}

int job_small_upload_write(int socket_fd, Package *p) {
    // 创建文件，truncate 到指定大小
    int flag = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    creat(p->filename, flag);
    truncate(p->filename, p->block_len);

    int wfd = open(p->filename, O_RDWR, flag);
    if (wfd == -1) {
        perror("Cannot open output file\n");
        return -1;
    }

    printf("Receiving data ... \n");

    uint64_t ret = 0;
    uint64_t received = 0;
    double last_percent = 0.0f;
    double curr_percent = 0.0f;

    auto *buff = new unsigned char[81920];
    while (received < p->block_len) {
        ret = tcp_receive(socket_fd, buff, sizeof(buff));

        curr_percent = (double) received * 100 / p->block_len;
        if (curr_percent - last_percent > 5) {
            printf("Progress: %.2f%%\n", last_percent = curr_percent);
        }

        write(wfd, buff, ret);
        received += ret;
    }

    close(wfd);
    printf("Transfer complete!\n");
    return 0;
}

int job_small_download(int socket_fd, Package *p) {
    return 0;
}

int job_big_upload(int socket_fd, Package *p) {
    return 0;
}

int job_big_download(int socket_fd, Package *p) {
    return 0;
}

int job_init_status(int socket_fd, Package *p) {
    return 0;
}

