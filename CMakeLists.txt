cmake_minimum_required(VERSION 3.10)

project(TcpServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add header directory
include_directories(inc)

# Server executable
add_executable(server
    src/server.cpp
)

# Client executable
add_executable(client
    src/client.cpp
)