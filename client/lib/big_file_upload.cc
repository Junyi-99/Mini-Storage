#include "../include/big_file.h"
#include <cstring>
#include <pthread.h>

Package *set_package(uint32_t package_len, MSG_TYPE msg_type,
                     const char *filename, uint32_t block_len,
                     uint32_t disk_no) {
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
  // socket init
  TcpSocket socket_fd = TcpSocket();
  socket_fd.Socket();
  socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
  std::cout << "connect success! sock_fd: " << socket_fd.GetFd() << std::endl;

  // init arg
  ThreadArgPtr tupPtr = *((ThreadArgPtr *)arg);
  off_t offset;
  char *file_name;
  uint32_t fd, real_block_size, disk_no;
  std::tie(file_name, fd, offset, real_block_size, disk_no) = *tupPtr;
  std::cout << "thr arg " << pthread_self() << "==>" << file_name << "," << fd
            << "," << offset << "," << real_block_size << disk_no << std::endl;

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

void do_big_file_upload(uint32_t fd, char *file_name,
                        const uint64_t file_size) {
  // last_block:
  const uint32_t thr_num = BIG_FILE_UPLOAD_BLOCK_NUM;
  const uint32_t block_size = file_size / thr_num;
  const uint32_t last_block = file_size % thr_num;

  pthread_t *tid = new pthread_t[thr_num];
  // change ThreadArgPtr's scope
  std::vector<ThreadArgPtr> vec(thr_num);

  for (uint32_t i = 0; i < thr_num; ++i) {
    // [i*block_size, (i+1)*block_size) => 左闭右开
    // [(thr_num-2)*block_size, (thr_num-1)*block_size+last_block) => 最后一块
    off_t offset = i * block_size;
    uint32_t real_block_size =
        (i == thr_num - 1) ? (block_size + last_block) : block_size;
    std::cout << i << ". offset && real_block_size ==>" << offset << ":"
              << real_block_size << std::endl;

    ThreadArgPtr arg =
        ThreadArgPtr(new ThreadArg(file_name, fd, offset, real_block_size, i));
    // WARN: 不可用std::move(arg)
    vec[i] = arg;

    uint32_t res = pthread_create(&tid[i], nullptr, thr_start, (void *)&arg);

    // if fail: again
    if (res != 0) {
      std::cerr << "(big_file_upload)创建第i=" << i--
                << "个线程时失败,pthread_id=" << pthread_self() << std::endl;
      continue;
    }
  }
  for (uint32_t i = 0; i < thr_num; ++i) {
    pthread_join(tid[i], nullptr);
  }
}
