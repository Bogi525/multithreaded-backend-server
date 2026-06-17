#include "../inc/server.hpp"
#include "../../concurrency/inc/thread_pool.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <algorithm>

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
        std::cerr << "Listen failed.\n";
        close(server_fd_);
        return -1;
    }

    std::cout << "Server listening on port 8080...\n";

    return 0;
}

void Server::accept_clients() {
    while(true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(
            server_fd_,
            (sockaddr*)&client_addr,
            &client_len
        );

        if (client_fd < 0) {
            std::cerr << "Accept failed.\n";
            continue;
        }

        std::cout << "Client connected.\n";

        auto session = std::make_shared<ClientSession>(client_fd);

        {
            std::lock_guard<std::mutex> lock(sessions_mutex_);
            client_sessions_.push_back(session);
        }

        thread_pool_.enqueue([this, session]() {
            session->handle();

            remove_session(session);
        });
    }
}

void Server::remove_session(std::shared_ptr<ClientSession> session) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);

    client_sessions_.erase(
        std::remove(
            client_sessions_.begin(),
            client_sessions_.end(),
            session
        ),
        client_sessions_.end()
    );
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

    accept_clients();

    close(server_fd_);
}

int main() {
    Server server;
    server.start();
    std::cin;
    return 0;
}