#include <iostream>
#include "MyUdp.h"
#include <vector>


#define BUFF_LEN 65520
#define FILE_LEN 60000


void send_some_time(MyUdp& udp, std::vector<bool> &mapp, char* buf, char* mp, int tnm, off_t file_size)
{
    bool flag = true;
    static int i = 0;
    int sum = 0;
    while(flag)
    {
        i = (i+1) % mapp.size();
        if (mapp[i] ==0)
        {
            sum++;
            if(sum >= tnm)
                flag = 0;
            if(i != mapp.size()-1)
            {
                bzero(buf,BUFF_LEN);
                memcpy(buf,&i,4);
                memcpy(buf+4,mp+i*FILE_LEN,FILE_LEN);
                udp.Send(buf,FILE_LEN+4);
            }else
            {
                bzero(buf,BUFF_LEN);
                memcpy(buf,&i,4);
                memcpy(buf+4,mp+i*FILE_LEN,file_size%FILE_LEN);
                udp.Send(buf,file_size%FILE_LEN+4);
            }
        }
    }
}




int main()
{

    TcpSocket skt;
    skt.Socket();
    skt.Connect("127.0.0.1",5555);
    MyUdp udp;
    udp.Set("127.0.0.1",6666);

    int file_fd = open("./test/file_test", O_RDONLY | O_CREAT, 0777);
    struct stat file_state;
    fstat(file_fd, &file_state);
    uint64_t file_size = file_state.st_size;
    std::cout << "file_size: " << file_size << std::endl;


    Package pk(0,file_size,0,"file_test");
    skt.Send(&pk,sizeof(pk));

    auto *mp = (char *) mmap(nullptr, pk.block_len, PROT_READ, MAP_SHARED, file_fd, 0);

    if (mp == MAP_FAILED) {
        perror("Error memory mapping the file");
        return -1;
    }


    int epoll_fd = epoll_create(5);
    epoll_register(EPOLL_CTL_ADD, EPOLLIN, epoll_fd, skt.GetFd());

    int num = file_size / FILE_LEN;
    std::vector<bool> flag(num+1,0);
    int fail_num = 0;
    int true_num = 0;
    int ret_num = 0;
    char buf[BUFF_LEN] = {0};
    
    send_some_time(udp, flag, buf ,mp ,100, file_size);

    //    for(int k = 0; k<100;k++ )
    //    {
    //        for(int i=0; i< flag.size();i++)
    //        {
    //            if(flag[i] == 0)
    //            {
    //                if(i != flag.size()-1)
    //                {
    //                    bzero(buf,sizeof(buf));
    //                    memcpy(buf,&i,4);
    //                    memcpy(buf+4,mp+i*FILE_LEN,FILE_LEN);
    //                    udp.Send(buf,FILE_LEN+4);
    //                }else
    //                {
    //                    bzero(buf,sizeof(buf));
    //                    memcpy(buf,&i,4);
    //                    memcpy(buf+4,mp+i*FILE_LEN,file_size%FILE_LEN);
    //                    udp.Send(buf,file_size%FILE_LEN+4);
    //                }
    //                break;
    //            }
    //        }
    //    }
    printf("11111111\n\n") ;  

    while(1)
    {
        struct epoll_event events[3];
        int ret = epoll_wait(epoll_fd, events, 3, -1);
        for (int i = 0; i < ret; i++) {
            auto fd = (int) events[i].data.fd;
            auto ev = (uint32_t) events[i].events;
            if ((fd == skt.GetFd()) && (ev & EPOLLIN)) {
                int pos;
                skt.recv_cyl(&pos,4);
                if (flag[pos] == 0)
                {   
                    true_num++;
                    if(true_num == num + 1)
                    {
                        printf("finish \n");
                        printf("file_num : %d \n",fail_num);
                        exit(1);
                    }
                    printf("pos = %d\n",pos);
                    printf("sum : %d\n",true_num);
                    flag[pos] = 1;
                    
                    ret_num++;
                    if(ret_num >= 50)
                    {
                        ret_num = 0;
                        send_some_time(udp,flag,buf,mp,50,file_size);
                    }

                }else
                {
                    fail_num++;
                    //    printf("error_sum : %d\n",fail_num);
                    continue;
                }
            }
        } 
    }

    return 0;
}

