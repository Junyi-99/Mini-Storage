cmake_minimum_required (VERSION 2.8)

project (fuck-storage)

# TODO: [-W -Wall] 显示更详细的警告信息, [-O3] 最大程度对代码进行优化
add_definitions(-std=c++11)

include_directories(${PROJECT_SOURCE_DIR}/../etc)

aux_source_directory(./lib LIB_FILE)
add_library (LIB ${LIB_FILE})

find_package(Threads)

# aux_source_directory(./src SRCS_FILE)
# add_executable(upload ${SRCS_FILE})
add_executable(upload ./src/upload.cc)
target_link_libraries(upload LIB ${CMAKE_THREAD_LIBS_INIT})
