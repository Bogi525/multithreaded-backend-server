#include "../inc/parser.hpp"

Command Parser::parse(std::string_view input) {
    Command command;

    std::vector<std::string> tokens;

    size_t pos = 0;

    while (pos < input.size()) {
        while (pos < input.size() && std::isspace(input[pos])) {
            pos++;
        }

        size_t start = pos;

        while (pos < input.size() && !std::isspace(input[pos])) {
            pos++;
        }

        if (start != pos) {
            tokens.emplace_back(input.substr(start, pos - start));
        }
    }

    if (tokens.empty()) {
        return command;
    }

    if (tokens[0] == "PING") {
        command.type = CommandType::PING;
    } else if (tokens[0] == "GET") {
        command.type = CommandType::GET;
    } else if (tokens[0] == "SET") {
        command.type = CommandType::SET;
    } else if (tokens[0] == "DEL") {
        command.type = CommandType::DEL;
    } else {
        command.type = CommandType::UNKNOWN;
    }

    command.args.assign(tokens.begin() + 1, tokens.end());

    return command;
}