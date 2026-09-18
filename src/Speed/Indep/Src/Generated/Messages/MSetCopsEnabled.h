#ifndef GENERATED_MESSAGES_MSETCOPSENABLED_H
#define GENERATED_MESSAGES_MSETCOPSENABLED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MSetCopsEnabled : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MSetCopsEnabled);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MSetCopsEnabled");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MSetCopsEnabled &message);

    MSetCopsEnabled(bool _CopsEnabled) : Hermes::Message(_GetKind(), _GetSize(), 0), fCopsEnabled(_CopsEnabled) {}

    ~MSetCopsEnabled() {}

    bool GetCopsEnabled() const {
        return fCopsEnabled;
    }

    void SetCopsEnabled(bool _CopsEnabled) {
        fCopsEnabled = _CopsEnabled;
    }

  private:
    bool fCopsEnabled; // offset 0x10, size 0x1
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MSetCopsEnabled::HandleMessage_LuaBinding(const MSetCopsEnabled &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MSetCopsEnabled::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MSetCopsEnabled *message = static_cast<const MSetCopsEnabled *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "CopsEnabled");
    lua_pushboolean(luaState, message->fCopsEnabled);
    lua_settable(luaState, -3);
}

#endif
