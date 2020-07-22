#include "../include/big_file.h"

void *thr_start(void *arg) {
  TcpSocket socket_fd = TcpSocket();
  socket_fd.Socket();
  socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
  std::cout << "connect success! sock_fd: " << socket_fd.GetFd() << std::endl;

  // TODO:

  return nullptr;
}

void do_big_file_download(char *file_name, const uint64_t file_size) {
  const uint32_t thr_num = BIG_FILE_DOWNLOAD_THR_NUM;
  const uint32_t block_size = file_size / thr_num;
  const uint32_t last_block = file_size % thr_num;

  pthread_t *tid = new pthread_t[thr_num];
  std::vector<ThreadArgPtr> vec(thr_num);

  for (uint32_t i = 0; i < thr_num; ++i) {
    off_t offset = i * block_size;
    uint32_t real_block_size =
        (i == thr_num - 1) ? (block_size + last_block) : block_size;
    ThreadArgPtr arg =
        ThreadArgPtr(new ThreadArg(file_name, -1, offset, real_block_size, i));
    vec[i] = arg;

    uint32_t res = pthread_create(&tid[i], nullptr, thr_start, (void *)&arg);

    if (res != 0) {
      std::cerr << "(big_file_downlaod)创建第i=" << i--
                << "个线程时失败,pthread_id=" << pthread_self() << std::endl;
      continue;
    }
  }
  for (uint32_t i = 0; i < thr_num; ++i) {
    pthread_join(tid[i], nullptr);
  }
}
