#include "../inc/server.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void Server::establish_connection() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        std::cerr << "Socket creation failed.";
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed";
        close(server_fd);
        return;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed";
        close(server_fd);
        return;
    }

    std::cout << "Server listening on port 8080...\n";
}