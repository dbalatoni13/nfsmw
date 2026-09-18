#ifndef GENERATED_MESSAGES_MQUITTOFRONTEND_H
#define GENERATED_MESSAGES_MQUITTOFRONTEND_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MQuitToFrontEnd : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MQuitToFrontEnd);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MQuitToFrontEnd");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MQuitToFrontEnd &message);

    MQuitToFrontEnd() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MQuitToFrontEnd() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MQuitToFrontEnd::HandleMessage_LuaBinding(const MQuitToFrontEnd &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MQuitToFrontEnd::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
