# Multithreaded Event-Driven Redis-like Backend Server (C++ / Linux)

A production-inspired backend server built in modern C++ on Linux to demonstrate systems programming, high-performance networking, concurrency, and backend software architecture.

The project implements a scalable I/O model using `epoll` combined with a custom thread pool, non-blocking sockets, and a session-based client architecture.

The server also implements a custom text-based protocol over TCP, supports user authentication with persistent storage, and provides an in-memory key-value store similar in spirit to Redis.

---

## Features

- Event-driven networking using `epoll`
- Non-blocking TCP sockets
- Edge-triggered (`EPOLLET`) event handling
- `EPOLLONESHOT` to ensure exclusive socket ownership
- Custom thread pool for concurrent request processing
- Session-based client architecture
- Thread-safe in-memory key-value store
- User registration and authentication
- SQLite-backed persistent user database
- SHA-256 password hashing using OpenSSL
- Custom newline-delimited text protocol
- Support for partial reads
- Thread-safe logging system

## Goals

This project was built to demonstrate:

- Linux systems programming
- High-performance networking
- Event-driven server architecture
- Multithreaded concurrency design
- Synchronization primitives in practice
- Session management
- Protocol design and parsing
- Backend software fundamentals

---

## Architecture Overview

The server is built around an **event-driven + thread pool hybrid model**:

```text
Clients
   |
   v
accept()
   |
   v
epoll (event loop)
   |
   v
Ready events (epoll_wait)
   |
   v
Thread Pool
   |
   v
ClientSession
   |
   v
Parser
   |
   V
CommandDispatcher
   |----> KVStore (in-memory)
   |----> UserStore (SQLite)

```

## Networking
- Non-blocking sockets
- Edge-triggered `epoll`
- `EPOLLONESHOT` prevents multiple workers from processing the same client simultaneously
- Connections are re-armed after each request

---

# Supported Commands

```
PING
REGISTER <username> <password>
AUTH <username> <password>

SET <key> <value>
GET <key>
DEL <key>
```

---

## Logging System

- Custom thread-safe logger
- Used across worker threads and server core
- Logs:
  - connections
  - errors
  - session events

---

## Tech Stack

- C++20
- Linux (WSL Ubuntu)
- epoll API
- POSIX sockets
- OpenSSL SHA-256
- SQLite3
- CMake
- g++
- VS Code (Remote WSL)

---

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

Run the server:

```bash
./server
```

Run the client:

```bash
./client
```

---