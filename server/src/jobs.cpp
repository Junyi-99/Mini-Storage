//
// Created by uohou on 2020/7/23.
//

#include "jobs.h"
#include "file.h"

struct job jobs[MAX_JOBS] = {
        ADDJOB(FILE_SIZE_REQUEST, job_get_file_size),
        ADDJOB(SMALL_UPLOAD, job_write_to_server_write),
        ADDJOB(SMALL_DOWNLOAD, job_read_from_server_read),
        ADDJOB(BIG_META, job_write_to_server_file_meta),
        ADDJOB(BIG_UPLOAD, job_write_to_server_mmap),
        ADDJOB(BIG_DOWNLOAD, job_read_from_server_mmap),
        ADDJOB(INIT_STATUS, job_init_status),
};




// 让程序支持新的 job，在 jobs[MAX_JOBS] 里添加相应的函数即可！

int job_get_file_size(int socket_fd, Package *p) {
    int64_t size = getTotalSize(p->file_name);
    tcp_send(socket_fd, (char *) (&size), sizeof(size));
    return 0;
}

int job_write_to_server_file_meta(int socket_fd, Package *p) {
    return file_set(p->file_name, p->block_len);
}

int job_write_to_server_mmap(int socket_fd, Package *p) {
    char filename[280];
    sprintf(filename, "disk%03d.%s", p->disk_no, p->file_name);

    auto *fileOpt = new File_Opt(filename, p->block_len);

    // file_set(p->file_name, p->block_len);
    //mmap的不用 file_set 因为mmap的是大文件处理，之前BIG_META已经设置了文件信息

    printf("Receiving data ... \n");

    int ret = 0;
    uint64_t received = 0;
    double last_percent = 0.0f;
    double curr_percent = 0.0f;
    bool synced = false;
    while (received < p->block_len) {
        ret = tcp_receive(socket_fd, (char *) fileOpt->mmap_addr + received, 81920);
        if (ret == -1) {
            printf("Error occurred!\n");
            break;
        }

        curr_percent = (double) received * 100 / p->block_len;
        if (curr_percent > 50 && !synced) { // 传输一半的时候sync一下
            system("sync &");
            synced = true;
        }
        if (curr_percent - last_percent > 5) {
            printf("Progress: %.2f%%\n", last_percent = curr_percent);
        }

        received += ret;
    }
    system("sync");
    delete fileOpt; // 析构 自动落盘
    close(socket_fd);  // 虽然外面会自动帮忙 close，但是我依然坚持在内close的写法
    printf("Transfer complete!\n");
    return 0;
}


int job_write_to_server_write(int socket_fd, Package *p) {
    file_set(p->file_name, p->block_len);
    // 使用 write() 写小文件 ❌
    // 偷懒使用 File_Opt    ✅
    return job_write_to_server_mmap(socket_fd, p);
}

int job_read_from_server_read(int socket_fd, Package *p) {
    return job_read_from_server_mmap(socket_fd, p); // 其实都用 sendfile 发送文件到 socket
}

int job_read_from_server_mmap(int socket_fd, Package *p) {
    // 收到 BIG_DOWNLOAD 请求
    // 根据请求头里的文件名和磁盘号，发送文件给客户端
    off64_t offset = 0;
    char filename[280] = {0};
    sprintf(filename, "disk%03d.%s", p->disk_no, p->file_name);

    auto *fileOpt = new File_Opt(filename);
    printf("SENDING FILE %d %ld %ld... \n", fileOpt->fd, offset, fileOpt->file_size);
    tcp_sendfile(socket_fd, fileOpt->fd, &offset, fileOpt->file_size);
    delete fileOpt;
    close(socket_fd); // 虽然外面会自动帮忙 close，但是我依然坚持在内close的写法

    return 0;
}

int job_init_status(int socket_fd, Package *p) {
    return 0;
}

