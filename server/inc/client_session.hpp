#ifndef _client_session_hpp_
#define _client_session_hpp_

#include <string>

class ClientSession {
public:
    ClientSession(int client_fd);

    bool handle_read();
    bool handle_write();

    void close_session();
private:
    int client_fd_;

    std::string read_buffer_;
    std::string write_buffer_;

    bool closed_ = false;

    void queue_response(std::string data);
};

#endif