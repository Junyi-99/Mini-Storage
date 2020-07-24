#include "../../etc/config.hpp"
#include "../include/big_file.h"
#include "../include/small_file.h"
#include "../utils/tcp_socket.hpp"

int main(int argc, char *argv[]) {
  if (argc == 1) {
    std::cout << "error   =>>   文件名??" << std::endl;
    return 0;
  }
  // request file_size(uint64_t)
  char *file_name = argv[1];
  TcpSocket socket_fd = TcpSocket();
  socket_fd.Socket();
  socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);
  socket_fd.Send((void *)file_name, strlen(file_name));
  uint64_t file_size;
  socket_fd.Recv((void *)&file_size, sizeof(file_size));

  if (file_size > SMALL_FILE_SIZE_MAX) {
    do_big_file_download(file_name, file_size);
  } else {
    // do_small_file_download(argv[1], file_size);
  }
  socket_fd.Close();
  return 0;
}
