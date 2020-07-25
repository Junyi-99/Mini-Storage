#pragma once

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../../etc/config.hpp"

int tcp_connect(const char *ip_addr);

int tcp_receive(int client_fd, void *buf, size_t n);

int tcp_send(int sock_fd, char *buffer, size_t length);

