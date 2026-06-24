#ifndef _kv_store_hpp_
#define _kv_store_hpp_

#include <unordered_map>
#include <string>

class KVStore {
public:
    std::string get(const std::string& key);
    void set(const std::string& key, std::string val);
    void erase(const std::string& key);
    bool exists(const std::string& key);
private:
    std::unordered_map<std::string, std::string> data_;
    
};

#endif