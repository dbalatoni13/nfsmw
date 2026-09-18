#ifndef GENERATED_MESSAGES_MNOTIFYMILESTONEREACHED_H
#define GENERATED_MESSAGES_MNOTIFYMILESTONEREACHED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MNotifyMilestoneReached : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyMilestoneReached);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyMilestoneReached");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyMilestoneReached &message);

    MNotifyMilestoneReached(const char *_MilestoneName, float _ValueReached)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fMilestoneName(_MilestoneName), fValueReached(_ValueReached) {}

    ~MNotifyMilestoneReached() {}

    const char *GetMilestoneName() const {
        return fMilestoneName;
    }

    void SetMilestoneName(const char *_MilestoneName) {
        fMilestoneName = _MilestoneName;
    }

    float GetValueReached() const {
        return fValueReached;
    }

    void SetValueReached(float _ValueReached) {
        fValueReached = _ValueReached;
    }

  private:
    const char *fMilestoneName; // offset 0x10, size 0x4
    float fValueReached;        // offset 0x14, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyMilestoneReached::HandleMessage_LuaBinding(const MNotifyMilestoneReached &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyMilestoneReached::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyMilestoneReached *message = static_cast<const MNotifyMilestoneReached *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "MilestoneName");
    lua_pushstring(luaState, message->fMilestoneName);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "ValueReached");
    lua_pushnumber(luaState, message->fValueReached);
    lua_settable(luaState, -3);
}

#endif
