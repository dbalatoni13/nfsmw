#ifndef GENERATED_MESSAGES_MNOTIFYPURSUITLENGTH_H
#define GENERATED_MESSAGES_MNOTIFYPURSUITLENGTH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MNotifyPursuitLength : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyPursuitLength);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyPursuitLength");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyPursuitLength &message);

    MNotifyPursuitLength(HSIMABLE _Perpetrator, float _PursuitLength)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fPerpetrator(_Perpetrator), fPursuitLength(_PursuitLength) {}

    ~MNotifyPursuitLength() {}

    HSIMABLE GetPerpetrator() const {
        return fPerpetrator;
    }

    void SetPerpetrator(HSIMABLE _Perpetrator) {
        fPerpetrator = _Perpetrator;
    }

    float GetPursuitLength() const {
        return fPursuitLength;
    }

    void SetPursuitLength(float _PursuitLength) {
        fPursuitLength = _PursuitLength;
    }

  private:
    HSIMABLE fPerpetrator; // offset 0x10, size 0x4
    float fPursuitLength;  // offset 0x14, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyPursuitLength::HandleMessage_LuaBinding(const MNotifyPursuitLength &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyPursuitLength::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyPursuitLength *message = static_cast<const MNotifyPursuitLength *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "Perpetrator");
    if (ISimable::FindInstance(message->fPerpetrator) != NULL) {
        *static_cast<HSIMABLE *>(lua_newuserdata(luaState, sizeof(HSIMABLE))) = message->fPerpetrator;
        LuaBindery::AttachMetatable(luaState, "ISimable");
    } else {
        lua_pushnil(luaState);
    }
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "PursuitLength");
    lua_pushnumber(luaState, message->fPursuitLength);
    lua_settable(luaState, -3);
}

#endif
