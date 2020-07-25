cmake_minimum_required(VERSION 2.8)

project(fuck-storage)


find_package(Threads)

aux_source_directory(./lib LIB_FILE)
add_library(LIB ${LIB_FILE})

# TODO: [-W -Wall] 显示更详细的警告信息, [-O3] 最大程度对代码进行优化
add_definitions(-std=c++11)
add_definitions(-static-libgcc -static-libstdc++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++") # 测试模式为了方便调试
include_directories(${PROJECT_SOURCE_DIR}/../etc)
add_executable(upload ./src/upload.cc)
target_link_libraries(upload LIB ${CMAKE_THREAD_LIBS_INIT})


# TODO: [-W -Wall] 显示更详细的警告信息, [-O3] 最大程度对代码进行优化
add_definitions(-std=c++11)
add_definitions(-static-libgcc -static-libstdc++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++") # 测试模式为了方便调试
include_directories(${PROJECT_SOURCE_DIR}/../etc)
add_executable(download ./src/download.cc)
target_link_libraries(download LIB ${CMAKE_THREAD_LIBS_INIT})
