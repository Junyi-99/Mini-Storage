#include "func_.h"
#include <thread>
                       
int main(int argc, char *argv[])
{
    clock_t t1=clock();

    int src_fd ;
    int dst_fd_1;
    int dst_fd_2;
    const char* path1 = "/Users/sjs/Documents/cpp_project/mmap_test/cpfile/1G1";    //切片储存位置1-测试用
    const char* path2 = "/Users/sjs/Documents/cpp_project/mmap_test/cpfile/1G2";    //切片储存位置2-测试用
    if ((src_fd = open("/Users/sjs/Documents/cpp_project/mmap_test/50G", O_RDONLY)) < 0){   //打开待切片复制的文件
        printf("file1 open failed！\n");
        return -1;
    }
    
    if ((dst_fd_1 = open(path1, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) < 0) {    //创建切片1
        printf("cp_file1 open failed! \n");
        return -1;
    }
    if ((dst_fd_2 = open(path2, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) < 0) {    //创建切片2
        printf("cp_file2 open failed! \n");
        return -1;
    }
    struct stat stat;
    fstat(src_fd, &stat);               // 获取文件信息
    off_t src_size = stat.st_size;
    off_t path1_size = src_size / 2;    //切片1的文件大小
    off_t path2_size = src_size - path1_size;   //切片2的文件大小
    truncate(path1, path1_size);    // 设置大小
    truncate(path2, path2_size);
    //std::thread tt1(mmapcopy,src_fd, path1_size, dst_fd_1,0);
    //std::thread w(mmapcopy,src_fd, path2_size, dst_fd_2, path1_size);
    mmapcopy(src_fd, path1_size, dst_fd_1,0);
    mmapcopy(src_fd,path2_size,dst_fd_2, path1_size);
    //tt1.join();
    //w.join();
    //mergeFile(dst_fd_1,dst_fd_2,src_size,"/Users/sjs/Documents/cpp_project/mmap_test/cpfile/mergefile");
    close(src_fd);
    close(dst_fd_1);
    close(dst_fd_2);
    clock_t t2=clock();
    std::cout << "time = " << (double)(t2-t1)/CLOCKS_PER_SEC << " s" << std::endl; 
    return 0;
}
                       