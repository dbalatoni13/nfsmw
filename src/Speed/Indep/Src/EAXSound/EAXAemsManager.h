#ifndef EAXSOUND_EAXAEMSMANAGER_H
#define EAXSOUND_EAXAEMSMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <deque>

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct SndBase;
struct EAX_CarState;

enum eBANK_SLOT_TYPE {
    eBANK_SLOT_NONE = -1,
    eBANK_SLOT_AI_AEMS_ENGINE = 0,
    eBANK_SLOT_AI_GINA_ENGINE = 1,
    eBANK_SLOT_AI_GIND_ENGINE = 2,
    eBANK_SLOT_PATHFINDER = 3,
    eBANK_SLOT_MAX_NUM = 4
};

enum eEVTSYS {
    EVTSYS_MAIN = 0,
    EVTSYS_FE = 1,
};

enum eSNDDATAPATH {
    SNDPATH_ROUTE = 0,
    SNDPATH_ENGINE = 1,
    SNDPATH_EVTSYS = 2,
    SNDPATH_FE = 3,
    SNDPATH_GLOBAL = 4,
    SNDPATH_INGAME = 5,
    SNDPATH_NOS = 6,
    SNDPATH_PATHFINDER = 7,
    SNDPATH_SKIDS = 8,
    SNDPATH_SPEECH = 9,
    SNDPATH_TURBO = 10,
    SNDPATH_SHIFT = 11,
    SNDPATH_FXEDIT = 12,
    MAX_SNDDATA_PATHS = 13,
};

enum eSNDDATATYPE {
    EAXSND_DT_NONE = -1,
    EAXSND_DT_AEMS_AUDIOMEM = 0,
    EAXSND_DT_AEMS_MAINMEM = 1,
    EAXSND_DT_AEMS_SYNCSPU = 2,
    EAXSND_DT_AEMS_ASYNCSPU = 3,
    EAXSND_DT_AEMS_ASYNCSPUMEM = 4,
    EAXSND_DT_GENERIC_DATA = 5,
};

enum eTEMPALLOCLOCATION {
    TMP_ALLOC_NONE = 0,
    TMP_ALLOC_INVALID = TMP_ALLOC_NONE,
    TMP_ALLOC_MAIN = 1,
    TMP_ALLOC_TRACKSTREAMER = 2,
    TMP_ALLOC_AUDIO = 3,
};

struct stAssetDescription {
    eSNDDATATYPE eDataType;     // offset 0x0, size 0x4
    Attrib::StringKey FileName; // offset 0x8, size 0x10
    eSNDDATAPATH DataPath;      // offset 0x18, size 0x4
    bool bLoadToTop;            // offset 0x1C, size 0x1

    void Clear();
};

DECLARE_CONTAINER_TYPE(ResAllocList);
DECLARE_CONTAINER_TYPE(RefCountList);

typedef UTL::Std::vector<unsigned int, _type_ResAllocList> ResAllocList;
typedef UTL::Std::vector<EAX_CarState *, _type_RefCountList> RefCountList;

struct stBankSlot;

struct stSndDataLoadParams {
    stAssetDescription AssetDescription; // offset 0x0, size 0x20
    eTEMPALLOCLOCATION MemLocation;      // offset 0x20, size 0x4
    stBankSlot *mBankSlot;               // offset 0x24, size 0x4
    void *pmem;                          // offset 0x28, size 0x4
    void *plocmem;                       // offset 0x2C, size 0x4
    int nSize;                           // offset 0x30, size 0x4
    int Handle;                          // offset 0x34, size 0x4
    bool bResolvedAsync;                 // offset 0x38, size 0x1
    bool bResolvedSync;                  // offset 0x3C, size 0x1
    ResAllocList resallocs;              // offset 0x40, size 0x10
    RefCountList RefCount;               // offset 0x50, size 0x10
    Timer t_req;                         // offset 0x60, size 0x4
    Timer t_load;                        // offset 0x64, size 0x4

    stSndDataLoadParams() {
        AssetDescription.Clear();
        Clear();
    }

    void Clear();
    stSndDataLoadParams &operator=(stSndDataLoadParams &copy);
};

struct stBankSlot {
    eBANK_SLOT_TYPE Type;              // offset 0x0, size 0x4
    int BANKmemLocation;               // offset 0x4, size 0x4
    char *MAINmemLocation;             // offset 0x8, size 0x4
    char *pLastAlloc;                  // offset 0xC, size 0x4
    int MAINmemSize;                   // offset 0x10, size 0x4
    int BANKMemSize;                   // offset 0x14, size 0x4
    int LoadFailed;                    // offset 0x18, size 0x4
    unsigned char Index;               // offset 0x1C, size 0x1
    stSndDataLoadParams *pAssetParams; // offset 0x20, size 0x4

    void Clear();
};

struct stSndAssetQueue {
    stAssetDescription Asset; // offset 0x0, size 0x20
    SndBase *pThis;           // offset 0x20, size 0x4
    EAX_CarState *pCar;       // offset 0x24, size 0x4

    bool operator==(const stSndAssetQueue &rhs) const {
        return pThis == rhs.pThis && Asset.FileName.GetHash32() == rhs.Asset.FileName.GetHash32() &&
               Asset.FileName.GetString() == rhs.Asset.FileName.GetString();
    }
};

DECLARE_CONTAINER_TYPE(BankSlotSystem);
DECLARE_CONTAINER_TYPE(SndAssetQueue);
DECLARE_CONTAINER_TYPE(AsyncQueue);
DECLARE_CONTAINER_TYPE(EvtSystems);

struct BankSlotSystem : public UTL::Std::list<stBankSlot, _type_BankSlotSystem> {
    stBankSlot *GetFreeSlot(eBANK_SLOT_TYPE Type);
    void DestroySlots();
};

struct SndAssetQueue : public UTL::Std::list<stSndAssetQueue, _type_SndAssetQueue> {};

struct AsyncQueue : public UTL::Std::deque<stSndDataLoadParams *, _type_AsyncQueue> {};

struct EvtSystems : public UTL::Std::vector<char *, _type_EvtSystems> {};

struct EAXAemsManager : public AudioMemBase {
    static int m_RequiredSlots[4];
    static const int m_SlotSizes[4][2];

    void (*m_ExternalLoadCallback)(int);       // offset 0x4, size 0x4
    int m_ExternalLoadCallbackParam;           // offset 0x8, size 0x4
    int m_nBigFileOffset;                      // offset 0xC, size 0x4
    char m_csTemp1[0x80];                      // offset 0x10, size 0x80
    int m_NumBankLoadResolves;                 // offset 0x90, size 0x4
    char *m_pAsyncBuff;                        // offset 0x94, size 0x4
    eTEMPALLOCLOCATION m_AsyncBuffLocation;    // offset 0x98, size 0x4
    int m_nCurLoadedBankIndex;                 // offset 0x9C, size 0x4
    int m_nEndOfList;                          // offset 0xA0, size 0x4
    BankSlotSystem mBankSlots;                 // offset 0xA4, size 0x8
    BankSlotSystem mPFBankSlot;                // offset 0xAC, size 0x8
    int m_SPUMainAllocsEnd;                    // offset 0xB4, size 0x4
    int m_SPU_UpperAddress;                    // offset 0xB8, size 0x4
    SndAssetQueue mWaitForResolve;             // offset 0xBC, size 0x8
    bool bLoadingHalted;                       // offset 0xC4, size 0x1
    AsyncQueue mWaitList;                      // offset 0xC8, size 0x30
    EvtSystems m_pEvtSystems;                  // offset 0xF8, size 0x10
    int m_NumEvtSysLoaded;                     // offset 0x108, size 0x4
    int mNumEvtSys;                            // offset 0x10C, size 0x4
    int mAsyncBuffSize;                        // offset 0x110, size 0x4
    int m_nCallbackEvtSys;                     // offset 0x114, size 0x4
    stSndDataLoadParams *m_pCurLoadSDLP;       // offset 0x118, size 0x4
    stSndDataLoadParams *m_pCurUNLOADSDLP;     // offset 0x11C, size 0x4
    stSndDataLoadParams *m_pAsyncLoadSDLP;     // offset 0x120, size 0x4
    int m_ItemsPendingAsyncResolve;            // offset 0x124, size 0x4
    bool m_bBulkLoad;                          // offset 0x128, size 0x1
    bool m_IsWaitingForFileCB;                 // offset 0x12C, size 0x1

    EAXAemsManager();
    virtual ~EAXAemsManager();
    void Init();
    int AddEventSystem(eEVTSYS eESIndex, eSNDDATAPATH eSDP);
    int InitiateLoad();
    void SetupNextLoad();
    void Update();
    bool AreResourceLoadsPending();
    void *GetCallbackEventSys();
    static void EvtSysLoadCallback(int param, int error_status);
    void UnloadSndData(Attrib::StringKey filename);
    void UnloadSndData(int index);
    void RemoveAEMSBank();
    static void AddAemsBank();
    void CheckForCompleteAsyncLoad();
    void ResolvePendingAsyncLoads();
    void InitSPUram();
    void InitializeSlots(bool bDoPFSlot);
    void RegisterSlots(eBANK_SLOT_TYPE Type, int NumSlots, int SizePerSlotSPU, int SizePerSlotMainMem, bool bDoPFSlot);
    void ResetBankLoadParams();
    void RemoveBankListing(int index);
    int AddBankListing(stAssetDescription &asset);
    void QueueFileLoad(stSndAssetQueue &queueitem, eBANK_SLOT_TYPE SlotType);
    static void *AsyncResidentAllocCB(int size);
    static void *ResidentAllocCB(void *pbank, int residentsize, int totalsize);
    static void DataLoadCB(int param, int error_status);
    void DestroySlots(bool bDoPFSlot);
    int IsAssetInList(Attrib::StringKey filename);
    int IsAssetLoaded(Attrib::StringKey filename);
    void CompleteAsyncLoad();
    void ResolveCurrentDataMemory();
};

extern EAXAemsManager gAEMSMgr;

#endif
