#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

extern void SNDAEMS_removemodulebank(int handle);
extern int SNDAEMS_addmodulebank(void *pmem, char *chunk, int offset, void *(*cb)(void *, int, int));
extern int SNDAEMS_asyncloadmodulebankdone();
extern int SNDAEMS_asyncloadmodulebankmemdone();
void SubscribeEventSys();
extern char *g_DataPaths[];
extern int DISABLE_SLOT_LOADING;
extern int SNDmemlargestunused(int *start);
extern void SNDmemlimits(int lower, int upper);
extern int bLargestMalloc(int pool);
extern void *bMalloc(int size, const char *name, int line, int pool);
extern int bFileSize(const char *filename);
extern char *bStrCat(char *destString, const char *s1, const char *s2);
extern void bFree(void *ptr);
struct QueuedFileParams;
extern void AddQueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(int, int), int callback_param,
                          QueuedFileParams *params);
extern stSndDataLoadParams g_SndAssetList[];
extern Timer WorldTimer;

namespace Csis {
namespace System {
void Subscribe(void *desc);
}
} // namespace Csis

struct stAssetDescription {
    int eDataType;
    int DataPath;
    Attrib::StringKey FileName;
    bool bLoadToTop;
    char _pad[3];

    void Clear();
};

struct stSndAssetQueue {
    stAssetDescription Asset;
    SndBase *pThis;
    EAX_CarState *pCar;
};

struct BankSlotNode {
    BankSlotNode *next;
    BankSlotNode *prev;
    stBankSlot data;
};

class BankSlotSystem {
  public:
    stBankSlot *GetFreeSlot(eBANK_SLOT_TYPE Type);
    void DestroySlots();

  private:
    BankSlotNode *mNode;
};

namespace {
typedef BankSlotNode ListNodeBankSlot;

struct ListNodeQueue {
    ListNodeQueue *next;
    ListNodeQueue *prev;
    stSndAssetQueue data;
};

void ClearSndAssetQueue(ListNodeQueue *head) {
    if (head == nullptr) {
        return;
    }

    ListNodeQueue *node = head->next;
    while (node != head) {
        ListNodeQueue *next = node->next;
        gFastMem.Free(node, 0x30, nullptr);
        node = next;
    }

    head->next = head;
    head->prev = head;
}

enum eTEMPALLOCLOCATION {
    TMP_ALLOC_INVALID = 0,
    TMP_ALLOC_MAIN = 1,
    TMP_ALLOC_TRACKSTREAMER = 2,
    TMP_ALLOC_AUDIO = 3,
};

enum eSndDataType {
    SDT_INVALID = 0,
    SDT_AEMS_MAINMEM = 1,
    SDT_GENERIC_DATA = 2,
    SDT_AEMS_ASYNCSPU = 3,
};
} // namespace

void stAssetDescription::Clear() {
    eDataType = -1;
    DataPath = 0;
    FileName = Attrib::StringKey("");
    bLoadToTop = false;
    _pad[0] = 0;
    _pad[1] = 0;
    _pad[2] = 0;
}

void stBankSlot::Clear() {
    Type = static_cast<eBANK_SLOT_TYPE>(-1);
    BANKmemLocation = 0;
    MAINmemLocation = nullptr;
    pLastAlloc = nullptr;
    MAINmemSize = 0;
    BANKMemSize = 0;
    LoadFailed = 0;
    Index = 0;
    pAssetParams = nullptr;
}

void stSndDataLoadParams::Clear() {
    char *base = reinterpret_cast<char *>(this);
    reinterpret_cast<stAssetDescription *>(base)->Clear();
    *reinterpret_cast<stBankSlot **>(base + 0x24) = nullptr;
    *reinterpret_cast<void **>(base + 0x28) = nullptr;
    *reinterpret_cast<void **>(base + 0x2C) = nullptr;
    *reinterpret_cast<int *>(base + 0x30) = 0;
    Handle = -1;
    bResolvedAsync = 0;
    bResolvedSync = 0;
    *reinterpret_cast<void **>(base + 0x44) = *reinterpret_cast<void **>(base + 0x40);
    *reinterpret_cast<void **>(base + 0x54) = *reinterpret_cast<void **>(base + 0x50);
    t_req = 0;
    t_load = 0;
}

stBankSlot *BankSlotSystem::GetFreeSlot(eBANK_SLOT_TYPE Type) {
    BankSlotNode *head = mNode;
    if (head == nullptr) {
        return nullptr;
    }
    for (BankSlotNode *node = head->next; node != head; node = node->next) {
        if (node->data.Type == Type && node->data.pAssetParams == nullptr) {
            return &node->data;
        }
    }

    return nullptr;
}

void BankSlotSystem::DestroySlots() {
    BankSlotNode *head = mNode;
    if (head == nullptr) {
        return;
    }
    BankSlotNode *node = head->next;

    while (node != head) {
        BankSlotNode *next = node->next;
        if (node->data.LoadFailed == 0 && node->data.pAssetParams != nullptr) {
            stAssetDescription *asset = reinterpret_cast<stAssetDescription *>(node->data.pAssetParams);
            gAEMSMgr.UnloadSndData(asset->FileName);
        }
        if (node->data.MAINmemLocation != nullptr) {
            AudioMemoryManager_FreeMemory(&gAudioMemoryManager, node->data.MAINmemLocation);
        }
        FastMem_Free(&gFastMem, node, 0x2C, nullptr);
        node = next;
    }

    head->next = head;
    head->prev = head;
}

EAXAemsManager::EAXAemsManager() {
    ListNodeBankSlot *bankSlots = reinterpret_cast<ListNodeBankSlot *>(FastMem_Alloc(&gFastMem, 0x2C, nullptr));
    if (bankSlots != nullptr) {
        bankSlots->next = bankSlots;
        bankSlots->prev = bankSlots;
    }
    *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xA4) = bankSlots;

    ListNodeBankSlot *pfSlots = reinterpret_cast<ListNodeBankSlot *>(FastMem_Alloc(&gFastMem, 0x2C, nullptr));
    if (pfSlots != nullptr) {
        pfSlots->next = pfSlots;
        pfSlots->prev = pfSlots;
    }
    *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xAC) = pfSlots;

    ListNodeBankSlot *waitSlots = reinterpret_cast<ListNodeBankSlot *>(FastMem_Alloc(&gFastMem, 0x30, nullptr));
    if (waitSlots != nullptr) {
        waitSlots->next = waitSlots;
        waitSlots->prev = waitSlots;
    }
    *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xBC) = waitSlots;

    m_pEvtSystems_start = nullptr;
    m_pEvtSystems_end = nullptr;
    m_pEvtSystems_end_of_storage = nullptr;
    *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x108) = 0;
    *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x10C) = -1;
    *reinterpret_cast<void **>(reinterpret_cast<char *>(this) + 0x110) = nullptr;
    m_nCallbackEvtSys = -1;
    m_pCurLoadSDLP = nullptr;
    m_pAsyncLoadSDLP = nullptr;
    m_ItemsPendingAsyncResolve = 0;
    m_IsWaitingForFileCB = 0;
    m_SPUMainAllocsEnd = 0;
    m_SPU_UpperAddress = 0;
}

EAXAemsManager::~EAXAemsManager() {
    int n = 0;
    int numEvtSysLoaded = *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x108);
    while (n < numEvtSysLoaded) {
        AudioMemoryManager_FreeMemory(&gAudioMemoryManager, m_pEvtSystems_start[n]);
        n++;
    }

    if (m_pEvtSystems_start != nullptr) {
        int byteCount = static_cast<int>(reinterpret_cast<char *>(m_pEvtSystems_end_of_storage) -
                                         reinterpret_cast<char *>(m_pEvtSystems_start));
        FastMem_Free(&gFastMem, m_pEvtSystems_start, byteCount, nullptr);
    }
    m_pEvtSystems_start = nullptr;
    m_pEvtSystems_end = nullptr;
    m_pEvtSystems_end_of_storage = nullptr;

    SndAssetQueue_clear(reinterpret_cast<char *>(this) + 0xBC);
    ListNodeBankSlot *waitSlotHead = *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xBC);
    if (waitSlotHead != nullptr) {
        FastMem_Free(&gFastMem, waitSlotHead, 0x30, nullptr);
    }

    BankSlotSystem_DestroySlots(reinterpret_cast<char *>(this) + 0xAC);
    ListNodeBankSlot *pfSlotHead = *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xAC);
    if (pfSlotHead != nullptr) {
        FastMem_Free(&gFastMem, pfSlotHead, 0x2C, nullptr);
    }

    BankSlotSystem_DestroySlots(reinterpret_cast<char *>(this) + 0xA4);
    ListNodeBankSlot *slotHead = *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xA4);
    if (slotHead != nullptr) {
        FastMem_Free(&gFastMem, slotHead, 0x2C, nullptr);
    }
}

void EAXAemsManager::Init() {
    Attrib::Gen::audiosystem *attrs =
        *reinterpret_cast<Attrib::Gen::audiosystem **>(reinterpret_cast<char *>(g_pEAXSound) + 0xA8);
    unsigned int numEvtSys = attrs->Num_EvtSys();
    *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x10C) = static_cast<int>(numEvtSys);
    *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x104) = 0;
    *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x108) = 0;
    if (static_cast<int>(numEvtSys) > 0) {
        if (m_pEvtSystems_start != nullptr) {
            int oldByteCount = static_cast<int>(reinterpret_cast<char *>(m_pEvtSystems_end_of_storage) -
                                                reinterpret_cast<char *>(m_pEvtSystems_start));
            FastMem_Free(&gFastMem, m_pEvtSystems_start, oldByteCount, nullptr);
        }

        m_pEvtSystems_start =
            reinterpret_cast<void **>(FastMem_Alloc(&gFastMem, static_cast<unsigned int>(numEvtSys * sizeof(void *)), nullptr));
        m_pEvtSystems_end = m_pEvtSystems_start;
        m_pEvtSystems_end_of_storage = m_pEvtSystems_start + numEvtSys;
    }

    int n = 0;
    do {
        AddEventSystem(static_cast<eEVTSYS>(n), SNDPATH_EVTSYS);
        n++;
    } while (n < 7);

    ResetBankLoadParams();
}

int EAXAemsManager::AddEventSystem(eEVTSYS eESIndex, eSNDDATAPATH eSDP) {
    char tempPath[256];
    Attrib::Gen::audiosystem *attrs =
        *reinterpret_cast<Attrib::Gen::audiosystem **>(reinterpret_cast<char *>(g_pEAXSound) + 0xA8);
    const Attrib::StringKey &evtStringKey = attrs->EvtSys(eESIndex);
    const char *evtName = *reinterpret_cast<const char *const *>(reinterpret_cast<const char *>(&evtStringKey) + 0xC);
    if (evtName == nullptr) {
        evtName = "";
    }

    bStrCat(tempPath, g_DataPaths[eSDP], evtName);
    int filesize = bFileSize(tempPath);
    m_pEvtSystems_start[eESIndex] = AudioMemoryManager_AllocateMemoryChar(&gAudioMemoryManager, filesize, evtName, false);

    int callback_param = *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x108);
    AddQueuedFile(m_pEvtSystems_start[callback_param], tempPath, 0, filesize, EvtSysLoadCallback, callback_param, nullptr);
    *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x108) = callback_param + 1;
    return callback_param;
}

void EAXAemsManager::InitializeSlots(bool bDoPFSlot) {
    if (DISABLE_SLOT_LOADING == 0) {
        if (bDoPFSlot) {
            BankSlotSystem_DestroySlots(reinterpret_cast<char *>(this) + 0xAC);
        }
        BankSlotSystem_DestroySlots(reinterpret_cast<char *>(this) + 0xA4);
        m_SPUMainAllocsEnd = m_SPU_UpperAddress;
        for (int n = 0; n < 4; n++) {
            RegisterSlots(static_cast<eBANK_SLOT_TYPE>(n), m_RequiredSlots[n], m_SlotSizes[n][0], m_SlotSizes[n][1], bDoPFSlot);
        }
        SNDmemlimits(-1, m_SPUMainAllocsEnd);
    }
}

void EAXAemsManager::RegisterSlots(eBANK_SLOT_TYPE Type, int NumSlots, int SizePerSlotSPU, int SizePerSlotMainMem, bool bDoPFSlot) {
    stBankSlot NewSlot;
    NewSlot.Clear();
    for (int n = 0; n < NumSlots; n++) {
        char DebugString[1];
        DebugString[0] = '\0';
        NewSlot.Index = static_cast<unsigned char>(n);
        NewSlot.Type = Type;
        NewSlot.MAINmemLocation =
            reinterpret_cast<char *>(AudioMemoryManager_AllocateMemory(&gAudioMemoryManager, SizePerSlotMainMem, DebugString, false));
        NewSlot.BANKmemLocation = m_SPUMainAllocsEnd - SizePerSlotSPU;
        m_SPUMainAllocsEnd = NewSlot.BANKmemLocation;
        NewSlot.pAssetParams = nullptr;
        NewSlot.pLastAlloc = NewSlot.MAINmemLocation;
        NewSlot.MAINmemSize = SizePerSlotMainMem;
        NewSlot.BANKMemSize = SizePerSlotSPU;

        ListNodeBankSlot *head = *reinterpret_cast<ListNodeBankSlot **>(
            reinterpret_cast<char *>(this) + ((Type == eBANK_SLOT_PATHFINDER && bDoPFSlot) ? 0xAC : 0xA4));
        ListNodeBankSlot *node = reinterpret_cast<ListNodeBankSlot *>(FastMem_Alloc(&gFastMem, 0x2C, nullptr));
        if (node != nullptr && head != nullptr) {
            node->data = NewSlot;
            node->next = head;
            node->prev = head->prev;
            head->prev->next = node;
            head->prev = node;
        }
    }
    NewSlot.Clear();
}

void EAXAemsManager::ResolveCurrentDataMemory() {
    if (gAEMSMgr.m_pCurLoadSDLP == nullptr) {
        return;
    }

    char *curLoad = reinterpret_cast<char *>(gAEMSMgr.m_pCurLoadSDLP);
    eTEMPALLOCLOCATION MemLocation = static_cast<eTEMPALLOCLOCATION>(*reinterpret_cast<int *>(curLoad + 0x30));
    void *pmem = *reinterpret_cast<void **>(curLoad + 0x28);

    if (MemLocation == TMP_ALLOC_MAIN) {
        bFree(pmem);
    } else {
        if (MemLocation == TMP_ALLOC_TRACKSTREAMER) {
            TrackStreamer_FreeUserMemory(&TheTrackStreamer, pmem);
        } else if (MemLocation == TMP_ALLOC_AUDIO) {
            void *plocmem = *reinterpret_cast<void **>(curLoad + 0x2C);
            stBankSlot *mBankSlot = *reinterpret_cast<stBankSlot **>(curLoad + 0x24);
            if (plocmem == nullptr && mBankSlot == nullptr) {
                return;
            }
            AudioMemoryManager_FreeMemory(&gAudioMemoryManager, pmem);
        } else {
            return;
        }
    }

    *reinterpret_cast<void **>(curLoad + 0x28) = nullptr;
}

int EAXAemsManager::InitiateLoad() {
    if (m_pCurLoadSDLP == nullptr) {
        return -1;
    }

    char *curLoad = reinterpret_cast<char *>(m_pCurLoadSDLP);
    char tempPath[256];
    char *filename = *reinterpret_cast<char **>(curLoad + 0x14);
    if (filename == nullptr) {
        filename = const_cast<char *>("");
    }

    int dataPath = *reinterpret_cast<int *>(curLoad + 0x4);
    bStrCat(tempPath, g_DataPaths[dataPath], filename);
    int fileSize = bFileSize(tempPath);
    *reinterpret_cast<int *>(curLoad + 0x20) = fileSize;
    if (fileSize < 1) {
        return -1;
    }

    int eDataType = *reinterpret_cast<int *>(curLoad + 0x0);
    if (eDataType < SDT_GENERIC_DATA) {
        *reinterpret_cast<int *>(curLoad + 0x30) = TMP_ALLOC_AUDIO;
        *reinterpret_cast<int *>(curLoad + 0x0) = SDT_AEMS_ASYNCSPU;
    } else {
        *reinterpret_cast<int *>(curLoad + 0x30) = TMP_ALLOC_MAIN;
    }

    int memLocation = *reinterpret_cast<int *>(curLoad + 0x30);
    if (memLocation == TMP_ALLOC_MAIN) {
        if (bLargestMalloc(0) < fileSize) {
            return -2;
        }
        void *pmem = bMalloc(fileSize, filename, 0, 0x1040);
        *reinterpret_cast<void **>(curLoad + 0x28) = pmem;
        AddQueuedFile(pmem, tempPath, 0, fileSize, DataLoadCB_EAXAemsManager, reinterpret_cast<int>(m_pCurLoadSDLP), nullptr);
        m_IsWaitingForFileCB = 1;
    } else if (memLocation == TMP_ALLOC_AUDIO) {
        void *pmem = AudioMemoryManager_AllocateMemory(&gAudioMemoryManager, fileSize, filename, false);
        *reinterpret_cast<void **>(curLoad + 0x28) = pmem;
        AddQueuedFile(pmem, tempPath, 0, fileSize, DataLoadCB_EAXAemsManager, reinterpret_cast<int>(m_pCurLoadSDLP), nullptr);
        m_IsWaitingForFileCB = 1;
    } else {
        m_IsWaitingForFileCB = 0;
    }

    if (*reinterpret_cast<int *>(curLoad + 0x0) == SDT_AEMS_ASYNCSPU) {
        stBankSlot *pBankSlot = *reinterpret_cast<stBankSlot **>(curLoad + 0x24);
        if (pBankSlot == nullptr) {
            SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
        } else {
            SNDmemlimits(pBankSlot->BANKmemLocation, pBankSlot->BANKmemLocation + pBankSlot->BANKMemSize);
        }
    }

    return 0;
}

void EAXAemsManager::SetupNextLoad() {
    m_pCurLoadSDLP = nullptr;
    if (m_nCurLoadedBankIndex < (m_nEndOfList - 1)) {
        m_nCurLoadedBankIndex++;
        m_pCurLoadSDLP = g_SndAssetList + m_nCurLoadedBankIndex;
        if (InitiateLoad() < 0) {
            RemoveBankListing(m_nCurLoadedBankIndex);
            SetupNextLoad();
        }
    }
}

void EAXAemsManager::Update() {
    if (m_pCurLoadSDLP == nullptr) {
        SetupNextLoad();
        if (m_pCurLoadSDLP == nullptr) {
            return;
        }
    }

    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurLoadSDLP) + 0x38) == 0) {
        CheckForCompleteAsyncLoad();
    }

    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurLoadSDLP) + 0x3C) != 0) {
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurLoadSDLP) + 0x0) < SDT_GENERIC_DATA) {
            ResolveCurrentDataMemory();
        }
        SetupNextLoad();
    }
}

void EAXAemsManager::RemoveBankListing(int index) {
    if (index < 0 || index >= m_nEndOfList) {
        return;
    }

    char *assetEntry = reinterpret_cast<char *>(&g_SndAssetList[index]);
    stBankSlot *pBankSlot = *reinterpret_cast<stBankSlot **>(assetEntry + 0x24);
    if (pBankSlot != nullptr) {
        pBankSlot->pAssetParams = nullptr;
    }

    int handle = *reinterpret_cast<int *>(assetEntry + 0x34);
    if (handle != -1) {
        SNDAEMS_removemodulebank(handle);
    }
    *reinterpret_cast<void **>(assetEntry + 0x28) = nullptr;
    *reinterpret_cast<int *>(assetEntry + 0x34) = -1;
    *reinterpret_cast<int *>(assetEntry + 0x38) = 0;
    *reinterpret_cast<int *>(assetEntry + 0x3C) = 0;
}

void EAXAemsManager::UnloadSndData(int Index) {
    if (Index < 0 || Index >= 0x30) {
        return;
    }

    m_pCurUNLOADSDLP = g_SndAssetList + Index;
    char *entry = reinterpret_cast<char *>(m_pCurUNLOADSDLP);
    if (*reinterpret_cast<int *>(entry + 0x3C) == 0) {
        return;
    }

    *reinterpret_cast<int *>(entry + 0x38) = 0;
    *reinterpret_cast<int *>(entry + 0x3C) = 0;
    int eDataType = *reinterpret_cast<int *>(entry + 0x0);
    if (eDataType < SDT_GENERIC_DATA) {
        RemoveAEMSBank();
    }

    void *plocmem = *reinterpret_cast<void **>(entry + 0x2C);
    if (plocmem != nullptr) {
        if (eDataType == SDT_AEMS_MAINMEM) {
            bFree(plocmem);
        } else {
            AudioMemoryManager_FreeMemory(&gAudioMemoryManager, plocmem);
        }
        *reinterpret_cast<void **>(entry + 0x2C) = nullptr;
    }

    void *pmem = *reinterpret_cast<void **>(entry + 0x28);
    if (pmem != nullptr) {
        if (eDataType == SDT_AEMS_MAINMEM) {
            bFree(pmem);
        } else {
            AudioMemoryManager_FreeMemory(&gAudioMemoryManager, pmem);
        }
        *reinterpret_cast<void **>(entry + 0x28) = nullptr;
    }

    stBankSlot *bankSlot = *reinterpret_cast<stBankSlot **>(entry + 0x24);
    if (bankSlot != nullptr) {
        bankSlot->pAssetParams = nullptr;
        bankSlot->pLastAlloc = bankSlot->MAINmemLocation;
        bMemSet(bankSlot->MAINmemLocation, '\0', bankSlot->MAINmemSize);
        *reinterpret_cast<stBankSlot **>(entry + 0x24) = nullptr;
    }

    unsigned int **resStart = reinterpret_cast<unsigned int **>(entry + 0x40);
    unsigned int **resFinish = reinterpret_cast<unsigned int **>(entry + 0x44);
    while (*resStart != *resFinish) {
        unsigned int *cur = *resFinish - 1;
        *resFinish = cur;
        AudioMemoryManager_FreeMemory(&gAudioMemoryManager, reinterpret_cast<void *>(*cur));
    }

    RemoveBankListing(Index);
    m_pCurUNLOADSDLP = nullptr;
}

void *EAXAemsManager::AsyncResidentAllocCB(int size) {
    stSndDataLoadParams *m_pCurrentlyLoading = gAEMSMgr.m_pAsyncLoadSDLP;
    if (m_pCurrentlyLoading == nullptr) {
        m_pCurrentlyLoading = gAEMSMgr.m_pCurLoadSDLP;
    }
    if (m_pCurrentlyLoading == nullptr) {
        return nullptr;
    }

    char *loadEntry = reinterpret_cast<char *>(m_pCurrentlyLoading);
    stBankSlot *pBankSlot = *reinterpret_cast<stBankSlot **>(loadEntry + 0x24);
    if (pBankSlot != nullptr) {
        void *newresalloc = pBankSlot->pLastAlloc;
        pBankSlot->pLastAlloc += size;
        return newresalloc;
    }

    char *filename = *reinterpret_cast<char **>(loadEntry + 0x14);
    if (filename == nullptr) {
        filename = const_cast<char *>("");
    }
    char dbgstring[64];
    bStrCat(dbgstring, filename, ": resident allocation");
    return AudioMemoryManager_AllocateMemory(&gAudioMemoryManager, size, dbgstring, true);
}

void *EAXAemsManager::ResidentAllocCB(void *pbank, int residentsize, int totalsize) {
    char *entry = reinterpret_cast<char *>(gAEMSMgr.m_pCurLoadSDLP);
    if (entry == nullptr) {
        return nullptr;
    }

    void *resmem;
    if (residentsize == totalsize) {
        resmem = *reinterpret_cast<void **>(entry + 0x28);
    } else {
        stBankSlot *pBankSlot = *reinterpret_cast<stBankSlot **>(entry + 0x24);
        if (pBankSlot != nullptr) {
            pBankSlot->pLastAlloc += residentsize;
            return pBankSlot->MAINmemLocation;
        }

        char *filename = *reinterpret_cast<char **>(entry + 0x14);
        if (filename == nullptr) {
            filename = const_cast<char *>("");
        }
        resmem = AudioMemoryManager_AllocateMemory(&gAudioMemoryManager, residentsize, filename, false);
        *reinterpret_cast<void **>(entry + 0x2C) = resmem;
        (*reinterpret_cast<int *>(reinterpret_cast<char *>(&gAEMSMgr) + 0x104))++;
    }
    return resmem;
}

void EAXAemsManager::DataLoadCB(int param, int error_status) {
    gAEMSMgr.m_pCurLoadSDLP = reinterpret_cast<stSndDataLoadParams *>(param);
    char *entry = reinterpret_cast<char *>(gAEMSMgr.m_pCurLoadSDLP);
    if (entry == nullptr) {
        return;
    }

    *reinterpret_cast<int *>(entry + 0x64) = *reinterpret_cast<int *>(&WorldTimer);
    int eDataType = *reinterpret_cast<int *>(entry + 0x0);
    if (eDataType <= SDT_GENERIC_DATA) {
        AddAemsBank();
        gAEMSMgr.ResolveCurrentDataMemory();
        *reinterpret_cast<int *>(entry + 0x38) = 1;
        *reinterpret_cast<int *>(entry + 0x3C) = 1;
    } else if (eDataType == 4) {
        gAEMSMgr.m_IsWaitingForFileCB = 0;
        gAEMSMgr.m_ItemsPendingAsyncResolve++;
    } else {
        *reinterpret_cast<int *>(entry + 0x38) = 1;
        *reinterpret_cast<int *>(entry + 0x3C) = 1;
    }

    gAEMSMgr.m_IsWaitingForFileCB = 0;
}

int EAXAemsManager::AddBankListing(stAssetDescription &asset) {
    int index = m_nEndOfList;
    char *entry = reinterpret_cast<char *>(&g_SndAssetList[index]);
    bMemSet(entry, '\0', 0x68);

    *reinterpret_cast<int *>(entry + 0x34) = -1;
    *reinterpret_cast<int *>(entry + 0x30) = TMP_ALLOC_INVALID;
    *reinterpret_cast<stBankSlot **>(entry + 0x24) = nullptr;
    *reinterpret_cast<void **>(entry + 0x28) = nullptr;
    *reinterpret_cast<void **>(entry + 0x2C) = nullptr;

    memmove(entry, &asset, sizeof(stAssetDescription));
    m_nEndOfList = index + 1;
    return index;
}

void EAXAemsManager::QueueFileLoad(stSndAssetQueue &queueitem, eBANK_SLOT_TYPE SlotType) {
    Attrib::StringKey filename(queueitem.Asset.FileName);
    int bankIndex = IsAssetInList(filename);
    if (bankIndex == -1) {
        if (DISABLE_SLOT_LOADING != 0) {
            SlotType = eBANK_SLOT_NONE;
        }
        bankIndex = AddBankListing(queueitem.Asset);
        if (SlotType != eBANK_SLOT_NONE) {
            stBankSlot *slot = nullptr;
            if (SlotType == eBANK_SLOT_PATHFINDER) {
                slot = *reinterpret_cast<stBankSlot **>(reinterpret_cast<char *>(this) + 0xAC);
            } else {
                slot = *reinterpret_cast<stBankSlot **>(reinterpret_cast<char *>(this) + 0xA4);
            }
            if (slot != nullptr) {
                slot->pAssetParams = &g_SndAssetList[bankIndex];
                *reinterpret_cast<stBankSlot **>(reinterpret_cast<char *>(&g_SndAssetList[bankIndex]) + 0x24) = slot;
            }
        }
    }

    ListNodeQueue *head = *reinterpret_cast<ListNodeQueue **>(reinterpret_cast<char *>(this) + 0xBC);
    ListNodeQueue *node = reinterpret_cast<ListNodeQueue *>(FastMem_Alloc(&gFastMem, 0x30, nullptr));
    if (node == nullptr || head == nullptr) {
        return;
    }
    node->data = queueitem;
    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
}

void EAXAemsManager::ResolvePendingAsyncLoads() {}

void *EAXAemsManager::GetCallbackEventSys() {
    int offset = m_nCallbackEvtSys << 2;
    return *reinterpret_cast<void **>(reinterpret_cast<char *>(m_pEvtSystems_start) + offset);
}

void SubscribeEventSys() {
    if (IsSoundEnabled == 1) {
        void *system_desc = gAEMSMgr.GetCallbackEventSys();
        if (system_desc != nullptr) {
            CsisSystem_Subscribe(system_desc);
        }
    }
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
