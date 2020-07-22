#pragma once
#include <arpa/inet.h>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <stdint.h>
#include <string>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <unistd.h>

inline bool CHECK_RET(int32_t res, const char *errMsg) {
  if (!(res < 0))
    return true;
  perror(errMsg);
  return false;
}

/*
 * @description: 对socket函数进行封装 : ipv4 tcp协议
 *               一个fd对应一个类实例，服务端和客户端通用
 * @说明: const 类型参数为传入参数
 *        *     指针类型参数为传出参数
 *        &     非const引用类型为传入传出参数
 */

class TcpSocket {
public:
  TcpSocket(uint32_t fd = -1) : _fd(fd) {}
  ~TcpSocket() {
    if (!(_fd < 0))
      Close();
  }

  bool Socket() {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    // TODO: setsockopt??
    return CHECK_RET(_fd, "socket error!");
  }

  bool Close() {
    int32_t res = close(_fd);
    return CHECK_RET(res, "close error!");
  }

  bool Bind(const std::string &ip, const uint16_t port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = std::move(inet_addr(ip.c_str()));
    addr.sin_port = std::move(htons(port));
    int res = bind(_fd, (sockaddr *)&addr, sizeof(addr));
    return CHECK_RET(res, "bind error!");
  }

  // TODO: max = 10?
  bool Listen(const uint32_t backlog = 10) {
    int32_t res = listen(_fd, backlog);
    return CHECK_RET(res, "listen error!");
  }

  bool Accept(TcpSocket &peer, std::string *ip = nullptr,
              uint16_t *port = nullptr) {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int32_t new_sock = accept(_fd, (sockaddr *)&addr, &len);
    if (new_sock < 0) {
      perror("accept error!");
      return false;
    }
    peer.SetFd(new_sock);
    if (ip != nullptr)
      *ip = inet_ntoa(addr.sin_addr);
    if (port != nullptr)
      *port = ntohs(addr.sin_port);
    return true;
  }

  bool Connect(const std::string &ip, const uint16_t port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    int32_t res = connect(_fd, (sockaddr *)&addr, sizeof(addr));
    return CHECK_RET(res, "connect error!");
  }

  bool Send(const void *buf, const size_t size) {
    int res = send(_fd, buf, size, 0);
    return CHECK_RET(res, "send error!");
  }

  bool SendFile(const uint32_t file_fd, off_t *offset, const size_t size) {
    // int32_t res = send(_fd, buf.c_str(), buf.size(), 0);
    ssize_t res = sendfile(_fd, file_fd, offset, size);
    return CHECK_RET(res, "sendfile error!");
  }

  /*
   * bool Recv(std::string *buf) {
   * char tmp[1024 * 4] = {0};
   * int32_t len = recv(_fd, tmp, sizeof(tmp) - 1, 0);
   * if (len < 0) {
   * perror("recv error!");
   * return false;
   * }
   * if (len == 0) {
   * std::cout << "recv return 0!" << std::endl;
   * return false;
   * }
   * buf->assign(tmp, len);
   * return true;
   * }
   */

  void SetFd(const int fd) { _fd = fd; }
  uint32_t GetFd() const { return _fd; }

private:
  uint32_t _fd;
};
