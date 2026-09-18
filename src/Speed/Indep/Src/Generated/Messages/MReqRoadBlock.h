#ifndef GENERATED_MESSAGES_MREQROADBLOCK_H
#define GENERATED_MESSAGES_MREQROADBLOCK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MReqRoadBlock : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MReqRoadBlock);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MReqRoadBlock");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MReqRoadBlock &message);

    MReqRoadBlock(int _Data) : Hermes::Message(_GetKind(), _GetSize(), 0), fData(_Data) {}

    ~MReqRoadBlock() {}

    int GetData() const {
        return fData;
    }

    void SetData(int _Data) {
        fData = _Data;
    }

  private:
    int fData; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MReqRoadBlock::HandleMessage_LuaBinding(const MReqRoadBlock &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MReqRoadBlock::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MReqRoadBlock *message = static_cast<const MReqRoadBlock *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "Data");
    lua_pushnumber(luaState, message->fData);
    lua_settable(luaState, -3);
}

#endif
