#include "../inc/password_hasher.hpp"

#include <openssl/sha.h>
#include <ostream>
#include <iomanip>
#include "../../logging/inc/logger.hpp"

std::string PasswordHasher::hash(std::string password) {
    unsigned char output[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char *>(password.c_str()), password.size(), output);

    std::ostringstream oss;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(output[i]);
    }

    return oss.str();
}