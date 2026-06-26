#ifndef _password_hasher_hpp_
#define _password_hasher_hpp_

#include <string>

class PasswordHasher {
public:
    static std::string hash(std::string password);
};

#endif