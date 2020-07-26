## Fuck Storage

一个轻量化储存系统的设计与实现，采用优秀的策略选择算法可快速存取 4MB ~ 50GB 文件

项目遵循 C/S 架构设计，分为客户端和服务端两部分。



## 编译

**Prerequisite:**

`CMake >= 2.8.8` 



进入项目根目录，执行 `build.sh` 即可构建整个项目 

```bash
sh build.sh
```



编译完成后，项目文件存放在当前文件夹的 `./bin` 目录下

```bash
.
├── bin
│   ├── download # 下载用客户端
│   ├── server   # 服务端
│   ├── upload   # 上传用客户端
│   └── ...      # 其它脚本文件
...
```



## 运行（服务端）

服务端会把接收到的文件存放在 `/raid` 目录下， 因为 RAID 设备被挂载在 `/raid` 上。

您可以在根目录 `/` 手动创建一个名为 `raid` 的目录，并且设置好目录权限，server 也可以正常工作。

```bash
mkdir /raid
```

运行 server 非常简单，因为 server 不接收任何参数：

```bash
./bin/server
```

此时，server 将会监听来自 `6667` 端口的所有连接。

<font color="red">注意，请同时在 `9.100.173.170` 和 `9.100.173.189` 两台服务器上运行 server，因为客户端会尝试向这两个服务器发送不同的文件分块，以达到 "分布式" 的效果</font>





## 运行（客户端）

上传和下载的使用方法一致，都**只接收** `文件名` 这一个参数：

```bash
./bin/upload <文件名>
./bin/downlaod <文件名>
```

<font color="red">⚠️警告：如果上传的文件名与之前的重复，服务端将覆盖之前上传的文件</font>

客户端默认尝试访问 `9.100.173.170` 和 `9.100.173.189` 两台机器





## 文件生成脚本

我们提供了供测试用的文件生成脚本，您可以利用这些脚本来验证 client 和 server 的传输速度与可靠性。



生成小文件及 filelist：

```
./bin/gen_small_file.sh
```



小文件上传（20个并发）

```
./bin/upload_20_small_file.sh
```



## 其它

在 /etc/config.hpp 里，有一些关于项目的配置：

```c++
#define SERVER_IP_ADDR_1       "9.100.173.170" // 客户端要连接的第一个IP
#define SERVER_IP_ADDR_2       "9.100.173.189" // 客户端要连接的第二个IP
#define SERVER_PORT            6667            // 服务端监听端口
#define SERVER_MAX_CONNECTION  500             // epoll 设置
#define SERVER_MAX_EVENTS      500             // epoll 设置
#define SERVER_MAX_THREADS     16              // 服务端有多少线程处理文件分块（一个分块一个线程）
```