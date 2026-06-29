#include "../inc/command_dispatcher.hpp"
#include "../../server/inc/client_session.hpp"
#include "../../auth/inc/password_hasher.hpp"

bool CommandDispatcher::is_authenticated_required(CommandType type) {
    return type == CommandType::GET ||
           type == CommandType::SET ||
           type == CommandType::DEL;
}

Response CommandDispatcher::dispatch(const Command& cmd, ClientSession& session) {
    if (is_authenticated_required(cmd.type) &&
        !session.authenticated()) {
        return {"ERROR: AUTH REQUIRED"};
    }

    switch (cmd.type) {
    case CommandType::PING:
        return {"PONG"};
        break;
    case CommandType::GET:
        {
            if (cmd.args.size() != 1) {
                return {"ERROR: GET requires 1 argument"};
            }

            std::optional<std::string> opt = kv_store_.get(cmd.args[0]);

            if (opt == std::nullopt) return {"NOT FOUND"};

            return {*opt};
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
    case CommandType::REGISTER:
        {
            if (cmd.args.size() != 2) {
                return {"ERROR: REGISTER requires 2 arguments"};
            }

            bool success  = user_store_.register_user(cmd.args[0], cmd.args[1]);

            if (!success ) {
                return {"ERROR: USER ALREADY EXISTS"};
            }

            session.authenticate(cmd.args[0]);
            
            return {"REGISTERED"};
        }
    case CommandType::AUTH:
        {
            if (cmd.args.size() != 2) {
                return {"ERROR: AUTH requires 2 arguments"};
            }

            bool authenticated = user_store_.authenticate(cmd.args[0], cmd.args[1]);

            if (!authenticated) {
                return {"INVALID CREDENTIALS"};
            }

            session.authenticate(cmd.args[0]);

            return {"AUTH OK"};
        }
    default:
        {
            return {"ERROR: Unknown command"};
        }
    }
}