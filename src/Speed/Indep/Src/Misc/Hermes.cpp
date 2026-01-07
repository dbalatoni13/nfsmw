#include "Hermes.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Generated/Events/EDeliverMessage.hpp"

int TotalNumHermesHandlers = 0;

namespace Hermes {

void PortMessage::RegisterHandler(Handler &handler) {
    TotalNumHermesHandlers++;
    if (mHandlers.size() == mHandlers.capacity()) {
        mHandlers.reserve(mHandlers.capacity() * 2);
    }
    mHandlers.push_back(handler);
}

void PortMessage::UnregisterHandler(HHANDLER key) {
    TotalNumHermesHandlers--;
    for (Handlers::iterator i = mHandlers.begin(); i != mHandlers.end(); i++) {
        if (i->mKey == key) {
            mHandlers.erase(i);
            break;
        }
    }
}

void PortMessage::SetIDFilter(HHANDLER key, bool enabled) {
    for (Handlers::iterator i = mHandlers.begin(); i != mHandlers.end(); i++) {
        Handler &handler = *i;
        if (handler.mKey == key) {
            handler.mNoFilter = !enabled;
            break;
        }
    }
}

void PortMessage::HandleMessage(Message *msg) {
    for (Handlers::iterator i = mHandlers.begin(); i != mHandlers.end(); i++) {
        i->Call(msg);
    }
}

void System::Init() {
    mObj = new ("TODO", __LINE__) System();
}

// UNSOLVED
uint64_t System::CreateKey(UCrc32 port, UCrc32 messageID) {
    uint64_t x = messageID.GetValue();
    uint64_t y = port.GetValue();
    x = (x | x << 16) & 0xffff0000ffffULL;
    y = (y | y << 16) & 0xffff0000ffffULL;
    x = (x | x << 8) & 0xff00ff00ff00ffULL;
    y = (y | y << 8) & 0xff00ff00ff00ffULL;
    x = (x | x << 4) & 0xf0f0f0f0f0f0f0fULL;
    y = (y | y << 4) & 0xf0f0f0f0f0f0f0fULL;
    x = (x | x << 2) & 0x3333333333333333ULL;
    y = (y | y << 2) & 0x3333333333333333ULL;
    x = (x | x << 1) & 0x5555555555555555ULL;
    y = (y | y << 1) & 0x5555555555555555ULL;
    uint64_t result = y | (x << 1);

    return result;
}

void System::AddPortMessage(uint64_t key, PortMessage *pm) {
    mPortMessageMap.Add(key, pm);
}

void System::RemovePortMessage(uint64_t key) {
    mPortMessageMap.Remove(key);
}

PortMessage *System::FindPortMessage(uint64_t key) {
    return mPortMessageMap.Find(key);
}

System::PortKeyMap &System::GetPortKeyMap() {
    return mPortKeyMap;
}

void Message::Post(UCrc32 port) {
    mPort = port;
    new EDeliverMessage(this, port);
}

void Message::Deliver() {
    uint64_t key = System::Get().CreateKey(mPort, mKind);
    PortMessage *pm = System::Get().FindPortMessage(key);
    if (pm) {
        pm->HandleMessage(this);
    }
}

HHANDLER Handler::_AddToPort(UCrc32 port) {
    HHANDLER HHANDLER = mKey;
    if (HHANDLER) {
        uint64_t key = System::Get().CreateKey(port, mKind);
        PortMessage *pm = System::Get().FindPortMessage(key);
        if (!pm) {
            pm = new ("TODO") PortMessage();
            System::Get().AddPortMessage(key, pm);
        }
        PortKey pk;
        pk.key = key;
        pk.pm = pm;

        System::PortKeyMap &theKeyMap = System::Get().GetPortKeyMap();
        theKeyMap[HHANDLER] = pk;
        pm->RegisterHandler(*this);
    }

    return HHANDLER;
}

void Handler::Destroy(HHANDLER key) {
    if (key) {
        System::PortKeyMap &theKeyMap = System::Get().GetPortKeyMap();

        System::PortKeyMap::iterator iter = theKeyMap.find(key);

        if (iter != theKeyMap.end()) {
            PortKey &pk = (*iter).second;
            PortMessage *pm = pk.pm;
            uint64_t key64 = pk.key;

            pm->UnregisterHandler(key);
            if (pm->IsEmpty()) {
                System::Get().RemovePortMessage(key64);
                delete pm;
            }
            theKeyMap.erase(iter);
        }
    }
}

void Handler::SetIDFilter(HHANDLER key, bool enabled) {
    if (key) {
        System::PortKeyMap &theKeyMap = System::Get().GetPortKeyMap();

        System::PortKeyMap::iterator iter = theKeyMap.find(key);

        if (iter != theKeyMap.end()) {
            PortKey &pk = (*iter).second;
            PortMessage *pm = pk.pm;

            pm->SetIDFilter(key, enabled);
        }
    }
}

}; // namespace Hermes
