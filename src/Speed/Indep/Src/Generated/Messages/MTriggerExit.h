#ifndef GENERATED_MESSAGES_MTRIGGEREXIT_H
#define GENERATED_MESSAGES_MTRIGGEREXIT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x18
class MTriggerExit : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MTriggerExit);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MTriggerExit");

        return k;
    }

    static void BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase);

    static void HandleMessage_LuaBinding(const MTriggerExit &message);

    MTriggerExit(GCollectionKey _Sender, HSIMABLE _Element) : Hermes::Message(_GetKind(), _GetSize(), 0), fSender(_Sender), fElement(_Element) {}

    ~MTriggerExit() {}

    GCollectionKey GetSender() const {
        return fSender;
    }

    void SetSender(GCollectionKey _Sender) {
        fSender = _Sender;
    }

    HSIMABLE GetElement() const {
        return fElement;
    }

    void SetElement(HSIMABLE _Element) {
        fElement = _Element;
    }

  private:
    GCollectionKey fSender; // offset 0x10, size 0x4
    HSIMABLE fElement;      // offset 0x14, size 0x4
};


#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

inline void MTriggerExit::HandleMessage_LuaBinding(const MTriggerExit &message) {
    LuaMessageDeliveryInfo info(_GetKind(), &message, BuildMessageTable);

    LuaPostOffice::Get().RouteMessage(&info);
}

inline void MTriggerExit::BuildMessageTable(lua_State *luaState, const Hermes::Message *messageBase) {
    const MTriggerExit *message = static_cast<const MTriggerExit *>(messageBase);

    lua_newtable(luaState);

    lua_pushstring(luaState, "Sender");
    GRuntimeInstance *pSender = message->fSender;

    if (pSender != NULL) {
        *static_cast<GRuntimeInstance **>(lua_newuserdata(luaState, sizeof(GRuntimeInstance *))) = pSender;
        LuaBindery::AttachMetatable(luaState, "GRuntimeInstance");
    } else {
        lua_pushnil(luaState);
    }
    lua_settable(luaState, -3);

    lua_pushstring(luaState, "Element");
    if (ISimable::FindInstance(message->fElement) != NULL) {
        *static_cast<HSIMABLE *>(lua_newuserdata(luaState, sizeof(HSIMABLE))) = message->fElement;
        LuaBindery::AttachMetatable(luaState, "ISimable");
    } else {
        lua_pushnil(luaState);
    }
    lua_settable(luaState, -3);
}

#endif
