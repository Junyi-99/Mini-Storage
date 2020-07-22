cmake_minimum_required(VERSION 2.8.8)

# 项目信息
project(fuck_storage_server)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g -O0") # 测试模式为了方便调试

# 头文件
include_directories(${PROJECT_SOURCE_DIR}/include)

# 查找当前目录下的所有源文件
# 并将名称保存到 SRCS 变量
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRCS)

add_executable(fuck_storage_server ${SRCS} src/file.cpp src/file.h src/package.cpp src/package.h) # 入口为 server.cpp