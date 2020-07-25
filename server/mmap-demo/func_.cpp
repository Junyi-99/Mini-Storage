#include "func_.h"


void mmapcopy(int src_fd, size_t src_len, int dst_fd, off_t offset) {
    void *src_ptr, *dst_ptr;
    src_ptr = mmap(NULL, src_len + 50, PROT_READ, MAP_PRIVATE, src_fd, offset);
    /*
    start: NULL: 映射区开始的地址，设置为0时表示有系统决定映射区的起始
    len:src_len: 映射区长度(页大小为单位)
    port:内存保护标志
    flags:映射选项和映射页是否可以共享
    fd:文件描述符
    offset:被映射对象内容的起点
    */
    dst_ptr = mmap(NULL, src_len + 50, PROT_WRITE | PROT_READ, MAP_SHARED, dst_fd, 0);
    if (dst_ptr == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        return;
    }
    memcpy(dst_ptr, src_ptr, src_len);  // 实现拷贝

    munmap(src_ptr, src_len);
    munmap(dst_ptr, src_len);
    msync(dst_ptr, src_len, MS_SYNC);//落盘
}

void mergeFile(int src_fd_1, int src_fd_2, size_t src_len_sum, const char *output_path) {    //两个拆分的文件合并成一个文件 并储存
    int output_fd;
    if ((output_fd = open(output_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) < 0) {
        printf("merge_file create failed! \n");
        return;
    }
    truncate(output_path, src_len_sum);
    struct stat stat;
    fstat(src_fd_1, &stat);
    void *src_ptr_1 = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, src_fd_1, 0);
    void *src_ptr_2 = mmap(NULL, src_len_sum - stat.st_size, PROT_READ, MAP_SHARED, src_fd_2, 0);
    void *output_head = mmap(NULL, src_len_sum, PROT_READ | PROT_WRITE, MAP_SHARED, output_fd, 0);
    void *output_ptr = output_head;
    memcpy(output_ptr, src_ptr_1, stat.st_size);
    output_ptr = ((char *) output_head + stat.st_size);
    memcpy(output_ptr, src_ptr_2, src_len_sum - stat.st_size);
    munmap(src_ptr_1, stat.st_size);
    munmap(src_ptr_2, src_len_sum - stat.st_size);
    munmap(output_head, src_len_sum);
    msync(output_head, src_len_sum, MS_SYNC);
}
