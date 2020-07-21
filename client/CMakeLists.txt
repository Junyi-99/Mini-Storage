cmake_minimum_required (VERSION 2.8)

project (fuck-storage)

add_definitions(-std=c++11)

aux_source_directory(./lib LIB_FILE)
add_library (LIB ${LIB_FILE})

# aux_source_directory(./src SRCS_FILE)
# add_executable(upload ${SRCS_FILE})
add_executable(upload ./src/upload.cc)
target_link_libraries(upload LIB)
