#ifndef GENERATED_MESSAGES_MNISCOMPLETE_H
#define GENERATED_MESSAGES_MNISCOMPLETE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNISComplete : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNISComplete);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNISComplete");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNISComplete &message);

    MNISComplete(const char *_NISName) : Hermes::Message(_GetKind(), _GetSize(), 0), fNISName(_NISName) {}

    ~MNISComplete() {}

    const char *GetNISName() const {
        return fNISName;
    }

    void SetNISName(const char *_NISName) {
        fNISName = _NISName;
    }

  private:
    const char *fNISName; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNISComplete::HandleMessage_LuaBinding(const MNISComplete &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNISComplete::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNISComplete *message = static_cast<const MNISComplete *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "NISName");
    lua_pushstring(luaState, message->fNISName);
    lua_settable(luaState, -3);
}

#endif
