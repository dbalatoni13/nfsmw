#ifndef GENERATED_MESSAGES_MENTERPOSTRACEFLOW_H
#define GENERATED_MESSAGES_MENTERPOSTRACEFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MEnterPostRaceFlow : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MEnterPostRaceFlow);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MEnterPostRaceFlow");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MEnterPostRaceFlow &message);

    MEnterPostRaceFlow() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MEnterPostRaceFlow() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MEnterPostRaceFlow::HandleMessage_LuaBinding(const MEnterPostRaceFlow &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MEnterPostRaceFlow::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
