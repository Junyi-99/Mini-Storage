#pragma once

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>



enum MSG_TYPE : int8_t {
    INIT_STATUS = 0,
    SMALL_UPLOAD,
    SMALL_DOWNLOAD,
    BIG_UPLOAD,
    BIG_DOWNLOAD,
};


#pragma pack(push, 1)
struct Package {
    int msg_type;
    uint64_t block_len;
    uint32_t disk_no;
    char file_name[256];

    Package(int tp = INIT_STATUS, uint64_t blen = 0, uint32_t dno = 0,
            char *fname = nullptr)
        : msg_type(tp), block_len(blen), disk_no(0) {
            bzero((void *)file_name, sizeof(file_name));
            memset(file_name, 0, sizeof(file_name));
            strcpy(file_name, fname);
        }
};
#pragma pack(pop)





int epoll_register(int operat, int events_, int epoll_fd_, int fd_) {
    struct epoll_event event{};
    event.data.fd = fd_;
    event.events = events_;
    if (epoll_ctl(epoll_fd_, operat, fd_, &event)) {
        perror("epoll add failed in accept()\n");
    }
    return 0;
}


inline bool CHECK_RET(int32_t res, const char *errMsg) {
    if (!(res < 0))
        return true;
    perror(errMsg);
    return false;

}




class TcpSocket {
public:
    TcpSocket(int32_t fd = -1) : _fd(fd) {}
    ~TcpSocket() { Close(); }

    bool Socket() {
        _fd = socket(AF_INET, SOCK_STREAM, 0);
        // TODO: setsockopt??
        return CHECK_RET(_fd, "socket error!");
    }

    bool Close() {
        if (_fd >= 0)
            return true;
        int32_t res = close(_fd);
        _fd = -1;
        return CHECK_RET(res, "close error!!");
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
    bool Listen(const int32_t backlog = 10) {
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

    bool Send(const void *buf, const ssize_t size) {
        ssize_t send_size = 0;
        while (send_size < size) {
            ssize_t res = send(_fd, buf, size, 0);
            if (CHECK_RET(res, "send error!!"))
                return false;
            send_size += res;
        }
        return true;
    }

    ssize_t Recv(void *buf, const size_t size) {
        ssize_t len = recv(_fd, buf, size, 0);
        if (len < 0) {
            perror("recv error!");

        }
        if (len == 0) {
            std::cout << "recv return 0!" << std::endl;

        }
        return len;

    }



    ssize_t recv_cyl(void *pbuf,int len)
    {
        char *buf = (char*)pbuf;
        int total = 0, ret;
        while(total < len)
        {
            ret = recv(_fd, buf+total, len-total, 0);
            if(ret == 0)
            {
                return -1;
            }
            total += ret;
        }
        return 0;
    }

    void SetFd(const int fd) { _fd = fd; }
    int32_t GetFd() const { return _fd; }

private:
    int32_t _fd;
};



class MyUdp
{
public:
    MyUdp()
    {
        if((_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            perror("socket error!\n");
        }
    }

    int Get()
    {
        return _fd;
    }

    void Bind(int port)
    {
        struct sockaddr_in server_addr;
        socklen_t addrlen=sizeof(struct sockaddr_in);
        bzero(&server_addr,addrlen);
        server_addr.sin_family=AF_INET;
        server_addr.sin_addr.s_addr=htonl(INADDR_ANY);//任何主机地址
        server_addr.sin_port=htons(port);
        if(bind(_fd,(struct sockaddr *)&server_addr,addrlen)<0){
            printf("bind");
        }
    }

    int Send(char *buf, int len)
    {
        int ret = sendto(_fd,buf,len,0,(struct sockaddr*)&_cli_addr,sizeof(struct sockaddr));
        return ret;
    }

    int Recv(char *buf, int len)
    {
        socklen_t addrlen=sizeof(struct sockaddr_in);
        int ret = recvfrom(_fd,buf,len,0,NULL,&addrlen);
        return ret;
    }

    void Set (const char* ip, int port)
    {
        bzero(&_cli_addr, sizeof(struct sockaddr_in));
        _cli_addr.sin_family=AF_INET;
        _cli_addr.sin_addr.s_addr=inet_addr(ip);//任何主机地址
        _cli_addr.sin_port=htons(port);
    }

    ~MyUdp()
    {
        close (_fd);
    }

private:
    int _fd;
    struct sockaddr_in _cli_addr;
};

