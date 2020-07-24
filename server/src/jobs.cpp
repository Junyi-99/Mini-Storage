//
// Created by uohou on 2020/7/23.
//

#include "jobs.h"


struct job jobs[MAX_JOBS] = {
        ADDJOB(SMALL_UPLOAD, job_write_to_server_write),
        ADDJOB(SMALL_DOWNLOAD, job_read_from_server_read),
        ADDJOB(BIG_UPLOAD, job_write_to_server_mmap),
        ADDJOB(BIG_DOWNLOAD, job_read_from_server_mmap),
        ADDJOB(INIT_STATUS, job_init_status),
};

// 让程序支持新的 job，在 jobs[MAX_JOBS] 里添加相应的函数即可！

int job_write_to_server_mmap(int socket_fd, Package *p) {

    // 创建文件，truncate 到指定大小
    int flag = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    char filename[32];
    sprintf(filename, "%s.disk%d", p->file_name, p->disk_no);

    int wfd = open(filename, O_RDWR | O_CREAT | O_TRUNC, flag);
    if (wfd == -1) {
        perror("Cannot open output file\n");
        return -1;
    }
    ftruncate(wfd, p->block_len);

    auto *mp = (unsigned char *) mmap(nullptr, p->block_len, PROT_READ | PROT_WRITE, MAP_SHARED, wfd, 0);
    if (mp == MAP_FAILED) {
        close(wfd);
        perror("Error memory mapping the file");
        return -1;
    }

    printf("Receiving data ... \n");

    int ret = 0;
    uint64_t received = 0;
    double last_percent = 0.0f;
    double curr_percent = 0.0f;

    auto *buff = new unsigned char[81920];
    while (received < p->block_len) {
        ret = tcp_receive(socket_fd, buff, sizeof(buff));
        if (ret == -1) {
            printf("Error occurred!\n");
            break;
        }

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

int job_write_to_server_write(int socket_fd, Package *p) {

    // 创建文件，truncate 到指定大小
    int flag = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    char filename[32];
    sprintf(filename, "%s.disk%d", p->file_name, p->disk_no);

    int wfd = open(filename, O_RDWR | O_CREAT | O_TRUNC, flag);
    if (wfd == -1) {
        perror("Cannot open output file\n");
        return -1;
    }
    ftruncate(wfd, p->block_len);

    printf("Receiving data ... \n");

    uint64_t ret = 0;
    uint64_t received = 0;
    double last_percent = 0.0f;
    double curr_percent = 0.0f;

    auto *buff = new unsigned char[81920];
    while (received < p->block_len) {
        ret = tcp_receive(socket_fd, buff, sizeof(buff));
        if (ret == -1) {
            printf("Error occurred! \n");
            break;
        }
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

int job_read_from_server_read(int socket_fd, Package *p) {
    return 0;
}

int job_read_from_server_mmap(int socket_fd, Package *p) {
    return 0;
}

int job_init_status(int socket_fd, Package *p) {
    return 0;
}

