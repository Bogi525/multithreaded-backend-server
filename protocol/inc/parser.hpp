#ifndef _parser_hpp
#define _parser_hpp

#include "command.hpp"
#include <string>
#include <string_view>

class Parser {
public:
    Command parse(std::string_view input);
private:
};


#endif