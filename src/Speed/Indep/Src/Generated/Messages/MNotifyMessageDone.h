#ifndef GENERATED_MESSAGES_MNOTIFYMESSAGEDONE_H
#define GENERATED_MESSAGES_MNOTIFYMESSAGEDONE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyMessageDone : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyMessageDone);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyMessageDone");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyMessageDone &message);

    MNotifyMessageDone() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyMessageDone() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyMessageDone::HandleMessage_LuaBinding(const MNotifyMessageDone &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyMessageDone::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
