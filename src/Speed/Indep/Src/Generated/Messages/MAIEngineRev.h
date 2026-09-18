#ifndef GENERATED_MESSAGES_MAIENGINEREV_H
#define GENERATED_MESSAGES_MAIENGINEREV_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

class EAX_CarState;

// total size: 0x20
class MAIEngineRev : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MAIEngineRev);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MAIEngineRev");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MAIEngineRev &message);

    MAIEngineRev(unsigned int _hSimable, unsigned int _CarID, EAX_CarState *_CarState, unsigned int _PatterToPlay)
        : Hermes::Message(_GetKind(), _GetSize(), 0), fhSimable(_hSimable), fCarID(_CarID), fCarState(_CarState), fPatterToPlay(_PatterToPlay) {}

    ~MAIEngineRev() {}

    unsigned int GethSimable() const {
        return fhSimable;
    }

    void SethSimable(unsigned int _hSimable) {
        fhSimable = _hSimable;
    }

    unsigned int GetCarID() const {
        return fCarID;
    }

    void SetCarID(unsigned int _CarID) {
        fCarID = _CarID;
    }

    EAX_CarState *GetCarState() const {
        return fCarState;
    }

    void SetCarState(EAX_CarState *_CarState) {
        fCarState = _CarState;
    }

    unsigned int GetPatterToPlay() const {
        return fPatterToPlay;
    }

    void SetPatterToPlay(unsigned int _PatterToPlay) {
        fPatterToPlay = _PatterToPlay;
    }

  private:
    unsigned int fhSimable;     // offset 0x10, size 0x4
    unsigned int fCarID;        // offset 0x14, size 0x4
    EAX_CarState *fCarState;    // offset 0x18, size 0x4
    unsigned int fPatterToPlay; // offset 0x1c, size 0x4
};

#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MAIEngineRev::HandleMessage_LuaBinding(const MAIEngineRev &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MAIEngineRev::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MAIEngineRev *message = static_cast<const MAIEngineRev *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "hSimable");
    lua_pushnumber(luaState, message->fhSimable);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "CarID");
    lua_pushnumber(luaState, message->fCarID);
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "CarState");
    if (message->fCarState != NULL) {
        *static_cast<EAX_CarState **>(lua_newuserdata(luaState, sizeof(EAX_CarState *))) = message->fCarState;
        LuaBindery::AttachMetatable(luaState, "EAX_CarState");
    } else {
        lua_pushnil(luaState);
    }
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "PatterToPlay");
    lua_pushnumber(luaState, message->fPatterToPlay);
    lua_settable(luaState, -3);
}

#endif
