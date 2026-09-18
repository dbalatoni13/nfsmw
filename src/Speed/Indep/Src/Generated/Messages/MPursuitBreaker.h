#ifndef GENERATED_MESSAGES_MPURSUITBREAKER_H
#define GENERATED_MESSAGES_MPURSUITBREAKER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MPursuitBreaker : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MPursuitBreaker);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MPursuitBreaker");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MPursuitBreaker &message);

    MPursuitBreaker(bool _StartBreaker) : Hermes::Message(_GetKind(), _GetSize(), 0), fStartBreaker(_StartBreaker) {}

    ~MPursuitBreaker() {}

    bool GetStartBreaker() const {
        return fStartBreaker;
    }

    void SetStartBreaker(bool _StartBreaker) {
        fStartBreaker = _StartBreaker;
    }

  private:
    bool fStartBreaker; // offset 0x10, size 0x1
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MPursuitBreaker::HandleMessage_LuaBinding(const MPursuitBreaker &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MPursuitBreaker::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MPursuitBreaker *message = static_cast<const MPursuitBreaker *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "StartBreaker");
    lua_pushboolean(luaState, message->fStartBreaker);
    lua_settable(luaState, -3);
}

#endif
