#ifndef GENERATED_MESSAGES_MNOTIFYKNOCKEDOUT_H
#define GENERATED_MESSAGES_MNOTIFYKNOCKEDOUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyKnockedOut : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyKnockedOut);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyKnockedOut");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyKnockedOut &message);

    MNotifyKnockedOut(HSIMABLE _Racer) : Hermes::Message(_GetKind(), _GetSize(), 0), fRacer(_Racer) {}

    ~MNotifyKnockedOut() {}

    HSIMABLE GetRacer() const {
        return fRacer;
    }

    void SetRacer(HSIMABLE _Racer) {
        fRacer = _Racer;
    }

  private:
    HSIMABLE fRacer; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyKnockedOut::HandleMessage_LuaBinding(const MNotifyKnockedOut &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyKnockedOut::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyKnockedOut *message = static_cast<const MNotifyKnockedOut *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "Racer");
    if (ISimable::FindInstance(message->fRacer) != NULL) {
        *static_cast<HSIMABLE *>(lua_newuserdata(luaState, sizeof(HSIMABLE))) = message->fRacer;
        LuaBindery::AttachMetatable(luaState, "ISimable");
    } else {
        lua_pushnil(luaState);
    }
    lua_settable(luaState, -3);
}

#endif
