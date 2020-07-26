#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "../include/tcp.h"

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

    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt error\n");
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


    int err = -1;        /* 返回值 */
    int snd_size = 0;   /* 发送缓冲区大小 */
    int rcv_size = 0;    /* 接收缓冲区大小 */
    socklen_t optlen;    /* 选项值长度 */


    /*
     * 先读取缓冲区设置的情况
     * 获得原始发送缓冲区大小
     */
    optlen = sizeof(snd_size);
    err = getsockopt(server_fd, SOL_SOCKET, SO_SNDBUF,&snd_size, &optlen);
    if(err<0){
        perror("error");
        printf("获取发送缓冲区大小错误\n");
    }
    /*
     * 打印原始缓冲区设置情况
     */

    /*
     * 获得原始接收缓冲区大小
     */
    optlen = sizeof(rcv_size);
    err = getsockopt(server_fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &optlen);
    if(err<0){
        perror("error");
        printf("获取接收缓冲区大小错误\n");
    }

    printf(" 发送缓冲区原始大小为: %d 字节\n",snd_size);
    printf(" 接收缓冲区原始大小为: %d 字节\n",rcv_size);

    /*
     * 设置发送缓冲区大小
     */
    snd_size = 4096*1024;    /* 发送缓冲区大小为8K */
    optlen = sizeof(snd_size);
    err = setsockopt(server_fd, SOL_SOCKET, SO_SNDBUF, &snd_size, optlen);
    if(err<0){
        printf("设置发送缓冲区大小错误\n");
    }

    /*
     * 设置接收缓冲区大小
     */
    rcv_size = 4096*1024;    /* 接收缓冲区大小为8K */
    optlen = sizeof(rcv_size);
    err = setsockopt(server_fd,SOL_SOCKET,SO_RCVBUF, (char *)&rcv_size, optlen);
    if(err<0){
        printf("设置接收缓冲区大小错误\n");
    }

    /*
     * 检查上述缓冲区设置的情况
     * 获得修改后发送缓冲区大小
     */
    optlen = sizeof(snd_size);
    err = getsockopt(server_fd, SOL_SOCKET, SO_SNDBUF,&snd_size, &optlen);
    if(err<0){
        printf("获取发送缓冲区大小错误\n");
    }

    /*
     * 获得修改后接收缓冲区大小
     */
    optlen = sizeof(rcv_size);
    err = getsockopt(server_fd, SOL_SOCKET, SO_RCVBUF,(char *)&rcv_size, &optlen);
    if(err<0){
        printf("获取接收缓冲区大小错误\n");
    }

    /*
     * 打印结果
     */
    printf(" 发送缓冲区大小为: %d 字节\n",snd_size);
    printf(" 接收缓冲区大小为: %d 字节\n",rcv_size);



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

    //set_unblock(connect_fd);

    // ET 模式 使用 ONESHOT，保证通知线程之后，不再次通知
    epoll_register(EPOLLIN | EPOLLET | EPOLLONESHOT, epoll_fd, connect_fd); // connection_fd 绑定到 epoll
    return connect_fd;
}

// 用这个函数的好处就是自动帮你关闭连接
int tcp_receive(int client_fd, void *buf, int64_t n) {
    auto ret = recv(client_fd, buf, n, 0); // 取一个 header 这么大的数据

    if (ret < 0) {
        // 连接被重置
        if (errno == ECONNRESET) {
            perror("ECONNRESET");
        } else if (errno == EAGAIN) {
            perror("EAGAIN");
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

int tcp_send(int sock_fd, char *buffer, int64_t length) {

    while (length > 0) {
        auto num = send(sock_fd, buffer, length, 0);
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

int tcp_sendfile(int sock_fd, int file_fd, off64_t *offset, int64_t bytes_need_to_send) {
    while (bytes_need_to_send > 0) {
        auto res = sendfile64(sock_fd, file_fd, offset, bytes_need_to_send);
        if (res <= 0) {
            perror("send file error");
            return false;
        }
        bytes_need_to_send -= res;
    }
    return true;
}