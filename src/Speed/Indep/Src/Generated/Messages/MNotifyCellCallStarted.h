#ifndef GENERATED_MESSAGES_MNOTIFYCELLCALLSTARTED_H
#define GENERATED_MESSAGES_MNOTIFYCELLCALLSTARTED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyCellCallStarted : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyCellCallStarted);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyCellCallStarted");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyCellCallStarted &message);

    MNotifyCellCallStarted() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyCellCallStarted() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyCellCallStarted::HandleMessage_LuaBinding(const MNotifyCellCallStarted &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyCellCallStarted::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
