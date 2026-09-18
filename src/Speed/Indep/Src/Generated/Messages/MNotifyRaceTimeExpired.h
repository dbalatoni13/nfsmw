#ifndef GENERATED_MESSAGES_MNOTIFYRACETIMEEXPIRED_H
#define GENERATED_MESSAGES_MNOTIFYRACETIMEEXPIRED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyRaceTimeExpired : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyRaceTimeExpired);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyRaceTimeExpired");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyRaceTimeExpired &message);

    MNotifyRaceTimeExpired() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyRaceTimeExpired() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyRaceTimeExpired::HandleMessage_LuaBinding(const MNotifyRaceTimeExpired &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyRaceTimeExpired::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
