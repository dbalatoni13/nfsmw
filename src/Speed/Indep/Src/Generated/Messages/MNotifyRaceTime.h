#ifndef GENERATED_MESSAGES_MNOTIFYRACETIME_H
#define GENERATED_MESSAGES_MNOTIFYRACETIME_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x1c
class MNotifyRaceTime : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyRaceTime);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyRaceTime");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyRaceTime &message);

    MNotifyRaceTime(float _TimeElapsed, bool _TimeIsLimited, float _TimeRemaining)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fTimeElapsed(_TimeElapsed), fTimeIsLimited(_TimeIsLimited), fTimeRemaining(_TimeRemaining) {}

    ~MNotifyRaceTime() {}

    float GetTimeElapsed() const {
        return fTimeElapsed;
    }

    void SetTimeElapsed(float _TimeElapsed) {
        fTimeElapsed = _TimeElapsed;
    }

    bool GetTimeIsLimited() const {
        return fTimeIsLimited;
    }

    void SetTimeIsLimited(bool _TimeIsLimited) {
        fTimeIsLimited = _TimeIsLimited;
    }

    float GetTimeRemaining() const {
        return fTimeRemaining;
    }

    void SetTimeRemaining(float _TimeRemaining) {
        fTimeRemaining = _TimeRemaining;
    }

  private:
    float fTimeElapsed;   // offset 0x10, size 0x4
    bool fTimeIsLimited;  // offset 0x14, size 0x1
    float fTimeRemaining; // offset 0x18, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyRaceTime::HandleMessage_LuaBinding(const MNotifyRaceTime &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyRaceTime::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyRaceTime *message = static_cast<const MNotifyRaceTime *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "TimeElapsed");
    lua_pushnumber(luaState, message->fTimeElapsed);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "TimeIsLimited");
    lua_pushboolean(luaState, message->fTimeIsLimited);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "TimeRemaining");
    lua_pushnumber(luaState, message->fTimeRemaining);
    lua_settable(luaState, -3);
}

#endif
