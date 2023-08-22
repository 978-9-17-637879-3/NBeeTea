cmake_minimum_required(VERSION 3.26)
project(NBeeTea)

set(CMAKE_CXX_STANDARD 17)

add_executable(NBeeTea src/test.cpp
        src/NBT.cpp
)

find_package(ZLIB)
target_link_libraries(NBeeTea PRIVATE ZLIB::ZLIB)

target_include_directories(NBeeTea PRIVATE "include/gzip-hpp/include")
