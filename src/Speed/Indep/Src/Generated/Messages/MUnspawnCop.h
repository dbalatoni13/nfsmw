#ifndef GENERATED_MESSAGES_MUNSPAWNCOP_H
#define GENERATED_MESSAGES_MUNSPAWNCOP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MUnspawnCop : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MUnspawnCop);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MUnspawnCop");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MUnspawnCop &message);

    MUnspawnCop(HSIMABLE _CopHandle, int _Param) : Hermes::Message(_GetKind(), _GetSize(), 0), fCopHandle(_CopHandle), fParam(_Param) {}

    ~MUnspawnCop() {}

    HSIMABLE GetCopHandle() const {
        return fCopHandle;
    }

    void SetCopHandle(HSIMABLE _CopHandle) {
        fCopHandle = _CopHandle;
    }

    int GetParam() const {
        return fParam;
    }

    void SetParam(int _Param) {
        fParam = _Param;
    }

  private:
    HSIMABLE fCopHandle; // offset 0x10, size 0x4
    int fParam;          // offset 0x14, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MUnspawnCop::HandleMessage_LuaBinding(const MUnspawnCop &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MUnspawnCop::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MUnspawnCop *message = static_cast<const MUnspawnCop *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "CopHandle");
    if (ISimable::FindInstance(message->fCopHandle) != NULL) {
        *static_cast<HSIMABLE *>(lua_newuserdata(luaState, sizeof(HSIMABLE))) = message->fCopHandle;
        LuaBindery::AttachMetatable(luaState, "ISimable");
    } else {
        lua_pushnil(luaState);
    }
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "Param");
    lua_pushnumber(luaState, message->fParam);
    lua_settable(luaState, -3);
}

#endif
