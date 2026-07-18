#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void *OnlineMemoryPool = nullptr;

void AllocOnlineMemoryPool() {
    TheTrackStreamer.MakeSpaceInPool(0x14000, true);
    OnlineMemoryPool = TheTrackStreamer.AllocateUserMemory(0x14000, "Online Buffer", 0);
    bInitMemoryPool(8, OnlineMemoryPool, 0x14000, "Online");
}

void FreeOnlineMemoryPool() {
    if (OnlineMemoryPool) {
        bCloseMemoryPool(8);
        bFree(OnlineMemoryPool);
        OnlineMemoryPool = nullptr;
    }
}

extern "C" {

void *LobbyAlloc(int iSize) {
    static int count;
    return bMalloc(iSize, "LobbyAlloc", count++, 0x40);
}

void LobbyFree(void *pMem) { bFree(pMem); }

void *NetAlloc(int iSize) {
    static int count;
    if (!OnlineMemoryPool) {
        return bMalloc(iSize, "NetAlloc (for Juice)", 0, 0);
    }
    if (iSize > 0x3e800) {
        return bMalloc(iSize, "NetAlloc", count++, 0);
    }
    return bMalloc(iSize, "NetAlloc", count++, 8);
}

void NetFree(void *pMem) { bFree(pMem); }

void *RpcAlloc(int iSize) {
    if (!OnlineMemoryPool) {
        return bMalloc(iSize, "RPC Alloc (for Juice)", 0, 0);
    }
    return bMalloc(iSize, "d:/p4_apex1666_d1001856/mw/speed/indep/src/online/OnlineMemory.cpp", 0x74, 0x48);
}

void RpcFree(void *pMem) { bFree(pMem); }

}
