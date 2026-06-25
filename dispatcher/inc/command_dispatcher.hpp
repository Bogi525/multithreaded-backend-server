#ifndef _command_dispatcher_hpp_
#define _command_dispatcher_hpp_

#include "../../protocol/inc/command.hpp"
#include "../../protocol/inc/response.hpp"
#include "../../storage/inc/kv_store.hpp"
#include "../../storage/inc/user_store.hpp"

class ClientSession;

class CommandDispatcher {
public:
    bool is_authenticated_required(CommandType type);
    Response dispatch(const Command& cmd, ClientSession& session);
private:
    KVStore kv_store_;
    UserStore user_store_;
};

#endif