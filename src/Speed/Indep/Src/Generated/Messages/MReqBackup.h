#ifndef GENERATED_MESSAGES_MREQBACKUP_H
#define GENERATED_MESSAGES_MREQBACKUP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MReqBackup : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MReqBackup);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MReqBackup");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MReqBackup &message);

    MReqBackup(int _BackupType) : Hermes::Message(_GetKind(), _GetSize(), 0), fBackupType(_BackupType) {}

    ~MReqBackup() {}

    int GetBackupType() const {
        return fBackupType;
    }

    void SetBackupType(int _BackupType) {
        fBackupType = _BackupType;
    }

  private:
    int fBackupType; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MReqBackup::HandleMessage_LuaBinding(const MReqBackup &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MReqBackup::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MReqBackup *message = static_cast<const MReqBackup *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "BackupType");
    lua_pushnumber(luaState, message->fBackupType);
    lua_settable(luaState, -3);
}

#endif
