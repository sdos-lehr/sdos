cmake_minimum_required(VERSION 3.16)
project(sdos_lib C)

set(CMAKE_C_STANDARD 11)

include_directories(/usr/local/include/json-c/)
include_directories(/usr/local/lib)

link_directories(/usr/local/include/json-c/)
link_directories(/usr/local/lib)

add_library(sdos_lib SHARED library.c library.h)
target_link_libraries(sdos_lib curl json-c)
