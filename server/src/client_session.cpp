#include "../inc/client_session.hpp"

#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>

ClientSession::ClientSession(int client_fd) : client_fd_(client_fd) {}

void ClientSession::handle() {
    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes_received = recv(client_fd_, buffer, sizeof(buffer), 0);

    if (bytes_received > 0) {
        std::cout << "Message received: \"" << buffer << "\"\n";

        send(client_fd_, buffer, bytes_received, 0);
    }

    close(client_fd_);
}