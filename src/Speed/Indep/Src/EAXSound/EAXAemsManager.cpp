#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

extern void SNDAEMS_removemodulebank(int handle);
extern int SNDAEMS_addmodulebank(void *pmem, char *chunk, int offset, void *(*cb)(void *, int, int));
extern int SNDAEMS_asyncloadmodulebank(char *filename, int unk0, char *chunk, int offset, void *pmem, int size,
                                       void *(*cb)(int));
extern int SNDAEMS_asyncloadmodulebankmem(void *pmem, char *chunk, int offset, void *(*cb)(int));
extern int SNDAEMS_asyncloadmodulebankdone();
extern int SNDAEMS_asyncloadmodulebankmemdone();
void SubscribeEventSys();
extern char *g_DataPaths[];
extern int DISABLE_SLOT_LOADING;
extern int SNDmemlargestunused(int *start);
extern void SNDmemlimits(int lower, int upper);
extern int bLargestMalloc(int pool);
extern void *bMalloc(int size, int allocation_params);
extern void *bMalloc(int size, const char *name, int line, int pool);
extern int bFileSize(const char *filename);
extern char *bStrCat(char *destString, const char *s1, const char *s2);
extern void bFree(void *ptr);
extern void SNDSYS_service();
extern int QueuedFileDefaultPriority;
extern int GetMemoryPoolSize__13TrackStreamer(TrackStreamer *trackStreamer);
extern stSndDataLoadParams g_SndAssetList[];
extern Timer WorldTimer;
extern unsigned int bGetTicker();
extern unsigned int StartBankLoadTicks;

namespace Csis {
struct InterfaceId;
struct ClassHandle {
    int Set(const InterfaceId *pInterfaceId);
};
struct FunctionHandle {
    int Set(const InterfaceId *pInterfaceId);
};
namespace System {
void Subscribe(void *desc);
}

extern ClassHandle gFX_NITROUSHandle;
extern InterfaceId FX_NITROUSId;
extern ClassHandle gFX_PURGEHandle;
extern InterfaceId FX_PURGEId;
extern ClassHandle gFX_SHIFTING_01Handle;
extern InterfaceId FX_SHIFTING_01Id;
extern ClassHandle gFX_SPARKCHATTERHandle;
extern InterfaceId FX_SPARKCHATTERId;
extern ClassHandle gFX_SKIDHandle;
extern InterfaceId FX_SKIDId;
extern ClassHandle gFX_HydraulicHandle;
extern InterfaceId FX_HydraulicId;
extern ClassHandle gFX_HelicopterHandle;
extern InterfaceId FX_HelicopterId;
extern ClassHandle gFX_Hydr_BounceHandle;
extern InterfaceId FX_Hydr_BounceId;
extern ClassHandle gFX_WeatherHandle;
extern InterfaceId FX_WeatherId;
extern ClassHandle gFX_CameraHandle;
extern InterfaceId FX_CameraId;
extern ClassHandle gFX_UVESHandle;
extern InterfaceId FX_UVESId;
extern ClassHandle gFX_RadarHandle;
extern InterfaceId FX_RadarId;
extern ClassHandle gFX_ScrapeHandle;
extern InterfaceId FX_ScrapeId;
extern FunctionHandle gSputter_MessageHandle;
extern InterfaceId Sputter_MessageId;
extern ClassHandle gCARHandle;
extern InterfaceId CARId;
extern ClassHandle gCAR_SWTNHandle;
extern InterfaceId CAR_SWTNId;
extern ClassHandle gCAR_WHINEHandle;
extern InterfaceId CAR_WHINEId;
extern ClassHandle gCAR_TRANNYHandle;
extern InterfaceId CAR_TRANNYId;
extern ClassHandle gCAR_SputterHandle;
extern InterfaceId CAR_SputterId;
extern ClassHandle gCAR_SputOutputHandle;
extern InterfaceId CAR_SputOutputId;
extern ClassHandle gFX_ROADNOISEHandle;
extern InterfaceId FX_ROADNOISEId;
extern ClassHandle gFX_ROADNOISE_TRANSHandle;
extern InterfaceId FX_ROADNOISE_TRANSId;
extern ClassHandle gENV_STATICHandle;
extern InterfaceId ENV_STATICId;
extern ClassHandle gFX_MAIN_MEMHandle;
extern InterfaceId FX_MAIN_MEMId;
extern ClassHandle gFX_WINDHandle;
extern InterfaceId FX_WINDId;
extern ClassHandle gFX_WIND_WeatherHandle;
extern InterfaceId FX_WIND_WeatherId;
extern ClassHandle gFX_TRAFFICHandle;
extern InterfaceId FX_TRAFFICId;
extern ClassHandle gFX_TRUCK_FXHandle;
extern InterfaceId FX_TRUCK_FXId;
extern ClassHandle gFX_TURBO_01Handle;
extern InterfaceId FX_TURBO_01Id;
extern ClassHandle gPlayCommonSampleHandle;
extern InterfaceId PlayCommonSampleId;
extern ClassHandle gPlayFrontEndSampleHandle;
extern InterfaceId PlayFrontEndSampleId;
extern ClassHandle gPlayFrontEndSample_RSHandle;
extern InterfaceId PlayFrontEndSample_RSId;
extern ClassHandle gFEDriveOnHandle;
extern InterfaceId FEDriveOnId;
extern ClassHandle gSIRENHandle;
extern InterfaceId SIRENId;
extern ClassHandle gSIREN_BEDHandle;
extern InterfaceId SIREN_BEDId;
extern ClassHandle gAEMS_StichCollisionHandle;
extern InterfaceId AEMS_StichCollisionId;
extern ClassHandle gAEMS_StichWooshHandle;
extern InterfaceId AEMS_StichWooshId;
extern ClassHandle gAEMS_StichStaticHandle;
extern InterfaceId AEMS_StichStaticId;
} // namespace Csis

struct stAssetDescription {
    int eDataType;
    int _pad4;
    Attrib::StringKey FileName;
    int DataPath;
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
    TMP_ALLOC_NONE = 0,
    TMP_ALLOC_INVALID = TMP_ALLOC_NONE,
    TMP_ALLOC_MAIN = 1,
    TMP_ALLOC_TRACKSTREAMER = 2,
    TMP_ALLOC_AUDIO = 3,
};

enum eSndDataType {
    SDT_NONE = -1,
    SDT_AEMS_AUDIOMEM = 0,
    SDT_AEMS_MAINMEM = 1,
    SDT_AEMS_SYNCSPU = 2,
    SDT_AEMS_ASYNCSPU = 3,
    SDT_AEMS_ASYNCSPUMEM = 4,
    SDT_GENERIC_DATA = 5,
};

DECLARE_CONTAINER_TYPE(ResAllocList);
DECLARE_CONTAINER_TYPE(RefCountList);

typedef UTL::Std::vector<unsigned int, _type_ResAllocList> ResAllocList;
typedef UTL::Std::vector<EAX_CarState *, _type_RefCountList> RefCountList;
typedef void (*ExternalLoadCallbackFn)(int);

template <typename T> struct RawVector {
    T *start;
    T *finish;
    void *allocator;
    T *end_of_storage;
};

struct stSndDataLoadParamsView {
    stAssetDescription AssetDescription;
    eTEMPALLOCLOCATION MemLocation;
    stBankSlot *mBankSlot;
    void *pmem;
    void *plocmem;
    int nSize;
    int Handle;
    bool bResolvedAsync;
    char _pad39[3];
    bool bResolvedSync;
    char _pad3d[3];
    ResAllocList resallocs;
    RefCountList RefCount;
    int t_req;
    int t_load;
};

template <typename T, typename Tag> static RawVector<T> &AsRawVector(UTL::Std::vector<T, Tag> &vec) {
    return *reinterpret_cast<RawVector<T> *>(&vec);
}

template <typename T> static int RawVectorSize(const RawVector<T> &vec) {
    return static_cast<int>(vec.finish - vec.start);
}

template <typename T> static int RawVectorCapacity(const RawVector<T> &vec) {
    return static_cast<int>(vec.end_of_storage - vec.start);
}

template <typename T> static void RawVectorReset(RawVector<T> &vec) {
    vec.finish = vec.start;
}

template <typename T> static void RawVectorReserve(RawVector<T> &vec, int newCapacity) {
    const int oldCapacity = RawVectorCapacity(vec);
    if (newCapacity <= oldCapacity) {
        return;
    }

    const int oldSize = RawVectorSize(vec);
    const unsigned int byteCount = static_cast<unsigned int>(newCapacity * static_cast<int>(sizeof(T)));
    T *newStart = byteCount > 0 ? reinterpret_cast<T *>(gFastMem.Alloc(byteCount, nullptr)) : nullptr;

    if (oldSize > 0) {
        memmove(newStart, vec.start, static_cast<size_t>(oldSize * static_cast<int>(sizeof(T))));
    }
    if (vec.start != nullptr) {
        gFastMem.Free(vec.start, static_cast<unsigned int>(oldCapacity * static_cast<int>(sizeof(T))), nullptr);
    }

    vec.start = newStart;
    vec.finish = newStart + oldSize;
    vec.end_of_storage = newStart + newCapacity;
}

template <typename T> static void RawVectorPushBack(RawVector<T> &vec, const T &value) {
    if (vec.finish == vec.end_of_storage) {
        const int oldSize = RawVectorSize(vec);
        const int grow = oldSize > 0 ? oldSize : 1;
        RawVectorReserve(vec, oldSize + grow);
    }

    if (vec.finish != nullptr) {
        *vec.finish = value;
    }
    vec.finish++;
}

static void ResetSndAssetParams(stSndDataLoadParamsView &params) {
    params.AssetDescription.eDataType = -1;
    params.AssetDescription._pad4 = 0;
    params.AssetDescription.FileName = Attrib::StringKey("");
    params.AssetDescription.DataPath = static_cast<eSNDDATAPATH>(0);
    params.Handle = -1;
    params.MemLocation = static_cast<eTEMPALLOCLOCATION>(0);
    params.mBankSlot = nullptr;
    params.pmem = nullptr;
    params.plocmem = nullptr;
    params.nSize = 0;
    *reinterpret_cast<unsigned int *>(&params.bResolvedAsync) = 0;
    *reinterpret_cast<unsigned int *>(&params.bResolvedSync) = 0;
    RawVectorReset(AsRawVector(params.resallocs));
    RawVectorReset(AsRawVector(params.RefCount));
    params.t_load = 0;
    params.t_req = 0;
}

static char *&GetAsyncBuffer(EAXAemsManager *mgr) {
    return mgr->m_AsyncBuffer;
}

static int &GetAsyncBufferSize(EAXAemsManager *mgr) {
    return mgr->m_AsyncBufferSize;
}

static eTEMPALLOCLOCATION &GetAsyncBufferLocation(EAXAemsManager *mgr) {
    return *reinterpret_cast<eTEMPALLOCLOCATION *>(&mgr->m_AsyncBufferLocation);
}

static stSndDataLoadParamsView &AsLoadParamsView(stSndDataLoadParams *params) {
    return *reinterpret_cast<stSndDataLoadParamsView *>(params);
}

static const stSndDataLoadParamsView &AsLoadParamsView(const stSndDataLoadParams *params) {
    return *reinterpret_cast<const stSndDataLoadParamsView *>(params);
}

static const char *GetStringKeyChars(const Attrib::StringKey &key) {
    return *reinterpret_cast<const char *const *>(reinterpret_cast<const char *>(&key) + 0xC);
}
} // namespace

void stAssetDescription::Clear() {
    eDataType = -1;
    _pad4 = 0;
    FileName = Attrib::StringKey("");
    DataPath = 0;
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
            gAudioMemoryManager.FreeMemory(node->data.MAINmemLocation);
        }
        gFastMem.Free(node, 0x2C, nullptr);
        node = next;
    }

    head->next = head;
    head->prev = head;
}

EAXAemsManager::EAXAemsManager() {
    ListNodeBankSlot *bankSlots = reinterpret_cast<ListNodeBankSlot *>(gFastMem.Alloc(0x2C, nullptr));
    if (bankSlots != nullptr) {
        bankSlots->next = bankSlots;
        bankSlots->prev = bankSlots;
    }
    m_pMainSlotHead = bankSlots;

    ListNodeBankSlot *pfSlots = reinterpret_cast<ListNodeBankSlot *>(gFastMem.Alloc(0x2C, nullptr));
    if (pfSlots != nullptr) {
        pfSlots->next = pfSlots;
        pfSlots->prev = pfSlots;
    }
    m_pPfSlotHead = pfSlots;

    ListNodeQueue *waitSlots = reinterpret_cast<ListNodeQueue *>(gFastMem.Alloc(0x30, nullptr));
    if (waitSlots != nullptr) {
        waitSlots->next = waitSlots;
        waitSlots->prev = waitSlots;
    }
    m_pQueuedFileHead = waitSlots;

    m_pEvtSystems_start = nullptr;
    m_pEvtSystems_end = nullptr;
    m_pEvtSystems_end_of_storage = nullptr;
    m_nEvtSysQueued = 0;
    m_nEvtSysCount = -1;
    m_pEvtSysUserData = nullptr;
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
    int numEvtSysLoaded = m_nEvtSysQueued;
    while (n < numEvtSysLoaded) {
        gAudioMemoryManager.FreeMemory(m_pEvtSystems_start[n]);
        n++;
    }

    if (m_pEvtSystems_start != nullptr) {
        int byteCount = static_cast<int>(reinterpret_cast<char *>(m_pEvtSystems_end_of_storage) -
                                         reinterpret_cast<char *>(m_pEvtSystems_start));
        gFastMem.Free(m_pEvtSystems_start, byteCount, nullptr);
    }
    m_pEvtSystems_start = nullptr;
    m_pEvtSystems_end = nullptr;
    m_pEvtSystems_end_of_storage = nullptr;

    ClearSndAssetQueue(static_cast<ListNodeQueue *>(m_pQueuedFileHead));
    ListNodeQueue *waitSlotHead = static_cast<ListNodeQueue *>(m_pQueuedFileHead);
    if (waitSlotHead != nullptr) {
        gFastMem.Free(waitSlotHead, 0x30, nullptr);
    }

    reinterpret_cast<BankSlotSystem *>(&m_pPfSlotHead)->DestroySlots();
    ListNodeBankSlot *pfSlotHead = static_cast<ListNodeBankSlot *>(m_pPfSlotHead);
    if (pfSlotHead != nullptr) {
        gFastMem.Free(pfSlotHead, 0x2C, nullptr);
    }

    reinterpret_cast<BankSlotSystem *>(&m_pMainSlotHead)->DestroySlots();
    ListNodeBankSlot *slotHead = static_cast<ListNodeBankSlot *>(m_pMainSlotHead);
    if (slotHead != nullptr) {
        gFastMem.Free(slotHead, 0x2C, nullptr);
    }
}

void EAXAemsManager::Init() {
    Attrib::Gen::audiosystem *attrs =
        *reinterpret_cast<Attrib::Gen::audiosystem **>(reinterpret_cast<char *>(g_pEAXSound) + 0xA8);
    unsigned int numEvtSys = attrs->Num_EvtSys();
    m_nEvtSysCount = static_cast<int>(numEvtSys);
    m_nResidentAllocs = 0;
    m_nEvtSysQueued = 0;
    if (static_cast<int>(numEvtSys) > 0) {
        if (m_pEvtSystems_start != nullptr) {
            int oldByteCount = static_cast<int>(reinterpret_cast<char *>(m_pEvtSystems_end_of_storage) -
                                                reinterpret_cast<char *>(m_pEvtSystems_start));
            gFastMem.Free(m_pEvtSystems_start, oldByteCount, nullptr);
        }

        m_pEvtSystems_start = reinterpret_cast<void **>(
            gFastMem.Alloc(static_cast<unsigned int>(numEvtSys * sizeof(void *)), nullptr));
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
    m_pEvtSystems_start[eESIndex] = gAudioMemoryManager.AllocateMemoryChar(filesize, evtName, false);

    int callback_param = m_nEvtSysQueued;
    AddQueuedFile(m_pEvtSystems_start[callback_param], tempPath, 0, filesize, EvtSysLoadCallback, callback_param, nullptr);
    m_nEvtSysQueued = callback_param + 1;
    return callback_param;
}

void EAXAemsManager::InitializeSlots(bool bDoPFSlot) {
    if (DISABLE_SLOT_LOADING == 0) {
        if (bDoPFSlot) {
            reinterpret_cast<BankSlotSystem *>(&m_pPfSlotHead)->DestroySlots();
        }
        reinterpret_cast<BankSlotSystem *>(&m_pMainSlotHead)->DestroySlots();
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
        NewSlot.MAINmemLocation = reinterpret_cast<char *>(
            gAudioMemoryManager.AllocateMemory(SizePerSlotMainMem, DebugString, false));
        NewSlot.BANKmemLocation = m_SPUMainAllocsEnd - SizePerSlotSPU;
        m_SPUMainAllocsEnd = NewSlot.BANKmemLocation;
        NewSlot.pAssetParams = nullptr;
        NewSlot.pLastAlloc = NewSlot.MAINmemLocation;
        NewSlot.MAINmemSize = SizePerSlotMainMem;
        NewSlot.BANKMemSize = SizePerSlotSPU;

        ListNodeBankSlot *head = *reinterpret_cast<ListNodeBankSlot **>(
            reinterpret_cast<char *>(this) + ((Type == eBANK_SLOT_PATHFINDER && bDoPFSlot) ? 0xAC : 0xA4));
        ListNodeBankSlot *node = reinterpret_cast<ListNodeBankSlot *>(gFastMem.Alloc(0x2C, nullptr));
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
            TheTrackStreamer.FreeUserMemory(pmem);
        } else if (MemLocation == TMP_ALLOC_AUDIO) {
            void *plocmem = *reinterpret_cast<void **>(curLoad + 0x2C);
            stBankSlot *mBankSlot = *reinterpret_cast<stBankSlot **>(curLoad + 0x24);
            if (plocmem == nullptr && mBankSlot == nullptr) {
                return;
            }
            gAudioMemoryManager.FreeMemory(pmem);
        } else {
            return;
        }
    }

    *reinterpret_cast<void **>(curLoad + 0x28) = nullptr;
}

int EAXAemsManager::InitiateLoad() {
    int result;
    stBankSlot *pBankSlot;
    void *allocatedMemory;
    char *fileString;
    stSndDataLoadParamsView *currentLoad;
    int loadSize;
    eTEMPALLOCLOCATION memLocation;
    QueuedFileParams queuedFileParams;

    currentLoad = reinterpret_cast<stSndDataLoadParamsView *>(m_pCurLoadSDLP);
    fileString = *reinterpret_cast<char **>(reinterpret_cast<char *>(&currentLoad->AssetDescription.FileName) + 0xC);
    if (fileString == nullptr) {
        fileString = "";
    }
    bStrCat(m_LoadFilename, g_DataPaths[currentLoad->AssetDescription.DataPath], fileString);
    result = bFileSize(m_LoadFilename);
    currentLoad->nSize = result;
    result = AudioMemoryPool;
    if (currentLoad->nSize < 1) {
        return -1;
    }

    if (currentLoad->AssetDescription.eDataType < SDT_GENERIC_DATA) {
        if (GetAsyncBuffer(this) != nullptr) {
            goto HaveAsyncBuffer;
        }
        GetAsyncBufferLocation(this) = TMP_ALLOC_AUDIO;
        bLargestMalloc(result);
        result = bLargestMalloc(0);
        if (result > 0x20000) {
            GetAsyncBufferLocation(this) = TMP_ALLOC_MAIN;
            fileString = reinterpret_cast<char *>(bMalloc(0x10000, 0));
            GetAsyncBuffer(this) = fileString;
            goto HaveAsyncBuffer;
        }
        fileString = reinterpret_cast<char *>(TheTrackStreamer.AllocateUserMemory(0x10000, "EAXAemsManager::m_pAsyncBuff", 0));
        GetAsyncBufferLocation(this) = TMP_ALLOC_TRACKSTREAMER;
        GetAsyncBuffer(this) = fileString;
        if (fileString != nullptr) {
            goto HaveAsyncBuffer;
        }
        pBankSlot = currentLoad->mBankSlot;
        if (pBankSlot != nullptr) {
            pBankSlot->LoadFailed = 1;
        }
        result = -2;
    } else {
        currentLoad->MemLocation = TMP_ALLOC_AUDIO;
HaveQueueParams:
        queuedFileParams.BlockSize = 0x7FFFFFF;
        queuedFileParams.Compressed = false;
        *reinterpret_cast<unsigned int *>(&queuedFileParams.Compressed) = 0;
        queuedFileParams.Priority = QueuedFileDefaultPriority - 2;
        queuedFileParams.UncompressedSize = 0;
        currentLoad = reinterpret_cast<stSndDataLoadParamsView *>(m_pCurLoadSDLP);
        memLocation = currentLoad->MemLocation;
        if (memLocation == TMP_ALLOC_MAIN) {
            result = bLargestMalloc(0);
            loadSize = currentLoad->nSize;
            if (result < loadSize) {
                result = -2;
                goto ReturnResult;
            }
            allocatedMemory = bMalloc(loadSize, 0x1040);
            currentLoad->pmem = allocatedMemory;
            AddQueuedFile(currentLoad->pmem, m_LoadFilename, 0, currentLoad->nSize, DataLoadCB,
                          reinterpret_cast<int>(currentLoad), &queuedFileParams);
SetWaitingState:
            *reinterpret_cast<eTEMPALLOCLOCATION *>(&m_IsWaitingForFileCB) = memLocation;
        } else if (static_cast<int>(memLocation) < 2) {
            if (memLocation == TMP_ALLOC_NONE) {
                goto SetWaitingState;
            }
        } else {
            if (memLocation == TMP_ALLOC_TRACKSTREAMER) {
                result = GetMemoryPoolSize__13TrackStreamer(&TheTrackStreamer);
                if (result == 0) {
                    return -3;
                }
                currentLoad = reinterpret_cast<stSndDataLoadParamsView *>(m_pCurLoadSDLP);
                fileString = *reinterpret_cast<char **>(reinterpret_cast<char *>(&currentLoad->AssetDescription.FileName) + 0xC);
                if (fileString == nullptr) {
                    fileString = "";
                }
                allocatedMemory = TheTrackStreamer.AllocateUserMemory(currentLoad->nSize, fileString, 0);
                currentLoad->pmem = allocatedMemory;
                fileString = reinterpret_cast<char *>(currentLoad->pmem);
                if (fileString == nullptr) {
                    return -3;
                }
                result = currentLoad->nSize;
            } else {
                if (memLocation != TMP_ALLOC_AUDIO) {
                    goto CheckAsyncSpuLoad;
                }
                if (currentLoad->AssetDescription.eDataType == SDT_GENERIC_DATA &&
                    (pBankSlot = currentLoad->mBankSlot, pBankSlot != nullptr)) {
                    result = currentLoad->nSize;
                    if (pBankSlot->MAINmemSize < result) {
                        return -4;
                    }
                    fileString = pBankSlot->MAINmemLocation;
                } else {
                    result = bLargestMalloc(AudioMemoryPool);
                    currentLoad = reinterpret_cast<stSndDataLoadParamsView *>(m_pCurLoadSDLP);
                    if (result < currentLoad->nSize) {
                        return -4;
                    }
                    fileString =
                        *reinterpret_cast<char **>(reinterpret_cast<char *>(&currentLoad->AssetDescription.FileName) + 0xC);
                    if (fileString == nullptr) {
                        fileString = "";
                    }
                    allocatedMemory = gAudioMemoryManager.AllocateMemory(currentLoad->nSize, fileString,
                                                                         currentLoad->AssetDescription.bLoadToTop);
                    currentLoad->pmem = allocatedMemory;
                    fileString = reinterpret_cast<char *>(currentLoad->pmem);
                    result = currentLoad->nSize;
                }
            }
            AddQueuedFile(fileString, m_LoadFilename, 0, result, DataLoadCB,
                          reinterpret_cast<int>(currentLoad), &queuedFileParams);
            *reinterpret_cast<unsigned int *>(&m_IsWaitingForFileCB) = 1;
        }
CheckAsyncSpuLoad:
        currentLoad = reinterpret_cast<stSndDataLoadParamsView *>(m_pCurLoadSDLP);
        if (currentLoad->AssetDescription.eDataType == SDT_AEMS_ASYNCSPU) {
            pBankSlot = currentLoad->mBankSlot;
            if (pBankSlot == nullptr) {
                SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
            } else {
                SNDmemlimits(pBankSlot->BANKmemLocation, pBankSlot->BANKmemLocation + pBankSlot->BANKMemSize);
            }
            result = SNDAEMS_asyncloadmodulebank(m_LoadFilename, 0, nullptr, 0, GetAsyncBuffer(this),
                                                 GetAsyncBufferSize(this), AsyncResidentAllocCB);
            currentLoad->Handle = result;
        }
        result = 0;
    }
ReturnResult:
    return result;

HaveAsyncBuffer:
    currentLoad = reinterpret_cast<stSndDataLoadParamsView *>(m_pCurLoadSDLP);
    pBankSlot = currentLoad->mBankSlot;
    if (pBankSlot != nullptr) {
        pBankSlot->LoadFailed = 0;
    }
    currentLoad->MemLocation = TMP_ALLOC_NONE;
    currentLoad->AssetDescription.eDataType = SDT_AEMS_ASYNCSPU;
    goto HaveQueueParams;
}

void EAXAemsManager::SetupNextLoad() {
    m_pCurLoadSDLP = nullptr;
    if (m_nCurLoadedBankIndex < (m_nEndOfList - 1)) {
        m_nCurLoadedBankIndex++;
        m_pCurLoadSDLP = g_SndAssetList + m_nCurLoadedBankIndex;
        if (InitiateLoad() < 0) {
            Attrib::StringKey failedFile =
                reinterpret_cast<stSndDataLoadParamsView *>(m_pCurLoadSDLP)->AssetDescription.FileName;

            SndBase *sfxToDelete[32];
            bMemSet(sfxToDelete, '\0', sizeof(sfxToDelete));
            int deleteCount = 0;

            ListNodeQueue *head = static_cast<ListNodeQueue *>(m_pQueuedFileHead);
            if (head == nullptr) {
                RemoveBankListing(m_nCurLoadedBankIndex);
                SetupNextLoad();
                return;
            }

        RestartScan:
            ListNodeQueue *match = head->next;
            while (match != head) {
                if (match->data.Asset.FileName == failedFile) {
                    break;
                }
                match = match->next;
            }

            if (match != head) {
                SndBase *targetThis = match->data.pThis;

                for (ListNodeQueue *scan = head->next; scan != head;) {
                    ListNodeQueue *next = scan->next;
                    if (scan->data.pThis == targetThis && scan->data.Asset.FileName == failedFile) {
                        scan->prev->next = scan->next;
                        scan->next->prev = scan->prev;
                        gFastMem.Free(scan, 0x30, nullptr);
                    }
                    scan = next;
                }

                if (deleteCount < 32) {
                    sfxToDelete[deleteCount] = targetThis;
                    ++deleteCount;
                }
                goto RestartScan;
            }

            for (int i = deleteCount - 1; i >= 0; --i) {
                SndBase *targetThis = sfxToDelete[i];
                ListNodeQueue *findThis = head->next;
                while (findThis != head && findThis->data.pThis != targetThis) {
                    findThis = findThis->next;
                }

                if (findThis == head) {
                    deleteCount = i;
                    continue;
                }

                Attrib::StringKey targetFile = findThis->data.Asset.FileName;
                for (ListNodeQueue *scan = head->next; scan != head;) {
                    ListNodeQueue *next = scan->next;
                    if (scan->data.pThis == targetThis && scan->data.Asset.FileName == targetFile) {
                        scan->prev->next = scan->next;
                        scan->next->prev = scan->prev;
                        gFastMem.Free(scan, 0x30, nullptr);
                    }
                    scan = next;
                }
            }

            if (deleteCount <= 0) {
                RemoveBankListing(m_nCurLoadedBankIndex);
                SetupNextLoad();
            }
        }
    }
}

void EAXAemsManager::Update() {
ReprocessQueue:
    ListNodeQueue *head = static_cast<ListNodeQueue *>(m_pQueuedFileHead);
    ListNodeQueue *iter = head->next;
    while (iter != head) {
        stSndAssetQueue currentRequest = iter->data;
        if (IsAssetLoaded(currentRequest.Asset.FileName) == -1) {
            iter = iter->next;
            continue;
        }

        bool shouldInit = true;
        for (ListNodeQueue *scan = head->next; scan != head; scan = scan->next) {
            if (scan->data.pThis == currentRequest.pThis && !(scan->data.Asset.FileName == currentRequest.Asset.FileName)) {
                shouldInit = false;
            }
        }

        if (shouldInit && currentRequest.pThis != nullptr) {
            currentRequest.pThis->InitSFX();
        }

        for (ListNodeQueue *removeIter = head->next; removeIter != head;) {
            ListNodeQueue *next = removeIter->next;
            if (removeIter->data.pThis == currentRequest.pThis &&
                removeIter->data.Asset.FileName == currentRequest.Asset.FileName) {
                removeIter->prev->next = removeIter->next;
                removeIter->next->prev = removeIter->prev;
                gFastMem.Free(removeIter, 0x30, nullptr);
            }
            removeIter = next;
        }

        goto ReprocessQueue;
    }

    if (m_ItemsPendingAsyncResolve != 0 && m_HasExternalLoadPending != 0) {
        ResolvePendingAsyncLoads();
    }

    if (m_pCurLoadSDLP == nullptr) {
        SetupNextLoad();
        if (m_pCurLoadSDLP != nullptr) {
            return;
        }
        if (m_ItemsPendingAsyncResolve != 0) {
            return;
        }

        char *mem = m_AsyncBuffer;
        if (mem != nullptr) {
            eTEMPALLOCLOCATION location = static_cast<eTEMPALLOCLOCATION>(m_AsyncBufferLocation);
            if (location == TMP_ALLOC_AUDIO) {
                gAudioMemoryManager.FreeMemory(mem);
            } else if (location == TMP_ALLOC_MAIN) {
                bFree(mem);
            } else if (location == TMP_ALLOC_TRACKSTREAMER) {
                TheTrackStreamer.FreeUserMemory(mem);
            }
            m_AsyncBuffer = nullptr;
        }

        ExternalLoadCallbackFn callback = reinterpret_cast<ExternalLoadCallbackFn>(m_ExternalLoadCallback);
        if (callback == nullptr) {
            return;
        }

        Csis::gFX_NITROUSHandle.Set(&Csis::FX_NITROUSId);
        Csis::gFX_PURGEHandle.Set(&Csis::FX_PURGEId);
        Csis::gFX_SHIFTING_01Handle.Set(&Csis::FX_SHIFTING_01Id);
        Csis::gFX_SPARKCHATTERHandle.Set(&Csis::FX_SPARKCHATTERId);
        Csis::gFX_SKIDHandle.Set(&Csis::FX_SKIDId);
        Csis::gFX_HydraulicHandle.Set(&Csis::FX_HydraulicId);
        Csis::gFX_HelicopterHandle.Set(&Csis::FX_HelicopterId);
        Csis::gFX_Hydr_BounceHandle.Set(&Csis::FX_Hydr_BounceId);
        Csis::gFX_WeatherHandle.Set(&Csis::FX_WeatherId);
        Csis::gFX_CameraHandle.Set(&Csis::FX_CameraId);
        Csis::gFX_UVESHandle.Set(&Csis::FX_UVESId);
        Csis::gFX_RadarHandle.Set(&Csis::FX_RadarId);
        Csis::gFX_ScrapeHandle.Set(&Csis::FX_ScrapeId);
        Csis::gSputter_MessageHandle.Set(&Csis::Sputter_MessageId);
        Csis::gCARHandle.Set(&Csis::CARId);
        Csis::gCAR_SWTNHandle.Set(&Csis::CAR_SWTNId);
        Csis::gCAR_WHINEHandle.Set(&Csis::CAR_WHINEId);
        Csis::gCAR_TRANNYHandle.Set(&Csis::CAR_TRANNYId);
        Csis::gCAR_SputterHandle.Set(&Csis::CAR_SputterId);
        Csis::gCAR_SputOutputHandle.Set(&Csis::CAR_SputOutputId);
        Csis::gFX_ROADNOISEHandle.Set(&Csis::FX_ROADNOISEId);
        Csis::gFX_ROADNOISE_TRANSHandle.Set(&Csis::FX_ROADNOISE_TRANSId);
        Csis::gENV_STATICHandle.Set(&Csis::ENV_STATICId);
        Csis::gFX_MAIN_MEMHandle.Set(&Csis::FX_MAIN_MEMId);
        Csis::gFX_WINDHandle.Set(&Csis::FX_WINDId);
        Csis::gFX_WIND_WeatherHandle.Set(&Csis::FX_WIND_WeatherId);
        Csis::gFX_TRAFFICHandle.Set(&Csis::FX_TRAFFICId);
        Csis::gFX_TRUCK_FXHandle.Set(&Csis::FX_TRUCK_FXId);
        Csis::gFX_TURBO_01Handle.Set(&Csis::FX_TURBO_01Id);
        Csis::gPlayCommonSampleHandle.Set(&Csis::PlayCommonSampleId);
        Csis::gPlayFrontEndSampleHandle.Set(&Csis::PlayFrontEndSampleId);
        Csis::gPlayFrontEndSample_RSHandle.Set(&Csis::PlayFrontEndSample_RSId);
        Csis::gFEDriveOnHandle.Set(&Csis::FEDriveOnId);
        Csis::gSIRENHandle.Set(&Csis::SIRENId);
        Csis::gSIREN_BEDHandle.Set(&Csis::SIREN_BEDId);
        Csis::gAEMS_StichCollisionHandle.Set(&Csis::AEMS_StichCollisionId);
        Csis::gAEMS_StichWooshHandle.Set(&Csis::AEMS_StichWooshId);
        Csis::gAEMS_StichStaticHandle.Set(&Csis::AEMS_StichStaticId);

        callback(m_ExternalLoadParam);
        m_HasExternalLoadPending = 0;
        m_ExternalLoadCallback = nullptr;
        m_ExternalLoadParam = 0;
        return;
    }

    stSndDataLoadParamsView &currentLoad =
        *reinterpret_cast<stSndDataLoadParamsView *>(reinterpret_cast<char *>(m_pCurLoadSDLP));
    if (!currentLoad.bResolvedAsync) {
        CheckForCompleteAsyncLoad();
    }

    currentLoad.bResolvedSync =
        Joylog::AddOrGetData(static_cast<unsigned int>(currentLoad.bResolvedAsync), 8, JOYLOG_CHANNEL_SOUND_LOADING) != 0;

    if (Joylog::IsReplaying() != 0 && currentLoad.bResolvedSync) {
        while (!currentLoad.bResolvedAsync) {
            SNDSYS_service();
            CheckForCompleteAsyncLoad();
        }
    }

    if (currentLoad.bResolvedSync) {
        if (currentLoad.AssetDescription.eDataType < SDT_GENERIC_DATA) {
            ResolveCurrentDataMemory();
        }
        SetupNextLoad();
    }
}

void EAXAemsManager::RemoveBankListing(int index) {
    ResetSndAssetParams(*reinterpret_cast<stSndDataLoadParamsView *>(&g_SndAssetList[index]));
    for (int n = index; n < 0x2F; n++) {
        stSndDataLoadParams *currentParams = g_SndAssetList + n;
        stSndDataLoadParams *futureParams = currentParams + 1;
        stSndDataLoadParamsView &current =
            *reinterpret_cast<stSndDataLoadParamsView *>(currentParams);
        stSndDataLoadParamsView &future =
            *reinterpret_cast<stSndDataLoadParamsView *>(futureParams);

        current.AssetDescription = future.AssetDescription;
        current.MemLocation = future.MemLocation;
        current.mBankSlot = future.mBankSlot;
        if (current.mBankSlot != nullptr && current.mBankSlot->pAssetParams == futureParams) {
            current.mBankSlot->pAssetParams = currentParams;
        }

        current.pmem = future.pmem;
        current.plocmem = future.plocmem;
        current.nSize = future.nSize;
        current.Handle = future.Handle;
        *reinterpret_cast<unsigned int *>(&current.bResolvedAsync) =
            *reinterpret_cast<unsigned int *>(&future.bResolvedAsync);
        *reinterpret_cast<unsigned int *>(&current.bResolvedSync) =
            *reinterpret_cast<unsigned int *>(&future.bResolvedSync);

        current.resallocs.clear();
        for (ResAllocList::const_iterator i = future.resallocs.begin(); i != future.resallocs.end(); ++i) {
            current.resallocs.push_back(*i);
        }
        future.resallocs.clear();

        current.RefCount.clear();
        for (RefCountList::const_iterator i = future.RefCount.begin(); i != future.RefCount.end(); ++i) {
            current.RefCount.push_back(*i);
        }
        future.RefCount.clear();

        current.t_req = future.t_req;
        current.t_load = future.t_load;

        ResetSndAssetParams(future);
    }

    m_nCurLoadedBankIndex--;
    m_nEndOfList--;
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
            gAudioMemoryManager.FreeMemory(plocmem);
        }
        *reinterpret_cast<void **>(entry + 0x2C) = nullptr;
    }

    void *pmem = *reinterpret_cast<void **>(entry + 0x28);
    if (pmem != nullptr) {
        if (eDataType == SDT_AEMS_MAINMEM) {
            bFree(pmem);
        } else {
            gAudioMemoryManager.FreeMemory(pmem);
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
        gAudioMemoryManager.FreeMemory(reinterpret_cast<void *>(*cur));
    }

    RemoveBankListing(Index);
    m_pCurUNLOADSDLP = nullptr;
}

void *EAXAemsManager::AsyncResidentAllocCB(int size) {
    stSndDataLoadParams *m_pCurrentlyLoading = gAEMSMgr.m_pAsyncLoadSDLP;
    void *newresalloc;
    if (m_pCurrentlyLoading == nullptr) {
        m_pCurrentlyLoading = gAEMSMgr.m_pCurLoadSDLP;
    }

    char *currentLoad = reinterpret_cast<char *>(m_pCurrentlyLoading);
    stBankSlot *pBankSlot = *reinterpret_cast<stBankSlot **>(currentLoad + 0x24);
    if (pBankSlot != nullptr) {
        newresalloc = pBankSlot->pLastAlloc;
        pBankSlot->pLastAlloc += size;
    } else {
        char *filename = *reinterpret_cast<char **>(currentLoad + 0x14);
        if (filename == nullptr) {
            filename = const_cast<char *>("");
        }
        char dbgstring[64];
        bStrCat(dbgstring, filename, ": resident allocation");
        newresalloc = gAudioMemoryManager.AllocateMemory(size, dbgstring, true);
        unsigned int newresallocaddr = reinterpret_cast<unsigned int>(newresalloc);
        reinterpret_cast<ResAllocList *>(currentLoad + 0x40)->push_back(newresallocaddr);
    }
    return newresalloc;
}

void *EAXAemsManager::ResidentAllocCB(void *pbank, int residentsize, int totalsize) {
    (void)pbank;
    void *resmem;
    if (residentsize == totalsize) {
        resmem = *reinterpret_cast<void **>(reinterpret_cast<char *>(gAEMSMgr.m_pCurLoadSDLP) + 0x28);
    } else {
        char *currentLoad = reinterpret_cast<char *>(gAEMSMgr.m_pCurLoadSDLP);
        if (*reinterpret_cast<int *>(currentLoad + 0x0) == SDT_AEMS_MAINMEM) {
            resmem = bMalloc(residentsize, 0x1040);
        } else {
            stBankSlot *pBankSlot = *reinterpret_cast<stBankSlot **>(currentLoad + 0x24);
            if (pBankSlot != nullptr) {
                pBankSlot->pLastAlloc += residentsize;
                return pBankSlot->MAINmemLocation;
            }

            char *filename = *reinterpret_cast<char **>(currentLoad + 0x14);
            if (filename == nullptr) {
                filename = const_cast<char *>("");
            }
            resmem = gAudioMemoryManager.AllocateMemory(residentsize, filename, false);
        }

        *reinterpret_cast<void **>(currentLoad + 0x2C) = resmem;
        (*reinterpret_cast<int *>(reinterpret_cast<char *>(&gAEMSMgr) + 0x104))++;
        resmem = *reinterpret_cast<void **>(currentLoad + 0x2C);
    }
    return resmem;
}

void EAXAemsManager::DataLoadCB(int param, int error_status) {
    (void)error_status;
    unsigned int ticker = bGetTicker();
    char *mgr = reinterpret_cast<char *>(&gAEMSMgr);
    int worldTime = *reinterpret_cast<int *>(&WorldTimer);
    StartBankLoadTicks = ticker;
    *reinterpret_cast<stSndDataLoadParams **>(mgr + 0x118) = reinterpret_cast<stSndDataLoadParams *>(param);
    char *currentLoad = reinterpret_cast<char *>(*reinterpret_cast<stSndDataLoadParams **>(mgr + 0x118));
    *reinterpret_cast<int *>(currentLoad + 0x64) = worldTime;
    int eDataType = *reinterpret_cast<int *>(currentLoad + 0x0);

    if (eDataType == SDT_AEMS_ASYNCSPU) {
        goto LoadDone;
    }

    if (eDataType <= SDT_AEMS_SYNCSPU) {
        if (eDataType >= SDT_AEMS_AUDIOMEM) {
            AddAemsBank();
            gAEMSMgr.ResolveCurrentDataMemory();
            *reinterpret_cast<unsigned int *>(currentLoad + 0x38) = 1;
            *reinterpret_cast<unsigned int *>(currentLoad + 0x3C) = 1;
        }
    } else if (eDataType == SDT_AEMS_ASYNCSPUMEM) {
        *reinterpret_cast<unsigned int *>(mgr + 0x12C) = 0;
        if (*reinterpret_cast<int *>(mgr + 0x128) != 0) {
            *reinterpret_cast<stSndDataLoadParams **>(mgr + 0x118) = nullptr;
            (*reinterpret_cast<int *>(mgr + 0x124))++;
            goto LoadDone;
        }

        stBankSlot *pBankSlot = *reinterpret_cast<stBankSlot **>(currentLoad + 0x24);
        if (pBankSlot == nullptr) {
            SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
        } else {
            SNDmemlimits(pBankSlot->BANKmemLocation, pBankSlot->BANKmemLocation + pBankSlot->BANKMemSize);
        }

        *reinterpret_cast<int *>(currentLoad + 0x34) =
            SNDAEMS_asyncloadmodulebankmem(*reinterpret_cast<void **>(currentLoad + 0x28), nullptr, 0, AsyncResidentAllocCB);
    } else if (eDataType == SDT_GENERIC_DATA) {
        *reinterpret_cast<unsigned int *>(currentLoad + 0x38) = 1;
        *reinterpret_cast<unsigned int *>(currentLoad + 0x3C) = 1;
    }

LoadDone:
    *reinterpret_cast<unsigned int *>(mgr + 0x12C) = 0;
}

int EAXAemsManager::AddBankListing(stAssetDescription &asset) {
    int index = m_nEndOfList;
    char *entry = reinterpret_cast<char *>(&g_SndAssetList[index]);
    unsigned long long hash64 = Attrib::StringHash64("");
    unsigned int hash32 = Attrib::StringHash32("");
    *reinterpret_cast<int *>(entry + 0x34) = -1;
    *reinterpret_cast<int *>(entry + 0x0) = SDT_NONE;
    *reinterpret_cast<unsigned int *>(entry + 0x8) = static_cast<unsigned int>(hash64 >> 32);
    *reinterpret_cast<unsigned int *>(entry + 0xC) = static_cast<unsigned int>(hash64);
    *reinterpret_cast<unsigned int *>(entry + 0x10) = hash32;
    *reinterpret_cast<const char **>(entry + 0x14) = "";
    *reinterpret_cast<int *>(entry + 0x18) = SNDPATH_ROUTE;
    *reinterpret_cast<int *>(entry + 0x20) = TMP_ALLOC_NONE;
    *reinterpret_cast<stBankSlot **>(entry + 0x24) = nullptr;
    *reinterpret_cast<void **>(entry + 0x28) = nullptr;
    *reinterpret_cast<void **>(entry + 0x2C) = nullptr;
    *reinterpret_cast<int *>(entry + 0x30) = 0;
    *reinterpret_cast<unsigned int *>(entry + 0x38) = 0;
    *reinterpret_cast<unsigned int *>(entry + 0x3C) = 0;

    RawVector<unsigned int> &resallocs = AsRawVector(*reinterpret_cast<ResAllocList *>(entry + 0x40));
    unsigned int *resallocsEnd = resallocs.finish;
    unsigned int *resallocsSrc = resallocs.finish;
    unsigned int *resallocsDst = resallocs.start;
    if (resallocsEnd != resallocsSrc) {
        size_t bytes = reinterpret_cast<char *>(resallocsEnd) - reinterpret_cast<char *>(resallocsSrc);
        void *copied = memmove(resallocsDst, resallocsSrc, bytes);
        resallocsDst = reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(copied) + bytes);
    }
    resallocs.finish = resallocsDst;

    RawVector<EAX_CarState *> &refCount = AsRawVector(*reinterpret_cast<RefCountList *>(entry + 0x50));
    EAX_CarState **refCountEnd = refCount.finish;
    EAX_CarState **refCountSrc = refCount.finish;
    EAX_CarState **refCountDst = refCount.start;
    if (refCountEnd != refCountSrc) {
        size_t bytes = reinterpret_cast<char *>(refCountEnd) - reinterpret_cast<char *>(refCountSrc);
        void *copied = memmove(refCountDst, refCountSrc, bytes);
        refCountDst = reinterpret_cast<EAX_CarState **>(reinterpret_cast<char *>(copied) + bytes);
    }
    refCount.finish = refCountDst;

    *reinterpret_cast<int *>(entry + 0x64) = 0;
    *reinterpret_cast<int *>(entry + 0x60) = 0;

    char *assetBytes = reinterpret_cast<char *>(&asset);
    *reinterpret_cast<int *>(entry + 0x0) = *reinterpret_cast<int *>(assetBytes + 0x0);
    *reinterpret_cast<unsigned int *>(entry + 0x8) = *reinterpret_cast<unsigned int *>(assetBytes + 0x8);
    *reinterpret_cast<unsigned int *>(entry + 0xC) = *reinterpret_cast<unsigned int *>(assetBytes + 0xC);
    *reinterpret_cast<unsigned int *>(entry + 0x10) = *reinterpret_cast<unsigned int *>(assetBytes + 0x10);
    *reinterpret_cast<const char **>(entry + 0x14) = *reinterpret_cast<const char **>(assetBytes + 0x14);
    *reinterpret_cast<int *>(entry + 0x18) = *reinterpret_cast<int *>(assetBytes + 0x18);
    *reinterpret_cast<unsigned int *>(entry + 0x1C) = *reinterpret_cast<unsigned int *>(assetBytes + 0x1C);

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
                slot = reinterpret_cast<BankSlotSystem *>(&m_pPfSlotHead)->GetFreeSlot(SlotType);
            } else {
                slot = reinterpret_cast<BankSlotSystem *>(&m_pMainSlotHead)->GetFreeSlot(SlotType);
            }
            if (slot != nullptr) {
                slot->pAssetParams = &g_SndAssetList[bankIndex];
                *reinterpret_cast<stBankSlot **>(reinterpret_cast<char *>(&g_SndAssetList[bankIndex]) + 0x24) = slot;
            }
        }
    }

    ListNodeQueue *head = static_cast<ListNodeQueue *>(m_pQueuedFileHead);
    ListNodeQueue *node = reinterpret_cast<ListNodeQueue *>(gFastMem.Alloc(0x30, nullptr));
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
            Csis::System::Subscribe(system_desc);
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
    stSndDataLoadParams *m_pCurrentlyLoading = gAEMSMgr.m_pAsyncLoadSDLP;
    if (m_pCurrentlyLoading == nullptr) {
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
    ClearSndAssetQueue(static_cast<ListNodeQueue *>(m_pQueuedFileHead));
    DestroySlots(true);
}

void EAXAemsManager::DestroySlots(bool bDoPFSlot) {
    if (bDoPFSlot == true) {
        reinterpret_cast<BankSlotSystem *>(&m_pPfSlotHead)->DestroySlots();
    }
    reinterpret_cast<BankSlotSystem *>(&m_pMainSlotHead)->DestroySlots();
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

    int nhandle =
        SNDAEMS_addmodulebank(*reinterpret_cast<void **>(reinterpret_cast<char *>(pCurLoad) + 0x28), nullptr, 0, ResidentAllocCB);
    SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    pCurLoad->Handle = nhandle;
}

void EAXAemsManager::CheckForCompleteAsyncLoad() {
    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurLoadSDLP) + 0x38) == 0) {
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurLoadSDLP) + 0x0) == 3) {
            if (SNDAEMS_asyncloadmodulebankdone() > 0) {
                CompleteAsyncLoad();
            }
        } else if ((*reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurLoadSDLP) + 0x0) > 3) &&
                   (*reinterpret_cast<int *>(reinterpret_cast<char *>(m_pCurLoadSDLP) + 0x0) == 4) &&
                   ((SNDAEMS_asyncloadmodulebankmemdone() > 0)) && (m_IsWaitingForFileCB == 0)) {
            CompleteAsyncLoad();
        }
    }
}
