#ifndef GENERATED_MESSAGES_MGAMEPLAYMOMENT_H
#define GENERATED_MESSAGES_MGAMEPLAYMOMENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x48
class MGamePlayMoment : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MGamePlayMoment);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MGamePlayMoment");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MGamePlayMoment &message);

    MGamePlayMoment(UMath::Vector4 _Position, UMath::Vector4 _Vector, UMath::Vector4 _Velocity, unsigned int _hSimable, unsigned int _AttribKey)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fPosition(_Position), fVector(_Vector), fVelocity(_Velocity), fhSimable(_hSimable),
          fAttribKey(_AttribKey) {}

    ~MGamePlayMoment() {}

    UMath::Vector4 GetPosition() const {
        return fPosition;
    }

    void SetPosition(UMath::Vector4 _Position) {
        fPosition = _Position;
    }

    UMath::Vector4 GetVector() const {
        return fVector;
    }

    void SetVector(UMath::Vector4 _Vector) {
        fVector = _Vector;
    }

    UMath::Vector4 GetVelocity() const {
        return fVelocity;
    }

    void SetVelocity(UMath::Vector4 _Velocity) {
        fVelocity = _Velocity;
    }

    unsigned int GethSimable() const {
        return fhSimable;
    }

    void SethSimable(unsigned int _hSimable) {
        fhSimable = _hSimable;
    }

    unsigned int GetAttribKey() const {
        return fAttribKey;
    }

    void SetAttribKey(unsigned int _AttribKey) {
        fAttribKey = _AttribKey;
    }

  private:
    UMath::Vector4 fPosition; // offset 0x10, size 0x10
    UMath::Vector4 fVector;   // offset 0x20, size 0x10
    UMath::Vector4 fVelocity; // offset 0x30, size 0x10
    unsigned int fhSimable;   // offset 0x40, size 0x4
    unsigned int fAttribKey;  // offset 0x44, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MGamePlayMoment::HandleMessage_LuaBinding(const MGamePlayMoment &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MGamePlayMoment::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MGamePlayMoment *message = static_cast<const MGamePlayMoment *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "Position");
    lua_pushnil(luaState);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "Vector");
    lua_pushnil(luaState);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "Velocity");
    lua_pushnil(luaState);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "hSimable");
    lua_pushnumber(luaState, message->fhSimable);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "AttribKey");
    lua_pushnil(luaState);
    lua_settable(luaState, -3);
}

#endif
