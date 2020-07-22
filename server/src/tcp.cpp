#include "tcp.h"

int tcp_init(int port, int max_connection) {

    int server_fd;
    struct sockaddr_in server_addr{};

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// 本地任意ip
    server_addr.sin_port = htons(port);

    // 创建 socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error\n");
        return -1;
    }

    // 绑定端口
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind error\n");
        return -1;
    }

    // 监听
    if (listen(server_fd, max_connection) < 0) {
        perror("listen error\n");
        return -1;
    }

    set_unblock(server_fd);

    return server_fd;
}

int tcp_accept(int epoll_fd, int fd) {

    struct sockaddr_in client_addr{};
    socklen_t client_length;
    int connect_fd;

    client_length = sizeof(client_addr);
    bzero(&client_addr, client_length);

    connect_fd = accept(fd, (struct sockaddr *) &client_addr, &client_length);
    if (connect_fd < 0) {
        perror("BAD ACCEPT\n");
        close(fd);
        return -1;
    }

    set_unblock(connect_fd);

    epoll_register(EPOLLIN | EPOLLET, epoll_fd, connect_fd); // connection_fd 绑定到 epoll
    return connect_fd;
}

int tcp_receive(int client_fd) {

    int ret;
    char buf[128] = {0};

    ret = recv(client_fd, buf, sizeof(buf), 0);
    if (ret < 0) {
        // 连接被重置
        if (errno == ECONNRESET) {
            close(client_fd);
        } else {
            perror("read error");
        }
        return -1;
    } else if (ret == 0) {
        // 客户端关闭连接
        close(client_fd);
        printf("client %d connection closed\n", client_fd);
    } else {
        // 正常接收到服务器数据
        printf("receive data from %d, length %d: %s\n", client_fd, ret, buf);

        tcp_send(client_fd, buf, ret); // echo back
        close(client_fd);
    }
    return 0;
}

int tcp_send(int sock_fd, char *buffer, std::size_t length) {

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