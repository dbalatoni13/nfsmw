#ifndef GENERATED_MESSAGES_MMISCSOUND_H
#define GENERATED_MESSAGES_MMISCSOUND_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MMiscSound : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MMiscSound);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MMiscSound");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MMiscSound &message);

    MMiscSound(int _SoundID) : Hermes::Message(_GetKind(), _GetSize(), 0), fSoundID(_SoundID) {}

    ~MMiscSound() {}

    int GetSoundID() const {
        return fSoundID;
    }

    void SetSoundID(int _SoundID) {
        fSoundID = _SoundID;
    }

  private:
    int fSoundID; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MMiscSound::HandleMessage_LuaBinding(const MMiscSound &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MMiscSound::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MMiscSound *message = static_cast<const MMiscSound *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "SoundID");
    lua_pushnumber(luaState, message->fSoundID);
    lua_settable(luaState, -3);
}

#endif
