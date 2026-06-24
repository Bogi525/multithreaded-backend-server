#ifndef _command_dispatcher_hpp_
#define _command_dispatcher_hpp_

#include "../../protocol/inc/command.hpp"
#include "../../protocol/inc/response.hpp"
#include "../../storage/inc/kv_store.hpp"

class CommandDispatcher {
public:
    Response dispatch(const Command& cmd);
private:
    KVStore kv_store_;
};

#endif