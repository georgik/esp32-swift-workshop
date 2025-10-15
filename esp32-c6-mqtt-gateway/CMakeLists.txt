cmake_minimum_required(VERSION 3.16)

# Set FD_SETSIZE to ensure LWIP_SOCKET_OFFSET >= 6
# LWIP_SOCKET_OFFSET = FD_SETSIZE - CONFIG_LWIP_MAX_SOCKETS
# With CONFIG_LWIP_MAX_SOCKETS=8, we need FD_SETSIZE >= 14, using 64 for safety
add_compile_definitions(FD_SETSIZE=64)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(esp32-c6-mqtt-gateway)
