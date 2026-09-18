#ifndef GENERATED_MESSAGES_MAUDIOREFLECTION_H
#define GENERATED_MESSAGES_MAUDIOREFLECTION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x1c
class MAudioReflection : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MAudioReflection);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MAudioReflection");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MAudioReflection &message);

    MAudioReflection(int _PlayerNum, float _Dist, bool _Covered)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fPlayerNum(_PlayerNum), fDist(_Dist), fCovered(_Covered) {}

    ~MAudioReflection() {}

    int GetPlayerNum() const {
        return fPlayerNum;
    }

    void SetPlayerNum(int _PlayerNum) {
        fPlayerNum = _PlayerNum;
    }

    float GetDist() const {
        return fDist;
    }

    void SetDist(float _Dist) {
        fDist = _Dist;
    }

    bool GetCovered() const {
        return fCovered;
    }

    void SetCovered(bool _Covered) {
        fCovered = _Covered;
    }

  private:
    int fPlayerNum; // offset 0x10, size 0x4
    float fDist;    // offset 0x14, size 0x4
    bool fCovered;  // offset 0x18, size 0x1
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MAudioReflection::HandleMessage_LuaBinding(const MAudioReflection &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MAudioReflection::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MAudioReflection *message = static_cast<const MAudioReflection *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "PlayerNum");
    lua_pushnumber(luaState, message->fPlayerNum);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "Dist");
    lua_pushnumber(luaState, message->fDist);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "Covered");
    lua_pushboolean(luaState, message->fCovered);
    lua_settable(luaState, -3);
}

#endif
