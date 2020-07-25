#include "../include/big_file.h"
#include <sys/mman.h>
#include <unistd.h>

// @description: 向上取整
inline uint64_t upper(uint64_t block_size, uint32_t page_size) {
  return (block_size % page_size) ? block_size * ((block_size / page_size) + 1)
                                  : block_size;
}

// @fuck: 图省事,先放这儿吧...
char *global_mmap_start_ptr = nullptr;
uint64_t global_block_size = 0;

void *thr_start(void *arg) {
  TcpSocket socket_fd = TcpSocket();
  socket_fd.Socket();

  ThreadArgPtr tupPtr = *((ThreadArgPtr *)arg);
  off_t offset;
  char *file_name;
  int32_t __fd, disk_no;
  uint64_t real_block_size;
  std::tie(file_name, __fd, offset, real_block_size, disk_no) = *tupPtr;

  // dispatch
  if (disk_no < SERVER_DISK_COUNT / 2)
    socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
  else
    socket_fd.Connect(SERVER_IP_ADDR_2, SERVER_PORT);

  // send head
  std::shared_ptr<Package> package(
      new Package(BIG_DOWNLOAD, real_block_size, disk_no, file_name));
  socket_fd.Send((void *)&(*package), sizeof(Package));

  // recv block data
  uint64_t writed_size = 0;
  char *thr_mmap_ptr = global_mmap_start_ptr + disk_no * global_block_size;
  while (true) {
    ssize_t recv_size = socket_fd.Recv(thr_mmap_ptr + writed_size,
                                       real_block_size - writed_size);
    if (!CHECK_RET(recv_size, "Recv error! please check big_file_downlaod!"))
      break;
    if (recv_size == 0)
      break;
    msync(thr_mmap_ptr + writed_size, recv_size, MS_SYNC);
    writed_size += recv_size;
  }

  socket_fd.Close();
  return nullptr;
}

void do_big_file_download(char *file_name, const uint64_t file_size) {
  const int32_t thr_num = BIG_FILE_DOWNLOAD_THR_NUM;
  const uint64_t block_size = file_size / thr_num;
  const uint64_t last_block = file_size % thr_num;

  int fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, 0777);
  truncate64(file_name, file_size);
  char *mmap_ptr = (char *)mmap(nullptr, file_size, PROT_WRITE | PROT_EXEC,
                                MAP_SHARED, fd, 0);
  if ((void *)mmap_ptr == MAP_FAILED) {
    perror("mmap error!");
    return;
  }

  // TODO: kill global varibles
  global_block_size = block_size;
  global_mmap_start_ptr = mmap_ptr;

  pthread_t *tid = new pthread_t[thr_num];
  std::vector<ThreadArgPtr> vec(thr_num);

  for (int32_t i = 0; i < thr_num; ++i) {
    off_t offset = i * block_size;
    uint64_t real_block_size =
        (i == thr_num - 1) ? (block_size + last_block) : block_size;
    ThreadArgPtr arg =
        ThreadArgPtr(new ThreadArg(file_name, -1, offset, real_block_size, i));
    vec[i] = arg;

    int32_t res =
        pthread_create(&tid[i], nullptr, thr_start, (void *)&(vec[i]));

    if (res != 0) {
      std::cerr << "create thr error: " << i-- << std::endl;
      continue;
    }
  }

  for (int32_t i = 0; i < thr_num; ++i) {
    pthread_join(tid[i], nullptr);
  }
  munmap(mmap_ptr, upper(file_size, sysconf(_SC_PAGESIZE)));
  close(fd);
}
