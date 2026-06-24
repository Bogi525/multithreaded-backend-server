#include "../inc/client_session.hpp"

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include "../../concurrency/inc/thread_pool.hpp"
#include "../../logging/inc/logger.hpp"

ClientSession::ClientSession(int client_fd, Parser& parser, CommandDispatcher& command_dispatcher)
    : client_fd_(client_fd),
    parser_(parser),
    dispatcher_(command_dispatcher)
    {}

int ClientSession::fd() const {
    return client_fd_;
}

bool ClientSession::write_empty() {
    return write_buffer_.empty();
}

void ClientSession::queue_response(std::string data) {
    write_buffer_ += std::move(data);
}

bool ClientSession::handle_read() {
    char buffer[4096];

    while(true) {
        ssize_t bytes_received = recv(client_fd_, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            read_buffer_.append(buffer, bytes_received);
        }
        else if (bytes_received == 0) {
            Logger::info("Worker ", ThreadPool::current_worker_id(), ": Client ", client_fd_, " disconnected");
            return false;
        }
        else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }

            Logger::error("recv failed on fd ", client_fd_);
            return false;
        }
    }

    // handle the data (right now just echo)
    Command cmd = parser_.parse(read_buffer_);

    Logger::info(
        "Parsed command type = ",
        static_cast<int>(cmd.type)
    );

    for (const auto& arg : cmd.args)
    {
        Logger::info("Arg = ", arg);
    }

    Response response = dispatcher_.dispatch(cmd);

    Logger::info(
        "Response = ",
        response.get_data()
    );

    queue_response(response.get_data());
    read_buffer_.clear();

    return true;
}

bool ClientSession::handle_write() {
    if (write_buffer_.empty()) return true;

    while (!write_buffer_.empty()) {
        ssize_t bytes_sent = send(client_fd_, write_buffer_.data(), write_buffer_.size(), 0);

        if (bytes_sent > 0) {
            write_buffer_.erase(0, bytes_sent);
        }
        else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }

            Logger::error("send failed on fd ", client_fd_);
            return false;
        }
    }

    return true;
}

void ClientSession::close_session() {
    if (!closed_) {
        close(client_fd_);
        closed_ = true;
    }
}