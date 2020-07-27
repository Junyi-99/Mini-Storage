#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "../include/tcp.h"

int tcp_connect(const char *ip_addr) {
    int confd = socket(AF_INET, SOCK_STREAM, 0);


    int err = -1;        /* 返回值 */
    int snd_size = 0;   /* 发送缓冲区大小 */
    int rcv_size = 0;    /* 接收缓冲区大小 */
    socklen_t optlen;    /* 选项值长度 */


    /*
     * 先读取缓冲区设置的情况
     * 获得原始发送缓冲区大小
     */
    optlen = sizeof(snd_size);
    err = getsockopt(confd, SOL_SOCKET, SO_SNDBUF,&snd_size, &optlen);
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
    err = getsockopt(confd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &optlen);
    if(err<0){
        perror("error");
        printf("获取接收缓冲区大小错误\n");
    }

    //printf(" 发送缓冲区原始大小为: %d 字节\n",snd_size);
    //printf(" 接收缓冲区原始大小为: %d 字节\n",rcv_size);

    /*
     * 设置发送缓冲区大小
     */
    snd_size = 4096*1024;    /* 发送缓冲区大小为8K */
    optlen = sizeof(snd_size);
    err = setsockopt(confd, SOL_SOCKET, SO_SNDBUF, &snd_size, optlen);
    if(err<0){
        printf("设置发送缓冲区大小错误\n");
    }

    /*
     * 设置接收缓冲区大小
     */
    rcv_size = 4096*1024;    /* 接收缓冲区大小为8K */
    optlen = sizeof(rcv_size);
    err = setsockopt(confd,SOL_SOCKET,SO_RCVBUF, (char *)&rcv_size, optlen);
    if(err<0){
        printf("设置接收缓冲区大小错误\n");
    }

    /*
     * 检查上述缓冲区设置的情况
     * 获得修改后发送缓冲区大小
     */
    optlen = sizeof(snd_size);
    err = getsockopt(confd, SOL_SOCKET, SO_SNDBUF,&snd_size, &optlen);
    if(err<0){
        printf("获取发送缓冲区大小错误\n");
    }

    /*
     * 获得修改后接收缓冲区大小
     */
    optlen = sizeof(rcv_size);
    err = getsockopt(confd, SOL_SOCKET, SO_RCVBUF,(char *)&rcv_size, &optlen);
    if(err<0){
        printf("获取接收缓冲区大小错误\n");
    }

    /*
     * 打印结果
     */
   // printf(" 发送缓冲区大小为: %d 字节\n",snd_size);
    //printf(" 接收缓冲区大小为: %d 字节\n",rcv_size);



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


/*
  SDBMHash function to calculate send to which disk(server)
  @param str  input filepath
  @return num
*/
unsigned int my_hash(char *str) {
    unsigned int hash = 0;

    while (*str) {
        // equivalent to: hash = 65599*hash + (*str++);
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
    }

    return (hash & SERVER_DISK_COUNT);
}
