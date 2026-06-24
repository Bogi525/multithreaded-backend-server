#ifndef _client_session_hpp_
#define _client_session_hpp_

#include <string>
#include <sys/epoll.h>

#include "../../protocol/inc/parser.hpp"

class ClientSession {
public:
    ClientSession(int client_fd);

    // Reads all currently available data from the socket.
    // Returns false if the connection should be closed.
    bool handle_read();

    // Flushes queued response data to the socket.
    // Returns false if the connection should be closed.
    bool handle_write();

    void close_session();

    int fd() const;

    bool write_empty();
private:
    int client_fd_;
    Parser parser;

    // Accumulates data received from the client.
    // Will later be consumed by the protocol parser.
    std::string read_buffer_;

    // Stores pending outbound data.
    // Necessary because nonblocking send() may write only
    // part of a response.
    std::string write_buffer_;

    bool closed_ = false;

    void queue_response(std::string data);
};

#endif