extern void SNDAEMS_removemodulebank(int handle);
extern void SubscribeEventSys();
extern int SNDmemlargestunused(int *start);
extern void SNDmemlimits(int lower, int upper);
extern Timer WorldTimer;

void EAXAemsManager::ResolvePendingAsyncLoads() {}

void *EAXAemsManager::GetCallbackEventSys() {
    int offset = m_nCallbackEvtSys << 2;
    return *reinterpret_cast<void **>(reinterpret_cast<char *>(m_pEvtSystems_start) + offset);
}

void EAXAemsManager::EvtSysLoadCallback(int param, int error_status) {
    gAEMSMgr.m_nCallbackEvtSys = param;
    SubscribeEventSys();
}

void EAXAemsManager::RemoveAEMSBank() {
    int mbhandle = m_pCurUNLOADSDLP->Handle;
    if (mbhandle != -1) {
        SNDAEMS_removemodulebank(mbhandle);
    }
}

void EAXAemsManager::InitSPUram() {
    int Start = -1;
    int Size = SNDmemlargestunused(&Start);
    m_SPU_UpperAddress = Start + Size;
}

bool EAXAemsManager::AreResourceLoadsPending() {
    if (m_pCurLoadSDLP != nullptr) {
        goto Pending;
    }
    if (m_ItemsPendingAsyncResolve != 0) {
        goto Pending;
    }
    if (m_IsWaitingForFileCB != 0) {
        goto Pending;
    }
    if (m_nCurLoadedBankIndex >= (m_nEndOfList - 1)) {
        return false;
    }

Pending:
    return true;
}

void EAXAemsManager::CompleteAsyncLoad() {
    int pAsyncLoad = reinterpret_cast<int>(gAEMSMgr.m_pAsyncLoadSDLP);
    stSndDataLoadParams *m_pCurrentlyLoading = reinterpret_cast<stSndDataLoadParams *>(pAsyncLoad);
    if (pAsyncLoad == 0) {
        m_pCurrentlyLoading = gAEMSMgr.m_pCurLoadSDLP;
    }
    *reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurrentlyLoading) + 0x38) = 1;
    SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    *reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurrentlyLoading) + 0x64) =
        *reinterpret_cast<int *>(&WorldTimer);
}
