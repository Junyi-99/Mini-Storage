#include <glob.h>
#include "../etc/config.h"

struct Package {
    size_t package_len;
    unsigned int msg_type; // upload or download, small or big file
    char filename[16];
    size_t block_len;
    unsigned int disk_no;
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
set_package(size_t package_len, unsigned int msg_type, char filename[16], size_t block_len, unsigned int disk_no) {
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
    off_t *offset = new off_t;
    while (*offset < file_size) {
        send_size = sendfile(connfd, fd, offset, file_size);
        printf("sent: %d\n", send_size);
        if (send_size < 0) {
            perror("Send error!");
            close(connfd);
            close(fd);
            exit(0);
        }
    }
    // printf("send file success: %s\n", file_name);
    delete offset;
    if (send_size != file_size) {
        perror("Send error!");
        close(connfd);
        close(fd);
        exit(0);
    }

    /* Get close signal */
    char *readBuffer[ACK_SIZE];
    ssize_t read_size = read(connfd, readBuffer, ACK_SIZE);
    if (read_size != 0) {
        printf("read_size: %d\n", read_size);
        
        perror("Send error!");
        close(connfd);
        close(fd);
        exit(0);
    }
    printf("Finished\n");
    // printf("Get ACK: %s\n\n", package->filename);
    /* Finish task */
    close(connfd);
    close(fd);
}