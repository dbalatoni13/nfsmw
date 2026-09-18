#ifndef GENERATED_MESSAGES_MNOTIFYVEHICLEDESTROYED_H
#define GENERATED_MESSAGES_MNOTIFYVEHICLEDESTROYED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyVehicleDestroyed : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyVehicleDestroyed);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyVehicleDestroyed");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyVehicleDestroyed &message);

    MNotifyVehicleDestroyed(HSIMABLE _Racer) : Hermes::Message(_GetKind(), _GetSize(), 0), fRacer(_Racer) {}

    ~MNotifyVehicleDestroyed() {}

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

inline void MNotifyVehicleDestroyed::HandleMessage_LuaBinding(const MNotifyVehicleDestroyed &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyVehicleDestroyed::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyVehicleDestroyed *message = static_cast<const MNotifyVehicleDestroyed *>(messageBase);

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
