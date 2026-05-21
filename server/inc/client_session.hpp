#pragma once

class ClientSession {
public:
    ClientSession(int client_fd);

    void handle();
private:
    int client_fd_;
};