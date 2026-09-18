#ifndef GENERATED_MESSAGES_MFORCEPURSUITSTART_H
#define GENERATED_MESSAGES_MFORCEPURSUITSTART_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MForcePursuitStart : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MForcePursuitStart);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MForcePursuitStart");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MForcePursuitStart &message);

    MForcePursuitStart(int _MinHeatLevel) : Hermes::Message(_GetKind(), _GetSize(), 0), fMinHeatLevel(_MinHeatLevel) {}

    ~MForcePursuitStart() {}

    int GetMinHeatLevel() const {
        return fMinHeatLevel;
    }

    void SetMinHeatLevel(int _MinHeatLevel) {
        fMinHeatLevel = _MinHeatLevel;
    }

  private:
    int fMinHeatLevel; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MForcePursuitStart::HandleMessage_LuaBinding(const MForcePursuitStart &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MForcePursuitStart::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MForcePursuitStart *message = static_cast<const MForcePursuitStart *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "MinHeatLevel");
    lua_pushnumber(luaState, message->fMinHeatLevel);
    lua_settable(luaState, -3);
}

#endif
