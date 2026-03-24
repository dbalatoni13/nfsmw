#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
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

namespace {
template <typename T> struct RawVector {
    T *start;
    T *finish;
    void *allocator;
    T *end_of_storage;
};

typedef void (*ExternalLoadCallbackFn)(int);

struct BankSlotNode {
    BankSlotNode *next;
    BankSlotNode *prev;
    stBankSlot data;
};

struct ListNodeQueue {
    ListNodeQueue *next;
    ListNodeQueue *prev;
    stSndAssetQueue data;
};

static void ClearSndAssetQueue(SndAssetQueue &queue) {
    queue.clear();
}

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

static void ResetSndAssetParams(stSndDataLoadParams &params) {
    params.AssetDescription.eDataType = EAXSND_DT_NONE;
    params.AssetDescription.FileName = Attrib::StringKey("");
    params.AssetDescription.DataPath = SNDPATH_ROUTE;
    params.Handle = -1;
    params.MemLocation = TMP_ALLOC_NONE;
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

} // namespace

inline void stAssetDescription::Clear() {
    eDataType = EAXSND_DT_NONE;
    FileName = Attrib::StringKey("");
    DataPath = SNDPATH_ROUTE;
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

inline void stSndDataLoadParams::Clear() {
    AssetDescription.Clear();
    Handle = -1;
    bResolvedSync = false;
    MemLocation = TMP_ALLOC_NONE;
    mBankSlot = nullptr;
    pmem = nullptr;
    plocmem = nullptr;
    nSize = 0;
    bResolvedAsync = false;
    resallocs.clear();
    RefCount.clear();
    t_req = Timer(0);
    t_load = Timer(0);
}

inline stSndDataLoadParams &stSndDataLoadParams::operator=(stSndDataLoadParams &copy) {
    AssetDescription = copy.AssetDescription;
    MemLocation = copy.MemLocation;
    mBankSlot = copy.mBankSlot;
    if (mBankSlot != nullptr && mBankSlot->pAssetParams == &copy) {
        mBankSlot->pAssetParams = this;
    }

    pmem = copy.pmem;
    plocmem = copy.plocmem;
    nSize = copy.nSize;
    Handle = copy.Handle;
    *static_cast<unsigned int *>(static_cast<void *>(&bResolvedAsync)) =
        *static_cast<unsigned int *>(static_cast<void *>(&copy.bResolvedAsync));
    *static_cast<unsigned int *>(static_cast<void *>(&bResolvedSync)) =
        *static_cast<unsigned int *>(static_cast<void *>(&copy.bResolvedSync));

    resallocs.clear();
    resallocs.reserve(copy.resallocs.size());
    for (const unsigned int *i = copy.resallocs.begin(); i != copy.resallocs.end(); ++i) {
        resallocs.push_back(*i);
    }
    copy.resallocs.clear();

    RefCount.clear();
    RefCount.reserve(copy.RefCount.size());
    for (EAX_CarState **j = copy.RefCount.begin(); j != copy.RefCount.end(); ++j) {
        RefCount.push_back(*j);
    }
    copy.RefCount.clear();

    t_req = copy.t_req;
    t_load = copy.t_load;
    return *this;
}

stBankSlot *BankSlotSystem::GetFreeSlot(eBANK_SLOT_TYPE Type) {
    for (BankSlotSystem::iterator i = begin(); i != end(); i++) {
        if ((*i).Type == Type && (*i).pAssetParams == nullptr) {
            return &(*i);
        }
    }

    return nullptr;
}

void BankSlotSystem::DestroySlots() {
    for (BankSlotSystem::iterator i = begin(); i != end(); i++) {
        if ((*i).LoadFailed == 0 && (*i).pAssetParams != nullptr) {
            gAEMSMgr.UnloadSndData((*i).pAssetParams->AssetDescription.FileName);
        }
        if ((*i).MAINmemLocation != nullptr) {
            gAudioMemoryManager.FreeMemory((*i).MAINmemLocation);
        }
    }

    clear();
}

EAXAemsManager::EAXAemsManager() {
    m_pAsyncBuff = nullptr;
    mAsyncBuffSize = 0x10000;
    m_nCallbackEvtSys = -1;
    m_ExternalLoadCallback = nullptr;
    m_NumEvtSysLoaded = 0;
    mNumEvtSys = -1;
    m_pCurLoadSDLP = nullptr;
    m_pAsyncLoadSDLP = nullptr;
    m_ItemsPendingAsyncResolve = 0;
    *static_cast<unsigned int *>(static_cast<void *>(&m_bBulkLoad)) = 0;
    m_pEvtSystems.clear();
    mWaitForResolve.clear();
    mBankSlots.clear();
    mPFBankSlot.clear();
    m_SPU_UpperAddress = 0;
    m_SPUMainAllocsEnd = 0;
}

EAXAemsManager::~EAXAemsManager() {
    int n = 0;
    int numEvtSysLoaded = m_NumEvtSysLoaded;
    while (n < numEvtSysLoaded) {
        gAudioMemoryManager.FreeMemory(m_pEvtSystems[n]);
        n++;
    }

    ClearSndAssetQueue(mWaitForResolve);
    mPFBankSlot.DestroySlots();
    mBankSlots.DestroySlots();
}

void EAXAemsManager::Init() {
    if (0) {
        void (stSndDataLoadParams::*forceClear)() = &stSndDataLoadParams::Clear;
        (void)forceClear;
    }

    m_NumBankLoadResolves = 0;
    unsigned int numEvtSys = g_pEAXSound->GetAttributes()->Num_EvtSys();
    mNumEvtSys = numEvtSys;
    if (static_cast<int>(numEvtSys) > 0) {
        m_pEvtSystems.resize(numEvtSys);
    }

    int n = 0;
    do {
        AddEventSystem(static_cast<eEVTSYS>(n), SNDPATH_EVTSYS);
        n++;
    } while (n < 7);

    ResetBankLoadParams();
}

int EAXAemsManager::AddEventSystem(eEVTSYS eESIndex, eSNDDATAPATH eSDP) {
    const char *evtName = g_pEAXSound->GetAttributes()->EvtSys(eESIndex).GetString();
    if (evtName == nullptr) {
        evtName = "";
    }

    bStrCat(m_csTemp1, g_DataPaths[eSDP], evtName);
    int filesize = bFileSize(m_csTemp1);
    evtName = g_pEAXSound->GetAttributes()->EvtSys(eESIndex).GetString();
    if (evtName == nullptr) {
        evtName = "";
    }
    m_pEvtSystems[eESIndex] = gAudioMemoryManager.AllocateMemoryChar(filesize, evtName, false);

    int callback_param = m_NumEvtSysLoaded;
    AddQueuedFile(m_pEvtSystems[callback_param], m_csTemp1, 0, filesize, EvtSysLoadCallback, callback_param, nullptr);
    m_NumEvtSysLoaded = callback_param + 1;
    return callback_param;
}

void EAXAemsManager::InitializeSlots(bool bDoPFSlot) {
    if (DISABLE_SLOT_LOADING == 0) {
        if (bDoPFSlot) {
            mPFBankSlot.DestroySlots();
        }
        mBankSlots.DestroySlots();
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

        if (Type == eBANK_SLOT_PATHFINDER && bDoPFSlot) {
            mPFBankSlot.push_back(NewSlot);
        } else {
            mBankSlots.push_back(NewSlot);
        }
    }
    NewSlot.Clear();
}

void EAXAemsManager::ResolveCurrentDataMemory() {
    EAXAemsManager *mgr = &gAEMSMgr;
    stSndDataLoadParams *curLoad = mgr->m_pCurLoadSDLP;
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
    mgr->m_pCurLoadSDLP->pmem = nullptr;
}

int EAXAemsManager::InitiateLoad() {
    int result;
    stBankSlot *pBankSlot;
    char *fileString;
    eTEMPALLOCLOCATION memLocation;
    QueuedFileParams queuedFileParams;

    fileString = const_cast<char *>(m_pCurLoadSDLP->AssetDescription.FileName.GetString());
    bStrCat(m_csTemp1, g_DataPaths[m_pCurLoadSDLP->AssetDescription.DataPath], fileString != nullptr ? fileString : "");
    result = bFileSize(m_csTemp1);
    m_pCurLoadSDLP->nSize = result;
    if (m_pCurLoadSDLP->nSize < 1) {
        return -1;
    }

    if (m_pCurLoadSDLP->AssetDescription.eDataType < EAXSND_DT_GENERIC_DATA) {
        if (m_pAsyncBuff == nullptr) {
            m_AsyncBuffLocation = TMP_ALLOC_AUDIO;
            bLargestMalloc(AudioMemoryPool);
            {
                int nlargestbuff = bLargestMalloc(0);
                if (nlargestbuff <= 0x20000) {
                    m_pAsyncBuff = static_cast<char *>(TheTrackStreamer.AllocateUserMemory(0x10000, "EAXAemsManager::m_pAsyncBuff", 0));
                    m_AsyncBuffLocation = TMP_ALLOC_TRACKSTREAMER;
                    if (m_pAsyncBuff == nullptr) {
                        pBankSlot = m_pCurLoadSDLP->mBankSlot;
                        if (pBankSlot != nullptr) {
                            pBankSlot->LoadFailed = 1;
                        }
                        result = -2;
                    } else {
                        {
                            pBankSlot = m_pCurLoadSDLP->mBankSlot;
                            if (pBankSlot != nullptr) {
                                pBankSlot->LoadFailed = 0;
                            }
                            m_pCurLoadSDLP->MemLocation = TMP_ALLOC_NONE;
                            m_pCurLoadSDLP->AssetDescription.eDataType = EAXSND_DT_AEMS_ASYNCSPU;
                        }
                        goto HaveQueueParams;
                    }
                } else {
                    m_AsyncBuffLocation = TMP_ALLOC_MAIN;
                    m_pAsyncBuff = static_cast<char *>(bMalloc(0x10000, 0));
                    {
                        pBankSlot = m_pCurLoadSDLP->mBankSlot;
                        if (pBankSlot != nullptr) {
                            pBankSlot->LoadFailed = 0;
                        }
                        m_pCurLoadSDLP->MemLocation = TMP_ALLOC_NONE;
                        m_pCurLoadSDLP->AssetDescription.eDataType = EAXSND_DT_AEMS_ASYNCSPU;
                    }
                    goto HaveQueueParams;
                }
            }
        } else {
            {
                pBankSlot = m_pCurLoadSDLP->mBankSlot;
                if (pBankSlot != nullptr) {
                    pBankSlot->LoadFailed = 0;
                }
                m_pCurLoadSDLP->MemLocation = TMP_ALLOC_NONE;
                m_pCurLoadSDLP->AssetDescription.eDataType = EAXSND_DT_AEMS_ASYNCSPU;
            }
            goto HaveQueueParams;
        }
    } else {
        m_pCurLoadSDLP->MemLocation = TMP_ALLOC_AUDIO;
    HaveQueueParams:
        queuedFileParams.UncompressedSize = 0;
        queuedFileParams.BlockSize = 0x7FFFFFF;
        queuedFileParams.Priority = QueuedFileDefaultPriority - 2;
        queuedFileParams.Compressed = false;
        QueuedFileParams *pQueuedFileParams = &queuedFileParams;
        memLocation = m_pCurLoadSDLP->MemLocation;
        switch (memLocation) {
        case TMP_ALLOC_MAIN:
            result = bLargestMalloc(0);
            if (result < m_pCurLoadSDLP->nSize) {
                result = -2;
                goto ReturnResult;
            }
            m_pCurLoadSDLP->pmem = bMalloc(m_pCurLoadSDLP->nSize, 0x1040);
            AddQueuedFile(m_pCurLoadSDLP->pmem, m_csTemp1, 0, m_pCurLoadSDLP->nSize, DataLoadCB,
                          reinterpret_cast<int>(m_pCurLoadSDLP), pQueuedFileParams);
        SetWaitingState:
            *static_cast<int *>(static_cast<void *>(&m_IsWaitingForFileCB)) = memLocation;
            break;
        case TMP_ALLOC_NONE:
            goto SetWaitingState;
        case TMP_ALLOC_TRACKSTREAMER:
            result = GetMemoryPoolSize__13TrackStreamer(&TheTrackStreamer);
            if (result == 0) {
                return -3;
            }
            fileString = const_cast<char *>(m_pCurLoadSDLP->AssetDescription.FileName.GetString());
            if (fileString == nullptr) {
                fileString = "";
            }
            m_pCurLoadSDLP->pmem = TheTrackStreamer.AllocateUserMemory(m_pCurLoadSDLP->nSize, fileString, 0);
            fileString = static_cast<char *>(m_pCurLoadSDLP->pmem);
            if (fileString == nullptr) {
                return -3;
            }
            result = m_pCurLoadSDLP->nSize;
            AddQueuedFile(fileString, m_csTemp1, 0, result, DataLoadCB,
                          reinterpret_cast<int>(m_pCurLoadSDLP), pQueuedFileParams);
            *static_cast<int *>(static_cast<void *>(&m_IsWaitingForFileCB)) = 1;
            break;
        case TMP_ALLOC_AUDIO:
            if (m_pCurLoadSDLP->AssetDescription.eDataType == EAXSND_DT_GENERIC_DATA &&
                (pBankSlot = m_pCurLoadSDLP->mBankSlot, pBankSlot != nullptr)) {
                result = m_pCurLoadSDLP->nSize;
                if (result > pBankSlot->MAINmemSize) {
                    return -4;
                }
                fileString = pBankSlot->MAINmemLocation;
            } else {
                result = bLargestMalloc(AudioMemoryPool);
                if (result < m_pCurLoadSDLP->nSize) {
                    return -4;
                }
                fileString = const_cast<char *>(m_pCurLoadSDLP->AssetDescription.FileName.GetString());
                if (fileString == nullptr) {
                    fileString = "";
                }
                m_pCurLoadSDLP->pmem = gAudioMemoryManager.AllocateMemory(m_pCurLoadSDLP->nSize, fileString,
                                                                          m_pCurLoadSDLP->AssetDescription.bLoadToTop);
                fileString = static_cast<char *>(m_pCurLoadSDLP->pmem);
                result = m_pCurLoadSDLP->nSize;
            }
            AddQueuedFile(fileString, m_csTemp1, 0, result, DataLoadCB,
                          reinterpret_cast<int>(m_pCurLoadSDLP), pQueuedFileParams);
            *static_cast<int *>(static_cast<void *>(&m_IsWaitingForFileCB)) = 1;
            break;
        default:
            break;
        }
    CheckAsyncSpuLoad:
        if (m_pCurLoadSDLP->AssetDescription.eDataType == EAXSND_DT_AEMS_ASYNCSPU) {
            pBankSlot = m_pCurLoadSDLP->mBankSlot;
            if (pBankSlot != nullptr) {
                SNDmemlimits(pBankSlot->BANKmemLocation, pBankSlot->BANKmemLocation + pBankSlot->BANKMemSize);
            } else {
                SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
            }
            result = SNDAEMS_asyncloadmodulebank(m_csTemp1, 0, nullptr, 0, m_pAsyncBuff, mAsyncBuffSize,
                                                 AsyncResidentAllocCB);
            m_pCurLoadSDLP->Handle = result;
        }
        result = 0;
    }
ReturnResult:
    return result;

HaveAsyncBuffer:
    {
        pBankSlot = m_pCurLoadSDLP->mBankSlot;
        if (pBankSlot != nullptr) {
            pBankSlot->LoadFailed = 0;
        }
        m_pCurLoadSDLP->MemLocation = TMP_ALLOC_NONE;
        m_pCurLoadSDLP->AssetDescription.eDataType = EAXSND_DT_AEMS_ASYNCSPU;
    }
    goto HaveQueueParams;
}

void EAXAemsManager::SetupNextLoad() {
    m_pCurLoadSDLP = nullptr;
    if (m_nCurLoadedBankIndex < (m_nEndOfList - 1)) {
        m_nCurLoadedBankIndex++;
        m_pCurLoadSDLP = g_SndAssetList + m_nCurLoadedBankIndex;
        if (InitiateLoad() < 0) {
            unsigned int Hash32;
            const char *String;
            SndBase *SfxToDel[32];
            SndAssetQueue::iterator i;
            int deleteCount = 0;

            Hash32 =
                *static_cast<unsigned int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pCurLoadSDLP)) + 8));
            String =
                *static_cast<const char **>(static_cast<void *>(static_cast<char *>(static_cast<void *>(m_pCurLoadSDLP)) + 0xC));
            bMemSet(SfxToDel, '\0', 0x80);
            while (true) {
                i = mWaitForResolve.begin();
                if (i == mWaitForResolve.end()) {
                    break;
                }

                while (true) {
                    stSndAssetQueue currequst = *i;
                    if (*static_cast<unsigned int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(&currequst)) + 8)) ==
                        *(&Hash32)) {
                        if (*static_cast<const char **>(
                                static_cast<void *>(static_cast<char *>(static_cast<void *>(&currequst)) + 0xC)) ==
                            *(&String)) {
                            mWaitForResolve.remove(currequst);
                            SfxToDel[deleteCount] = currequst.pThis;
                            deleteCount++;
                            goto ContinueScanning;
                        }
                    }

                    ++i;
                    if (i == mWaitForResolve.end()) {
                        goto RemoveQueuedLoads;
                    }
                }

            ContinueScanning:
                ;
            }

            deleteCount--;
        RemoveQueuedLoads:
            do {
                i = mWaitForResolve.begin();
                while (true) {
                    if (i == mWaitForResolve.end()) {
                        goto ContinueDeleting;
                    }

                    stSndAssetQueue currequst = *i;
                    if (*(&SfxToDel[deleteCount]) == currequst.pThis) {
                        mWaitForResolve.remove(currequst);
                        goto ContinueDeleting;
                    }

                    ++i;
                }

            ContinueDeleting:
                ;
            } while (deleteCount-- > 0);

            RemoveBankListing(m_nCurLoadedBankIndex);
            SetupNextLoad();
        }
    }
}

void EAXAemsManager::Update() {
ReprocessQueue:
    SndAssetQueue::iterator i = mWaitForResolve.begin();
    while (i != mWaitForResolve.end()) {
        stSndAssetQueue currequst = *i;
        if (IsAssetLoaded(currequst.Asset.FileName) != -1) {
            bool callinit = true;
            i++;
            while (i != mWaitForResolve.end() && callinit) {
                stSndAssetQueue futureitems = *i;
                if (futureitems.pThis == currequst.pThis) {
                    callinit = false;
                }
                i++;
            }

            if (callinit && currequst.pThis != nullptr) {
                currequst.pThis->InitSFX();
            }

            mWaitForResolve.remove(currequst);
            goto ReprocessQueue;
        }
        i++;
    }

    if (m_ItemsPendingAsyncResolve != 0 && m_bBulkLoad) {
        ResolvePendingAsyncLoads();
    }

    if (m_pCurLoadSDLP != nullptr) {
        if (!m_pCurLoadSDLP->bResolvedAsync) {
            CheckForCompleteAsyncLoad();
        }

        m_pCurLoadSDLP->bResolvedSync =
            Joylog::AddOrGetData(static_cast<unsigned int>(m_pCurLoadSDLP->bResolvedAsync), 8, JOYLOG_CHANNEL_SOUND_LOADING) != 0;

        if (Joylog::IsReplaying() != 0 && m_pCurLoadSDLP->bResolvedSync) {
            while (!m_pCurLoadSDLP->bResolvedAsync) {
                SNDSYS_service();
                CheckForCompleteAsyncLoad();
            }
        }

        if (m_pCurLoadSDLP->bResolvedSync) {
            if (m_pCurLoadSDLP->AssetDescription.eDataType < EAXSND_DT_GENERIC_DATA) {
                ResolveCurrentDataMemory();
            }
            SetupNextLoad();
        }
    } else {
        SetupNextLoad();
        if (m_pCurLoadSDLP == nullptr && m_ItemsPendingAsyncResolve == 0) {
            char *mem = m_pAsyncBuff;
            if (mem != nullptr) {
                eTEMPALLOCLOCATION location = m_AsyncBuffLocation;
                if (location == TMP_ALLOC_AUDIO) {
                    gAudioMemoryManager.FreeMemory(mem);
                } else if (location == TMP_ALLOC_MAIN) {
                    bFree(mem);
                } else if (location == TMP_ALLOC_TRACKSTREAMER) {
                    TheTrackStreamer.FreeUserMemory(mem);
                }
                m_pAsyncBuff = nullptr;
            }

            if (m_ExternalLoadCallback != nullptr) {
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

                m_ExternalLoadCallback(m_ExternalLoadCallbackParam);
                m_bBulkLoad = false;
                m_ExternalLoadCallback = nullptr;
                m_ExternalLoadCallbackParam = 0;
            }
        }
    }
}

void EAXAemsManager::RemoveBankListing(int Index) {
    g_SndAssetList[Index].Clear();
    {
        for (int n = Index; n < 0x2F; n++) {
            g_SndAssetList[n] = g_SndAssetList[n + 1];
            g_SndAssetList[n + 1].Clear();
        }
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
    if (eDataType < EAXSND_DT_GENERIC_DATA) {
        RemoveAEMSBank();
    }

    void *plocmem = *static_cast<void **>(static_cast<void *>(entry + 0x2C));
    if (plocmem != nullptr) {
        if (eDataType == EAXSND_DT_AEMS_MAINMEM) {
            bFree(plocmem);
        } else {
            gAudioMemoryManager.FreeMemory(plocmem);
        }
        *static_cast<void **>(static_cast<void *>(entry + 0x2C)) = nullptr;
    }

    void *pmem = *static_cast<void **>(static_cast<void *>(entry + 0x28));
    if (pmem != nullptr) {
        if (eDataType == EAXSND_DT_AEMS_MAINMEM) {
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
        stSndDataLoadParams *currentLoad = gAEMSMgr.m_pCurLoadSDLP;
        void *resmem;
        if (currentLoad->AssetDescription.eDataType == EAXSND_DT_AEMS_MAINMEM) {
            resmem = bMalloc(residentsize, 0x1040);
        } else {
            stBankSlot *pBankSlot = currentLoad->mBankSlot;
            if (pBankSlot != nullptr) {
                pBankSlot->pLastAlloc += residentsize;
                return pBankSlot->MAINmemLocation;
            }

            char *filename = const_cast<char *>(currentLoad->AssetDescription.FileName.GetString());
            if (filename == nullptr) {
                filename = const_cast<char *>("");
            }
            resmem = gAudioMemoryManager.AllocateMemory(residentsize, filename, false);
        }

        currentLoad->plocmem = resmem;
        gAEMSMgr.m_NumBankLoadResolves++;
        return currentLoad->plocmem;
    }

    return gAEMSMgr.m_pCurLoadSDLP->pmem;
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

    if (eDataType == EAXSND_DT_AEMS_ASYNCSPU) {
        goto LoadDone;
    }

    if (eDataType <= EAXSND_DT_AEMS_SYNCSPU) {
        if (eDataType >= EAXSND_DT_AEMS_AUDIOMEM) {
            AddAemsBank();
            gAEMSMgr.ResolveCurrentDataMemory();
            *static_cast<unsigned int *>(static_cast<void *>(currentLoad + 0x38)) = 1;
            *static_cast<unsigned int *>(static_cast<void *>(currentLoad + 0x3C)) = 1;
        }
    } else if (eDataType == EAXSND_DT_AEMS_ASYNCSPUMEM) {
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
    } else if (eDataType == EAXSND_DT_GENERIC_DATA) {
        *static_cast<unsigned int *>(static_cast<void *>(currentLoad + 0x38)) = 1;
        *static_cast<unsigned int *>(static_cast<void *>(currentLoad + 0x3C)) = 1;
    }

LoadDone:
    *static_cast<unsigned int *>(static_cast<void *>(mgr + 0x12C)) = 0;
}

int EAXAemsManager::AddBankListing(stAssetDescription &asset) {
    stSndDataLoadParams *entry = g_SndAssetList + m_nEndOfList;
    entry->AssetDescription.eDataType = EAXSND_DT_NONE;
    entry->AssetDescription.FileName.operator=(Attrib::StringKey(""));
    entry->AssetDescription.DataPath = SNDPATH_ROUTE;
    entry->AssetDescription.bLoadToTop = false;
    entry->Handle = -1;
    entry->bResolvedSync = false;
    entry->MemLocation = TMP_ALLOC_NONE;
    entry->mBankSlot = nullptr;
    entry->pmem = nullptr;
    entry->plocmem = nullptr;
    entry->nSize = 0;
    entry->bResolvedAsync = false;
    entry->resallocs.clear();
    entry->RefCount.clear();
    entry->t_req = Timer(0);
    entry->t_load = Timer(0);

    stSndDataLoadParams &dst = g_SndAssetList[m_nEndOfList];
    dst.AssetDescription = asset;

    return m_nEndOfList++;
}

void EAXAemsManager::QueueFileLoad(stSndAssetQueue &queueitem, eBANK_SLOT_TYPE SlotType) {
    Attrib::StringKey filename(queueitem.Asset.FileName);
    int bankIndex = IsAssetInList(filename);
    if (bankIndex == -1) {
        stBankSlot *slot = nullptr;
        if (DISABLE_SLOT_LOADING != 0) {
            SlotType = eBANK_SLOT_NONE;
        }

        if (SlotType != eBANK_SLOT_NONE) {
            if (SlotType == eBANK_SLOT_PATHFINDER) {
                slot = mPFBankSlot.GetFreeSlot(eBANK_SLOT_PATHFINDER);
            } else {
                slot = mBankSlots.GetFreeSlot(SlotType);
            }

            if (slot == nullptr) {
                SndBase *owner = queueitem.pThis;
                if (!owner) {
                    return;
                }

                while (true) {
                    SndAssetQueue::iterator i = mWaitForResolve.begin();
                    while (true) {
                        if (i == mWaitForResolve.end()) {
                            return;
                        }

                        stSndAssetQueue currequst = *i;
                        if (currequst.pThis == owner) {
                            mWaitForResolve.remove(currequst);
                            break;
                        }

                        ++i;
                    }
                }
            }
        }

        bankIndex = AddBankListing(queueitem.Asset);
        if (slot != nullptr) {
            slot->pAssetParams = &g_SndAssetList[bankIndex];
            g_SndAssetList[bankIndex].mBankSlot = slot;
        }
    }

    mWaitForResolve.push_back(queueitem);
}

void EAXAemsManager::ResolvePendingAsyncLoads() {}

void *EAXAemsManager::GetCallbackEventSys() {
    int offset = m_nCallbackEvtSys << 2;
    return m_pEvtSystems[m_nCallbackEvtSys];
}

void SubscribeEventSys() {
    if (IsSoundEnabled == 1) {
        if (gAEMSMgr.GetCallbackEventSys() != nullptr) {
            Csis::System::Result status = Csis::System::Subscribe(gAEMSMgr.GetCallbackEventSys());
            (void)status;
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
    if (m_pCurLoadSDLP == nullptr && m_ItemsPendingAsyncResolve == 0 && m_IsWaitingForFileCB == 0) {
        if (m_nCurLoadedBankIndex < (m_nEndOfList - 1)) {
            return true;
        }
        return false;
    }
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
    mWaitForResolve.clear();
    DestroySlots(true);
}

void EAXAemsManager::DestroySlots(bool bDoPFSlot) {
    if (bDoPFSlot == true) {
        mPFBankSlot.DestroySlots();
    }
    mBankSlots.DestroySlots();
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
    int Result;

    if (gAEMSMgr.m_pCurLoadSDLP->mBankSlot != nullptr) {
        SNDmemlimits(gAEMSMgr.m_pCurLoadSDLP->mBankSlot->BANKmemLocation,
                     gAEMSMgr.m_pCurLoadSDLP->mBankSlot->BANKmemLocation + gAEMSMgr.m_pCurLoadSDLP->mBankSlot->BANKMemSize);
    } else {
        SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    }

    int nhandle = SNDAEMS_addmodulebank(gAEMSMgr.m_pCurLoadSDLP->pmem, nullptr, 0, ResidentAllocCB);
    SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    gAEMSMgr.m_pCurLoadSDLP->Handle = nhandle;
}

void EAXAemsManager::CheckForCompleteAsyncLoad() {
    if (!m_pCurLoadSDLP->bResolvedAsync) {
        if (m_pCurLoadSDLP->AssetDescription.eDataType == EAXSND_DT_AEMS_ASYNCSPU) {
            if (SNDAEMS_asyncloadmodulebankdone() > 0) {
                CompleteAsyncLoad();
            }
        } else if (m_pCurLoadSDLP->AssetDescription.eDataType == EAXSND_DT_AEMS_ASYNCSPUMEM &&
                   SNDAEMS_asyncloadmodulebankmemdone() > 0 && m_IsWaitingForFileCB == 0) {
            CompleteAsyncLoad();
        }
    }
}
