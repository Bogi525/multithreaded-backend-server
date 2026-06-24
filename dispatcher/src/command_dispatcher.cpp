#include "../inc/command_dispatcher.hpp"

Response CommandDispatcher::dispatch(const Command& cmd) {
    switch (cmd.type) {
    case CommandType::PING:
        return {"PONG"};
        break;
    case CommandType::GET:
        {
            if (cmd.args.size() != 1) {
                return {"ERROR: GET requires 1 argument"};
            }

            if (!kv_store_.exists(cmd.args[0])) {
                return {"NOT FOUND"};
            }

            return {kv_store_.get(cmd.args[0])};
        }
    case CommandType::SET:
        {
            if (cmd.args.size() != 2) {
                return {"ERROR: SET requires 2 arguments"};
            }

            kv_store_.set(
                cmd.args[0],
                cmd.args[1]
            );

            return {"OK"};
        }
    case CommandType::DEL:
        {
            if (cmd.args.size() != 1) {
                return {"ERROR DEL requires 1 argument"};
            }

            kv_store_.erase(cmd.args[0]);

            return {"OK"};
        }
    default:
        {
            return {"ERROR: Unknown command"};
        }
    }
}