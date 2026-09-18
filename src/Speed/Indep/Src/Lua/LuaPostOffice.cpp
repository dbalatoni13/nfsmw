#include "Speed/Indep/Src/Lua/LuaPostOffice.h"

#include "Speed/Indep/Src/Main/Event.h"

#include <algorithm>

extern "C" {
#include "Speed/Indep/Src/Lua/source/lauxlib.h"
}

LuaPostOffice *LuaPostOffice::fObj = nullptr;

LuaPostOffice::LuaPostOffice() {
    EventManager::BindMessagesToLua(mBindingHandlers);

    for (unsigned int onHandler = 0; onHandler < mBindingHandlers.size(); onHandler++) {
        Hermes::Handler::SetIDFilter(mBindingHandlers[onHandler], false);
    }
}

LuaPostOffice::~LuaPostOffice() {
    for (unsigned int onHandler = 0; onHandler < mBindingHandlers.size(); onHandler++) {
        Hermes::Handler::Destroy(mBindingHandlers[onHandler]);
    }

    mBindingHandlers.clear();
    mSubscribers.clear();
}

void LuaPostOffice::Init() {
    fObj = new ("LuaPostOffice", 0) LuaPostOffice();
}

void LuaPostOffice::Shutdown() {
    delete fObj;
    fObj = NULL;
}

void LuaPostOffice::RouteMessage(LuaMessageDeliveryInfo *deliveryInfo) {
    UTL::Std::vector<GActivity *, _type_ID_LuaActivityList> &subscribers = mSubscribers[deliveryInfo->GetMessageKind()];

    if (subscribers.size() == 0) {
        return;
    }

    UTL::Std::vector<GActivity *, _type_ID_LuaActivityList> subscriberCopy;

    subscriberCopy.reserve(subscribers.size());

    for (UTL::Std::vector<GActivity *, _type_ID_LuaActivityList>::const_iterator onSubscriber = subscribers.begin(); onSubscriber != subscribers.end(); ++onSubscriber) {
        subscriberCopy.push_back(*onSubscriber);
    }

    deliveryInfo->SetLuaState(LuaRuntime::Get().GetState());
    for (UTL::Std::vector<GActivity *, _type_ID_LuaActivityList>::const_iterator onSubscriber = subscriberCopy.begin(); onSubscriber != subscriberCopy.end(); ++onSubscriber) {
        GActivity *activity = *onSubscriber;

        deliveryInfo->SetActivityContext(activity);
        activity->HandleMessage(deliveryInfo);
    }
}

void LuaPostOffice::RegisterHandler(unsigned int messageID, GActivity *activity) {
    UTL::Std::vector<GActivity *, _type_ID_LuaActivityList> &subscribers = mSubscribers[messageID];

    if (std::find(subscribers.begin(), subscribers.end(), activity) == subscribers.end()) {
        subscribers.push_back(activity);
    }
}

void LuaPostOffice::UnregisterHandler(unsigned int messageID, GActivity *activity) {
    UTL::Std::vector<GActivity *, _type_ID_LuaActivityList> &subscribers = mSubscribers[messageID];

    subscribers.erase(std::remove(subscribers.begin(), subscribers.end(), activity), subscribers.end());
}

void LuaMessageDeliveryInfo::BuildMessageTable() {
    if (!mLuaTableBuilt) {
        if (mBuildTableFunc != NULL) {
            mBuildTableFunc(mLuaState, mMessageBase);
        } else {
            lua_pushnil(mLuaState);
        }

        mLuaTableBuilt = true;
    }
}
