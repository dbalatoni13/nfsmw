#ifndef GENERATED_MESSAGES_MSTATEEXIT_H
#define GENERATED_MESSAGES_MSTATEEXIT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MStateExit : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MStateExit);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MStateExit");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MStateExit &message);

    MStateExit() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MStateExit() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MStateExit::HandleMessage_LuaBinding(const MStateExit &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MStateExit::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
