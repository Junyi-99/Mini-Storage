#include <iostream>
#include "MyUdp.h"

#define BUFF_LEN 65520
#define FILE_LEN 60000


int main()
{

    TcpSocket skt,cli;
    skt.Socket();
    cli.Socket();
    skt.Bind("0.0.0.0",5555);
    skt.Listen(128);
    skt.Accept(cli);
    MyUdp udp;
    udp.Bind(6666);

    printf("%d\n\n",cli.GetFd());
    Package pk(0,0,0,"ss");
    cli.recv_cyl(&pk,sizeof(pk));
    //recv(5,&pk,sizeof(pk),0);
    int file_fd = open(pk.file_name, O_RDWR | O_CREAT, 0777);
    ftruncate(file_fd, pk.block_len);
    auto *mp = (char *) mmap(nullptr, pk.block_len, PROT_READ|PROT_WRITE , MAP_SHARED, file_fd, 0);
    if (mp == MAP_FAILED) {
        perror("Error memory mapping the file");
        return -1;

    }
    printf("%s---%d\n",pk.file_name,pk.block_len);
    int epoll_fd = epoll_create(5);
    epoll_register(EPOLL_CTL_ADD, EPOLLIN, epoll_fd, cli.GetFd());
    epoll_register(EPOLL_CTL_ADD, EPOLLIN, epoll_fd, udp.Get());


    char buf[BUFF_LEN] = {0};


    while(1)
    {
        struct epoll_event events[3];
        int ret = epoll_wait(epoll_fd, events, 3, -1);
        printf("epoll ret = : %d\n", ret);
        for (int i = 0; i < ret; i++) {
            auto fd = (int) events[i].data.fd;
            auto ev = (uint32_t) events[i].events;
            if ((fd == cli.GetFd()) && (ev & EPOLLIN)) {
                printf("ACCEPT: %d\n", fd);
                printf("finish: %d\n", fd);
                munmap(mp, pk.block_len);
            } else if (fd == udp.Get() && (ev & EPOLLIN)) {
                int cv_ret = 0;
                cv_ret = udp.Recv(buf,BUFF_LEN);
                if(cv_ret != FILE_LEN + 4 )
                    printf("error ret = : %d\n", cv_ret);
                int index = *(int*)buf;
                memcpy(mp+index*FILE_LEN, buf+4,cv_ret-4);
                cli.Send(&index, 4);
                printf("recv index = : %d\n", index);
            } 
        }
    }

    return 0;
}

