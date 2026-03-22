#ifndef GAMEPLAY_GACTIVITY_H
#define GAMEPLAY_GACTIVITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GHandler.h"
#include "GState.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

DECLARE_CONTAINER_TYPE(ID_GHandlerVector);
DECLARE_CONTAINER_TYPE(ID_StateToVectors);

typedef UTL::Std::map<GState *, UTL::Std::vector<GHandler *, _type_ID_GHandlerVector>, _type_ID_StateToVectors> StateToHandlers;

struct LuaMessageDeliveryInfo;
struct lua_State;

// total size: 0x48
class GActivity : public GRuntimeInstance {
  public:
    // total size: 0x8
    struct SerializedHeader {
        unsigned int mStateNameHash; // offset 0x0, size 0x4
        unsigned short mFlags;       // offset 0x4, size 0x2
        unsigned short mTableBytes;  // offset 0x6, size 0x2
    };

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    GActivity(const Attrib::Key &activityKey);

    ~GActivity() override;

    GameplayObjType GetType() const override {
        return kGameplayObjType_Activity;
    }

    const char *GetActivityName() {
        return CollectionName();
    }

    bool GetIsRunning() const {
        return mRunning;
    }

    void GatherStatesAndHandlers();

    unsigned int StoreHandlers(GState *state, UTL::Std::vector<GHandler *, _type_ID_GHandlerVector> *handlerVec);

    bool CollectionIsStateForActivity(GState *state);

    bool CollectionIsHandlerForState(GState *state, GHandler *handler);

    void RegisterMessageHandlers(GState *state);

    void UnregisterMessageHandlers();

    void ActivateReferencedTriggers(bool activate, GRuntimeInstance *instance);

    void Run();

    void Suspend();

    void Reset();

    GState *GetStateByName(const char *stateName);

    void EnterStateByName(const char *stateName);

    void EnterState(GState *newState);

    static int ChangeStateFromScript(lua_State *luaState);

    void HandleLocalMessage(UCrc32 messageType);

    void PushActivityVars(lua_State *luaState);

    void ClearActivityVars(lua_State *luaState);

    int BuildActivityTables(lua_State *luaState);

    void HandleMessage(LuaMessageDeliveryInfo *deliveryInfo);

    void SerializeVars(bool abandonLuaTable);

    void DeserializeVars();

  private:
    GState *mCurrentState;            // offset 0x28, size 0x4
    GState *mRegisteredHandlersState; // offset 0x2C, size 0x4
    StateToHandlers mStateHandlers;   // offset 0x30, size 0x10
    bool mRunning;                    // offset 0x40, size 0x1
    bool mVarsInLuaVM;                // offset 0x44, size 0x1
};

#endif
