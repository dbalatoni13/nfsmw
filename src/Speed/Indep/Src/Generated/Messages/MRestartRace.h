#ifndef GENERATED_MESSAGES_MRESTARTRACE_H
#define GENERATED_MESSAGES_MRESTARTRACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MRestartRace : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MRestartRace);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MRestartRace");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MRestartRace &message);

    MRestartRace() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MRestartRace() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MRestartRace::HandleMessage_LuaBinding(const MRestartRace &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MRestartRace::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
