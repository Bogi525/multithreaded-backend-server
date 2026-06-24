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
    concurrency/src/task_queue.cpp
    concurrency/src/thread_pool.cpp
    logging/src/logger.cpp
    protocol/src/command.cpp
    protocol/src/parser.cpp
    protocol/src/response.cpp
)

target_include_directories(server PRIVATE
    server/inc
    concurrency/inc
    logging/inc
    protocol/inc
)

# =========================
# Client executable
# =========================

add_executable(client
    client/src/client.cpp
    logging/src/logger.cpp
)

target_include_directories(client PRIVATE
    client/inc
    logging/inc
)