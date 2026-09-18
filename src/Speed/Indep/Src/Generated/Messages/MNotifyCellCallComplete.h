#ifndef GENERATED_MESSAGES_MNOTIFYCELLCALLCOMPLETE_H
#define GENERATED_MESSAGES_MNOTIFYCELLCALLCOMPLETE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyCellCallComplete : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyCellCallComplete);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyCellCallComplete");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyCellCallComplete &message);

    MNotifyCellCallComplete() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyCellCallComplete() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyCellCallComplete::HandleMessage_LuaBinding(const MNotifyCellCallComplete &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyCellCallComplete::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
