#include "../inc/user_store.hpp"

#include <stdexcept>
#include "../../logging/inc/logger.hpp"

UserStore::UserStore () {
    if (sqlite3_open("data/users.db", &db_) != SQLITE_OK) {
        throw std::runtime_error("Failed to open users.db");
    }
}

UserStore::~UserStore () {
    if (db_) {
        sqlite3_close(db_);
    }
}

bool UserStore::register_user(const std::string& username, const std::string& password) {
    const char* sql =
    "INSERT INTO users(username, password_hash) "
    "VALUES (?,?);";

    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        Logger::error("Statement preparation went wrong");
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    std::string hash = PasswordHasher::hash(password);

    sqlite3_bind_text(stmt, 2, hash.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    sqlite3_finalize(stmt);

    return success;
}

bool UserStore::authenticate(std::string username, std::string password) {
    const char* sql =
        "SELECT password_hash "
        "FROM users "
        "WHERE username = ?";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        Logger::error("Statement preparation went wrong");
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    bool authenticated = false;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* stored_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        authenticated = PasswordHasher::hash(password) == stored_hash;
    }

    sqlite3_finalize(stmt);

    return authenticated;
}

std::unordered_map<std::string, std::string>::iterator UserStore::find(std::string username) {
    return users_.find(username);
}

std::unordered_map<std::string, std::string>::iterator UserStore::end() {
    return users_.end();
}