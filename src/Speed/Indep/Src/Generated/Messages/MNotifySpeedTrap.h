#ifndef GENERATED_MESSAGES_MNOTIFYSPEEDTRAP_H
#define GENERATED_MESSAGES_MNOTIFYSPEEDTRAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x1c
class MNotifySpeedTrap : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifySpeedTrap);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifySpeedTrap");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifySpeedTrap &message);

    MNotifySpeedTrap(GCollectionKey _TrapActivity, HSIMABLE _Racer, float _SpeedKmh)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fTrapActivity(_TrapActivity), fRacer(_Racer), fSpeedKmh(_SpeedKmh) {}

    ~MNotifySpeedTrap() {}

    GCollectionKey GetTrapActivity() const {
        return fTrapActivity;
    }

    void SetTrapActivity(GCollectionKey _TrapActivity) {
        fTrapActivity = _TrapActivity;
    }

    HSIMABLE GetRacer() const {
        return fRacer;
    }

    void SetRacer(HSIMABLE _Racer) {
        fRacer = _Racer;
    }

    float GetSpeedKmh() const {
        return fSpeedKmh;
    }

    void SetSpeedKmh(float _SpeedKmh) {
        fSpeedKmh = _SpeedKmh;
    }

  private:
    GCollectionKey fTrapActivity; // offset 0x10, size 0x4
    HSIMABLE fRacer;              // offset 0x14, size 0x4
    float fSpeedKmh;              // offset 0x18, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifySpeedTrap::HandleMessage_LuaBinding(const MNotifySpeedTrap &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifySpeedTrap::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifySpeedTrap *message = static_cast<const MNotifySpeedTrap *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "TrapActivity");
    GRuntimeInstance *pTrapActivity = message->fTrapActivity;

    if (pTrapActivity != NULL) {
        *static_cast<GRuntimeInstance **>(lua_newuserdata(luaState, sizeof(GRuntimeInstance *))) = pTrapActivity;
        LuaBindery::AttachMetatable(luaState, "GRuntimeInstance");
    } else {
        lua_pushnil(luaState);
    }
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "Racer");
    if (ISimable::FindInstance(message->fRacer) != NULL) {
        *static_cast<HSIMABLE *>(lua_newuserdata(luaState, sizeof(HSIMABLE))) = message->fRacer;
        LuaBindery::AttachMetatable(luaState, "ISimable");
    } else {
        lua_pushnil(luaState);
    }
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "SpeedKmh");
    lua_pushnumber(luaState, message->fSpeedKmh);
    lua_settable(luaState, -3);
}

#endif
