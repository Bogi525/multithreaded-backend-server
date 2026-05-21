cmake_minimum_required(VERSION 3.10)

project(TcpServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================
# Server executable
# =========================

add_executable(server
    server/src/server.cpp
    server/src/client_session.cpp
)

target_include_directories(server PRIVATE
    server/inc
)

# =========================
# Client executable
# =========================

add_executable(client
    client/src/client.cpp
)

target_include_directories(client PRIVATE
    client/inc
)