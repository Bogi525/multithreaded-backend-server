#include "../inc/server.hpp"

#include <iostream>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void Server::establish_connection() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        std::cerr << "Socket creation failed.\n";
        return;
    }

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    int opt = 1;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
    }

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed.\n";
        close(server_fd);
        return;
    }

    std::cout << "Server listening on port 8080...\n";

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    
    if (client_fd < 0) {
        std::cerr << "Accept failed.\n";
        close(server_fd);
        return;
    }

    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));

    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

    if (bytes_received > 0) {
        std::cout << "Message received: \"" << buffer << "\"\n";

        send(client_fd, buffer, bytes_received, 0);
    }

    close(client_fd);
    close(server_fd);

    return;
}

int main() {
    Server srv;
    srv.establish_connection();
    std::cin;
    return 0;
}