#ifndef GENERATED_MESSAGES_MJUMPCUT_H
#define GENERATED_MESSAGES_MJUMPCUT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MJumpCut : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MJumpCut);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MJumpCut");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MJumpCut &message);

    MJumpCut(unsigned int _AnchorWorldID) : Hermes::Message(_GetKind(), _GetSize(), 0), fAnchorWorldID(_AnchorWorldID) {}

    ~MJumpCut() {}

    unsigned int GetAnchorWorldID() const {
        return fAnchorWorldID;
    }

    void SetAnchorWorldID(unsigned int _AnchorWorldID) {
        fAnchorWorldID = _AnchorWorldID;
    }

  private:
    unsigned int fAnchorWorldID; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MJumpCut::HandleMessage_LuaBinding(const MJumpCut &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MJumpCut::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MJumpCut *message = static_cast<const MJumpCut *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "AnchorWorldID");
    lua_pushnumber(luaState, message->fAnchorWorldID);
    lua_settable(luaState, -3);
}

#endif
