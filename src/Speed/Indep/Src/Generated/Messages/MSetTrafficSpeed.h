#ifndef GENERATED_MESSAGES_MSETTRAFFICSPEED_H
#define GENERATED_MESSAGES_MSETTRAFFICSPEED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x1c
class MSetTrafficSpeed : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MSetTrafficSpeed);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MSetTrafficSpeed");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MSetTrafficSpeed &message);

    MSetTrafficSpeed(float _SpeedDefault, float _SpeedHighway, unsigned int _FixSpeed)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fSpeedDefault(_SpeedDefault), fSpeedHighway(_SpeedHighway), fFixSpeed(_FixSpeed) {}

    ~MSetTrafficSpeed() {}

    float GetSpeedDefault() const {
        return fSpeedDefault;
    }

    void SetSpeedDefault(float _SpeedDefault) {
        fSpeedDefault = _SpeedDefault;
    }

    float GetSpeedHighway() const {
        return fSpeedHighway;
    }

    void SetSpeedHighway(float _SpeedHighway) {
        fSpeedHighway = _SpeedHighway;
    }

    unsigned int GetFixSpeed() const {
        return fFixSpeed;
    }

    void SetFixSpeed(unsigned int _FixSpeed) {
        fFixSpeed = _FixSpeed;
    }

  private:
    float fSpeedDefault;    // offset 0x10, size 0x4
    float fSpeedHighway;    // offset 0x14, size 0x4
    unsigned int fFixSpeed; // offset 0x18, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MSetTrafficSpeed::HandleMessage_LuaBinding(const MSetTrafficSpeed &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MSetTrafficSpeed::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MSetTrafficSpeed *message = static_cast<const MSetTrafficSpeed *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "SpeedDefault");
    lua_pushnumber(luaState, message->fSpeedDefault);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "SpeedHighway");
    lua_pushnumber(luaState, message->fSpeedHighway);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "FixSpeed");
    lua_pushnumber(luaState, message->fFixSpeed);
    lua_settable(luaState, -3);
}

#endif
