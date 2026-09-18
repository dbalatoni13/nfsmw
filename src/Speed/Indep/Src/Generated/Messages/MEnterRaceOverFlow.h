#ifndef GENERATED_MESSAGES_MENTERRACEOVERFLOW_H
#define GENERATED_MESSAGES_MENTERRACEOVERFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MEnterRaceOverFlow : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MEnterRaceOverFlow);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MEnterRaceOverFlow");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MEnterRaceOverFlow &message);

    MEnterRaceOverFlow() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MEnterRaceOverFlow() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MEnterRaceOverFlow::HandleMessage_LuaBinding(const MEnterRaceOverFlow &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MEnterRaceOverFlow::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
