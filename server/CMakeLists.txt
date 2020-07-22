cmake_minimum_required(VERSION 2.8.8)

# 项目信息
project(fuck_storage_server)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wformat -Wall -fPIC -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${live_cdn_SOURCE_DIR}/src/mcd,,$(abspath $<))\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__PRJ_VERSION__='\"${version}\"'")

# 头文件
# include_directories(${THIRD_PARTY}/dtls_wrap)
