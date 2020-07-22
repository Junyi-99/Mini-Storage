//
// Created by Junyi Hou on 7/21/20.
//

#include <cstdio>
#include "../include/package.h"

Package *
pack_header(uint64_t package_len, uint32_t msg_type, char *filename, uint64_t block_len, uint32_t disk_no) {
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
Package *unpack_header(unsigned char *buffer, uint64_t length) {
    auto p = reinterpret_cast<Package *>(buffer);
    return pack_header(p->package_len, p->msg_type, p->filename, p->block_len, p->disk_no);
}