#include "../inc/user_store.hpp"

std::unordered_map<std::string, std::string>::iterator UserStore::find(std::string username) {
    return users_.find(username);
}

std::unordered_map<std::string, std::string>::iterator UserStore::end() {
    return users_.end();
}