#ifndef GENERATED_MESSAGES_MNOTIFYONLINERACEOVER_H
#define GENERATED_MESSAGES_MNOTIFYONLINERACEOVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyOnlineRaceOver : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyOnlineRaceOver);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyOnlineRaceOver");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyOnlineRaceOver &message);

    MNotifyOnlineRaceOver(bool _ShowPostRace) : Hermes::Message(_GetKind(), _GetSize(), 0), fShowPostRace(_ShowPostRace) {}

    ~MNotifyOnlineRaceOver() {}

    bool GetShowPostRace() const {
        return fShowPostRace;
    }

    void SetShowPostRace(bool _ShowPostRace) {
        fShowPostRace = _ShowPostRace;
    }

  private:
    bool fShowPostRace; // offset 0x10, size 0x1
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyOnlineRaceOver::HandleMessage_LuaBinding(const MNotifyOnlineRaceOver &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyOnlineRaceOver::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyOnlineRaceOver *message = static_cast<const MNotifyOnlineRaceOver *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "ShowPostRace");
    lua_pushboolean(luaState, message->fShowPostRace);
    lua_settable(luaState, -3);
}

#endif
