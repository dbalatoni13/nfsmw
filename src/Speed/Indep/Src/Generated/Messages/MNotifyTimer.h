#ifndef GENERATED_MESSAGES_MNOTIFYTIMER_H
#define GENERATED_MESSAGES_MNOTIFYTIMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MNotifyTimer : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyTimer);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyTimer");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyTimer &message);

    MNotifyTimer(const char *_Name) : Hermes::Message(_GetKind(), _GetSize(), 0), fName(_Name) {}

    ~MNotifyTimer() {}

    const char *GetName() const {
        return fName;
    }

    void SetName(const char *_Name) {
        fName = _Name;
    }

  private:
    const char *fName; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyTimer::HandleMessage_LuaBinding(const MNotifyTimer &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyTimer::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifyTimer *message = static_cast<const MNotifyTimer *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "Name");
    lua_pushstring(luaState, message->fName);
    lua_settable(luaState, -3);
}

#endif
