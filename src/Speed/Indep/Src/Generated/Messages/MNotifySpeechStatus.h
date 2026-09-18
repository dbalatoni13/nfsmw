#ifndef GENERATED_MESSAGES_MNOTIFYSPEECHSTATUS_H
#define GENERATED_MESSAGES_MNOTIFYSPEECHSTATUS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

namespace Speech {
struct ScheduledSpeechEvent;
}

// total size: 0x14
class MNotifySpeechStatus : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifySpeechStatus);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifySpeechStatus");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifySpeechStatus &message);

    MNotifySpeechStatus(Speech::ScheduledSpeechEvent *_Event) : Hermes::Message(_GetKind(), _GetSize(), 0), fEvent(_Event) {}

    ~MNotifySpeechStatus() {}

    Speech::ScheduledSpeechEvent *GetEvent() const {
        return fEvent;
    }

    void SetEvent(Speech::ScheduledSpeechEvent *_Event) {
        fEvent = _Event;
    }

  private:
    Speech::ScheduledSpeechEvent *fEvent; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifySpeechStatus::HandleMessage_LuaBinding(const MNotifySpeechStatus &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifySpeechStatus::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MNotifySpeechStatus *message = static_cast<const MNotifySpeechStatus *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "Event");
    if (message->fEvent != NULL) {
        *static_cast<Speech::ScheduledSpeechEvent **>(lua_newuserdata(luaState, sizeof(Speech::ScheduledSpeechEvent *))) = message->fEvent;
        LuaBindery::AttachMetatable(luaState, "Speech::ScheduledSpeechEvent");
    } else {
        lua_pushnil(luaState);
    }
    lua_settable(luaState, -3);
}

#endif
