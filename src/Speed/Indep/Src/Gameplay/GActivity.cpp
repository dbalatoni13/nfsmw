#include "GActivity.h"

#include "GManager.h"
#include "GMarker.h"
#include "GTrigger.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"
#include "Speed/Indep/Src/Lua/LuaRuntime.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Generated/Messages/MStateEnter.h"
#include "Speed/Indep/Src/Generated/Messages/MStateExit.h"

struct EChangeState : public Event {
    EChangeState(unsigned int pActivityKey, unsigned int pStateKey);
    ~EChangeState() override;
    const char *GetEventName() const override;

    unsigned int fActivityKey; // offset 0x8, size 0x4
    unsigned int fStateKey;    // offset 0xC, size 0x4
};

GActivity::GActivity(const Attrib::Key &activityKey)
    : GRuntimeInstance(activityKey, kGameplayObjType_Activity), mCurrentState(NULL),
      mRegisteredHandlersState(NULL), mStateHandlers(), mRunning(0), mVarsInLuaVM(0) {
    DeserializeVars();
}

GActivity::~GActivity() {
    if (Persistent()) {
        SerializeVars(true);
    }
    UnregisterMessageHandlers();
    mStateHandlers.clear();
}

void GActivity::GatherStatesAndHandlers() {
    mStateHandlers.clear();
    for (GObjectIterator<GState> stateIterator(0xFFFFFFFF); stateIterator.IsValid(); stateIterator.Advance()) {
        GState *state = stateIterator.GetInstance();
        if (CollectionIsStateForActivity(state)) {
            mStateHandlers[state].clear();
        }
    }
    for (StateToHandlers::iterator iterState = mStateHandlers.begin(); iterState != mStateHandlers.end(); ++iterState) {
        GState *state = (*iterState).first;
        UTL::Std::vector<GHandler *, _type_ID_GHandlerVector> &handlerVec = (*iterState).second;
        unsigned int handlerCount = StoreHandlers(state, NULL);
        handlerVec.reserve(handlerCount);
        StoreHandlers(state, &handlerVec);
    }
}

unsigned int GActivity::StoreHandlers(GState *state, UTL::Std::vector<GHandler *, _type_ID_GHandlerVector> *handlerVec) {
    unsigned int count = 0;
    for (GObjectIterator<GHandler> handlerIterator(0xFFFFFFFF); handlerIterator.IsValid(); handlerIterator.Advance()) {
        GHandler *handler = handlerIterator.GetInstance();
        if (CollectionIsHandlerForState(state, handler)) {
            if (handlerVec != NULL) {
                handlerVec->push_back(handler);
            }
            count = count + 1;
        }
    }
    return count;
}

bool GActivity::CollectionIsStateForActivity(GState *state) {
    unsigned int parent = GetParent();
    while (parent != 0) {
        Attrib::Gen::gameplay instance(parent, 0, NULL);
        if (state->templateref() == instance.GetCollection()) {
            return true;
        }
        parent = instance.GetParent();
    }
    return false;
}

bool GActivity::CollectionIsHandlerForState(GState *state, GHandler *handler) {
    if (handler->stateref() != state->GetCollection()) {
        return false;
    }
    const GCollectionKey &ownerKey = handler->handler_owner();
    if (ownerKey == GetCollection()) {
        return true;
    }
    Attrib::Gen::gameplay stateListInstance(ownerKey, 0, NULL);
    if (bStrCmp(stateListInstance.CollectionName(), CollectionName()) == 0) {
        return true;
    }
    unsigned int collection = GetCollection();
    while (collection != 0) {
        if (collection == ownerKey) {
            return true;
        }
        Attrib::Gen::gameplay instance(collection, 0, NULL);
        collection = instance.GetParent();
    }
    return false;
}

void GActivity::RegisterMessageHandlers(GState *state) {
    if (mRegisteredHandlersState == state) {
        return;
    }
    if (mRegisteredHandlersState != NULL) {
        UnregisterMessageHandlers();
    }
    StateToHandlers::const_iterator iterState = mStateHandlers.find(state);
    const UTL::Std::vector<GHandler *, _type_ID_GHandlerVector> &handlerVec = (*iterState).second;
    for (UTL::Std::vector<GHandler *, _type_ID_GHandlerVector>::const_iterator iterHandler = handlerVec.begin(); iterHandler != handlerVec.end(); ++iterHandler) {
        GHandler *handler = *iterHandler;
        LuaPostOffice::Get().RegisterHandler(handler->message_id(), this);
    }
    mRegisteredHandlersState = state;
}

void GActivity::UnregisterMessageHandlers() {
    if (mRegisteredHandlersState == NULL) {
        return;
    }
    StateToHandlers::const_iterator iterState = mStateHandlers.find(mRegisteredHandlersState);
    const UTL::Std::vector<GHandler *, _type_ID_GHandlerVector> &handlerVec = (*iterState).second;
    for (UTL::Std::vector<GHandler *, _type_ID_GHandlerVector>::const_iterator iterHandler = handlerVec.begin(); iterHandler != handlerVec.end(); ++iterHandler) {
        GHandler *handler = *iterHandler;
        LuaPostOffice::Get().UnregisterHandler(handler->message_id(), this);
    }
    mRegisteredHandlersState = NULL;
}

void GActivity::ActivateReferencedTriggers(bool activate, GRuntimeInstance *instance) {
    for (unsigned int i = 0; i < instance->GetConnectionCount(); i++) {
        GTrigger *trigger = GRuntimeInstance::FindObject<GTrigger>(instance->GetConnectionAt(i)->GetCollection());
        if (trigger != NULL) {
            if (activate) {
                trigger->AddActivationReference();
            } else {
                trigger->RemoveActivationReference();
            }
        }
    }
    for (unsigned int j = 0; j < instance->Num_Children(); j++) {
        const GCollectionKey &childSpec = instance->Children(j);
        GRuntimeInstance *child = GManager::Get().FindInstance(childSpec.GetCollectionKey());
        if (child != NULL) {
            ActivateReferencedTriggers(activate, child);
        }
    }
}

void GActivity::Run() {
    if (mRunning == 0) {
        if (mStateHandlers.empty()) {
            GatherStatesAndHandlers();
        }
        mRunning = 1;
        ActivateReferencedTriggers(true, this);
        if (mCurrentState == NULL) {
            GState *initialState = GetStateByName("initial");
            Event *event = new EChangeState(GetCollection(), initialState->GetCollection());
        } else {
            RegisterMessageHandlers(mCurrentState);
        }
    }
}

void GActivity::Suspend() {
    if (mRunning != 0) {
        mRunning = 0;
        ActivateReferencedTriggers(false, this);
        UnregisterMessageHandlers();
        mStateHandlers.clear();
    }
}

void GActivity::Reset() {
    Suspend();
    ClearActivityVars(LuaRuntime::Get().GetState());
    GManager::Get().ClearObjectStateBlock(GetCollection());
    mCurrentState = NULL;
}

GState *GActivity::GetStateByName(const char *stateName) {
    for (StateToHandlers::iterator iterState = mStateHandlers.begin(); iterState != mStateHandlers.end(); ++iterState) {
        GState *state = (*iterState).first;
        if (bStrCmp(state->Name(), stateName) == 0) {
            return state;
        }
    }
    return NULL;
}

void GActivity::EnterState(GState *newState) {
    if (mVarsInLuaVM == 0) {
        DeserializeVars();
    }
    if (newState != mCurrentState) {
        if (mCurrentState != NULL) {
            HandleLocalMessage(MStateExit::_GetKind());
        }
        UnregisterMessageHandlers();
        if (mVarsInLuaVM == 0) {
            DeserializeVars();
        }
        mCurrentState = newState;
        if (newState != NULL) {
            RegisterMessageHandlers(newState);
            HandleLocalMessage(MStateEnter::_GetKind());
        }
    }
}

int GActivity::ChangeStateFromScript(lua_State *luaState) {
    GActivity *activity = (GActivity *)lua_touserdata(luaState, lua_upvalueindex(1));
    const char *stateName = lua_tostring(luaState, 1);
    GState *destState = activity->GetStateByName(stateName);
    Event *event = new EChangeState(activity->GetCollection(), destState->GetCollection());
    return 0;
}

void GActivity::HandleLocalMessage(UCrc32 messageType) {
    LuaMessageDeliveryInfo deliveryInfo(messageType, NULL, NULL);
    deliveryInfo.SetLuaState(LuaRuntime::Get().GetState());
    HandleMessage(&deliveryInfo);
}

void GActivity::PushActivityVars(lua_State *luaState) {
    if (mVarsInLuaVM == 0) {
        DeserializeVars();
    }
    const char *activityName = GetActivityName();
    lua_pushstring(luaState, activityName);
    lua_gettable(luaState, LUA_REGISTRYINDEX);
    if (lua_type(luaState, -1) == LUA_TNIL) {
        lua_settop(luaState, -2);
        lua_newtable(luaState);
        lua_pushstring(luaState, activityName);
        lua_pushvalue(luaState, -2);
        lua_settable(luaState, LUA_REGISTRYINDEX);
    }
    mVarsInLuaVM = 1;
}

void GActivity::ClearActivityVars(lua_State *luaState) {
    const char *activityName = GetActivityName();
    lua_pushstring(luaState, activityName);
    lua_pushnil(luaState);
    lua_settable(luaState, LUA_REGISTRYINDEX);
    mVarsInLuaVM = 0;
}

int GActivity::BuildActivityTables(lua_State *luaState) {
    int previousTop = lua_gettop(luaState);
    GRuntimeInstance **activityPtr = (GRuntimeInstance **)lua_newuserdata(luaState, 4);
    *activityPtr = this;
    LuaBindery::AttachMetatable(luaState, "GRuntimeInstance");
    PushActivityVars(luaState);
    lua_pushstring(luaState, "ChangeState");
    lua_pushlightuserdata(luaState, this);
    lua_pushcclosure(luaState, ChangeStateFromScript, 1);
    lua_settable(luaState, LUA_GLOBALSINDEX);
    return previousTop;
}

void GActivity::HandleMessage(LuaMessageDeliveryInfo *deliveryInfo) {
    lua_State *luaState = deliveryInfo->GetLuaState();
    int luaStackPreviousTop = 0;
    bool activityTablesBuilt = false;
    const UTL::Std::vector<GHandler *, _type_ID_GHandlerVector> &handlerVec =
        (*mStateHandlers.find(mCurrentState)).second;
    for (UTL::Std::vector<GHandler *, _type_ID_GHandlerVector>::const_iterator iterHandler = handlerVec.begin(); iterHandler != handlerVec.end(); ++iterHandler) {
        GHandler *handler = *iterHandler;
        if (handler->message_id() == deliveryInfo->GetMessageKind()) {
            deliveryInfo->SetActivityContext(this);
            deliveryInfo->SetHandlerContext(handler);
            if (handler->MessagePassesFilters(deliveryInfo)) {
                deliveryInfo->BuildMessageTable();
                if (activityTablesBuilt == false) {
                    luaStackPreviousTop = BuildActivityTables(luaState);
                    activityTablesBuilt = true;
                }
                handler->HandleMessage(deliveryInfo);
            }
        }
    }
    if (activityTablesBuilt) {
        lua_settop(luaState, luaStackPreviousTop);
    }
}

void GActivity::SerializeVars(bool abandonLuaTable) {
    if (mVarsInLuaVM == 0) {
        return;
    }
    const char *stateName = "";
    if (mCurrentState != NULL) {
        stateName = mCurrentState->Name();
    }
    unsigned int stateNameLen = bStrLen(stateName);
    bool terminalState = false;
    if (mCurrentState != NULL) {
        if (mCurrentState->IsTerminalState()) {
            terminalState = true;
        }
    }
    SerializedHeader header;
    header.mStateNameHash = (stateNameLen != 0) ? stringhash32(stateName) : 0;
    header.mFlags = 0;
    header.mTableBytes = 0;
    if (mRunning != 0) {
        header.mFlags |= 1;
    }
    if (terminalState) {
        header.mFlags |= 2;
    }
    lua_State *luaState = LuaRuntime::Get().GetState();
    int prevStackTop = lua_gettop(luaState);
    if (terminalState == false) {
        lua_pushstring(luaState, CollectionName());
        lua_gettable(luaState, LUA_REGISTRYINDEX);
        if (lua_type(luaState, -1) == LUA_TTABLE) {
            header.mTableBytes = LuaRuntime::SerializeTable(luaState, NULL, !Persistent());
        }
    }
    unsigned int footprint = header.mTableBytes + 8;
    unsigned char *buffer = GManager::Get().AllocObjectStateBlock(GetCollection(), footprint, Persistent());
    if (buffer != NULL) {
        bMemCpy(buffer, &header, 8);
        buffer += 8;
        if (header.mTableBytes != 0) {
            unsigned int writtenBytes = LuaRuntime::SerializeTable(luaState, buffer, !Persistent());
        }
    }
    lua_settop(luaState, prevStackTop);
    if (abandonLuaTable != 0) {
        ClearActivityVars(luaState);
    }
}

void GActivity::DeserializeVars() {
    unsigned char *buffer = GManager::Get().GetObjectStateBlock(GetCollection());
    if (buffer == NULL) {
        return;
    }
    bool handlerListWasEmpty = false;
    if (mStateHandlers.empty()) {
        handlerListWasEmpty = true;
        GatherStatesAndHandlers();
    }
    SerializedHeader header;
    bMemCpy(&header, buffer, 8);
    buffer += 8;
    if (header.mStateNameHash != 0) {
        const char *stateName;
        for (StateToHandlers::iterator iterState = mStateHandlers.begin(); iterState != mStateHandlers.end(); ++iterState) {
            GState *state = (*iterState).first;
            if (header.mStateNameHash == stringhash32(state->Name())) {
                stateName = state->Name();
                mCurrentState = state;
                break;
            }
        }
    }
    if (header.mTableBytes != 0) {
        lua_State *luaState = LuaRuntime::Get().GetState();
        unsigned int bytesLoaded = LuaRuntime::DeserializeTable(luaState, buffer, !Persistent());
        if (bytesLoaded != 0) {
            lua_pushstring(luaState, CollectionName());
            lua_pushvalue(luaState, -2);
            lua_settable(luaState, LUA_REGISTRYINDEX);
            lua_settop(luaState, -2);
            mVarsInLuaVM = 1;
        }
    }
    if (handlerListWasEmpty) {
        if (mStateHandlers.empty() == false) {
            mStateHandlers.clear();
        }
    }
}
