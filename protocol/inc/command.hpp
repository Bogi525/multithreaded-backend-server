#ifndef _command_hpp_
#define _command_hpp_

#include <vector>
#include <string>

enum class CommandType {
    UNKNOWN,
    PING,
    GET,
    SET,
    DEL
};

struct Command {
    CommandType type;
    std::vector<std::string> args;
};

#endif