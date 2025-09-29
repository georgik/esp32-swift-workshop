cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Define FD_SETSIZE to support more LWIP sockets + system file descriptors
# Need at least 50 sockets + 6 for system FDs = 56, using 64 for safety
add_compile_definitions(FD_SETSIZE=64)

project(esp32-c6-mqtt-gateway)