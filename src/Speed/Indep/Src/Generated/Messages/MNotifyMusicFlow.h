#ifndef GENERATED_MESSAGES_MNOTIFYMUSICFLOW_H
#define GENERATED_MESSAGES_MNOTIFYMUSICFLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyMusicFlow : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyMusicFlow);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyMusicFlow");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyMusicFlow &message);

    MNotifyMusicFlow(int _Part) : Hermes::Message(_GetKind(), _GetSize(), 0), fPart(_Part) {}

    ~MNotifyMusicFlow() {}

    int GetPart() const {
        return fPart;
    }

    void SetPart(int _Part) {
        fPart = _Part;
    }

  private:
    int fPart; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyMusicFlow::HandleMessage_LuaBinding(const MNotifyMusicFlow &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyMusicFlow::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyMusicFlow *message = static_cast<const MNotifyMusicFlow *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "Part");
    lua_pushnumber(luaState, message->fPart);
    lua_settable(luaState, -3);
}

#endif
