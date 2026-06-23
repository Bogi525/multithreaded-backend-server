# Multithreaded Event-Driven Backend Server (C++ / Linux)

A production-style backend server built in modern C++ on Linux, designed to demonstrate systems programming, networking, concurrency, and event-driven architecture.

The project implements a scalable I/O model using `epoll` combined with a custom thread pool, non-blocking sockets, and a session-based client architecture.

---

## Goals

This project was built to demonstrate:

- Linux systems programming
- High-performance networking
- Event-driven server architecture
- Multithreaded concurrency design
- Synchronization primitives in practice
- Backend engineering fundamentals

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
ClientSession processing
```

Key idea:
- `epoll` handles I/O readiness (event-driven core)
- Worker threads handle request processing
- `EPOLLONESHOT` is used to safely prevent concurrent handling of the same socket
- Connections are re-armed after processing

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
./server
```