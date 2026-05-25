#ifndef _server_hpp_
#define _server_hpp_

#include <vector>
#include <memory>
#include <mutex>

#include "client_session.hpp"

class Server {
public:
    void start();
private:
    int setup_socket();
    int bind_socket();
    int start_listening();
    void accept_clients();

    void remove_session(std::shared_ptr<ClientSession> session);

    int server_fd_;
    int port_;

    std::vector<std::shared_ptr<ClientSession>> client_sessions_;
    std::mutex sessions_mutex_;
};

#endif