#include "../include/big_file.h"
#include <cstring>
#include <pthread.h>

struct HeadPackage {
  int32_t head_package_len;
  MSG_TYPE msg_type;
  char file_name[1024];
  uint32_t total_size;
  HeadPackage(int32_t len = sizeof(HeadPackage), MSG_TYPE tp = INIT_STATUS,
              char *name = nullptr, uint32_t size = 0)
      : head_package_len(len), msg_type(tp), total_size(size) {
    strcpy(file_name, name);
  }
};

Package *set_package(int32_t package_len, MSG_TYPE msg_type,
                     const char *filename, int32_t block_len, int32_t disk_no) {
  Package *package = new Package;
  package->package_len = package_len;
  package->msg_type = msg_type;
  // strncpy(package->filename, filename, strlen(filename) + 1);
  strcpy(package->filename, filename);
  package->block_len = block_len;
  package->disk_no = disk_no;
  return package;
}

/*
 * thread func
 */
void *thr_start(void *arg) {
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
  socket_fd.Connect("9.134.13.102", 6666); // test

  // // dispatch
  // if (disk_no < SERVER_DISK_COUNT / 2)
  // socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
  // else
  // socket_fd.Connect(SERVER_IP_ADDR_2, SERVER_PORT);

  // send head
  std::shared_ptr<Package> package(set_package(
      sizeof(Package), BIG_UPLOAD, file_name, real_block_size, disk_no));
  socket_fd.Send((void *)&(*package), package->package_len);

  // send file block
  socket_fd.SendFile(fd, &offset, real_block_size);

  // TODO: recv到关闭信号后close
  socket_fd.Close();
  std::cout << "(big_file_upload)pthread exit " << pthread_self() << std::endl;
  return nullptr;
}

void do_big_file_upload(int32_t fd, char *file_name, const uint64_t file_size) {
  // last_block:
  // const int32_t thr_num = BIG_FILE_UPLOAD_BLOCK_NUM;
  const int32_t thr_num = 4;
  const int32_t block_size = file_size / thr_num;
  const int32_t last_block = file_size % thr_num;

  pthread_t *tid = new pthread_t[thr_num];
  // change ThreadArgPtr's scope
  std::vector<ThreadArgPtr> vec(thr_num);

  // send head package
  TcpSocket socket_fd = TcpSocket();
  socket_fd.Socket();
  socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
  std::shared_ptr<HeadPackage> head_package(
      new HeadPackage(sizeof(HeadPackage), BIG_UPLOAD, file_name, file_size));
  socket_fd.Send((void *)&(*head_package), head_package->head_package_len);
  socket_fd.Close();

  for (int32_t i = 0; i < thr_num; ++i) {
    // [i*block_size, (i+1)*block_size) => 左闭右开
    // [(thr_num-2)*block_size, (thr_num-1)*block_size+last_block) => 最后一块
    off_t offset = i * block_size;
    int32_t real_block_size =
        (i == thr_num - 1) ? (block_size + last_block) : block_size;
    // std::cout << i << ". offset && real_block_size ==>" << offset << ":"
    // << real_block_size << std::endl;

    ThreadArgPtr arg =
        ThreadArgPtr(new ThreadArg(file_name, fd, offset, real_block_size, i));
    // WARN: 不可用std::move(arg)
    vec[i] = arg;

    int32_t res =
        pthread_create(&tid[i], nullptr, thr_start, (void *)&(vec[i]));

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
