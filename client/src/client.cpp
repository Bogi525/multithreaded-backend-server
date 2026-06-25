#include "../inc/client.hpp"

#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <random>
#include <chrono>

#include "../../logging/inc/logger.hpp"

Client::~Client() {
    disconnect();
}

bool Client::connect() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd_ < 0) {
        std::cerr << "Socket creation failed.\n";
        return false;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

    inet_pton(
        AF_INET,
        "127.0.0.1",
        &server_addr.sin_addr
    );

    if (::connect(
            server_fd_,
            (sockaddr*)&server_addr,
            sizeof(server_addr))
        < 0)
    {
        std::cerr << "Connection failed.\n";
        close(server_fd_);
        server_fd_ = -1;
        return false;
    }

    return true;
}

void Client::send(const std::string& message) {
    if (server_fd_ < 0) {
        std::cerr << "Not connected.\n";
        return;
    }

    ::send(
        server_fd_,
        message.c_str(),
        message.size(),
        0
    );

    char buffer[1024]{};

    int bytes_received =
        recv(
            server_fd_,
            buffer,
            sizeof(buffer),
            0
        );

    if (bytes_received > 0) {
        std::cout
            << "Response: "
            << std::string(buffer, bytes_received)
            << '\n';
    }
}

void Client::disconnect() {
    if (server_fd_ >= 0) {
        close(server_fd_);
        server_fd_ = -1;
    }
}

int main()
{
    Client client;

    if (!client.connect()) {
        return 1;
    }

    std::string input;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "CLOSE") {
            break;
        }

        if (input.empty()) {
            continue;
        }

        client.send(input);
    }

    client.disconnect();

    return 0;
}