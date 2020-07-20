#include "base.h"

int tcp_init(int port) {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in ser;
  bzero(&ser, sizeof(ser));
  ser.sin_family = AF_INET;
  ser.sin_port = htons(port);
  ser.sin_addr.s_addr = inet_addr("0.0.0.0");
  int ret = bind(socket_fd, (struct sockaddr *)&ser, sizeof(ser));
  if (ret == -1) {
    perror("bind");
    close(socket_fd);
    exit(-1);
  }
  ret = listen(socket_fd, 128);
  if (ret == -1) {
    perror("listen");
    close(socket_fd);
    exit(-1);
  }
  return socket_fd;
}

int tcp_accept(int sfd) {
  struct sockaddr_in client_addr;
  bzero(&client_addr, sizeof(client_addr));
  socklen_t addr_len = sizeof(struct sockaddr);
  int new_fd = accept(sfd, (struct sockaddr *)&client_addr, &addr_len);
  if (new_fd == -1) {
    perror("accept");
    close(sfd);
  }
  return new_fd;
}
