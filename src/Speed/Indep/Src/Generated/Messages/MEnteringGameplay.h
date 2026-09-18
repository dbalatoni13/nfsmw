#ifndef GENERATED_MESSAGES_MENTERINGGAMEPLAY_H
#define GENERATED_MESSAGES_MENTERINGGAMEPLAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MEnteringGameplay : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MEnteringGameplay);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MEnteringGameplay");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MEnteringGameplay &message);

    MEnteringGameplay() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MEnteringGameplay() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MEnteringGameplay::HandleMessage_LuaBinding(const MEnteringGameplay &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MEnteringGameplay::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
