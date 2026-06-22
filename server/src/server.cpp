#include "../inc/server.hpp"
#include "../../concurrency/inc/thread_pool.hpp"
#include "../../logging/inc/logger.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <algorithm>
#include <sys/epoll.h>
#include <fcntl.h>

void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return;

    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Server::enable_epoll_out(int fd) {
    auto it = fd_events_.find(fd);
    if (it == fd_events_.end()) return;

    it->second |= EPOLLOUT;

    epoll_event ev{};
    ev.data.fd = fd;
    ev.events = it->second | EPOLLET;

    epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
}

void Server::disable_epoll_out(int fd) {
    auto it = fd_events_.find(fd);
    if (it == fd_events_.end()) return;

    it->second &= ~EPOLLOUT;

    epoll_event ev{};
    ev.data.fd = fd;
    ev.events = it->second | EPOLLET;

    epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
}

void Server::start() {
    if (setup_socket() < 0) {
        return;
    }

    if (bind_socket() < 0) {
        return;
    }

    if (start_listening() < 0) {
        return;
    }

    if (setup_epoll() < 0) {
        return;
    }
    
    Logger::info("Server listening on port 8080...");

    event_loop();
}

int Server::setup_socket() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd_ < 0) {
        Logger::error("Socket creation failed.");
        return -1;
    }

    set_non_blocking(server_fd_);

    return 0;
}

int Server::bind_socket() {
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    int opt = 1;

    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Logger::error("setsockopt failed");
    }

    if (bind(server_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        Logger::error("Bind failed");
        close(server_fd_);
        return -1;
    }

    return 0;
}

int Server::start_listening() {
    if (listen(server_fd_, 5) < 0) {
        Logger::error("listen failed");
        close(server_fd_);
        return -1;
    }

    return 0;
}

int Server::setup_epoll() {
    epoll_fd_ = epoll_create1(0);
    
    if (epoll_fd_ < 0) {
        Logger::error("epoll creation failed.");
        return -1;
    }

    epoll_event ev{};

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = server_fd_;

    return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, server_fd_, &ev);
}

void Server::accept_connections() {
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(
            server_fd_,
            (sockaddr*)&client_addr,
            &client_len
        );

        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            Logger::error("Accept failed.");
            break;
        }

        set_non_blocking(client_fd);
        fd_events_[client_fd] = EPOLLIN | EPOLLET | EPOLLONESHOT;

        auto session = std::make_shared<ClientSession>(client_fd);

        {
            std::lock_guard<std::mutex> lock(sessions_mutex_);
            sessions_[client_fd] = session;
        }

        epoll_event ev{};

        ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        ev.data.fd = client_fd;

        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev);

        Logger::info("Client connected. fd = ", client_fd);
    }
}

void Server::execute_worker_task(std::shared_ptr<ClientSession> session, uint32_t events) {
    int fd = session->fd();

    if (events & EPOLLIN) {
        if (!session->handle_read()) {
            remove_session(fd);
            return;
        }
    }
    
    if (events & EPOLLOUT || !session->write_empty()) {
        if (!session->handle_write()) {
            remove_session(fd);
            return;
        }
    }

    if (events & (EPOLLERR | EPOLLRDHUP)) {
        remove_session(fd);
        return;
    }

    uint32_t next_events = EPOLLIN;
    
    if (!session->write_empty()) {
        next_events |= EPOLLOUT;
    }

    epoll_event ev{};
    ev.data.fd = fd;
    ev.events = next_events | EPOLLET | EPOLLONESHOT;

    epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
}

void Server::handle_client_event(int fd, uint32_t events) {
    std::shared_ptr<ClientSession> session;
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        auto it = sessions_.find(fd);
        if (it == sessions_.end()) {
            return;
        }
        session = it->second;
    }

    thread_pool_.enqueue([this, session, events] () {
        execute_worker_task(session, events);
    });
}

void Server::remove_session(int fd) {
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);

    std::lock_guard<std::mutex> lock(sessions_mutex_);
    sessions_.erase(fd);
    close(fd);
}

void Server::event_loop() {
    constexpr int MAX_EVENTS = 64;

    epoll_event events[MAX_EVENTS];

    while (true) {
        int ready = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);

        for (int i = 0; i < ready; i++) {
            int fd = events[i].data.fd;
            uint32_t ev = events[i].events;

            if (fd == server_fd_) {
                accept_connections();
            }
            else {
                handle_client_event(fd, ev);
            }
        }
    }
}

int main() {
    Server server;
    server.start();
    std::cin;
    return 0;
}