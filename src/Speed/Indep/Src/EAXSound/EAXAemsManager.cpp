#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
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
extern void SNDSYS_service();
struct QueuedFileParams;
extern void AddQueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(int, int), int callback_param,
                          QueuedFileParams *params);
extern stSndDataLoadParams g_SndAssetList[];
extern Timer WorldTimer;

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

static stSndDataLoadParamsView &GetSndAssetParams(int index) {
    return *reinterpret_cast<stSndDataLoadParamsView *>(&g_SndAssetList[index]);
}

static void ResetSndAssetParams(stSndDataLoadParamsView &params) {
    params.AssetDescription.eDataType = -1;
    params.AssetDescription.FileName = Attrib::StringKey("");
    params.Handle = -1;
    params.AssetDescription.DataPath = static_cast<eSNDDATAPATH>(0);
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

static ListNodeQueue *&GetWaitQueueHead(EAXAemsManager *mgr) {
    return *reinterpret_cast<ListNodeQueue **>(reinterpret_cast<char *>(mgr) + 0xBC);
}

static ExternalLoadCallbackFn &GetExternalLoadCallback(EAXAemsManager *mgr) {
    return *reinterpret_cast<ExternalLoadCallbackFn *>(reinterpret_cast<char *>(mgr) + 0x4);
}

static int &GetExternalLoadCallbackParam(EAXAemsManager *mgr) {
    return *reinterpret_cast<int *>(reinterpret_cast<char *>(mgr) + 0x8);
}

static char *&GetAsyncBuffer(EAXAemsManager *mgr) {
    return *reinterpret_cast<char **>(reinterpret_cast<char *>(mgr) + 0x94);
}

static eTEMPALLOCLOCATION &GetAsyncBufferLocation(EAXAemsManager *mgr) {
    return *reinterpret_cast<eTEMPALLOCLOCATION *>(reinterpret_cast<char *>(mgr) + 0x98);
}

static int &GetBulkLoadFlag(EAXAemsManager *mgr) {
    return *reinterpret_cast<int *>(reinterpret_cast<char *>(mgr) + 0x128);
}
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
    *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xA4) = bankSlots;

    ListNodeBankSlot *pfSlots = reinterpret_cast<ListNodeBankSlot *>(gFastMem.Alloc(0x2C, nullptr));
    if (pfSlots != nullptr) {
        pfSlots->next = pfSlots;
        pfSlots->prev = pfSlots;
    }
    *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xAC) = pfSlots;

    ListNodeBankSlot *waitSlots = reinterpret_cast<ListNodeBankSlot *>(gFastMem.Alloc(0x30, nullptr));
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

    ClearSndAssetQueue(*reinterpret_cast<ListNodeQueue **>(reinterpret_cast<char *>(this) + 0xBC));
    ListNodeBankSlot *waitSlotHead = *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xBC);
    if (waitSlotHead != nullptr) {
        gFastMem.Free(waitSlotHead, 0x30, nullptr);
    }

    reinterpret_cast<BankSlotSystem *>(reinterpret_cast<char *>(this) + 0xAC)->DestroySlots();
    ListNodeBankSlot *pfSlotHead = *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xAC);
    if (pfSlotHead != nullptr) {
        gFastMem.Free(pfSlotHead, 0x2C, nullptr);
    }

    reinterpret_cast<BankSlotSystem *>(reinterpret_cast<char *>(this) + 0xA4)->DestroySlots();
    ListNodeBankSlot *slotHead = *reinterpret_cast<ListNodeBankSlot **>(reinterpret_cast<char *>(this) + 0xA4);
    if (slotHead != nullptr) {
        gFastMem.Free(slotHead, 0x2C, nullptr);
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

    int callback_param = *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x108);
    AddQueuedFile(m_pEvtSystems_start[callback_param], tempPath, 0, filesize, EvtSysLoadCallback, callback_param, nullptr);
    *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x108) = callback_param + 1;
    return callback_param;
}

void EAXAemsManager::InitializeSlots(bool bDoPFSlot) {
    if (DISABLE_SLOT_LOADING == 0) {
        if (bDoPFSlot) {
            reinterpret_cast<BankSlotSystem *>(reinterpret_cast<char *>(this) + 0xAC)->DestroySlots();
        }
        reinterpret_cast<BankSlotSystem *>(reinterpret_cast<char *>(this) + 0xA4)->DestroySlots();
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
        AddQueuedFile(pmem, tempPath, 0, fileSize, DataLoadCB, reinterpret_cast<int>(m_pCurLoadSDLP), nullptr);
        m_IsWaitingForFileCB = 1;
    } else if (memLocation == TMP_ALLOC_AUDIO) {
        void *pmem = gAudioMemoryManager.AllocateMemory(fileSize, filename, false);
        *reinterpret_cast<void **>(curLoad + 0x28) = pmem;
        AddQueuedFile(pmem, tempPath, 0, fileSize, DataLoadCB, reinterpret_cast<int>(m_pCurLoadSDLP), nullptr);
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
            Attrib::StringKey failedFile =
                reinterpret_cast<stSndDataLoadParamsView *>(m_pCurLoadSDLP)->AssetDescription.FileName;

            SndBase *sfxToDelete[32];
            bMemSet(sfxToDelete, '\0', sizeof(sfxToDelete));
            int deleteCount = 0;

            ListNodeQueue *head = *reinterpret_cast<ListNodeQueue **>(reinterpret_cast<char *>(this) + 0xBC);
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
    ListNodeQueue *head = *reinterpret_cast<ListNodeQueue **>(reinterpret_cast<char *>(this) + 0xBC);
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

    if (m_ItemsPendingAsyncResolve != 0 &&
        *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x128) != 0) {
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

        char *mem = *reinterpret_cast<char **>(reinterpret_cast<char *>(this) + 0x94);
        if (mem != nullptr) {
            eTEMPALLOCLOCATION location =
                *reinterpret_cast<eTEMPALLOCLOCATION *>(reinterpret_cast<char *>(this) + 0x98);
            if (location == TMP_ALLOC_AUDIO) {
                gAudioMemoryManager.FreeMemory(mem);
            } else if (location == TMP_ALLOC_MAIN) {
                bFree(mem);
            } else if (location == TMP_ALLOC_TRACKSTREAMER) {
                TheTrackStreamer.FreeUserMemory(mem);
            }
            *reinterpret_cast<char **>(reinterpret_cast<char *>(this) + 0x94) = nullptr;
        }

        ExternalLoadCallbackFn callback =
            *reinterpret_cast<ExternalLoadCallbackFn *>(reinterpret_cast<char *>(this) + 0x4);
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

        callback(*reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x8));
        *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x128) = 0;
        *reinterpret_cast<ExternalLoadCallbackFn *>(reinterpret_cast<char *>(this) + 0x4) = nullptr;
        *reinterpret_cast<int *>(reinterpret_cast<char *>(this) + 0x8) = 0;
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
    if (index < 0 || index >= m_nEndOfList) {
        return;
    }

    stSndDataLoadParamsView *entry = reinterpret_cast<stSndDataLoadParamsView *>(&g_SndAssetList[index]);
    entry->AssetDescription.eDataType = -1;
    entry->AssetDescription.FileName = Attrib::StringKey("");
    entry->Handle = -1;
    entry->AssetDescription.DataPath = static_cast<eSNDDATAPATH>(0);
    entry->MemLocation = static_cast<eTEMPALLOCLOCATION>(0);
    entry->mBankSlot = nullptr;
    entry->pmem = nullptr;
    entry->plocmem = nullptr;
    entry->nSize = 0;
    *reinterpret_cast<unsigned int *>(&entry->bResolvedAsync) = 0;
    *reinterpret_cast<unsigned int *>(&entry->bResolvedSync) = 0;
    {
        RawVector<unsigned int> &resallocs = *reinterpret_cast<RawVector<unsigned int> *>(&entry->resallocs);
        resallocs.finish = resallocs.start;
    }
    {
        RawVector<EAX_CarState *> &refCount = *reinterpret_cast<RawVector<EAX_CarState *> *>(&entry->RefCount);
        refCount.finish = refCount.start;
    }
    entry->t_load = 0;
    entry->t_req = 0;

    while (index < 0x2F) {
        const int next = index + 1;
        stSndDataLoadParamsView &current = *reinterpret_cast<stSndDataLoadParamsView *>(&g_SndAssetList[index]);
        stSndDataLoadParamsView &future = *reinterpret_cast<stSndDataLoadParamsView *>(&g_SndAssetList[next]);

        current.AssetDescription.eDataType = future.AssetDescription.eDataType;
        memmove(&current.AssetDescription.FileName, &future.AssetDescription.FileName, sizeof(Attrib::StringKey));
        current.AssetDescription.DataPath = future.AssetDescription.DataPath;
        *reinterpret_cast<unsigned int *>(&current.AssetDescription.bLoadToTop) =
            *reinterpret_cast<unsigned int *>(&future.AssetDescription.bLoadToTop);

        current.MemLocation = future.MemLocation;
        current.mBankSlot = future.mBankSlot;
        if (current.mBankSlot != nullptr && current.mBankSlot->pAssetParams == &g_SndAssetList[next]) {
            current.mBankSlot->pAssetParams = &g_SndAssetList[index];
        }

        current.pmem = future.pmem;
        current.plocmem = future.plocmem;
        current.nSize = future.nSize;
        current.Handle = future.Handle;
        *reinterpret_cast<unsigned int *>(&current.bResolvedAsync) =
            *reinterpret_cast<unsigned int *>(&future.bResolvedAsync);
        *reinterpret_cast<unsigned int *>(&current.bResolvedSync) =
            *reinterpret_cast<unsigned int *>(&future.bResolvedSync);

        RawVector<unsigned int> &currentResallocs = *reinterpret_cast<RawVector<unsigned int> *>(&current.resallocs);
        RawVector<unsigned int> &futureResallocs = *reinterpret_cast<RawVector<unsigned int> *>(&future.resallocs);
        currentResallocs.finish = currentResallocs.start;
        {
            int reserveSize = static_cast<int>(futureResallocs.finish - futureResallocs.start);
            int currentCapacity = static_cast<int>(currentResallocs.end_of_storage - currentResallocs.start);
            if (currentCapacity < reserveSize) {
                int currentSize = static_cast<int>(currentResallocs.finish - currentResallocs.start);
                unsigned int *newStart =
                    reserveSize != 0 ? reinterpret_cast<unsigned int *>(gFastMem.Alloc(reserveSize * 4, nullptr)) : nullptr;
                if (currentSize != 0) {
                    memmove(newStart, currentResallocs.start, static_cast<size_t>(currentSize * 4));
                }
                if (currentResallocs.start != nullptr) {
                    gFastMem.Free(currentResallocs.start, static_cast<unsigned int>(currentCapacity * 4), nullptr);
                }
                currentResallocs.start = newStart;
                currentResallocs.finish = newStart + currentSize;
                currentResallocs.end_of_storage = newStart + reserveSize;
            }
        }
        for (unsigned int *it = futureResallocs.start; it != futureResallocs.finish; ++it) {
            if (currentResallocs.finish == currentResallocs.end_of_storage) {
                int currentSize = static_cast<int>(currentResallocs.finish - currentResallocs.start);
                int growSize = currentSize == 0 ? 1 : currentSize;
                int newCapacity = currentSize + growSize;
                unsigned int *newStart = reinterpret_cast<unsigned int *>(gFastMem.Alloc(newCapacity * 4, nullptr));
                if (currentSize != 0) {
                    memmove(newStart, currentResallocs.start, static_cast<size_t>(currentSize * 4));
                }
                if (currentResallocs.start != nullptr) {
                    int oldCapacity = static_cast<int>(currentResallocs.end_of_storage - currentResallocs.start);
                    gFastMem.Free(currentResallocs.start, static_cast<unsigned int>(oldCapacity * 4), nullptr);
                }
                currentResallocs.start = newStart;
                currentResallocs.finish = newStart + currentSize;
                currentResallocs.end_of_storage = newStart + newCapacity;
            }
            if (currentResallocs.finish != nullptr) {
                *currentResallocs.finish = *it;
            }
            currentResallocs.finish++;
        }
        futureResallocs.finish = futureResallocs.start;

        RawVector<EAX_CarState *> &currentRefCount =
            *reinterpret_cast<RawVector<EAX_CarState *> *>(&current.RefCount);
        RawVector<EAX_CarState *> &futureRefCount =
            *reinterpret_cast<RawVector<EAX_CarState *> *>(&future.RefCount);
        currentRefCount.finish = currentRefCount.start;
        {
            int reserveSize = static_cast<int>(futureRefCount.finish - futureRefCount.start);
            int currentCapacity = static_cast<int>(currentRefCount.end_of_storage - currentRefCount.start);
            if (currentCapacity < reserveSize) {
                int currentSize = static_cast<int>(currentRefCount.finish - currentRefCount.start);
                EAX_CarState **newStart =
                    reserveSize != 0 ? reinterpret_cast<EAX_CarState **>(gFastMem.Alloc(reserveSize * 4, nullptr)) : nullptr;
                if (currentSize != 0) {
                    memmove(newStart, currentRefCount.start, static_cast<size_t>(currentSize * 4));
                }
                if (currentRefCount.start != nullptr) {
                    gFastMem.Free(currentRefCount.start, static_cast<unsigned int>(currentCapacity * 4), nullptr);
                }
                currentRefCount.start = newStart;
                currentRefCount.finish = newStart + currentSize;
                currentRefCount.end_of_storage = newStart + reserveSize;
            }
        }
        for (EAX_CarState **it = futureRefCount.start; it != futureRefCount.finish; ++it) {
            if (currentRefCount.finish == currentRefCount.end_of_storage) {
                int currentSize = static_cast<int>(currentRefCount.finish - currentRefCount.start);
                int growSize = currentSize == 0 ? 1 : currentSize;
                int newCapacity = currentSize + growSize;
                EAX_CarState **newStart =
                    reinterpret_cast<EAX_CarState **>(gFastMem.Alloc(newCapacity * 4, nullptr));
                if (currentSize != 0) {
                    memmove(newStart, currentRefCount.start, static_cast<size_t>(currentSize * 4));
                }
                if (currentRefCount.start != nullptr) {
                    int oldCapacity = static_cast<int>(currentRefCount.end_of_storage - currentRefCount.start);
                    gFastMem.Free(currentRefCount.start, static_cast<unsigned int>(oldCapacity * 4), nullptr);
                }
                currentRefCount.start = newStart;
                currentRefCount.finish = newStart + currentSize;
                currentRefCount.end_of_storage = newStart + newCapacity;
            }
            if (currentRefCount.finish != nullptr) {
                *currentRefCount.finish = *it;
            }
            currentRefCount.finish++;
        }
        futureRefCount.finish = futureRefCount.start;

        current.t_req = future.t_req;
        current.t_load = future.t_load;

        future.AssetDescription.eDataType = -1;
        future.AssetDescription.FileName = Attrib::StringKey("");
        future.Handle = -1;
        future.AssetDescription.DataPath = static_cast<eSNDDATAPATH>(0);
        future.MemLocation = static_cast<eTEMPALLOCLOCATION>(0);
        future.mBankSlot = nullptr;
        future.pmem = nullptr;
        future.plocmem = nullptr;
        future.nSize = 0;
        *reinterpret_cast<unsigned int *>(&future.bResolvedAsync) = 0;
        *reinterpret_cast<unsigned int *>(&future.bResolvedSync) = 0;
        {
            RawVector<unsigned int> &resallocs = *reinterpret_cast<RawVector<unsigned int> *>(&future.resallocs);
            resallocs.finish = resallocs.start;
        }
        {
            RawVector<EAX_CarState *> &refCount = *reinterpret_cast<RawVector<EAX_CarState *> *>(&future.RefCount);
            refCount.finish = refCount.start;
        }
        future.t_load = 0;
        future.t_req = 0;
        index = next;
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
    return gAudioMemoryManager.AllocateMemory(size, dbgstring, true);
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
        resmem = gAudioMemoryManager.AllocateMemory(residentsize, filename, false);
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
    ClearSndAssetQueue(*reinterpret_cast<ListNodeQueue **>(reinterpret_cast<char *>(this) + 0xBC));
    DestroySlots(true);
}

void EAXAemsManager::DestroySlots(bool bDoPFSlot) {
    if (bDoPFSlot == true) {
        reinterpret_cast<BankSlotSystem *>(reinterpret_cast<char *>(this) + 0xAC)->DestroySlots();
    }
    reinterpret_cast<BankSlotSystem *>(reinterpret_cast<char *>(this) + 0xA4)->DestroySlots();
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
