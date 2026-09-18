#ifndef GENERATED_MESSAGES_MDECLINEENTERCAREEREVENT_H
#define GENERATED_MESSAGES_MDECLINEENTERCAREEREVENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MDeclineEnterCareerEvent : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MDeclineEnterCareerEvent);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MDeclineEnterCareerEvent");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MDeclineEnterCareerEvent &message);

    MDeclineEnterCareerEvent() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MDeclineEnterCareerEvent() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MDeclineEnterCareerEvent::HandleMessage_LuaBinding(const MDeclineEnterCareerEvent &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MDeclineEnterCareerEvent::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
