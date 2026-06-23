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
private:
    // Creates and configures the listening socket.
    int setup_socket();

    // Binds the listening socket to the configured port.
    int bind_socket();

    // Starts accepting incoming TCP connections.
    int start_listening();

    // Creates epoll instance and registers the listening socket.
    int setup_epoll();

    // Accepts all pending connections until EAGAIN.
    // Required when using edge-triggered epoll.
    void accept_connections();

    // Looks up the session and schedules processing on a worker thread.
    void handle_client_event(int fd, uint32_t events);

    // Removes the session and closes the connection.
    void remove_session(int fd);

    // Main event loop. Waits for readiness notification and
    // dispatches work to the thread pool.
    void event_loop();

    // Executed by worker threads.
    // Handles socket I/O and rearms EPOLLONESHOT.
    void execute_worker_task(std::shared_ptr<ClientSession> session, uint32_t events);

    int server_fd_;
    int epoll_fd_;

    int port_;
    
    // Active client sessions indexed by file descriptor.
    // Protected by sessions_mutex_.
    std::unordered_map<int, std::shared_ptr<ClientSession>> sessions_;
    std::mutex sessions_mutex_;

    ThreadPool thread_pool_ = ThreadPool(4);
};

#endif