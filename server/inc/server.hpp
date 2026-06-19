#ifndef _server_hpp_
#define _server_hpp_

#include <vector>
#include <memory>
#include <mutex>
#include <string>
#include "../../concurrency/inc/thread_pool.hpp"

#include "client_session.hpp"

class Server {
public:
    void start();
    void update_events(int fd, uint32_t events);
private:
    int setup_socket();
    int bind_socket();
    int start_listening();
    int setup_epoll();

    void accept_connections();
    void handle_client_event(int fd, uint32_t events);

    void remove_session(int fd);

    void event_loop();

    void disable_epoll_out(int fd);
    void enable_epoll_out(int fd);

    int server_fd_;
    int epoll_fd_;

    std::unordered_map<int, uint32_t> fd_events_;

    int port_;

    std::vector<std::shared_ptr<ClientSession>> client_sessions_;
    
    std::unordered_map<int, std::shared_ptr<ClientSession>> sessions_;
    std::mutex sessions_mutex_;

    ThreadPool thread_pool_ = ThreadPool(4);
};

#endif