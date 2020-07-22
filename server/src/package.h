//
// Created by Junyi Hou on 7/21/20.
//

#ifndef FUCK_STORAGE_SERVER_PACKAGE_H
#define FUCK_STORAGE_SERVER_PACKAGE_H

#include <cstdint>
#include <cstring>

struct Package {
    uint32_t package_len;
    uint16_t msg_type;
    char filename[16];
    uint32_t block_len;
    uint16_t disk_no;
};

Package *
pack_header(uint32_t package_len, uint16_t msg_type, char *filename, uint32_t block_len, uint16_t disk_no) {
    auto *package = new Package{
            .package_len = package_len,
            .msg_type = msg_type,
            .block_len = block_len,
            .disk_no = disk_no,
    };
    strcpy(package->filename, filename);
    return package;
}

// First step，通过packagelen可以算出块的大小
Package *unpack_header(unsigned char *buffer, uint32_t length) {
    auto *p = reinterpret_cast<Package *>(buffer);
    return pack_header(p->package_len, p->msg_type, p->filename, p->block_len, p->disk_no);
}


#endif //FUCK_STORAGE_SERVER_PACKAGE_H
