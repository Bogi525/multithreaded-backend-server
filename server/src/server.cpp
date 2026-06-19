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
        std::cerr << "Socket creation failed.\n";
        return -1;
    }

    return 0;
}

int Server::bind_socket() {
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    int opt = 1;

    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
    }

    if (bind(server_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
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

    ev.events = EPOLLIN;
    ev.data.fd = server_fd_;

    return epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, server_fd_, &ev);
}

void Server::accept_connections() {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(
        server_fd_,
        (sockaddr*)&client_addr,
        &client_len
    );

    if (client_fd < 0) {
        Logger::error("Accept failed.");
        return; // why return;, why not continue;
    }

    Logger::info("Client connected.");

    auto session = std::make_shared<ClientSession>(client_fd);

    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        sessions_[client_fd] = session;
    }

    epoll_event ev{};

    ev.events = EPOLLIN;
    ev.data.fd = client_fd;

    epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev);

    Logger::info("Client connected. fd = ", client_fd);
}

void Server::handle_client_event(int fd) {
    std::shared_ptr<ClientSession> session;
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);

        auto it = sessions_.find(fd);

        if (it == sessions_.end()) {
            return;
        }

        session = it->second;
    }

    if (!session->handle_read()) {
        remove_session(fd);
        return;
    }
    
    if (!session->handle_write()) {
        remove_session(fd);
        return;
    }
}

void Server::remove_session(int fd) {
    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);

    std::lock_guard<std::mutex> lock(sessions_mutex_);
    sessions_.erase(fd);
}

void Server::event_loop() {
    constexpr int MAX_EVENTS = 64;

    epoll_event events[MAX_EVENTS];

    while (true) {
        int ready = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);

        for (int i = 0; i < ready; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd_) {
                accept_connections();
            }
            else {
                handle_client_event(fd);
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