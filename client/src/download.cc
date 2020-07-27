#include "../../etc/config.hpp"
#include "../include/big_file.h"
#include "../include/small_file.h"
#include "../utils/tcp_socket.hpp"

int main(int argc, char *argv[]) {
    if (argc !=2 ) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 0;
    }
    // request file_size(uint64_t)
    char *file_name = split_filename(argv[1]);;

    TcpSocket socket_fd = TcpSocket();
    socket_fd.Socket();
    socket_fd.Connect(SERVER_IP_ADDR_1, SERVER_PORT);

    Package package(FILE_SIZE_REQUEST, 0, 0, file_name);
    socket_fd.Send(&package, sizeof(Package));

    int64_t file_size;
    socket_fd.Recv(&file_size, sizeof(file_size));
    socket_fd.Close();
    std::cout << "file_size: " << file_size << std::endl;

    if (file_size < 0) {
        printf("file %s does not exist!\n", file_name);
        return 0;
    }

    if (file_size > SMALL_FILE_SIZE_MAX) {
        printf("The file %s is big file\n", file_name);
        do_big_file_download(file_name, file_size);
    } else {
        printf("The file %s is small file\n", file_name);
        do_small_file_download(socket_fd, file_name, file_size);
    }

    return 0;
}
