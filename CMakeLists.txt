cmake_minimum_required(VERSION 3.10)

project(TcpServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

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
    dispatcher/src/command_dispatcher.cpp
    storage/src/kv_store.cpp
    storage/src/user_store.cpp
    auth/src/password_hasher.cpp
)

target_include_directories(server PRIVATE
    server/inc
    concurrency/inc
    logging/inc
    protocol/inc
    dispatcher/inc
    storage/inc
    auth/inc
    config/inc
)

target_link_libraries(server PRIVATE OpenSSL::Crypto)

target_link_libraries(server PRIVATE SQLite::SQLite3)

target_compile_definitions(server PRIVATE
    DATA_DIR="${CMAKE_SOURCE_DIR}/data"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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