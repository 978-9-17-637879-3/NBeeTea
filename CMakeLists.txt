cmake_minimum_required(VERSION 3.26)
project(NBeeTeaTest)

set(CMAKE_CXX_STANDARD 17)

add_executable(NBeeTeaTest test.cpp)

find_package(ZLIB)
target_link_libraries(NBeeTeaTest PRIVATE ZLIB::ZLIB)

target_include_directories(NBeeTeaTest PRIVATE "include/gzip-hpp/include")

add_subdirectory("lib/")
target_link_libraries(NBeeTeaTest PRIVATE NBeeTea)