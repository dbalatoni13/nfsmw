#include "GActivity.h"

#include "GManager.h"
#include "GTrigger.h"
#include "LuaMessageDeliveryInfo.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Src/Generated/Events/EChangeState.hpp"
#include "Speed/Indep/Src/Lua/LuaBindery.h"
#include "Speed/Indep/Src/Lua/LuaPostOffice.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Lua/LuaRuntime.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

unsigned short SerializeTable(lua_State *state, unsigned char *buffer, bool allowUserData)
    __asm__("SerializeTable__10LuaRuntimeP9lua_StatePUcb");
void AttachMetatable(lua_State *luaState, const char *name) __asm__("AttachMetatable__10LuaBinderyP9lua_StatePCc");
void RegisterHandler(LuaPostOffice *postOffice, unsigned int messageType, GActivity *activity)
    __asm__("RegisterHandler__13LuaPostOfficeUiP9GActivity");
void UnregisterHandler(LuaPostOffice *postOffice, unsigned int messageType, GActivity *activity)
    __asm__("UnregisterHandler__13LuaPostOfficeUiP9GActivity");
void BeginDelivery(LuaRuntime *runtime) __asm__("BeginDelivery__10LuaRuntime");
void EndDelivery(LuaRuntime *runtime) __asm__("EndDelivery__10LuaRuntime");
extern LuaPostOffice *fObj__13LuaPostOffice __asm__("fObj__13LuaPostOffice");

template <typename T> struct GObjectIteratorTraits;

template <> struct GObjectIteratorTraits<GState> {
    enum { kType = kGameplayObjType_State };
};

template <> struct GObjectIteratorTraits<GHandler> {
    enum { kType = kGameplayObjType_Handler };
};

template <> struct GObjectIteratorTraits<GActivity> {
    enum { kType = kGameplayObjType_Activity };
};

template <typename T> class GObjectIterator {
  public:
    GObjectIterator(unsigned int flagMask);

    bool IsValid() const {
        return mInstance != nullptr;
    }

    T *GetInstance() const {
        return mInstance;
    }

    void Advance();

  private:
    T *mInstance;
    unsigned int mFlagMask;
};

template <typename T>
GObjectIterator<T>::GObjectIterator(unsigned int flagMask)
    : mInstance(nullptr), //
      mFlagMask(flagMask) {
    mInstance = static_cast<T *>(GRuntimeInstance::sRingListHead[GObjectIteratorTraits<T>::kType]);
    if (mInstance && (mInstance->GetFlag(mFlagMask) == 0)) {
        Advance();
    }
}

template <typename T>
void GObjectIterator<T>::Advance() {
    if (!mInstance) {
        return;
    }

    do {
        mInstance = static_cast<T *>(mInstance->GetNextRuntimeInstance());
        if (mInstance == GRuntimeInstance::sRingListHead[GObjectIteratorTraits<T>::kType]) {
            mInstance = nullptr;
            return;
        }
    } while (mInstance->GetFlag(mFlagMask) == 0);
}

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

LuaMessageDeliveryInfo::~LuaMessageDeliveryInfo() {}

void LuaMessageDeliveryInfo::BuildMessageTable() {
    if (!mLuaTableBuilt && mBuildTableFunc) {
        mBuildTableFunc(mLuaState, mMessageBase);
        mLuaTableBuilt = true;
    }
}

void GActivity::GatherStatesAndHandlers() {
    if (!mStateHandlers.empty()) {
        mStateHandlers.clear();
    }

    GObjectIterator<GState> iter(0xFFFFFFFF);
    while (iter.IsValid()) {
        GState *state = iter.GetInstance();

        if (CollectionIsStateForActivity(state)) {
            mStateHandlers[state].clear();
        }

        iter.Advance();
    }

    for (StateToHandlers::iterator iterState = mStateHandlers.begin(); iterState != mStateHandlers.end(); ++iterState) {
        GState *state = iterState->first;
        UTL::Std::vector<GHandler *, _type_ID_GHandlerVector> &handlerVec = iterState->second;
        unsigned int handlerCount = StoreHandlers(state, nullptr);

        handlerVec.reserve(handlerCount);
        StoreHandlers(state, &handlerVec);
    }
}

unsigned int GActivity::StoreHandlers(GState *state, UTL::Std::vector<GHandler *, _type_ID_GHandlerVector> *handlerVec) {
    unsigned int count = 0;
    GObjectIterator<GHandler> iter(0xFFFFFFFF);

    while (iter.IsValid()) {
        GHandler *handler = iter.GetInstance();

        if (CollectionIsHandlerForState(state, handler)) {
            if (handlerVec) {
                handlerVec->push_back(handler);
            }

            count++;
        }

        iter.Advance();
    }

    return count;
}

bool GActivity::CollectionIsStateForActivity(GState *state) {
    unsigned int parentKey = GetParent();

    while (parentKey) {
        Attrib::Gen::gameplay parent(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), parentKey), 0, nullptr);
        const unsigned int *activityKey = reinterpret_cast<const unsigned int *>(state->GetAttributePointer(0xA0697302, 0));

        if (!activityKey) {
            activityKey = reinterpret_cast<const unsigned int *>(Attrib::DefaultDataArea(sizeof(unsigned int)));
        }

        if (*activityKey == parent.GetCollection()) {
            return true;
        }

        parentKey = parent.GetParent();
    }

    return false;
}

void GActivity::RegisterMessageHandlers(GState *state) {
    if (mRegisteredHandlersState != state) {
        if (mRegisteredHandlersState) {
            UnregisterMessageHandlers();
        }

        StateToHandlers::iterator iter = mStateHandlers.find(state);
        for (UTL::Std::vector<GHandler *, _type_ID_GHandlerVector>::iterator handler = iter->second.begin();
             handler != iter->second.end(); ++handler) {
            RegisterHandler(fObj__13LuaPostOffice, (*handler)->GetCollection(), this);
        }

        mRegisteredHandlersState = state;
    }
}

void GActivity::UnregisterMessageHandlers() {
    if (mRegisteredHandlersState) {
        StateToHandlers::iterator iter = mStateHandlers.find(mRegisteredHandlersState);
        for (UTL::Std::vector<GHandler *, _type_ID_GHandlerVector>::iterator handler = iter->second.begin();
             handler != iter->second.end(); ++handler) {
            UnregisterHandler(fObj__13LuaPostOffice, (*handler)->GetCollection(), this);
        }

        mRegisteredHandlersState = nullptr;
    }
}

void GActivity::ActivateReferencedTriggers(bool activate, GRuntimeInstance *instance) {
    unsigned int i = 0;

    while (true) {
        if (instance->GetConnectionCount() <= i) {
            break;
        }
        GTrigger *trigger = GRuntimeInstance::FindObject<GTrigger>(instance->GetConnectionAt(i)->GetCollection());

        if (trigger) {
            if (activate) {
                trigger->AddActivationReference();
            } else {
                trigger->RemoveActivationReference();
            }
        }
        i++;
    }

    i = 0;
    while (true) {
        unsigned int childCount;

        {
            Attrib::Attribute children = instance->Get(0x916E0E78);

            childCount = children.GetLength();
        }

        if (childCount <= i) {
            break;
        }

        const unsigned int *childKey = reinterpret_cast<const unsigned int *>(instance->GetAttributePointer(0x916E0E78, i));

        if (!childKey) {
            childKey = reinterpret_cast<const unsigned int *>(Attrib::DefaultDataArea(sizeof(unsigned int)));
        }

        GRuntimeInstance *child = GManager::Get().FindInstance(*childKey);
        if (child) {
            ActivateReferencedTriggers(activate, child);
        }
        i++;
    }
}

bool GActivity::CollectionIsHandlerForState(GState *state, GHandler *handler) {
    const Attrib::Key *handlerState = reinterpret_cast<const Attrib::Key *>(handler->GetAttributePointer(0x918c796e, 0));

    if (!handlerState) {
        handlerState = reinterpret_cast<const Attrib::Key *>(Attrib::DefaultDataArea(sizeof(Attrib::Key)));
    }

    if (*handlerState == state->GetCollection()) {
        const Attrib::Key *handlerActivity = reinterpret_cast<const Attrib::Key *>(handler->GetAttributePointer(0x857fe432, 0));

        if (!handlerActivity) {
            handlerActivity = reinterpret_cast<const Attrib::Key *>(Attrib::DefaultDataArea(sizeof(Attrib::Key)));
        }

        if (*handlerActivity == GetCollection()) {
            return true;
        } else {
            Attrib::Instance activity(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), *handlerActivity), 0, nullptr);
            const Attrib::Gen::gameplay::_LayoutStruct *layout =
                reinterpret_cast<const Attrib::Gen::gameplay::_LayoutStruct *>(activity.GetLayoutPointer());

            if (!layout) {
                layout = reinterpret_cast<const Attrib::Gen::gameplay::_LayoutStruct *>(
                    Attrib::DefaultDataArea(sizeof(Attrib::Gen::gameplay::_LayoutStruct)));
            }

            if (bStrCmp(layout->CollectionName, CollectionName()) == 0) {
                return true;
            } else {
                Attrib::Key collection = GetCollection();

                if (collection) {
                    do {
                        if (collection == *handlerActivity) {
                            return true;
                        }

                        Attrib::Instance parent(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), collection), 0, nullptr);
                        const Attrib::Gen::gameplay::_LayoutStruct *parentLayout =
                            reinterpret_cast<const Attrib::Gen::gameplay::_LayoutStruct *>(parent.GetLayoutPointer());

                        if (!parentLayout) {
                            parentLayout = reinterpret_cast<const Attrib::Gen::gameplay::_LayoutStruct *>(
                                Attrib::DefaultDataArea(sizeof(Attrib::Gen::gameplay::_LayoutStruct)));
                        }

                        collection = parent.GetParent();
                    } while (collection != 0);
                }
            }
        }
    }

    return false;
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

int GActivity::ChangeStateFromScript(lua_State *luaState) {
    GActivity *activity = reinterpret_cast<GActivity *>(lua_touserdata(luaState, -10002));
    GState *state = activity->GetStateByName(lua_tostring(luaState, 1));
    new EChangeState(activity->GetCollection(), state->GetCollection());
    return 0;
}

void GActivity::HandleLocalMessage(UCrc32 messageType) {
    LuaMessageDeliveryInfo deliveryInfo(messageType, nullptr, nullptr);

    BeginDelivery(&LuaRuntime::Get());
    deliveryInfo.SetLuaState(LuaRuntime::Get().GetState());
    HandleMessage(&deliveryInfo);
    if (deliveryInfo.GetLuaState()) {
        lua_settop(deliveryInfo.GetLuaState(), -2);
    }
    EndDelivery(&LuaRuntime::Get());
}

void GActivity::PushActivityVars(lua_State *luaState) {
    if (!mVarsInLuaVM) {
        DeserializeVars();
    }

    const char *activityName = *reinterpret_cast<const char *const *>(GetLayoutPointer());

    lua_pushstring(luaState, activityName);
    lua_gettable(luaState, LUA_REGISTRYINDEX);
    if (lua_type(luaState, -1) == LUA_TNIL) {
        lua_settop(luaState, -2);
        lua_newtable(luaState);
        lua_pushstring(luaState, activityName);
        lua_pushvalue(luaState, -2);
        lua_settable(luaState, LUA_REGISTRYINDEX);
    }

    mVarsInLuaVM = true;
}

int GActivity::BuildActivityTables(lua_State *luaState) {
    int top = lua_gettop(luaState);
    GActivity **userdata = reinterpret_cast<GActivity **>(lua_newuserdata(luaState, sizeof(GActivity *)));

    *userdata = this;
    AttachMetatable(luaState, "GRuntimeInstance");
    PushActivityVars(luaState);
    lua_pushstring(luaState, "ChangeState");
    lua_pushlightuserdata(luaState, this);
    lua_pushcclosure(luaState, ChangeStateFromScript, 1);
    lua_settable(luaState, LUA_GLOBALSINDEX);
    return top;
}

void GActivity::HandleMessage(LuaMessageDeliveryInfo *deliveryInfo) {
    bool builtActivityTables = false;
    int top = 0;
    StateToHandlers::iterator iter = mStateHandlers.find(mCurrentState);

    for (UTL::Std::vector<GHandler *, _type_ID_GHandlerVector>::iterator handler = iter->second.begin();
         handler != iter->second.end(); ++handler) {
        if ((*handler)->GetCollection() == deliveryInfo->GetMessageKind()) {
            deliveryInfo->SetActivityContext(this);
            deliveryInfo->SetHandlerContext(*handler);
            if ((*handler)->MessagePassesFilters(deliveryInfo)) {
                deliveryInfo->BuildMessageTable();
                if (!builtActivityTables) {
                    top = BuildActivityTables(deliveryInfo->GetLuaState());
                    builtActivityTables = true;
                }
                (*handler)->HandleMessage(deliveryInfo);
            }
        }
    }

    if (builtActivityTables) {
        lua_settop(deliveryInfo->GetLuaState(), top);
    }
}

template GObjectIterator<GState>::GObjectIterator(unsigned int flagMask);
template void GObjectIterator<GState>::Advance();
template GObjectIterator<GHandler>::GObjectIterator(unsigned int flagMask);
template void GObjectIterator<GHandler>::Advance();
template GObjectIterator<GActivity>::GObjectIterator(unsigned int flagMask);
template void GObjectIterator<GActivity>::Advance();

void GActivity::SerializeVars(bool abandonLuaTable) {
    SerializedHeader header;
    ObjectStateBlockHeader *block;
    const char *stateName;
    bool isDoneState;

    if (!mVarsInLuaVM) {
        return;
    }

    stateName = "";
    if (mCurrentState) {
        const char *const *stateNamePtr = reinterpret_cast<const char *const *>(mCurrentState->GetAttributePointer(0x3E225EC1, 0));

        if (!stateNamePtr) {
            stateNamePtr = reinterpret_cast<const char *const *>(Attrib::DefaultDataArea(sizeof(const char *)));
        }

        stateName = *stateNamePtr;
    }

    isDoneState = false;
    if (mCurrentState) {
        const char *const *stateNamePtr = reinterpret_cast<const char *const *>(mCurrentState->GetAttributePointer(0x3E225EC1, 0));

        if (!stateNamePtr) {
            stateNamePtr = reinterpret_cast<const char *const *>(Attrib::DefaultDataArea(sizeof(const char *)));
        }

        if (bStrCmp(*stateNamePtr, "done") == 0) {
            isDoneState = true;
        }
    }

    header.mStateNameHash = *stateName ? stringhash32(stateName) : 0;
    header.mFlags = static_cast<unsigned short>(mRunning != 0);
    header.mTableBytes = 0;

    if (isDoneState) {
        header.mFlags = header.mFlags | 2;
    }

    lua_State *luaState = LuaRuntime::Get().GetState();
    int top = lua_gettop(luaState);

    if (!isDoneState) {
        lua_pushstring(luaState, *reinterpret_cast<const char *const *>(GetLayoutPointer()));
        lua_gettable(luaState, LUA_REGISTRYINDEX);
        if (lua_type(luaState, -1) == LUA_TTABLE) {
            const bool *persistent = reinterpret_cast<const bool *>(GetAttributePointer(0xE4542E9B, 0));

            if (!persistent) {
                persistent = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
            }

            header.mTableBytes = SerializeTable(luaState, nullptr, !*persistent);
        }
    }

    block = GManager::Get().AllocObjectStateBlock(
        GetCollection(), header.mTableBytes + sizeof(header),
        *reinterpret_cast<const bool *>(GetAttributePointer(0xE4542E9B, 0) ? GetAttributePointer(0xE4542E9B, 0)
                                                                            : Attrib::DefaultDataArea(sizeof(bool))));

    if (block) {
        bMemCpy(block, &header, sizeof(header));
        if (header.mTableBytes != 0) {
            const bool *persistent = reinterpret_cast<const bool *>(GetAttributePointer(0xE4542E9B, 0));

            if (!persistent) {
                persistent = reinterpret_cast<const bool *>(Attrib::DefaultDataArea(sizeof(bool)));
            }

            SerializeTable(luaState, reinterpret_cast<unsigned char *>(block) + sizeof(header), !*persistent);
        }
    }

    lua_settop(luaState, top);

    if (abandonLuaTable) {
        ClearActivityVars(luaState);
    }
}

void GActivity::DeserializeVars() {
    unsigned char *buffer = reinterpret_cast<unsigned char *>(GManager::Get().GetObjectStateBlock(GetCollection()));

    if (buffer) {
        bool handlerListWasEmpty = mStateHandlers.empty();
        SerializedHeader header;
        const char *stateName;

        if (handlerListWasEmpty) {
            GatherStatesAndHandlers();
        }

        bMemCpy(&header, buffer, sizeof(header));
        if (header.mStateNameHash != 0) {
            for (StateToHandlers::iterator iterState = mStateHandlers.begin(); iterState != mStateHandlers.end(); ++iterState) {
                GState *state = iterState->first;
                const char *const *stateNamePtr =
                    reinterpret_cast<const char *const *>(state->GetAttributePointer(0x3E225EC1, 0));

                if (!stateNamePtr) {
                    stateNamePtr = reinterpret_cast<const char *const *>(Attrib::DefaultDataArea(sizeof(const char *)));
                }

                stateName = *stateNamePtr;
                if (header.mStateNameHash == stringhash32(stateName)) {
                    if (!state->GetAttributePointer(0x3E225EC1, 0)) {
                        Attrib::DefaultDataArea(sizeof(const char *));
                    }
                    mCurrentState = state;
                    break;
                }
            }
        }

        if (header.mTableBytes != 0) {
            lua_State *luaState = LuaRuntime::Get().GetState();
            unsigned int bytesLoaded = LuaRuntime::DeserializeTable(luaState, buffer + sizeof(header), !Persistent(0));

            if (bytesLoaded != 0) {
                lua_pushstring(luaState, *reinterpret_cast<const char *const *>(GetLayoutPointer()));
                lua_pushvalue(luaState, -2);
                lua_settable(luaState, LUA_REGISTRYINDEX);
                lua_settop(luaState, -2);
                mVarsInLuaVM = true;
            }
        }

        if (handlerListWasEmpty && !mStateHandlers.empty()) {
            mStateHandlers.clear();
        }
    }
}

void GActivity::ClearActivityVars(lua_State *luaState) {
    const char *activityName = *reinterpret_cast<const char *const *>(GetLayoutPointer());

    lua_pushstring(luaState, activityName);
    lua_pushnil(luaState);
    lua_settable(luaState, LUA_REGISTRYINDEX);
    mVarsInLuaVM = false;
}
