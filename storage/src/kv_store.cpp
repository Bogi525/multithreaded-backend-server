#include "../inc/kv_store.hpp"

#include "../../logging/inc/logger.hpp"
#include <cstdint>

std::string KVStore::get(const std::string& key) {
    if (exists(key)) {
        return data_[key];
    }
    return "";
}

void KVStore::set(const std::string& key, std::string val) {
    data_[key] = val;
}

void KVStore::erase(const std::string& key) {
    if (exists(key)) {
        data_.erase(key);
    }
}

bool KVStore::exists(const std::string& key) {
    return data_.find(key) != data_.end();
}