#include "GActivity.h"

#include "GManager.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

GActivity::GActivity(const Attrib::Key &activityKey)
    : GRuntimeInstance(activityKey, kGameplayObjType_Activity) //
    , mCurrentState(nullptr) //
    , mRegisteredHandlersState(nullptr) //
    , mStateHandlers() //
    , mRunning(false) //
    , mVarsInLuaVM(false) {
    GatherStatesAndHandlers();
}

GActivity::~GActivity() {
    SerializeVars(true);
    UnregisterMessageHandlers();
    mStateHandlers.clear();
}

void GActivity::Run() {
    if (mRunning) {
        return;
    }

    mRunning = true;
    if (mCurrentState) {
        RegisterMessageHandlers(mCurrentState);
    }
    ActivateReferencedTriggers(true, this);
}

void GActivity::Suspend() {
    if (!mRunning) {
        return;
    }

    mRunning = false;
    UnregisterMessageHandlers();
    ActivateReferencedTriggers(false, this);
}

void GActivity::Reset() {
    Suspend();
    mCurrentState = nullptr;
    mRegisteredHandlersState = nullptr;
}

GState *GActivity::GetStateByName(const char *stateName) {
    StateToHandlers::iterator it = mStateHandlers.begin();
    StateToHandlers::iterator end = mStateHandlers.end();

    while (it != end) {
        GState *state = it->first;
        if (state && bStrCmp(state->CollectionName(), stateName) == 0) {
            return state;
        }
        ++it;
    }

    return nullptr;
}

void GActivity::EnterStateByName(const char *stateName) {
    EnterState(GetStateByName(stateName));
}

void GActivity::EnterState(GState *newState) {
    if (mCurrentState == newState) {
        return;
    }

    UnregisterMessageHandlers();
    mCurrentState = newState;
    if (mRunning && mCurrentState) {
        RegisterMessageHandlers(mCurrentState);
    }
}

void GActivity::SerializeVars(bool abandonLuaTable) {
    SerializedHeader header;
    ObjectStateBlockHeader *block;
    const bool *persistent;
    const char *stateName;

    if (!mVarsInLuaVM) {
        return;
    }

    stateName = nullptr;
    if (mCurrentState) {
        stateName = mCurrentState->CollectionName();
    }

    header.mStateNameHash = stateName && *stateName ? Attrib::StringHash32(stateName) : 0;
    header.mFlags = mRunning ? 1 : 0;
    header.mTableBytes = 0;

    persistent = reinterpret_cast<const bool *>(GetAttributePointer(0xE4542E9B, 0) ?
                                                    GetAttributePointer(0xE4542E9B, 0) :
                                                    Attrib::DefaultDataArea(sizeof(bool)));
    block = GManager::Get().AllocObjectStateBlock(GetCollection(), sizeof(header), *persistent);
    if (block) {
        bMemCpy(block, &header, sizeof(header));
    }

    if (abandonLuaTable) {
        mVarsInLuaVM = false;
    }
}
