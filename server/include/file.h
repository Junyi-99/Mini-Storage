#ifndef __FILE__SYSTEM__
#define __FILE__SYSTEM__

#include<iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <errno.h>
//#include <sys/types.h>
//#include <assert.h>


#define ROOT_PATH "/raid/"


//totalFile的文件读写提供了两个单独的函数：
//  储存totalFile大小： file_set(filename, filesize); 
//  获取totalFile大小： getTotalSize(filename);



//用于读写文件的封装类：
/*
声明格式：
    1. 读取模式  File_Opt read(filename)
        1.1 读取全部内容:  read.read_all()
        1.2 按偏移量和字符数读取内容: read.read_part(off_set, NumOfChar) 
        1.3 提供读取该分块对应totalFile的文件大小: read.get_total_file_size();
            (也可以使用 getTotalSize(filename) 函数来读取)
    2. 写入模式  File_Opt write(filename, filesize)
        只提供 按照内容 和 偏移量 写入的接口： write.write_by_offset(content, off_set);
*/
class File_Opt {
public:
    File_Opt(const char *filename, off_t filesize); //写文件时，需要提供：文件名 及 文件大小
    File_Opt(const char *filename); //读文件时，只需传入：文件名
    ~File_Opt();

    void write_by_offset(const char *content, off_t off_set);   //写入文件，需要提供：内容 及 偏移量
    char *read_all();   //读取全部内容
    char *read_part(off_t off_set, off_t NumOfChar);    //读取部分内容，需要提供：偏移量 及 字符个数
    int64_t get_total_file_size();

    void *mmap_addr;
    int fd;
    off64_t file_size;

private:
    char *operation_postion;
    const char *file_name;
    const char *file_path;
    off64_t total_file_size;

    char *mmap_write_byte_part(char *addr, const char *content);
};

//记录total文件大小
int file_set(const char *filename, uint32_t filesize);

//获取total文件大小
off_t getTotalSize(const char *filename);

#endif // !__FILE__SYSTEM__
