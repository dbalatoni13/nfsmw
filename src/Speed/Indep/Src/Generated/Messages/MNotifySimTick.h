#ifndef GENERATED_MESSAGES_MNOTIFYSIMTICK_H
#define GENERATED_MESSAGES_MNOTIFYSIMTICK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MNotifySimTick : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifySimTick);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifySimTick");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifySimTick &message);

    MNotifySimTick(float _SimTime, float _TimeStep) : Hermes::Message(_GetKind(), _GetSize(), 0), fSimTime(_SimTime), fTimeStep(_TimeStep) {}

    ~MNotifySimTick() {}

    float GetSimTime() const {
        return fSimTime;
    }

    void SetSimTime(float _SimTime) {
        fSimTime = _SimTime;
    }

    float GetTimeStep() const {
        return fTimeStep;
    }

    void SetTimeStep(float _TimeStep) {
        fTimeStep = _TimeStep;
    }

  private:
    float fSimTime;  // offset 0x10, size 0x4
    float fTimeStep; // offset 0x14, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifySimTick::HandleMessage_LuaBinding(const MNotifySimTick &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifySimTick::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifySimTick *message = static_cast<const MNotifySimTick *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "SimTime");
    lua_pushnumber(luaState, message->fSimTime);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "TimeStep");
    lua_pushnumber(luaState, message->fTimeStep);
    lua_settable(luaState, -3);
}

#endif
