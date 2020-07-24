#include "../include/file.h"

//写入文件模式时，创建对应文件，设置文件大小，建立映射
File_Opt::File_Opt(const char* filename, off_t filesize) {
    this->file_name = filename;
    this->file_size = filesize;
    this->total_file_size = -1;
    std::string fatherPath = ROOT_PATH ;
    std::string filePath = fatherPath + filename;// "/raid/${file_name}"
    this->file_path = filePath.c_str();
    if ((this->fd = open(file_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) < 0) {    //创建文件
        printf("%s_info create failed! \n",filename);
        //perror("creat file failed");
    }
    truncate(file_path, file_size);
    this->mmap_addr = mmap(NULL, file_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0); 
    if (mmap_addr == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        //perror("mmap create failed");
    }
};

//读取模式时，打开对应文件，建立映射
File_Opt::File_Opt(const char* filename) {
    this->file_name = filename;
    this->total_file_size = -1;
    std::string fatherPath = ROOT_PATH ;
    std::string filePath = fatherPath + filename;// "/raid/${file_name}"
    this->file_path = filePath.c_str(); 
    if ((this->fd = open(file_path, O_RDONLY)) < 0) {    //打开文件
        printf("%s open failed! \n",filename);
        //perror("creat file failed");
    }
    struct stat stat;
    fstat(fd, &stat);               // 获取文件信息
    file_size = stat.st_size;

    this->mmap_addr = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0); 
    if (mmap_addr == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        //perror("mmap create failed");
    } 
}

//析构时，关闭文件，解除映射，落盘
File_Opt::~File_Opt(){
    if (close(fd) < 0) {
        printf("close %s failed!\n",file_name);
    }
    if (munmap(mmap_addr,file_size) < 0) {
        printf("close map of %s failed! \n",file_name);
    }
    if (msync(mmap_addr,file_size,MS_SYNC) < 0) {
        //printf("%s write to disk failed! \n",file_name);
    }
}

//文件系统自用工具函数
/*
mmap映射时，给出首地址和待储存内容，会自动储存到映射区域，并返回下一个可储存的位置
*/
char* File_Opt::mmap_write_byte_part(char* addr, const char* content){
    int len = strlen(content);
    char* opt_addr_ptr = addr;
    memcpy(opt_addr_ptr,content,len);
    opt_addr_ptr[len] = '\n';
    opt_addr_ptr = opt_addr_ptr + len + 1;
    return opt_addr_ptr;
}

//给出要储存的内容 和 该部分内容的偏移量即可
void File_Opt::write_by_offset(const char* content, off_t off_set){
    operation_postion = (char* )mmap_addr + off_set;
    operation_postion = mmap_write_byte_part(operation_postion,content);
}


//读取全部文件内容
char* File_Opt::read_all() {
    char* res = new char[file_size];
    memcpy(res,(char* )mmap_addr,file_size);
    return res;
}

//读取部分文件内容，给出偏移量 和 长度
char* File_Opt::read_part(off_t off_set, off_t NumOfChar) {
    char* res = new char[NumOfChar];
    memcpy(res,(char* )mmap_addr + off_set,NumOfChar);
    return res;
}

//从小文件里面读取total文件信息
int File_Opt::get_total_file_size() {
    if (this->total_file_size < 0){
        this->total_file_size = getTotalSize(this->file_name);
    }
    return this->total_file_size;
}

//读取total文件大小
/*
return error:
    -1: 文件打开失败
    -2：建立映射失败
    -3：解除映射失败
    -4：文件关闭失败
    >0: 成功
*/
off_t getTotalSize(const char* filename) {
    std::string fatherPath = ROOT_PATH ;
    std::string fileinfoPath = fatherPath + filename + "_info"; // "/raid/${file_name}_info"
    int info_fd;
    if ((info_fd = open(fileinfoPath.c_str(), O_RDONLY)) < 0) {    //创建信息文件
        printf("%s_info open failed! \n",filename);
        return -1;
    }
    struct stat stat;
    fstat(info_fd, &stat);               // 获取文件信息
    off_t file_size = stat.st_size;
    void* info_ptr = mmap(NULL, file_size, PROT_READ, MAP_SHARED, info_fd, 0); 
    if (info_ptr == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        return -2;
    }
    char* res = new char[strlen((char*)info_ptr) + 1];
    memcpy(res,(char*)info_ptr,strlen((char*)info_ptr));
    off_t total_size = std::atoll(res);
    delete[] res;
    if (munmap(info_ptr,file_size) < 0) {
        printf("unmap error: %s \n", filename);
        return -3;
    }
    if (close(info_fd) < 0) {
        printf("close %s failed \n",filename);
        return -4;
    }
    return total_size;
}



//储存文件信息(是切块之前的文件的信息，即记录该文件一共有多大，用于client判断是大文件还是小文件)
/*
return error:
    -1: 文件创建失败
    -2：建立映射失败
    -3：解除映射失败
    -4：文件关闭失败
    1: 成功
*/
int file_set(const char* filename,  uint32_t filesize) {
    //不单独为每个文件创建文件夹，统一放在/raid下面
    //总文件的信息记录在/raid/filename_info这里
    std::string fatherPath = ROOT_PATH ;
    std::string fileinfoPath = fatherPath + filename + "_info"; // "/raid/${file_name}_info"
    const char* filesize_str = std::to_string(filesize).c_str();
    int str_len = strlen(filesize_str);

    int info_fd;
    if ((info_fd = open(fileinfoPath.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) < 0) {    //创建信息文件
        printf("%s_info create failed! \n",filename);
        return -1;
    }
    truncate(fileinfoPath.c_str(), str_len);
    void* info_ptr = mmap(NULL, str_len + 1, PROT_WRITE | PROT_READ, MAP_SHARED, info_fd, 0); 
    if (info_ptr == MAP_FAILED) {
        printf("mmap error: %s\n", strerror(errno));
        return -2;
    }
    char* opt_for_info = (char*)info_ptr;

    memcpy(opt_for_info,filesize_str,str_len);
    if ((munmap(info_ptr, str_len + 1)) < 0) {
       printf("unmap %s_info error \n",filename); 
       return -3;
    }

    if (close(info_fd) < 0 ) {
        printf("close %s_info error \n",filename);
        return -4;
    }
    /*
    int temp;
    if ((temp = msync(info_ptr,str_len + 1,MS_ASYNC)) < 0) {
        printf("%s write to disk failed! \n",filename);
        return -4;
    }
    */
    return 1;
}

