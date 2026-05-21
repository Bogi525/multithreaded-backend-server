#pragma once

#include <vector>
#include <memory>

#include "client_session.hpp"

class Server {
public:
    void start();
private:
    int setup_socket();
    int bind_socket();
    int start_listening();
    int accept_clients();

    int server_fd_;
    int port_;

    std::vector<std::unique_ptr<ClientSession>> client_sessions_;
};