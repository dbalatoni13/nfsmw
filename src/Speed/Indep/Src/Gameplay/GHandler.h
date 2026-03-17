#ifndef GAMEPLAY_GHANDLER_H
#define GAMEPLAY_GHANDLER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRuntimeInstance.h"

struct LuaMessageDeliveryInfo;
struct lua_State;

// total size: 0x2C
class GHandler : public GRuntimeInstance {
  public:
    GHandler(const Attrib::Key &handlerKey);

    ~GHandler() override;

    GameplayObjType GetType() const override {
        return kGameplayObjType_Handler;
    }

    bool IsScripted() const {
        return mAttached;
    }

    void Attach(lua_State *luaState);
    void Detach(lua_State *luaState);
    void NotifyBytecodeFlushed();
    bool MessagePassesFilters(LuaMessageDeliveryInfo *deliveryInfo);
    void HandleMessage(LuaMessageDeliveryInfo *deliveryInfo);
    void ExecuteScriptedHandler(LuaMessageDeliveryInfo *deliveryInfo);

  private:
    bool mAttached; // offset 0x28, size 0x1
};

#endif
