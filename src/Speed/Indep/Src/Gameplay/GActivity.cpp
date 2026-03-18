#include "GActivity.h"

#include "GManager.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Lua/LuaRuntime.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

template <typename T> struct GObjectIteratorTraits;

template <> struct GObjectIteratorTraits<GState> {
    enum { kType = kGameplayObjType_State };
};

template <> struct GObjectIteratorTraits<GHandler> {
    enum { kType = kGameplayObjType_Handler };
};

template <typename T> class GObjectIterator {
  public:
    GObjectIterator(unsigned int)
        : mInstance(static_cast<T *>(GRuntimeInstance::sRingListHead[GObjectIteratorTraits<T>::kType])) //
        , mHead(mInstance) {}

    bool IsValid() const {
        return mInstance != nullptr;
    }

    T *GetInstance() const {
        return mInstance;
    }

    void Advance() {
        if (!mInstance) {
            return;
        }

        GRuntimeInstance *next = mInstance->GetNextRuntimeInstance();
        mInstance = next == mHead ? nullptr : static_cast<T *>(next);
    }

  private:
    T *mInstance;
    GRuntimeInstance *mHead;
};

template <>
GObjectIterator<GHandler>::GObjectIterator(unsigned int)
    : mInstance(static_cast<GHandler *>(GRuntimeInstance::sRingListHead[GObjectIteratorTraits<GHandler>::kType])) //
    , mHead(mInstance) {}

template <>
void GObjectIterator<GHandler>::Advance() {
    if (!mInstance) {
        return;
    }

    GRuntimeInstance *next = mInstance->GetNextRuntimeInstance();
    mInstance = next == mHead ? nullptr : static_cast<GHandler *>(next);
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
