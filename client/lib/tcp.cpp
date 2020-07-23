#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "../include/tcp.h"

int tcp_connect(const char *ip_addr) {
    int confd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip_addr, &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons(SERVER_PORT);

    connect(confd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
    if (confd == -1) {
        perror("Connect error!");
        close(confd);
    }
    return confd;
}

// 用这个函数的好处就是自动帮你关闭连接
int tcp_receive(int client_fd, void *buf, size_t n) {
    int ret = recv(client_fd, buf, n, 0); // 取一个 header 这么大的数据

    if (ret < 0) {
        // 连接被重置
        if (errno == ECONNRESET) {
        } else {
            perror("tcp_receive error");
        }
        close(client_fd);
        return -1;
    } else if (ret == 0) {
        // 客户端关闭连接
        close(client_fd);
        printf("client %d connection closed\n", client_fd);
        return -1;
    }
    return ret;
}

int tcp_send(int sock_fd, char *buffer, size_t length) {

    while (length > 0) {
        int num = send(sock_fd, buffer, length, 0);
        if (num < 0) {
            perror("send error:");
            close(sock_fd);
            return false;
        } else {
            length -= num;
            buffer += num;
        }
    }

    return 0;
}