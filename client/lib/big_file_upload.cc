#include "../../utils/tcp_socket.hpp"
#include "../etc/config.h"
#include "../include/big_file.h"
#include <cstring>
#include <memory>
#include <pthread.h>
#include <tuple>

using ThreadArg = std::tuple<const char *, uint32_t, off_t, uint32_t>;

// TODO: 抽象my_hash, Package
// TODO: encode
struct Package {
  uint32_t package_len;
  MSG_TYPE msg_type; // upload or download, small or big file
  char filename[1024];
  uint32_t block_len;
  uint32_t disk_no;
};

/*
 * SDBMHash function to calculate send to which disk(server)
 * @param str  input filepath
 * @return num
 */
uint32_t my_hash(const char *str) {
  uint32_t hash = 0;
  while (*str) {
    // equivalent to: hash = 65599*hash + (*str++);
    hash = (*str++) + (hash << 6) + (hash << 16) - hash;
  }
  return (hash & SERVER_DISK_COUNT);
}

Package *set_package(uint32_t package_len, MSG_TYPE msg_type,
                     const char *filename, uint32_t block_len,
                     uint32_t disk_no) {
  Package *package = new Package;
  package->package_len = package_len;
  package->msg_type = msg_type;
  strncpy(package->filename, filename, strlen(filename) + 1);
  // strcpy(package->filename, filename);
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

  // init arg
  ThreadArg *tupPtr = (ThreadArg *)arg;
  off_t offset;
  const char *file_name;
  uint32_t fd, real_block_size;
  std::tie(file_name, fd, offset, real_block_size) = *tupPtr;
  std::cout << "!!!!!" << offset << std::endl;

  // send head
  uint32_t disk_no = my_hash(file_name);
  Package *package = set_package(sizeof(Package), BIG_UPLOAD, file_name,
                                 real_block_size, disk_no);
  std::shared_ptr<Package> s_package(package);
  socket_fd.Send((void *)package, package->package_len);

  // send file block
  socket_fd.SendFile(fd, &offset, real_block_size);

  // TODO: recv
  socket_fd.Close();
  return nullptr;
}

void do_big_file_upload(const uint32_t fd, const char *file_name,
                        const uint64_t file_size) {
  // last_block:
  const uint32_t thr_num = BIG_FILE_UPLOAD_BLOCK_NUM;
  const uint32_t block_size = file_size / (BIG_FILE_UPLOAD_BLOCK_NUM);
  const uint32_t last_block = file_size % (BIG_FILE_UPLOAD_BLOCK_NUM);

  pthread_t *tid = new pthread_t[thr_num];
  for (uint32_t i = 0; i < thr_num; ++i) {
    // [i*block_size, (i+1)*block_size) => 左闭右开
    // [(thr_num-2)*block_size, (thr_num-1)*block_size+last_block) => 最后一块
    const off_t offset = i * BIG_FILE_UPLOAD_BLOCK_NUM;
    const uint32_t real_block_size =
        (i == thr_num - 1) ? (block_size + last_block) : block_size;
    ThreadArg arg = std::make_tuple(file_name, fd, offset, real_block_size);

    uint32_t res = pthread_create(&tid[i], nullptr, thr_start, (void *)&arg);
    // if fail: again
    if (res != 0) {
      std::cerr << "创建第i=" << i--
                << "个线程时失败,pthread_id=" << pthread_self() << std::endl;
      continue;
    }
  }
  for (uint32_t i = 0; i < thr_num; ++i) {
    pthread_join(tid[i], nullptr);
  }
}
