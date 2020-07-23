#include "../include/big_file.h"
#include <sys/mman.h>
#include <unistd.h>

inline int32_t upper(int32_t block_size, int32_t page_size) {
  return (block_size % page_size) ? block_size * ((block_size / page_size) + 1)
                                  : block_size;
}

void *thr_start(void *arg) {
  TcpSocket socket_fd = TcpSocket();
  socket_fd.Socket();
  socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
  std::cout << "connect success! sock_fd: " << socket_fd.GetFd() << std::endl;

  ThreadArgPtr tupPtr = *((ThreadArgPtr *)arg);
  off_t offset;
  char *file_name;
  int32_t fd, real_block_size, disk_no;
  std::tie(file_name, fd, offset, real_block_size, disk_no) = *tupPtr;
  std::cout << "thr arg " << pthread_self() << "==>" << file_name << "," << fd
            << "," << offset << "," << real_block_size << disk_no << std::endl;

  // send head
  std::shared_ptr<Package> package(set_package(
      sizeof(Package), BIG_DOWNLOAD, file_name, real_block_size, disk_no));
  socket_fd.Send((void *)&(*package), package->package_len);

  // recv
  // char *real_file_name = getcwd(nullptr, 0);
  char real_file_name[1024] = {0};
  getcwd(real_file_name, sizeof(real_file_name));
  strcat(real_file_name, "\\");
  strcat(real_file_name, file_name);
  int file_fd = open(file_name, O_WRONLY | O_CREAT, 0777);

  // long page_size = sysconf(_SC_PAGESIZE);
  // real_block_size:
  //    必须是page_size整数倍,否则向上对齐,对齐部分用'\0'填充,改变填充部分不会影响文件数据
  // MAP_SHARED:
  //    写入内存后直接写入文件
  char *mmap_ptr =
      (char *)mmap(nullptr, real_block_size, PROT_WRITE | PROT_EXEC, MAP_SHARED,
                   file_fd, offset);
  if ((void *)mmap_ptr == MAP_FAILED) {
    perror("mmap error!");
    return nullptr;
  }

  // char buf[1024 * 4] = {0};
  uint64_t writed_size = 0;
  while (true) {
    // ssize_t recv_size = socket_fd.Recv(buf, sizeof(buf) - 1);
    ssize_t recv_size =
        socket_fd.Recv(mmap_ptr + writed_size, real_block_size - writed_size);
    if (!CHECK_RET(recv_size, "Recv error! please check big_file_downlaod!"))
      break;
    if (recv_size == 0)
      break;
    writed_size += recv_size;
  }

  socket_fd.Close();
  close(file_fd);
  munmap(mmap_ptr, upper(real_block_size, sysconf(_SC_PAGESIZE)));
  std::cout << "(big_file_downlaod)pthread exit " << pthread_self()
            << std::endl;
  return nullptr;
}

void do_big_file_download(char *file_name, const uint64_t file_size) {
  const int32_t thr_num = BIG_FILE_DOWNLOAD_THR_NUM;
  const int32_t block_size = file_size / thr_num;
  const int32_t last_block = file_size % thr_num;

  pthread_t *tid = new pthread_t[thr_num];
  std::vector<ThreadArgPtr> vec(thr_num);

  for (int32_t i = 0; i < thr_num; ++i) {
    off_t offset = i * block_size;
    int32_t real_block_size =
        (i == thr_num - 1) ? (block_size + last_block) : block_size;
    ThreadArgPtr arg =
        ThreadArgPtr(new ThreadArg(file_name, -1, offset, real_block_size, i));
    vec[i] = arg;

    int32_t res = pthread_create(&tid[i], nullptr, thr_start, (void *)&arg);

    if (res != 0) {
      std::cerr << "(big_file_downlaod)创建第i=" << i--
                << "个线程时失败,pthread_id=" << pthread_self() << std::endl;
      continue;
    }
  }
  for (int32_t i = 0; i < thr_num; ++i) {
    pthread_join(tid[i], nullptr);
  }
}
