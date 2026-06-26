#include "../inc/kv_store.hpp"

#include "../../logging/inc/logger.hpp"
#include <cstdint>

std::optional<std::string> KVStore::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (data_.find(key) != data_.end()) {
        return data_[key];
    }
    return std::nullopt;
}

void KVStore::set(const std::string& key, std::string val) {
    std::lock_guard<std::mutex> lock(mutex_);
    data_[key] = val;
}

void KVStore::erase(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (data_.find(key) != data_.end()) {
        data_.erase(key);
    }
}

bool KVStore::exists(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    return data_.find(key) != data_.end();
}