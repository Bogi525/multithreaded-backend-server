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

void Client::send_message(int i) {
    static thread_local std::mt19937 rng(std::random_device{}());

    std::uniform_int_distribution<int> dist(1, 1000);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(dist(rng))
    );

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

    std::string msg = "Hello from client " + std::to_string(i) + '!';

    send(server_fd, msg.c_str(), strlen(msg.c_str()), 0);

    char buffer[1024];

    memset(buffer, 0, sizeof(buffer));

    int bytes_received = recv(server_fd, buffer, sizeof(buffer), 0);

    if (bytes_received > 0) {
        Logger::info("Server sent: ", std::string(buffer, bytes_received));
    }

    close(server_fd);
}

void Client::send_message(const std::string& message){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        std::cerr << "Socket creation failed.\n";
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(server_fd,
                (sockaddr*)&server_addr,
                sizeof(server_addr)) < 0)
    {
        std::cerr << "Connection failed.\n";
        close(server_fd);
        return;
    }

    send(
        server_fd,
        message.c_str(),
        message.size(),
        0
    );

    char buffer[1024]{};

    int bytes_received =
        recv(server_fd, buffer, sizeof(buffer), 0);

    if (bytes_received > 0) {
        std::cout
            << "Response: "
            << std::string(buffer, bytes_received)
            << '\n';
    }

    close(server_fd);
}

int main()
{
    Client client;

    client.send_message("PING");
    client.send_message("GET username");
    client.send_message("SET username bogdan");
    client.send_message("GET username");
    client.send_message("DEL username");

    return 0;
}