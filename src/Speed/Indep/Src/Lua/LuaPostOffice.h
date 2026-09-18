#ifndef LUA_LUAPOSTOFFICE_H
#define LUA_LUAPOSTOFFICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Lua/LuaRuntime.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Misc/Hermes.h"

class GHandler;

DECLARE_CONTAINER_TYPE(ID_LuaActivityList);
DECLARE_CONTAINER_TYPE(ID_LuaMessageSubscriberMap);

// total size: 0x1
struct IMessageFilterContext : public UTL::COM::IUnknown {
    DECL_INTERFACE(IMessageFilterContext)

    virtual lua_State *GetLuaState() const = 0;
    virtual GActivity *GetActivity() const = 0;
    virtual GHandler *GetHandler() const = 0;
    virtual const Hermes::Message *GetMessage() const = 0;
};

// total size: 0x34
struct LuaMessageDeliveryInfo : public UTL::COM::Object, public IMessageFilterContext {
    LuaMessageDeliveryInfo(UCrc32 messageKind, const Hermes::Message *messageBase,
                           void (*buildTableFunc)(lua_State *, const Hermes::Message *))
        : UTL::COM::Object(1), IMessageFilterContext(this), mMessageKind(messageKind),
          mMessageBase(messageBase), mBuildTableFunc(buildTableFunc) {
        mLuaTableBuilt = false;
        mLuaState = NULL;
        mActivityContext = NULL;
        mHandlerContext = NULL;
        LuaRuntime::Get().BeginDelivery();
    }

    ~LuaMessageDeliveryInfo() override {
        if (mLuaTableBuilt) {
            lua_settop(mLuaState, -2);
        }
        LuaRuntime::Get().EndDelivery();
    }

    unsigned int GetMessageKind() const {
        return mMessageKind.GetValue();
    }

    void SetLuaState(lua_State *luaState) {
        mLuaState = luaState;
    }

    void SetActivityContext(GActivity *activity) {
        mActivityContext = activity;
    }

    void SetHandlerContext(GHandler *handler) {
        mHandlerContext = handler;
    }

    lua_State *GetLuaState() const override {
        return mLuaState;
    }

    GActivity *GetActivity() const override {
        return mActivityContext;
    }

    GHandler *GetHandler() const override {
        return mHandlerContext;
    }

    const Hermes::Message *GetMessage() const override {
        return mMessageBase;
    }

    void BuildMessageTable();

    UCrc32 mMessageKind;                                                // offset 0x18, size 0x4
    const Hermes::Message *mMessageBase;                                // offset 0x1C, size 0x4
    void (*mBuildTableFunc)(lua_State *, const Hermes::Message *);      // offset 0x20, size 0x4
    bool mLuaTableBuilt;                                                // offset 0x24, size 0x1
    lua_State *mLuaState;                                               // offset 0x28, size 0x4
    GActivity *mActivityContext;                                        // offset 0x2C, size 0x4
    GHandler *mHandlerContext;                                          // offset 0x30, size 0x4
};

// total size: 0x20
class LuaPostOffice {
  public:
    LuaPostOffice();
    ~LuaPostOffice();

    static void Init();
    static void Shutdown();

    static LuaPostOffice &Get() {
        return *fObj;
    }

    void RouteMessage(LuaMessageDeliveryInfo *deliveryInfo);

    void RegisterHandler(unsigned int port, GActivity *activity);
    void UnregisterHandler(unsigned int port, GActivity *activity);

  private:
    static LuaPostOffice *fObj;

    UTL::Std::map<unsigned int, UTL::Std::vector<GActivity *, _type_ID_LuaActivityList>, _type_ID_LuaMessageSubscriberMap>
        mSubscribers;                                                                          // offset 0x0, size 0x10
    UTL::Std::vector<Hermes::HHANDLER, Hermes::_type_ID_HermesHandlerVector> mBindingHandlers; // offset 0x10, size 0x10
};

#endif
