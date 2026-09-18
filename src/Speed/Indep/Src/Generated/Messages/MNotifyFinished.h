#ifndef GENERATED_MESSAGES_MNOTIFYFINISHED_H
#define GENERATED_MESSAGES_MNOTIFYFINISHED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyFinished : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyFinished);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyFinished");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyFinished &message);

    MNotifyFinished(GCollectionKey _Sender) : Hermes::Message(_GetKind(), _GetSize(), 0), fSender(_Sender) {}

    ~MNotifyFinished() {}

    GCollectionKey GetSender() const {
        return fSender;
    }

    void SetSender(GCollectionKey _Sender) {
        fSender = _Sender;
    }

  private:
    GCollectionKey fSender; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyFinished::HandleMessage_LuaBinding(const MNotifyFinished &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyFinished::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyFinished *message = static_cast<const MNotifyFinished *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "Sender");
    GRuntimeInstance *pSender = message->fSender;

    if (pSender != NULL) {
        *static_cast<GRuntimeInstance **>(lua_newuserdata(luaState, sizeof(GRuntimeInstance *))) = pSender;
        LuaBindery::AttachMetatable(luaState, "GRuntimeInstance");
    } else {
        lua_pushnil(luaState);
    }
    lua_settable(luaState, -3);
}

#endif
