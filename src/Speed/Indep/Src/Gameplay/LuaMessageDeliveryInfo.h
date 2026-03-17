#ifndef GAMEPLAY_LUAMESSAGEDELIVERYINFO_H
#define GAMEPLAY_LUAMESSAGEDELIVERYINFO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct GActivity;
struct GHandler;
struct lua_State;
struct Message;

struct IMessageFilterContext : public UTL::COM::IUnknown {
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IMessageFilterContext(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IMessageFilterContext() {}

    virtual lua_State *GetLuaState() const = 0;
    virtual GActivity *GetActivity() const = 0;
    virtual GHandler *GetHandler() const = 0;
    virtual const Message *GetMessage() const = 0;
};

// total size: 0x34
struct LuaMessageDeliveryInfo : public UTL::COM::Object, public IMessageFilterContext {
    LuaMessageDeliveryInfo(UCrc32 messageKind, const Message *messageBase,
                           void (*buildTableFunc)(lua_State *, const Message *))
        : UTL::COM::Object(1) //
        , IMessageFilterContext(this) //
        , mMessageKind(messageKind) //
        , mMessageBase(messageBase) //
        , mBuildTableFunc(buildTableFunc) //
        , mLuaTableBuilt(false) //
        , mLuaState(nullptr) //
        , mActivityContext(nullptr) //
        , mHandlerContext(nullptr) //
    {
    }

    ~LuaMessageDeliveryInfo() override {}

    unsigned int GetMessageKind() const { return mMessageKind.GetValue(); }

    void SetLuaState(lua_State *luaState) { mLuaState = luaState; }
    void SetActivityContext(GActivity *activity) { mActivityContext = activity; }
    void SetHandlerContext(GHandler *handler) { mHandlerContext = handler; }

    lua_State *GetLuaState() const override { return mLuaState; }
    GActivity *GetActivity() const override { return mActivityContext; }
    GHandler *GetHandler() const override { return mHandlerContext; }
    const Message *GetMessage() const override { return mMessageBase; }

  private:
    UCrc32 mMessageKind;                                     // offset 0x18, size 0x4
    const Message *mMessageBase;                             // offset 0x1C, size 0x4
    void (*mBuildTableFunc)(lua_State *, const Message *);   // offset 0x20, size 0x4
    bool mLuaTableBuilt;                                     // offset 0x24, size 0x1
    lua_State *mLuaState;                                    // offset 0x28, size 0x4
    GActivity *mActivityContext;                             // offset 0x2C, size 0x4
    GHandler *mHandlerContext;                               // offset 0x30, size 0x4
};

#endif
