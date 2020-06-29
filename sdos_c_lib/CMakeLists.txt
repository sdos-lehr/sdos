cmake_minimum_required(VERSION 3.16)
project(sdos C)

set(CMAKE_C_STANDARD 11)

include_directories(/usr/local/include/json-c/)
link_directories(/usr/local/lib)


add_library(sdos SHARED sdos.c sdos.h)
target_link_libraries(sdos curl json-c)
