#ifndef _user_store_hpp_
#define _user_store_hpp_

#include <unordered_map>
#include <string>

class UserStore {
public:
    std::unordered_map<std::string, std::string>::iterator find(std::string username);
    std::unordered_map<std::string, std::string>::iterator end();
private:
    std::unordered_map<std::string, std::string> users_ = {{"admin", "password123"}, {"bogi", "password1"}};
};

#endif