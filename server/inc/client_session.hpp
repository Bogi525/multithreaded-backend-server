#ifndef _client_session_hpp_
#define _client_session_hpp_

class ClientSession {
public:
    ClientSession(int client_fd);

    void handle();
private:
    int client_fd_;
};

#endif