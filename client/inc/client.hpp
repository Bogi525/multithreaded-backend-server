#pragma once

#include <iostream>

class Client {
public:
    Client() = default;
    ~Client();

    bool connect();
    void send(const std::string& message);
    void disconnect();
    int is_connected();
private:
    int server_fd_ = -1;
};