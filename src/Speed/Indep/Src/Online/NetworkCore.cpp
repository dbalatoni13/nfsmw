#include "NetworkCore.hpp"
#include "LobbyCore.hpp"

#include <string.h>

#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"

void *operator new(size_t size, const char *file, int line, int allocationParams);

extern "C" {
char *SocketInAddrGetText(uint32 address);
int32 DirtyAddrFromHostAddr(DirtyAddrT *dirtyAddress, const void *hostAddress);
int NetGameLinkControl(NetGameLinkRefT *link, int control, int value);
int NetGameLinkRecv(NetGameLinkRefT *link, NetGamePacketT *packet, int length);
int NetGameLinkSend(NetGameLinkRefT *link, NetGamePacketT *packet, int length);
}

void TIMER_addtask(void (*timerRoutine)());
void TIMER_removetask(void (*timerRoutine)());
void LobbyDisconnect();

extern bool NetworkUseLobbies;

// TODO: Replace this with the original owner header when VoiceCore is reached in zOnline order.
class VoiceCore {
  public:
    void Update();
    static VoiceCore *mInstance;
};

uint64 networkTime;

static OnlineMutex networkMutex("networkMutex");

uint32 NetworkCore::myIPAddress;
int NetworkCore::msecPerThreadInterval = 33;
NetworkCore NetworkCore::mInstance;

NetworkCore::NetworkCore()
    : networkIsInitialized(false) {}

NetworkCore::~NetworkCore() {}

bool NetworkCore::Init() {
    uint32 tick;

    if (networkIsInitialized) {
        return true;
    }
    if (!PlatformNetworkCore::Init()) {
        return false;
    }

    LobbyCore::Instance();
    tick = bGetTicker();
    networkTime = static_cast<uint64>(tick) << 32;
    TIMER_addtask(NetworkTimerFunc);
    networkIsInitialized = true;
    return true;
}

int NetworkCore::StaticInit() { return mInstance.Init() ? 0 : -1; }

void NetworkCore::Destroy() {
    if (networkIsInitialized) {
        TIMER_removetask(NetworkTimerFunc);
        TheOnlineManager.Disconnect(true);
        if (NetworkUseLobbies) {
            LobbyDisconnect();
        }
        NetConnIdle();
        while (!procFuncList.IsEmpty()) {
            delete procFuncList.RemoveHead();
        }
        PlatformNetworkCore::Destroy();
    }
    networkIsInitialized = false;
}

int NetworkCore::Send(NetGameLinkRefT *pLinkRef, char *data, int numBytes, bool isReliable) {
    NetGamePacketT sGamePacketOutbound;
    sGamePacketOutbound.head.kind = isReliable ? 5 : 6;
    memcpy(sGamePacketOutbound.body.data, data, numBytes);
    sGamePacketOutbound.head.len = numBytes;

    networkMutex.Lock(nullptr);
    int rc = NetGameLinkSend(pLinkRef, &sGamePacketOutbound, 1);
    networkMutex.Unlock(nullptr);
    return rc;
}

bool NetworkCore::Recv(NetGameLinkRefT *pLinkRef, NetGamePacketT &sGamePacketInbound, bool &reliable) {
    if (pLinkRef) {
        networkMutex.Lock(nullptr);
        int rc = NetGameLinkRecv(pLinkRef, &sGamePacketInbound, 1);
        networkMutex.Unlock(nullptr);
        if (rc) {
            reliable = sGamePacketInbound.head.kind == 5;
            return true;
        }
    }
    return false;
}

void NetworkCore::Flush(NetGameLinkRefT *const *pLinkRef, int numEntries) {
    float flush_duration_s;
    int iteration = 0;
    bool finished;

    if (!pLinkRef) {
        return;
    }

    networkMutex.Lock(nullptr);
    uint32 start_ticks = bGetTicker();
    for (int i = 0; i < numEntries; i++) {
        NetGameLinkControl(pLinkRef[i], 'send', 0);
    }

    do {
        finished = true;
        RpcDone();
        for (int i = 0; i < numEntries; i++) {
            uint32 ret = NetGameLinkControl(pLinkRef[i], 'sque', 0);
            if (!ret) {
                iteration++;
                flush_duration_s = bGetTickerDifference(start_ticks, bGetTicker()) * 0.001f;
                finished = false;
                if (iteration > 1000000 || flush_duration_s > 1.0f) {
                    finished = true;
                }
                break;
            }
        }
    } while (!finished);

    networkMutex.Unlock(nullptr);
}

uint32 NetworkCore::MyIPAddress() { return myIPAddress; }

char *NetworkCore::MyIPAddressText() { return SocketInAddrGetText(MyIPAddress()); }

DirtyAddrT &NetworkCore::MyDirtyAddr() {
    static DirtyAddrT dirtyAddr;
    DirtyAddrFromHostAddr(&dirtyAddr, &myIPAddress);
    return dirtyAddr;
}

uint8 NetworkCore::GetNetworkVersionNumber() { return 20; }

int NetworkCore::RegisterProcessingFunc(ProcFunc func, void *context) {
    if (!func) {
        return -1;
    }

    networkMutex.Lock(nullptr);
    ProcFuncNode *pfn;
    for (pfn = procFuncList.GetHead(); pfn != procFuncList.EndOfList(); pfn = pfn->GetNext()) {
        if (pfn->func == func && pfn->context == context) {
            networkMutex.Unlock(nullptr);
            return 0;
        }
    }

    pfn = new ("d:/p4_apex1666_d1001856/mw/speed/indep/src/online/NetworkCore.cpp", 0x1cc, 8) ProcFuncNode(func, context);
    if (!pfn) {
        networkMutex.Unlock(nullptr);
        return -1;
    }

    procFuncList.AddTail(pfn);
    networkMutex.Unlock(nullptr);
    return 0;
}

void NetworkCore::UnregisterProcessingFunc(ProcFunc func, void *context) {
    networkMutex.Lock(nullptr);
    for (ProcFuncNode *pfn = procFuncList.GetHead(); pfn != procFuncList.EndOfList(); pfn = pfn->GetNext()) {
        if (pfn->func == func && pfn->context == context) {
            procFuncList.Remove(pfn);
            delete pfn;
            break;
        }
    }
    networkMutex.Unlock(nullptr);
}

uint32 NetworkCore::GetTime() {
    uint32 tick = bGetTicker();
    uint64 net = networkTime;
    return static_cast<uint32>(net) + (tick - static_cast<uint32>(net >> 32)) / 0x48058;
}

void NetworkCore::NetworkTimerFunc() {
    uint32 tick = bGetTicker();
    uint32 diff = tick - static_cast<uint32>(networkTime >> 32);
    uint32 time = static_cast<uint32>(networkTime) + diff / 0x48058;
    networkTime = (static_cast<uint64>(tick - diff % 0x48058) << 32) | time;
}

void NetworkCore::DoNetworkProcessing() {
    NetworkCore &networkCore = mInstance;
    if (!networkCore.networkIsInitialized) {
        return;
    }

    const char *functionName = "NetworkCore::DoNetworkProcessing";
    networkMutex.Lock(functionName);
    NetConnIdle();
    ProcFuncNode *pfn = networkCore.procFuncList.GetHead();
    while (pfn != networkCore.procFuncList.EndOfList()) {
        if (pfn->func(pfn->context) < 0) {
            ProcFuncNode *previous = pfn->GetPrev();
            networkCore.procFuncList.Remove(pfn);
            delete pfn;
            pfn = previous->GetNext();
        } else {
            pfn = pfn->GetNext();
        }
    }
    networkMutex.Unlock(functionName);

    if (networkCore.IsOnline()) {
        static bool firstTime = true;
        if (firstTime) {
            firstTime = false;
            myIPAddress = NetConnStatus('addr', 0, 0);
        }
        TheOnlineManager.Update(true);
        if (NetworkUseLobbies) {
            LobbyCore::Instance().DoProcessing();
        }
        VoiceCore::mInstance->Update();
    }
}
