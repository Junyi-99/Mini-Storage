<<<<<<< HEAD
#include "../etc/config.h"
#include "../include/big_file.h"
#include "../include/small_file.h"
#include "../include/tcp.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    // TODO: 文件名切分，去除路径
    char *filename = split_filename(argv[1]);
    // TODO: 向server请求文件大小
    int connfd = tcp_connect(SERVER_IP_ADDR_1);
    Package *package = set_package(sizeof(Package), INIT_STATUS, filename, 0, 0);
    delete filename;

    ssize_t send_size = send(connfd, (void *) package, package->package_len, 0);
    if (send_size != package->package_len) {
        perror("Send error!");
        close(connfd);
        exit(0);
    }
    ssize_t read_size = recv(connfd, (void*)package, ACK_SIZE, 0);

    if (package->block_len > SMALL_FILE_SIZE_MAX) {
        do_big_file_download(package->filename, package->block_len);
    } else {
        do_small_file_download(connfd, package->filename, package->block_len);
    }
    delete package;
    exit(0);
=======
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
>>>>>>> 9a994cd6b73872836cca57f9d19c0d70a9ce13eb
}
