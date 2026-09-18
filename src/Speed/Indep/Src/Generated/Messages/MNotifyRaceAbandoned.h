#ifndef GENERATED_MESSAGES_MNOTIFYRACEABANDONED_H
#define GENERATED_MESSAGES_MNOTIFYRACEABANDONED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyRaceAbandoned : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyRaceAbandoned);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyRaceAbandoned");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyRaceAbandoned &message);

    MNotifyRaceAbandoned() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyRaceAbandoned() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyRaceAbandoned::HandleMessage_LuaBinding(const MNotifyRaceAbandoned &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyRaceAbandoned::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
