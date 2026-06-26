#ifndef _user_store_hpp_
#define _user_store_hpp_

#include <unordered_map>
#include <string>
#include "../../auth/inc/password_hasher.hpp"

class UserStore {
public:
    std::unordered_map<std::string, std::string>::iterator find(std::string username);
    std::unordered_map<std::string, std::string>::iterator end();
private:
    std::unordered_map<std::string, std::string> users_ = {
        {"admin", "ef92b778bafe771e89245b89ecbc08a44a4e166c06659911881f383d4473e94f"},
        {"bogi", "0b14d501a594442a01c6859541bcb3e8164d183d32937b851835442f69d5c94e"}
    };
};

#endif