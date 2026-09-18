#ifndef GENERATED_MESSAGES_MSETCOPAUTOSPAWNMODE_H
#define GENERATED_MESSAGES_MSETCOPAUTOSPAWNMODE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MSetCopAutoSpawnMode : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MSetCopAutoSpawnMode);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MSetCopAutoSpawnMode");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MSetCopAutoSpawnMode &message);

    MSetCopAutoSpawnMode(bool _AutoSpawnMode) : Hermes::Message(_GetKind(), _GetSize(), 0), fAutoSpawnMode(_AutoSpawnMode) {}

    ~MSetCopAutoSpawnMode() {}

    bool GetAutoSpawnMode() const {
        return fAutoSpawnMode;
    }

    void SetAutoSpawnMode(bool _AutoSpawnMode) {
        fAutoSpawnMode = _AutoSpawnMode;
    }

  private:
    int fAutoSpawnMode; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MSetCopAutoSpawnMode::HandleMessage_LuaBinding(const MSetCopAutoSpawnMode &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MSetCopAutoSpawnMode::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MSetCopAutoSpawnMode *message = static_cast<const MSetCopAutoSpawnMode *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "AutoSpawnMode");
    lua_pushboolean(luaState, message->fAutoSpawnMode);
    lua_settable(luaState, -3);
}

#endif
