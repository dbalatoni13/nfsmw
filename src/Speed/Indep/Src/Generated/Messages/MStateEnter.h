#ifndef GENERATED_MESSAGES_MSTATEENTER_H
#define GENERATED_MESSAGES_MSTATEENTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MStateEnter : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MStateEnter);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MStateEnter");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MStateEnter &message);

    MStateEnter() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MStateEnter() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MStateEnter::HandleMessage_LuaBinding(const MStateEnter &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MStateEnter::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
