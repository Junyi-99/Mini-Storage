
src/helper.cpp:
    一些辅助函数

sec/package.cpp:
    数据包 包装和解析
    
src/file.cpp:
    **文件操作的具体实现部分**
    
src/server.cpp:
    epoll 边缘触发 + 线程池 高并发实现
    
    
src/tcp.cpp:
    TCP 连接相关函数

src/worker.cpp:
    负责处理业务，多线程，work_queue 添加数据的时候唤醒某一个线程，从 work_queue 中取出数据之后进行处理