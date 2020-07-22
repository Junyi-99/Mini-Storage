//
// Created by Junyi Hou on 7/21/20.
//

#ifndef FUCK_STORAGE_SERVER_PACKAGE_H
#define FUCK_STORAGE_SERVER_PACKAGE_H

#include <cstdint>
#include <cstring>

struct Package {
    uint64_t package_len;
    uint32_t msg_type;
    char filename[16];
    uint64_t block_len;
    uint32_t disk_no;
};

Package *
pack_header(uint64_t package_len, uint32_t msg_type, char *filename, uint64_t block_len, uint32_t disk_no);

// First step，通过 package len 可以算出块的大小
Package *unpack_header(unsigned char *buffer, uint64_t length);

#endif //FUCK_STORAGE_SERVER_PACKAGE_H
