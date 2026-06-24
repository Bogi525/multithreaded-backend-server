#pragma once

#include <iostream>

class Client {
public:
    void send_message(int i);
    void send_message(const std::string& message);
private:
};