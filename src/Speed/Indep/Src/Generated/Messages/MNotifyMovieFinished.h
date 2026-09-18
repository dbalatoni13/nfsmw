#ifndef GENERATED_MESSAGES_MNOTIFYMOVIEFINISHED_H
#define GENERATED_MESSAGES_MNOTIFYMOVIEFINISHED_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x10
class MNotifyMovieFinished : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MNotifyMovieFinished);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MNotifyMovieFinished");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MNotifyMovieFinished &message);

    MNotifyMovieFinished() : Hermes::Message(_GetKind(), _GetSize(), 0) {}

    ~MNotifyMovieFinished() {}
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MNotifyMovieFinished::HandleMessage_LuaBinding(const MNotifyMovieFinished &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MNotifyMovieFinished::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    lua_newtable(luaState);
}

#endif
