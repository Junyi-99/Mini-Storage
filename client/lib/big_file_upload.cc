#include "../include/big_file.h"
#include <cstring>
#include <pthread.h>

<<<<<<< HEAD
Package *set_package(int64_t package_len, MSG_TYPE msg_type,
                     const char *filename, int64_t block_len, int32_t disk_no) {
    auto *package = new Package;
    package->package_len = package_len;
    package->msg_type = msg_type;
    // strncpy(package->filename, filename, strlen(filename) + 1);
    strcpy(package->filename, filename);
    package->block_len = block_len;
    package->disk_no = disk_no;
    return package;
}

=======
>>>>>>> 9a994cd6b73872836cca57f9d19c0d70a9ce13eb
/*
 * thread func
 */
void *thr_start(void *arg) {
<<<<<<< HEAD
    // socket init
    TcpSocket socket_fd = TcpSocket();
    socket_fd.Socket();
    // TODO: 两个服务器存储
    socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);

    // init arg
    ThreadArgPtr tupPtr = *((ThreadArgPtr *) arg);
    off_t offset;
    char *file_name;
    int32_t fd, real_block_size, disk_no;
    std::tie(file_name, fd, offset, real_block_size, disk_no) = *tupPtr;
//  std::cout << "thr arg " << pthread_self() << "==>"
//            << "file_name:" << file_name << "    fd:" << fd
//            << "    offset:" << offset
//            << "    real_block_size:" << real_block_size
//            << "    idisk_no:" << disk_no << std::endl;
=======
  // init arg
  ThreadArgPtr tupPtr = *((ThreadArgPtr *)arg);
  off_t offset;
  char *file_name;
  int32_t fd, real_block_size, disk_no;
  std::tie(file_name, fd, offset, real_block_size, disk_no) = *tupPtr;
  std::cout << "thr arg " << pthread_self() << "==>"
            << "file_name:" << file_name << "    fd:" << fd
            << "    offset:" << offset
            << "    real_block_size:" << real_block_size
            << "    idisk_no:" << disk_no << std::endl;

  // socket init
  TcpSocket socket_fd = TcpSocket();
  socket_fd.Socket();
  // socket_fd.Connect("9.134.13.102", 6666); // test

  // dispatch
  if (disk_no < SERVER_DISK_COUNT / 2)
    socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
  else
    socket_fd.Connect(SERVER_IP_ADDR_2, SERVER_PORT);

  // send head
  std::shared_ptr<Package> package(
      new Package(BIG_UPLOAD, real_block_size, disk_no, file_name));
  socket_fd.Send((void *)&(*package), sizeof(Package));
>>>>>>> 9a994cd6b73872836cca57f9d19c0d70a9ce13eb

    // send head
    std::shared_ptr<Package> package(set_package(
            sizeof(Package), BIG_UPLOAD, file_name, real_block_size, disk_no));
    socket_fd.Send((void *) &(*package), package->package_len);
    std::cout << " === THREAD " << disk_no << " SENDDATA ===" << std::endl;
    // send file block
    socket_fd.SendFile(disk_no, fd, &offset, real_block_size);

<<<<<<< HEAD
    // TODO: recv到关闭信号后close
    socket_fd.Close();
    std::cout << " === THREAD " << disk_no << " EXIT ===" << std::endl;
//  std::cout << "(big_file_upload)pthread exit " << pthread_self() << std::endl;
    return nullptr;
}

void do_big_file_upload(int32_t fd, char *file_name, const uint64_t file_size) {
    // last_block:
    const int32_t thr_num = BIG_FILE_UPLOAD_BLOCK_NUM;
    const int32_t block_size = file_size / thr_num;
    const int32_t last_block = file_size % thr_num;
=======
  // TODO: recv到关闭信号后close?
  socket_fd.Close();
  std::cout << "(big_file_upload)pthread exit " << pthread_self() << std::endl;
  return nullptr;
}

void do_big_file_upload(int32_t fd, char *file_name, const uint64_t file_size) {
  // last_block:
  const int32_t thr_num = BIG_FILE_UPLOAD_BLOCK_NUM;
  // const int32_t thr_num = 4; // test
  const int32_t block_size = file_size / thr_num;
  const int32_t last_block = file_size % thr_num;
>>>>>>> 9a994cd6b73872836cca57f9d19c0d70a9ce13eb

    pthread_t *tid = new pthread_t[thr_num];
    // change ThreadArgPtr's scope
    std::vector<ThreadArgPtr> vec(thr_num);

<<<<<<< HEAD
    for (int32_t i = 0; i < thr_num; ++i) {
        // [i*block_size, (i+1)*block_size) => 左闭右开
        // [(thr_num-2)*block_size, (thr_num-1)*block_size+last_block) => 最后一块
        off_t offset = i * block_size;
        int32_t real_block_size =
                (i == thr_num - 1) ? (block_size + last_block) : block_size;
        // std::cout << i << ". offset && real_block_size ==>" << offset << ":"
        // << real_block_size << std::endl;
=======
  // send head package
  TcpSocket socket_fd = TcpSocket();
  socket_fd.Socket();
  socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
  std::shared_ptr<Package> head_package(
      new Package(BIG_UPLOAD, file_size, 0, file_name));
  socket_fd.Send((void *)&(*head_package), sizeof(Package));
  socket_fd.Close();
>>>>>>> 9a994cd6b73872836cca57f9d19c0d70a9ce13eb

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
