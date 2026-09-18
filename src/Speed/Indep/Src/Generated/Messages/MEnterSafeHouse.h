#ifndef GENERATED_MESSAGES_MENTERSAFEHOUSE_H
#define GENERATED_MESSAGES_MENTERSAFEHOUSE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MEnterSafeHouse : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MEnterSafeHouse);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MEnterSafeHouse");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MEnterSafeHouse &message);

    MEnterSafeHouse(const char *_ZoneType) : Hermes::Message(_GetKind(), _GetSize(), 0), fZoneType(_ZoneType) {}

    ~MEnterSafeHouse() {}

    const char *GetZoneType() const {
        return fZoneType;
    }

    void SetZoneType(const char *_ZoneType) {
        fZoneType = _ZoneType;
    }

  private:
    const char *fZoneType; // offset 0x10, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MEnterSafeHouse::HandleMessage_LuaBinding(const MEnterSafeHouse &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MEnterSafeHouse::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MEnterSafeHouse *message = static_cast<const MEnterSafeHouse *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "ZoneType");
    lua_pushstring(luaState, message->fZoneType);
    lua_settable(luaState, -3);
}

#endif
