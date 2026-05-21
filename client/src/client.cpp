#include "../inc/client.hpp"

#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void Client::send_message() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        std::cerr << "Socket creation failed.";
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

    std::string host_addr = "127.0.0.1";

    inet_pton(AF_INET, host_addr.c_str(), &server_addr.sin_addr);

    if (connect(server_fd, (sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed.";
        close(server_fd);
        return;
    }

    std::string msg = "Hello from client!";

    send(server_fd, msg.c_str(), strlen(msg.c_str()), 0);

    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));

    int bytes_received = recv(server_fd, buffer, sizeof(buffer), 0);

    if (bytes_received > 0) {
        std::cout << "Server echoed: " << buffer << '\n';
    }

    close(server_fd);
}

int main() {
    Client client;
    client.send_message();
    std::cin;
    return 0;
}