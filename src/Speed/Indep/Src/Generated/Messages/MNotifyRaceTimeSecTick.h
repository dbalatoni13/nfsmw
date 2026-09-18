#ifndef GENERATED_MESSAGES_MNOTIFYRACETIMESECTICK_H
#define GENERATED_MESSAGES_MNOTIFYRACETIMESECTICK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyRaceTimeSecTick : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyRaceTimeSecTick);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyRaceTimeSecTick");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyRaceTimeSecTick &message);

    MNotifyRaceTimeSecTick(float _TimeElapsed) : Hermes::Message(_GetKind(), _GetSize(), 0), fTimeElapsed(_TimeElapsed) {}

    ~MNotifyRaceTimeSecTick() {}

    float GetTimeElapsed() const {
        return fTimeElapsed;
    }

    void SetTimeElapsed(float _TimeElapsed) {
        fTimeElapsed = _TimeElapsed;
    }

  private:
    float fTimeElapsed; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyRaceTimeSecTick::HandleMessage_LuaBinding(const MNotifyRaceTimeSecTick &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyRaceTimeSecTick::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyRaceTimeSecTick *message = static_cast<const MNotifyRaceTimeSecTick *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "TimeElapsed");
    lua_pushnumber(luaState, message->fTimeElapsed);
    lua_settable(luaState, -3);
}

#endif
