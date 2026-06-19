#include "../inc/client_session.hpp"

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include "../../concurrency/inc/thread_pool.hpp"
#include "../../logging/inc/logger.hpp"

ClientSession::ClientSession(int client_fd) : client_fd_(client_fd) {}

void ClientSession::queue_response(std::string data) {
    write_buffer_ += data;
}

bool ClientSession::handle_read() {
    char buffer[1024];

    ssize_t bytes_received = recv(client_fd_, buffer, sizeof(buffer), 0);

    if (bytes_received < 0) {
        Logger::error("recv failed on fd ", client_fd_);
        return false;
    }
    if (bytes_received == 0) {
        Logger::info("Client ", client_fd_, " disconnected");
        return false;
    }

    read_buffer_.append(buffer, bytes_received);

    Logger::info("Received: \"", std::string(buffer, bytes_received), "\"");

    queue_response(read_buffer_); // handle the data (right now just echo)

    read_buffer_.clear();

    return true;
}

bool ClientSession::handle_write() {
    if (write_buffer_.empty()) return true;

    ssize_t bytes_sent = send(client_fd_, write_buffer_.data(), write_buffer_.size(), 0);

    if (bytes_sent <= 0) {
        Logger::error("send failed on fd ", client_fd_);
        return false;
    }

    Logger::info(ThreadPool::current_worker_id(), " sent: \"", std::string(write_buffer_.data(), bytes_sent) , "\"");

    write_buffer_.erase(0, bytes_sent);

    return true;
}

void ClientSession::close_session() {
    if (!closed_) {
        close(client_fd_);
        closed_ = true;
    }
}