#include "../etc/config.h"

struct Package {
    uint64_t package_len;
    uint32_t msg_type;
    char filename[256];
    uint64_t block_len;
    uint32_t disk_no;
};

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

Package *
set_package(size_t package_len, unsigned int msg_type, char filename[256], size_t block_len, unsigned int disk_no) {
    Package *package = new Package;
    package->package_len = package_len;
    package->msg_type = msg_type;
    strcpy(package->filename, filename);
    package->block_len = block_len;
    package->disk_no = disk_no;
    return package;
}

void do_small_file_upload(int fd, char *file_name, size_t file_size) {
    /* Calculate send to which disk(server) */
    unsigned int disk_no = my_hash(file_name);
    // printf("send file: %s\n", file_name);

    /* Connect to server(single connection) */
    const char *ip_addr = (disk_no > SINGLE_SERVER_DISK) ?
                          SERVER_IP_ADDR_1 : SERVER_IP_ADDR_2;
    int connfd = tcp_connect(ip_addr);

    /* Send head */
    Package *package = set_package(sizeof(Package), SMALL_UPLOAD, file_name, file_size, disk_no);
    ssize_t send_size = write(connfd, (void *) package, package->package_len);
    if (send_size != package->package_len) {
        perror("Send error!");
        close(connfd);
        close(fd);
        exit(0);
    }
    delete package;

    /* Send file */
    ssize_t total_sent = 0;

    off_t offset = 0;
    while (offset < file_size) {
        send_size = sendfile(connfd, fd, &offset, file_size);
        total_sent += send_size;
        printf("bytes sent: %zd, total: %zd\n", send_size, total_sent);
        if (send_size < 0) {
            perror("sendfile error");
            close(connfd);
            close(fd);
            exit(0);
        }
    }


    if (total_sent != file_size) {
        perror("Send error! total_sent != file_size");
        close(connfd);
        close(fd);
        exit(0);
    }

    /* Get ack signal & close connection */
    char readBuffer[ACK_SIZE];
    ssize_t read_size = recv(connfd, readBuffer, ACK_SIZE, 0);
    if (read_size == 0) {
        printf("Transfer complete!\n");
    } else {
        printf("UNKNOWN MESSAGE FROM SERVER! Length: %zd\n", read_size);
    }

    close(connfd);
    close(fd);
    exit(0);
}