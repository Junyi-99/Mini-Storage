#include "../include/big_file.h"
#include "../include/small_file.h"
#include <cstring>
#include <pthread.h>

/*
 * thread func
 */
void *thr_start(void *arg) {
    // init arg
    ThreadArgPtr tupPtr = *((ThreadArgPtr *) arg);
    off64_t offset;
    char *file_name;
    int32_t fd, real_block_size, disk_no;
    std::tie(file_name, fd, offset, real_block_size, disk_no) = *tupPtr;
//    std::cout << "thr arg " << pthread_self() << "==>"
//              << "file_name:" << file_name << "    fd:" << fd
//              << "    offset:" << offset
//              << "    real_block_size:" << real_block_size
//              << "    idisk_no:" << disk_no << std::endl;

    // socket init
    TcpSocket socket_fd = TcpSocket();
    socket_fd.Socket();
    // socket_fd.Connect("9.134.13.102", 6666); // test

    // dispatch
    if (disk_no < SERVER_DISK_COUNT / 2)
        socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
    else
        socket_fd.Connect(SERVER_IP_ADDR_2, SERVER_PORT);
    //socket_fd.Close();
    //return nullptr;
    // send head
    std::shared_ptr<Package> package(set_package(
            BIG_UPLOAD, file_name, real_block_size, disk_no));

    if (!socket_fd.Send((void *) &(*package), Package_len))
        std::cout << "THREAD " << disk_no << "SENDING ERROR ===" << std::endl;
    std::cout << " === THREAD " << disk_no << " HEADER SENT ===" << std::endl;

    // send file block
    socket_fd.SendFile(disk_no, fd, &offset, real_block_size);
    std::cout << " === THREAD " << disk_no << " BODY SENT ===" << std::endl;

    // TODO: recv到关闭信号后close?
    socket_fd.Close();
    //std::cout << "(big_file_upload)pthread exit " << pthread_self() << std::endl;
    return nullptr;
}

void do_big_file_upload(int32_t fd, char *file_name, const uint64_t file_size) {
    // last_block:
    const uint64_t thr_num = BIG_FILE_UPLOAD_BLOCK_NUM;
    // const int32_t thr_num = 4; // test
    const uint64_t block_size = file_size / thr_num;
    const uint64_t last_block = file_size % thr_num;

    auto *tid = new pthread_t[thr_num];
    // change ThreadArgPtr's scope
    std::vector<ThreadArgPtr> vec(thr_num);


    // send head package
    TcpSocket socket_fd = TcpSocket();
    socket_fd.Socket();
    socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
    std::shared_ptr<Package> head_package(
            new Package(BIG_META, file_size, 0, file_name));
    socket_fd.Send((void *) &(*head_package), sizeof(Package));
    socket_fd.Close();

    for (uint32_t i = 0; i < thr_num; ++i) {
        // [i*block_size, (i+1)*block_size) => 左闭右开
        // [(thr_num-2)*block_size, (thr_num-1)*block_size+last_block) => 最后一块
        off64_t offset = i * block_size;
        uint64_t real_block_size = (i == thr_num - 1) ? (block_size + last_block) : block_size;

        ThreadArgPtr arg =
                ThreadArgPtr(new ThreadArg(file_name, fd, offset, real_block_size, i));
        // WARN: 不可用std::move(arg)
        vec[i] = arg;

        int32_t res =
                pthread_create(&tid[i], nullptr, thr_start, (void *) &(vec[i]));

        // if fail: again
        if (res != 0) {
            std::cerr << "(big_file_upload)创建第i=" << i--
                      << "个线程时失败,pthread_id=" << pthread_self() << std::endl;
            continue;
        }
    }
    for (int32_t i = 0; i < thr_num; ++i) {
        pthread_join(tid[i], nullptr);
    }
}
