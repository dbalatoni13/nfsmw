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

DECLARE_CONTAINER_TYPE(ResAllocList);
DECLARE_CONTAINER_TYPE(RefCountList);
DECLARE_CONTAINER_TYPE(SndAssetQueue);

typedef UTL::Std::vector<unsigned int, _type_ResAllocList> ResAllocList;
typedef UTL::Std::vector<EAX_CarState *, _type_RefCountList> RefCountList;
typedef UTL::Std::list<stSndAssetQueue, _type_SndAssetQueue> SndAssetQueue;
typedef void (*ExternalLoadCallbackFn)(int);

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

template <typename T> struct RawVector {
    T *start;
    T *finish;
    void *allocator;
    T *end_of_storage;
};

struct StringKeyView {
    unsigned long long mHash64;
    unsigned int mHash32;
    const char *mString;
};

struct stAssetDescriptionClearView {
    int eDataType;
    int _pad4;
    StringKeyView FileName;
    int DataPath;
    int bLoadToTop;
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
    bool bResolvedSync;
    ResAllocList resallocs;
    RefCountList RefCount;
    Timer t_req;
    Timer t_load;
};

struct stSndDataLoadParamsClearView {
    stAssetDescriptionClearView AssetDescription;
    eTEMPALLOCLOCATION MemLocation;
    stBankSlot *mBankSlot;
    void *pmem;
    void *plocmem;
    int nSize;
    int Handle;
    int bResolvedAsync;
    int bResolvedSync;
    ResAllocList resallocs;
    RefCountList RefCount;
    Timer t_req;
    Timer t_load;
};

struct stSndDataLoadParamsAllocView {
    char _assetDescription[0x20];
    eTEMPALLOCLOCATION MemLocation;
    stBankSlot *mBankSlot;
    void *pmem;
    void *plocmem;
};

template <typename T, typename Tag> static RawVector<T> &AsRawVector(UTL::Std::vector<T, Tag> &vec) {
    return *static_cast<RawVector<T> *>(static_cast<void *>(&vec));
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
    T *newStart = byteCount > 0 ? static_cast<T *>(gFastMem.Alloc(byteCount, nullptr)) : nullptr;

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
    params.bResolvedAsync = false;
    params.bResolvedSync = false;
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
    return *static_cast<eTEMPALLOCLOCATION *>(static_cast<void *>(&mgr->m_AsyncBufferLocation));
}

static stSndDataLoadParamsView &AsLoadParamsView(stSndDataLoadParams *params) {
    return *static_cast<stSndDataLoadParamsView *>(static_cast<void *>(params));
}

static const stSndDataLoadParamsView &AsLoadParamsView(const stSndDataLoadParams *params) {
    return *static_cast<const stSndDataLoadParamsView *>(static_cast<const void *>(params));
}

static const char *GetStringKeyChars(const Attrib::StringKey &key) {
    return *static_cast<const char *const *>(
        static_cast<const void *>(static_cast<const char *>(static_cast<const void *>(&key)) + 0xC));
}
} // namespace

void stAssetDescription::Clear() {
    eDataType = -1;
    FileName.operator=(Attrib::StringKey(""));
    DataPath = 0;
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
    stSndDataLoadParamsClearView &view = *static_cast<stSndDataLoadParamsClearView *>(static_cast<void *>(this));
    view.AssetDescription.eDataType = -1;
    const char *empty = "";
    StringKeyView key;
    key.mHash64 = Attrib::StringHash64(empty);
    key.mHash32 = Attrib::StringHash32(empty);
    key.mString = empty;
    view.AssetDescription.FileName.mString = key.mString;
    view.AssetDescription.FileName.mHash64 = key.mHash64;
    view.AssetDescription.FileName.mHash32 = key.mHash32;
    view.Handle = -1;
    view.bResolvedSync = 0;
    view.AssetDescription.DataPath = 0;
    view.MemLocation = static_cast<eTEMPALLOCLOCATION>(0);
    view.mBankSlot = nullptr;
    view.pmem = nullptr;
    view.plocmem = nullptr;
    view.nSize = 0;
    view.bResolvedAsync = 0;
    view.resallocs.clear();
    view.RefCount.clear();
    view.t_req = Timer(0);
    view.t_load = Timer(0);
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
            stAssetDescription *asset =
                static_cast<stAssetDescription *>(static_cast<void *>(node->data.pAssetParams));
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
    ListNodeBankSlot *bankSlots = static_cast<ListNodeBankSlot *>(gFastMem.Alloc(0x2C, nullptr));
    if (bankSlots != nullptr) {
        bankSlots->next = bankSlots;
        bankSlots->prev = bankSlots;
    }
    m_pMainSlotHead = bankSlots;

    ListNodeBankSlot *pfSlots = static_cast<ListNodeBankSlot *>(gFastMem.Alloc(0x2C, nullptr));
    if (pfSlots != nullptr) {
        pfSlots->next = pfSlots;
        pfSlots->prev = pfSlots;
    }
    m_pPfSlotHead = pfSlots;

    ListNodeQueue *waitSlots = static_cast<ListNodeQueue *>(gFastMem.Alloc(0x30, nullptr));
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
        int byteCount = static_cast<int>(
            static_cast<char *>(static_cast<void *>(m_pEvtSystems_end_of_storage)) -
            static_cast<char *>(static_cast<void *>(m_pEvtSystems_start)));
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

    static_cast<BankSlotSystem *>(static_cast<void *>(&m_pPfSlotHead))->DestroySlots();
    ListNodeBankSlot *pfSlotHead = static_cast<ListNodeBankSlot *>(m_pPfSlotHead);
    if (pfSlotHead != nullptr) {
        gFastMem.Free(pfSlotHead, 0x2C, nullptr);
    }

    static_cast<BankSlotSystem *>(static_cast<void *>(&m_pMainSlotHead))->DestroySlots();
    ListNodeBankSlot *slotHead = static_cast<ListNodeBankSlot *>(m_pMainSlotHead);
    if (slotHead != nullptr) {
        gFastMem.Free(slotHead, 0x2C, nullptr);
    }
}

void EAXAemsManager::Init() {
    Attrib::Gen::audiosystem *attrs = g_pEAXSound->GetAttributes();
    unsigned int numEvtSys = attrs->Num_EvtSys();
    m_nEvtSysCount = static_cast<int>(numEvtSys);
    m_nResidentAllocs = 0;
    m_nEvtSysQueued = 0;
    if (static_cast<int>(numEvtSys) > 0) {
        if (m_pEvtSystems_start != nullptr) {
            int oldByteCount = static_cast<int>(
                static_cast<char *>(static_cast<void *>(m_pEvtSystems_end_of_storage)) -
                static_cast<char *>(static_cast<void *>(m_pEvtSystems_start)));
            gFastMem.Free(m_pEvtSystems_start, oldByteCount, nullptr);
        }

        m_pEvtSystems_start = static_cast<void **>(
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
    Attrib::Gen::audiosystem *attrs = g_pEAXSound->GetAttributes();
    const Attrib::StringKey &evtStringKey = attrs->EvtSys(eESIndex);
    const char *evtName = *static_cast<const char *const *>(
        static_cast<const void *>(static_cast<const char *>(static_cast<const void *>(&evtStringKey)) + 0xC));
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
            static_cast<BankSlotSystem *>(static_cast<void *>(&m_pPfSlotHead))->DestroySlots();
        }
        static_cast<BankSlotSystem *>(static_cast<void *>(&m_pMainSlotHead))->DestroySlots();
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
        NewSlot.MAINmemLocation = static_cast<char *>(
            gAudioMemoryManager.AllocateMemory(SizePerSlotMainMem, DebugString, false));
        NewSlot.BANKmemLocation = m_SPUMainAllocsEnd - SizePerSlotSPU;
        m_SPUMainAllocsEnd = NewSlot.BANKmemLocation;
        NewSlot.pAssetParams = nullptr;
        NewSlot.pLastAlloc = NewSlot.MAINmemLocation;
        NewSlot.MAINmemSize = SizePerSlotMainMem;
        NewSlot.BANKMemSize = SizePerSlotSPU;

        ListNodeBankSlot *head = *static_cast<ListNodeBankSlot **>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(this)) + ((Type == eBANK_SLOT_PATHFINDER && bDoPFSlot) ? 0xAC : 0xA4)));
        ListNodeBankSlot *node = static_cast<ListNodeBankSlot *>(gFastMem.Alloc(0x2C, nullptr));
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
    EAXAemsManager *mgr = &gAEMSMgr;
    stSndDataLoadParamsAllocView *curLoad =
        static_cast<stSndDataLoadParamsAllocView *>(static_cast<void *>(mgr->m_pCurLoadSDLP));
    eTEMPALLOCLOCATION MemLocation = curLoad->MemLocation;

    if (MemLocation == TMP_ALLOC_MAIN) {
        bFree(curLoad->pmem);
    } else if (MemLocation <= TMP_ALLOC_MAIN) {
        return;
    } else if (MemLocation == TMP_ALLOC_TRACKSTREAMER) {
        TheTrackStreamer.FreeUserMemory(curLoad->pmem);
    } else if (MemLocation == TMP_ALLOC_AUDIO) {
        if (curLoad->plocmem == nullptr) {
            if (curLoad->mBankSlot == nullptr) {
                return;
            }
        }
        gAudioMemoryManager.FreeMemory(curLoad->pmem);
    } else {
        return;
    }
    static_cast<stSndDataLoadParamsAllocView *>(static_cast<void *>(mgr->m_pCurLoadSDLP))->pmem = nullptr;
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

    currentLoad = static_cast<stSndDataLoadParamsView *>(static_cast<void *>(m_pCurLoadSDLP));
    fileString = *static_cast<char **>(
        static_cast<void *>(static_cast<char *>(static_cast<void *>(&currentLoad->AssetDescription.FileName)) + 0xC));
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
            fileString = static_cast<char *>(bMalloc(0x10000, 0));
            GetAsyncBuffer(this) = fileString;
            goto HaveAsyncBuffer;
        }
        fileString = static_cast<char *>(TheTrackStreamer.AllocateUserMemory(0x10000, "EAXAemsManager::m_pAsyncBuff", 0));
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
        *static_cast<unsigned int *>(static_cast<void *>(&queuedFileParams.Compressed)) = 0;
        queuedFileParams.Priority = QueuedFileDefaultPriority - 2;
        queuedFileParams.UncompressedSize = 0;
        currentLoad = static_cast<stSndDataLoadParamsView *>(static_cast<void *>(m_pCurLoadSDLP));
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
            m_IsWaitingForFileCB = memLocation;
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
                currentLoad = static_cast<stSndDataLoadParamsView *>(static_cast<void *>(m_pCurLoadSDLP));
                fileString = *static_cast<char **>(
                    static_cast<void *>(static_cast<char *>(static_cast<void *>(&currentLoad->AssetDescription.FileName)) + 0xC));
                if (fileString == nullptr) {
                    fileString = "";
                }
                allocatedMemory = TheTrackStreamer.AllocateUserMemory(currentLoad->nSize, fileString, 0);
                currentLoad->pmem = allocatedMemory;
                fileString = static_cast<char *>(currentLoad->pmem);
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
                    currentLoad = static_cast<stSndDataLoadParamsView *>(static_cast<void *>(m_pCurLoadSDLP));
                    if (result < currentLoad->nSize) {
                        return -4;
                    }
                    fileString = *static_cast<char **>(
                        static_cast<void *>(static_cast<char *>(static_cast<void *>(&currentLoad->AssetDescription.FileName)) + 0xC));
                    if (fileString == nullptr) {
                        fileString = "";
                    }
                    allocatedMemory = gAudioMemoryManager.AllocateMemory(currentLoad->nSize, fileString,
                                                                         currentLoad->AssetDescription.bLoadToTop);
                    currentLoad->pmem = allocatedMemory;
                    fileString = static_cast<char *>(currentLoad->pmem);
                    result = currentLoad->nSize;
                }
            }
            AddQueuedFile(fileString, m_LoadFilename, 0, result, DataLoadCB,
                          reinterpret_cast<int>(currentLoad), &queuedFileParams);
            m_IsWaitingForFileCB = 1;
        }
CheckAsyncSpuLoad:
        currentLoad = static_cast<stSndDataLoadParamsView *>(static_cast<void *>(m_pCurLoadSDLP));
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
    currentLoad = static_cast<stSndDataLoadParamsView *>(static_cast<void *>(m_pCurLoadSDLP));
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
            stSndDataLoadParamsView *currentLoad =
                static_cast<stSndDataLoadParamsView *>(static_cast<void *>(m_pCurLoadSDLP));
            Attrib::StringKey failedFile = currentLoad->AssetDescription.FileName;

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

        ExternalLoadCallbackFn callback = m_ExternalLoadCallback;
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
        *static_cast<stSndDataLoadParamsView *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pCurLoadSDLP))));
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
    ResetSndAssetParams(*static_cast<stSndDataLoadParamsView *>(static_cast<void *>(&g_SndAssetList[index])));
    for (int n = index; n < 0x2F; n++) {
        stSndDataLoadParams *currentParams = g_SndAssetList + n;
        stSndDataLoadParams *futureParams = currentParams + 1;
        stSndDataLoadParamsView &current =
            *static_cast<stSndDataLoadParamsView *>(static_cast<void *>(currentParams));
        stSndDataLoadParamsView &future =
            *static_cast<stSndDataLoadParamsView *>(static_cast<void *>(futureParams));

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
        *static_cast<unsigned int *>(static_cast<void *>(&current.bResolvedAsync)) =
            *static_cast<unsigned int *>(static_cast<void *>(&future.bResolvedAsync));
        *static_cast<unsigned int *>(static_cast<void *>(&current.bResolvedSync)) =
            *static_cast<unsigned int *>(static_cast<void *>(&future.bResolvedSync));

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
    char *entry = static_cast<char *>(static_cast<void *>(m_pCurUNLOADSDLP));
    if (*static_cast<int *>(static_cast<void *>(entry + 0x3C)) == 0) {
        return;
    }

    *static_cast<int *>(static_cast<void *>(entry + 0x38)) = 0;
    *static_cast<int *>(static_cast<void *>(entry + 0x3C)) = 0;
    int eDataType = *static_cast<int *>(static_cast<void *>(entry + 0x0));
    if (eDataType < SDT_GENERIC_DATA) {
        RemoveAEMSBank();
    }

    void *plocmem = *static_cast<void **>(static_cast<void *>(entry + 0x2C));
    if (plocmem != nullptr) {
        if (eDataType == SDT_AEMS_MAINMEM) {
            bFree(plocmem);
        } else {
            gAudioMemoryManager.FreeMemory(plocmem);
        }
        *static_cast<void **>(static_cast<void *>(entry + 0x2C)) = nullptr;
    }

    void *pmem = *static_cast<void **>(static_cast<void *>(entry + 0x28));
    if (pmem != nullptr) {
        if (eDataType == SDT_AEMS_MAINMEM) {
            bFree(pmem);
        } else {
            gAudioMemoryManager.FreeMemory(pmem);
        }
        *static_cast<void **>(static_cast<void *>(entry + 0x28)) = nullptr;
    }

    stBankSlot *bankSlot = *static_cast<stBankSlot **>(static_cast<void *>(entry + 0x24));
    if (bankSlot != nullptr) {
        bankSlot->pAssetParams = nullptr;
        bankSlot->pLastAlloc = bankSlot->MAINmemLocation;
        bMemSet(bankSlot->MAINmemLocation, '\0', bankSlot->MAINmemSize);
        *static_cast<stBankSlot **>(static_cast<void *>(entry + 0x24)) = nullptr;
    }

    unsigned int **resStart = static_cast<unsigned int **>(static_cast<void *>(entry + 0x40));
    unsigned int **resFinish = static_cast<unsigned int **>(static_cast<void *>(entry + 0x44));
    while (*resStart != *resFinish) {
        unsigned int *cur = *resFinish - 1;
        *resFinish = cur;
        gAudioMemoryManager.FreeMemory(reinterpret_cast<void *>(*cur));
    }

    RemoveBankListing(Index);
    m_pCurUNLOADSDLP = nullptr;
}

void *EAXAemsManager::AsyncResidentAllocCB(int size) {
    stSndDataLoadParams *tmpLoading = gAEMSMgr.m_pAsyncLoadSDLP;
    stSndDataLoadParams *m_pCurrentlyLoading;
    void *newresalloc;
    if (tmpLoading == nullptr) {
        m_pCurrentlyLoading = gAEMSMgr.m_pCurLoadSDLP;
    } else {
        m_pCurrentlyLoading = tmpLoading;
    }

    char *currentLoad = static_cast<char *>(static_cast<void *>(m_pCurrentlyLoading));
    stBankSlot *pBankSlot = *static_cast<stBankSlot **>(static_cast<void *>(currentLoad + 0x24));
    if (pBankSlot != nullptr) {
        newresalloc = pBankSlot->pLastAlloc;
        pBankSlot->pLastAlloc += size;
    } else {
        char *filename = *static_cast<char **>(static_cast<void *>(currentLoad + 0x14));
        if (filename == nullptr) {
            filename = const_cast<char *>("");
        }
        char dbgstring[64];
        bStrCat(dbgstring, filename, ": resident allocation");
        newresalloc = gAudioMemoryManager.AllocateMemory(size, dbgstring, true);
        unsigned int newresallocaddr = reinterpret_cast<unsigned int>(newresalloc);
        static_cast<ResAllocList *>(static_cast<void *>(currentLoad + 0x40))->push_back(newresallocaddr);
    }
    return newresalloc;
}

void *EAXAemsManager::ResidentAllocCB(void *pbank, int residentsize, int totalsize) {
    (void)pbank;
    if (residentsize != totalsize) {
        stSndDataLoadParams *currentLoadRaw = gAEMSMgr.m_pCurLoadSDLP;
        stSndDataLoadParamsView &currentLoad = AsLoadParamsView(currentLoadRaw);
        void *resmem;
        if (currentLoad.AssetDescription.eDataType == SDT_AEMS_MAINMEM) {
            resmem = bMalloc(residentsize, 0x1040);
        } else {
            stBankSlot *pBankSlot = currentLoad.mBankSlot;
            if (pBankSlot != nullptr) {
                pBankSlot->pLastAlloc += residentsize;
                currentLoadRaw = gAEMSMgr.m_pCurLoadSDLP;
                pBankSlot = AsLoadParamsView(currentLoadRaw).mBankSlot;
                return pBankSlot->MAINmemLocation;
            }

            char *filename = const_cast<char *>(GetStringKeyChars(currentLoad.AssetDescription.FileName));
            if (filename == nullptr) {
                filename = const_cast<char *>("");
            }
            resmem = gAudioMemoryManager.AllocateMemory(residentsize, filename, false);
        }

        currentLoadRaw = gAEMSMgr.m_pCurLoadSDLP;
        AsLoadParamsView(currentLoadRaw).plocmem = resmem;
        gAEMSMgr.m_nResidentAllocs++;
        currentLoadRaw = gAEMSMgr.m_pCurLoadSDLP;
        return AsLoadParamsView(currentLoadRaw).plocmem;
    }

    return AsLoadParamsView(gAEMSMgr.m_pCurLoadSDLP).pmem;
}

void EAXAemsManager::DataLoadCB(int param, int error_status) {
    (void)error_status;
    unsigned int ticker = bGetTicker();
    char *mgr = static_cast<char *>(static_cast<void *>(&gAEMSMgr));
    int worldTime = *static_cast<int *>(static_cast<void *>(&WorldTimer));
    StartBankLoadTicks = ticker;
    *static_cast<stSndDataLoadParams **>(static_cast<void *>(mgr + 0x118)) = reinterpret_cast<stSndDataLoadParams *>(param);
    char *currentLoad = static_cast<char *>(
        static_cast<void *>(*static_cast<stSndDataLoadParams **>(static_cast<void *>(mgr + 0x118))));
    *static_cast<int *>(static_cast<void *>(currentLoad + 0x64)) = worldTime;
    int eDataType = *static_cast<int *>(static_cast<void *>(currentLoad + 0x0));

    if (eDataType == SDT_AEMS_ASYNCSPU) {
        goto LoadDone;
    }

    if (eDataType <= SDT_AEMS_SYNCSPU) {
        if (eDataType >= SDT_AEMS_AUDIOMEM) {
            AddAemsBank();
            gAEMSMgr.ResolveCurrentDataMemory();
            *static_cast<unsigned int *>(static_cast<void *>(currentLoad + 0x38)) = 1;
            *static_cast<unsigned int *>(static_cast<void *>(currentLoad + 0x3C)) = 1;
        }
    } else if (eDataType == SDT_AEMS_ASYNCSPUMEM) {
        *static_cast<unsigned int *>(static_cast<void *>(mgr + 0x12C)) = 0;
        if (*static_cast<int *>(static_cast<void *>(mgr + 0x128)) != 0) {
            *static_cast<stSndDataLoadParams **>(static_cast<void *>(mgr + 0x118)) = nullptr;
            (*static_cast<int *>(static_cast<void *>(mgr + 0x124)))++;
            goto LoadDone;
        }

        stBankSlot *pBankSlot = *static_cast<stBankSlot **>(static_cast<void *>(currentLoad + 0x24));
        if (pBankSlot == nullptr) {
            SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
        } else {
            SNDmemlimits(pBankSlot->BANKmemLocation, pBankSlot->BANKmemLocation + pBankSlot->BANKMemSize);
        }

        *static_cast<int *>(static_cast<void *>(currentLoad + 0x34)) =
            SNDAEMS_asyncloadmodulebankmem(*static_cast<void **>(static_cast<void *>(currentLoad + 0x28)), nullptr, 0,
                                           AsyncResidentAllocCB);
    } else if (eDataType == SDT_GENERIC_DATA) {
        *static_cast<unsigned int *>(static_cast<void *>(currentLoad + 0x38)) = 1;
        *static_cast<unsigned int *>(static_cast<void *>(currentLoad + 0x3C)) = 1;
    }

LoadDone:
    *static_cast<unsigned int *>(static_cast<void *>(mgr + 0x12C)) = 0;
}

int EAXAemsManager::AddBankListing(stAssetDescription &asset) {
    stSndDataLoadParamsView *entry =
        static_cast<stSndDataLoadParamsView *>(static_cast<void *>(g_SndAssetList + m_nEndOfList));
    ResetSndAssetParams(*entry);

    stSndDataLoadParamsView &dst =
        *static_cast<stSndDataLoadParamsView *>(static_cast<void *>(g_SndAssetList + m_nEndOfList));
    dst.AssetDescription.eDataType = asset.eDataType;
    static_cast<StringKeyView *>(static_cast<void *>(&dst.AssetDescription.FileName))->mString =
        static_cast<const StringKeyView *>(static_cast<const void *>(&asset.FileName))->mString;
    static_cast<StringKeyView *>(static_cast<void *>(&dst.AssetDescription.FileName))->mHash64 =
        static_cast<const StringKeyView *>(static_cast<const void *>(&asset.FileName))->mHash64;
    static_cast<StringKeyView *>(static_cast<void *>(&dst.AssetDescription.FileName))->mHash32 =
        static_cast<const StringKeyView *>(static_cast<const void *>(&asset.FileName))->mHash32;
    dst.AssetDescription.DataPath = asset.DataPath;
    dst.AssetDescription.bLoadToTop = asset.bLoadToTop;

    return m_nEndOfList++;
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
                slot = static_cast<BankSlotSystem *>(static_cast<void *>(&m_pPfSlotHead))->GetFreeSlot(SlotType);
            } else {
                slot = static_cast<BankSlotSystem *>(static_cast<void *>(&m_pMainSlotHead))->GetFreeSlot(SlotType);
            }
            if (slot != nullptr) {
                slot->pAssetParams = &g_SndAssetList[bankIndex];
                *static_cast<stBankSlot **>(
                    static_cast<void *>(static_cast<char *>(static_cast<void *>(&g_SndAssetList[bankIndex])) + 0x24)) = slot;
            }
        }
    }

    ListNodeQueue *head = static_cast<ListNodeQueue *>(m_pQueuedFileHead);
    ListNodeQueue *node = static_cast<ListNodeQueue *>(gFastMem.Alloc(0x30, nullptr));
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
    return *static_cast<void **>(
        static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pEvtSystems_start)) + offset));
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
    stSndDataLoadParams *m_pCurrentlyLoading;
    int Result;
    float delta;
    m_pCurrentlyLoading = gAEMSMgr.m_pAsyncLoadSDLP;
    if (gAEMSMgr.m_pAsyncLoadSDLP == nullptr) {
        m_pCurrentlyLoading = gAEMSMgr.m_pCurLoadSDLP;
    }
    *static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pCurrentlyLoading)) + 0x38)) = 1;
    SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    *static_cast<Timer *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pCurrentlyLoading)) + 0x64)) =
        WorldTimer;
    delta = (WorldTimer - *static_cast<Timer *>(static_cast<void *>(&StartBankLoadTicks))).GetSeconds();
}

void EAXAemsManager::ResetBankLoadParams() {
    m_nCurLoadedBankIndex = -1;
    m_nEndOfList = 0;
    static_cast<SndAssetQueue *>(static_cast<void *>(&m_pQueuedFileHead))->clear();
    DestroySlots(true);
}

void EAXAemsManager::DestroySlots(bool bDoPFSlot) {
    if (bDoPFSlot == true) {
        static_cast<BankSlotSystem *>(static_cast<void *>(&m_pPfSlotHead))->DestroySlots();
    }
    static_cast<BankSlotSystem *>(static_cast<void *>(&m_pMainSlotHead))->DestroySlots();
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
            if (*static_cast<unsigned int *>(static_cast<void *>(&filename)) ==
                *static_cast<unsigned int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(&g_SndAssetList[n])) + 0x8))) {
                bAssetMatch = *static_cast<unsigned int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(&filename)) + 0x4)) ==
                              *static_cast<unsigned int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(&g_SndAssetList[n])) + 0xC));
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
        if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(&g_SndAssetList[n])) + 0x3C)) != 0) {
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
    stBankSlot *pBankSlot =
        *static_cast<stBankSlot **>(static_cast<void *>(static_cast<char *>(static_cast<void *>(pCurLoad)) + 0x24));
    if (pBankSlot == nullptr) {
        SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    } else {
        SNDmemlimits(pBankSlot->BANKmemLocation, pBankSlot->BANKmemLocation + pBankSlot->BANKMemSize);
    }

    int nhandle = SNDAEMS_addmodulebank(
        *static_cast<void **>(static_cast<void *>(static_cast<char *>(static_cast<void *>(pCurLoad)) + 0x28)), nullptr, 0,
        ResidentAllocCB);
    SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    pCurLoad->Handle = nhandle;
}

void EAXAemsManager::CheckForCompleteAsyncLoad() {
    if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pCurLoadSDLP)) + 0x38)) == 0) {
        if (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pCurLoadSDLP)) + 0x0)) == 3) {
            if (SNDAEMS_asyncloadmodulebankdone() > 0) {
                CompleteAsyncLoad();
            }
        } else if ((*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pCurLoadSDLP)) + 0x0)) > 3) &&
                   (*static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pCurLoadSDLP)) + 0x0)) == 4) &&
                   ((SNDAEMS_asyncloadmodulebankmemdone() > 0)) && (m_IsWaitingForFileCB == 0)) {
            CompleteAsyncLoad();
        }
    }
}
