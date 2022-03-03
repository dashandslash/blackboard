#pragma once

#ifdef _WIN32
#include "Rpc.h"
#include <windows.h>
#pragma comment(lib, "Rpcrt4.lib")
#elif __APPLE__
#include <uuid/uuid.h>
#endif

namespace blackboard::components {

struct Uuid
{
    Uuid()
    {
#ifdef _WIN32
        UUID uuid;
        UuidCreate(&uuid);
        char *uuid_cstr = nullptr;
        UuidToStringA(&uuid, reinterpret_cast<RPC_CSTR *>(&uuid_cstr));
        m_value = std::string(uuid_cstr);
        RpcStringFreeA(reinterpret_cast<RPC_CSTR *>(&uuid_cstr));
#elif __APPLE__
        uuid_t uuid;
        // 36 byte uuid plus null
        char uuid_cstr[37];
        uuid_generate(uuid);
        uuid_unparse(uuid, uuid_cstr);
        m_value = uuid_cstr;
#endif
    }
    std::string get() const
    {
        return m_value;
    }

private:
    std::string m_value{};
};

}    // namespace blackboard::components
