#ifndef GENERATED_MESSAGES_MJACKKNIFE_H
#define GENERATED_MESSAGES_MJACKKNIFE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MJackKnife : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MJackKnife);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MJackKnife");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MJackKnife &message);

    MJackKnife() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MJackKnife() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MJackKnife::HandleMessage_LuaBinding(const MJackKnife &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MJackKnife::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
