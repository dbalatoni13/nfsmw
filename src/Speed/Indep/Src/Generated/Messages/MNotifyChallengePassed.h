#ifndef GENERATED_MESSAGES_MNOTIFYCHALLENGEPASSED_H
#define GENERATED_MESSAGES_MNOTIFYCHALLENGEPASSED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyChallengePassed : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyChallengePassed);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyChallengePassed");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyChallengePassed &message);

    MNotifyChallengePassed(const char *_ChallengeName) : Hermes::Message(_GetKind(), _GetSize(), 0), fChallengeName(_ChallengeName) {}

    ~MNotifyChallengePassed() {}

    const char *GetChallengeName() const {
        return fChallengeName;
    }

    void SetChallengeName(const char *_ChallengeName) {
        fChallengeName = _ChallengeName;
    }

  private:
    const char *fChallengeName; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyChallengePassed::HandleMessage_LuaBinding(const MNotifyChallengePassed &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyChallengePassed::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyChallengePassed *message = static_cast<const MNotifyChallengePassed *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "ChallengeName");
    lua_pushstring(luaState, message->fChallengeName);
    lua_settable(luaState, -3);
}

#endif
