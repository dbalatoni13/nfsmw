extern void SNDAEMS_removemodulebank(int handle);
extern int SNDAEMS_addmodulebank(void *pmem, char *chunk, int offset, void *(*cb)(void *, int, int));
extern int SNDAEMS_asyncloadmodulebankdone();
extern int SNDAEMS_asyncloadmodulebankmemdone();
extern void SubscribeEventSys();
extern int SNDmemlargestunused(int *start);
extern void SNDmemlimits(int lower, int upper);
extern "C" void *ResidentAllocCB_EAXAemsManager(void *ptr, int reqsize, int maxsize) asm("ResidentAllocCB__14EAXAemsManagerPvii");
extern "C" void BankSlotSystem_DestroySlots(void *slots) asm("DestroySlots__14BankSlotSystem");
extern "C" void SndAssetQueue_clear(void *queue) asm(
    "clear__Q24_STLt10_List_base2Z15stSndAssetQueueZQ33UTL3Stdt9Allocator2Z15stSndAssetQueueZ19_type_SndAssetQueue");
extern stSndDataLoadParams g_SndAssetList[];
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

void EAXAemsManager::ResetBankLoadParams() {
    m_nCurLoadedBankIndex = -1;
    m_nEndOfList = 0;
    SndAssetQueue_clear(reinterpret_cast<char *>(this) + 0xBC);
    DestroySlots(true);
}

void EAXAemsManager::DestroySlots(bool bDoPFSlot) {
    if (bDoPFSlot == true) {
        BankSlotSystem_DestroySlots(reinterpret_cast<char *>(this) + 0xAC);
    }
    BankSlotSystem_DestroySlots(reinterpret_cast<char *>(this) + 0xA4);
    m_SPUMainAllocsEnd = m_SPU_UpperAddress;
    SNDmemlimits(-1, m_SPU_UpperAddress);
    bMemSet(EAXAemsManager::m_RequiredSlots, 0, 0x10);
}

int EAXAemsManager::IsAssetInList(Attrib::StringKey filename) {
    int n = 0;
    int nEndOfList = m_nEndOfList;
    if (n < nEndOfList) {
        do {
            bool bAssetMatch = false;
            if (*reinterpret_cast<unsigned int *>(&filename) ==
                *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&g_SndAssetList[n]) + 0x8)) {
                bAssetMatch = *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&filename) + 0x4) ==
                              *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(&g_SndAssetList[n]) + 0xC);
            }
            if (bAssetMatch) {
                return n;
            }
            n++;
        } while (n < nEndOfList);
    }
    return -1;
}

int EAXAemsManager::IsAssetLoaded(Attrib::StringKey filename) {
    int n = IsAssetInList(filename);
    int nLoadedAsset = -1;
    if (n != -1) {
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(&g_SndAssetList[n]) + 0x3C) != 0) {
            nLoadedAsset = n;
        }
    }
    return nLoadedAsset;
}

void EAXAemsManager::UnloadSndData(Attrib::StringKey filename) {
    Attrib::StringKey local_18(filename);
    int index = IsAssetInList(local_18);
    if (index != -1) {
        UnloadSndData(index);
    }
}

void EAXAemsManager::AddAemsBank() {
    stSndDataLoadParams *pCurLoad = gAEMSMgr.m_pCurLoadSDLP;
    stBankSlot *pBankSlot = *reinterpret_cast<stBankSlot **>(reinterpret_cast<char *>(pCurLoad) + 0x24);
    if (pBankSlot == nullptr) {
        SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    } else {
        SNDmemlimits(pBankSlot->BANKmemLocation, pBankSlot->BANKmemLocation + pBankSlot->BANKMemSize);
    }

    int nhandle = SNDAEMS_addmodulebank(
        *reinterpret_cast<void **>(reinterpret_cast<char *>(pCurLoad) + 0x28), nullptr, 0, ResidentAllocCB_EAXAemsManager);
    SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    pCurLoad->Handle = nhandle;
}

void EAXAemsManager::CheckForCompleteAsyncLoad() {
    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurLoadSDLP) + 0x38) == 0) {
        int eDataType = *reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurLoadSDLP) + 0x0);
        if (eDataType == 3) {
            int Result = SNDAEMS_asyncloadmodulebankdone();
            if (Result > 0) {
                CompleteAsyncLoad();
            }
        } else if ((eDataType > 3) && (eDataType == 4) && ((SNDAEMS_asyncloadmodulebankmemdone() > 0)) &&
                   (m_IsWaitingForFileCB == 0)) {
            CompleteAsyncLoad();
        }
    }
}
