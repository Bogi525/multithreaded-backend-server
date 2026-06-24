#include "../inc/response.hpp"

Response::Response(std::string msg) {
    data = msg;
}

std::string Response::get_data() {
    return data;
}