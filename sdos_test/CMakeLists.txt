cmake_minimum_required(VERSION 3.16)
project(sdos_test C)

set(CMAKE_C_STANDARD 99)

include_directories(../sdos_lib)
include_directories(../sdos_lib/cmake-build-debug)
link_directories(../sdos_lib)
link_directories(../sdos_lib/cmake-build-debug)

add_executable(sdos_test main.c)
target_link_libraries(sdos_test sdos_lib)