#ifndef GENERATED_MESSAGES_MENTERFREEROAM_H
#define GENERATED_MESSAGES_MENTERFREEROAM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MEnterFreeRoam : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MEnterFreeRoam);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MEnterFreeRoam");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MEnterFreeRoam &message);

    MEnterFreeRoam() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MEnterFreeRoam() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MEnterFreeRoam::HandleMessage_LuaBinding(const MEnterFreeRoam &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MEnterFreeRoam::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
