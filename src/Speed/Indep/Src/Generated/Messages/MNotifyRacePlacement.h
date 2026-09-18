#ifndef GENERATED_MESSAGES_MNOTIFYRACEPLACEMENT_H
#define GENERATED_MESSAGES_MNOTIFYRACEPLACEMENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x1c
class MNotifyRacePlacement : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyRacePlacement);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyRacePlacement");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyRacePlacement &message);

    MNotifyRacePlacement(GCollectionKey _Activity, HSIMABLE _Racer, int _Placing)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fActivity(_Activity), fRacer(_Racer), fPlacing(_Placing) {}

    ~MNotifyRacePlacement() {}

    GCollectionKey GetActivity() const {
        return fActivity;
    }

    void SetActivity(GCollectionKey _Activity) {
        fActivity = _Activity;
    }

    HSIMABLE GetRacer() const {
        return fRacer;
    }

    void SetRacer(HSIMABLE _Racer) {
        fRacer = _Racer;
    }

    int GetPlacing() const {
        return fPlacing;
    }

    void SetPlacing(int _Placing) {
        fPlacing = _Placing;
    }

  private:
    GCollectionKey fActivity; // offset 0x10, size 0x4
    HSIMABLE fRacer;          // offset 0x14, size 0x4
    int fPlacing;             // offset 0x18, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyRacePlacement::HandleMessage_LuaBinding(const MNotifyRacePlacement &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyRacePlacement::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyRacePlacement *message = static_cast<const MNotifyRacePlacement *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "Activity");
    GRuntimeInstance *pActivity = message->fActivity;

    if (pActivity != NULL) {
        *static_cast<GRuntimeInstance **>(lua_newuserdata(luaState, sizeof(GRuntimeInstance *))) = pActivity;
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

    lua_pushstring(luaState, "Placing");
    lua_pushnumber(luaState, message->fPlacing);
    lua_settable(luaState, -3);
}

#endif
