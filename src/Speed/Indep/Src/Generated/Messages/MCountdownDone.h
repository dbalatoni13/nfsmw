#ifndef GENERATED_MESSAGES_MCOUNTDOWNDONE_H
#define GENERATED_MESSAGES_MCOUNTDOWNDONE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MCountdownDone : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MCountdownDone);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MCountdownDone");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MCountdownDone &message);

    MCountdownDone() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MCountdownDone() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MCountdownDone::HandleMessage_LuaBinding(const MCountdownDone &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MCountdownDone::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
