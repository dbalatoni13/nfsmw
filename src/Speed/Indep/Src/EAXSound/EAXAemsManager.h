//
//
//
//
#ifndef AFX_AEMSBANKMANAGER_H__3C37B05E_40ED_4CEF_B845_016DE9160308__INCLUDED_
#define AFX_AEMSBANKMANAGER_H__3C37B05E_40ED_4CEF_B845_016DE9160308__INCLUDED_

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/UG/SndDataParams.hpp"

#define AEMS_EVENTSYS_LOADED 0x01 // Decl: 13

#define MAX_SIZE_SNDASSETLIST 48 // Decl: 22

DECLARE_CONTAINER_TYPE(EvtSystems);

// total size: 0x10
// Decl: 32
class EvtSystems : public UTL::Std::vector<char *, _type_EvtSystems> {
  public:
    EvtSystems() {} // Decl: 35

    ~EvtSystems() {}

    // TODO
    void *operator new(size_t size, void *ptr) {}

    void operator delete(void *mem, void *ptr) {}

    void *operator new(size_t size) {}

    void operator delete(void *mem, const char *name) {} // Decl: 36

    void operator delete(void *mem, size_t size) {}

    void *operator new(size_t size, const char *name) {}

    void operator delete(void *mem, size_t size, const char *name) {}
};

DECLARE_CONTAINER_TYPE(BankSlotSystem);

// total size: 0x8
// Decl: 41
class BankSlotSystem : public UTL::Std::list<stBankSlot, _type_BankSlotSystem> {
  public:
    BankSlotSystem() {} // Decl: 44

    ~BankSlotSystem() {}

    // TODO
    void *operator new(size_t size, void *ptr) {}

    void operator delete(void *mem, void *ptr) {}

    void *operator new(size_t size) {}

    void operator delete(void *mem, size_t size) {}

    void *operator new(size_t size, const char *name) {}

    void operator delete(void *mem, size_t size, const char *name) {}

    void operator delete(void *mem, const char *name) {} // Decl: 45

    stBankSlot *GetFreeSlot(eBANK_SLOT_TYPE Type); // Decl: 47
    void DestroySlots();                           // Decl: 48
};

DECLARE_CONTAINER_TYPE(AsyncQueue);

// total size: 0x30
// Decl: 55
class AsyncQueue : public UTL::Std::deque<stSndDataLoadParams *, _type_AsyncQueue> {
  public:
    AsyncQueue() {}

    ~AsyncQueue() {}

    // TODO
    void *operator new(size_t size, void *ptr) {}

    void operator delete(void *mem, void *ptr) {}

    void *operator new(size_t size) {}

    void operator delete(void *mem, size_t size) {}

    void *operator new(size_t size, const char *name) {}

    void operator delete(void *mem, const char *name) {} // Decl: 59

    void operator delete(void *mem, size_t size, const char *name) {}
};

DECLARE_CONTAINER_TYPE(SndAssetQueue);

// total size: 0x8
// Decl: 62
class SndAssetQueue : public UTL::Std::list<stSndAssetQueue, _type_SndAssetQueue> {
  public:
    SndAssetQueue() {} // Decl: 66

    ~SndAssetQueue() {}

    // TODO
    void *operator new(size_t size, void *ptr) {}

    void operator delete(void *mem, void *ptr) {}

    void *operator new(size_t size) {}

    void operator delete(void *mem, const char *name) {} // Decl: 67

    void operator delete(void *mem, size_t size) {}

    void *operator new(size_t size, const char *name) {}

    void operator delete(void *mem, size_t size, const char *name) {}

    void PrintContents() {} // Decl: 69

    void DeleteRefToAsset(Attrib::StringKey FileName) {}

    void DeleteRefToAsset(SndBase *pSfxObj) {} // Decl: 120
};

// total size: 0x130
// Decl: 139
class EAXAemsManager : public AudioMemBase {
  public:
    EAXAemsManager();

    // Overrides: AudioMemBase
    ~EAXAemsManager() override;

    void QueueFileLoad(stSndAssetQueue &queueitem, eBANK_SLOT_TYPE SlotType);
    void DestroyCar(EAX_CarState *);
    void UnloadSndData(Attrib::StringKey filename);
    void UnloadSndData(int Index);
    int AddEventSystem(eEVTSYS eESIndex, eSNDDATAPATH eSDP);
    void RemoveEventSystem(eEVTSYS eESIndex);
    void Init();
    void Update();
    void RemoveAEMSBank();
    void ResetBankLoadParams();
    void *GetCallbackEventSys();
    static void AddAemsBank();
    void DebugPrints();
    void ResolveCurrentDataMemory();

    void SetupNextLoad();
    void CheckForCompleteAsyncLoad();
    void CompleteAsyncLoad();
    bool AreResourceLoadsPending();
    void ResolvePendingAsyncLoads();

    static void EvtSysLoadCallback(int32 param, int error_status);
    static void *ResidentAllocCB(void *pbank, int residentsize, int totalsize);
    static void *AsyncResidentAllocCB(int size);
    static void DataLoadCB(int32 param, int error_status);

    int AddBankListing(stAssetDescription &asset);
    void RemoveBankListing(int Index);
    int IsAssetInList(Attrib::StringKey filename);
    int IsAssetLoaded(Attrib::StringKey filename);

    void InitializeSlots(bool bDoPFSlot);
    void DestroySlots(bool bDoPFSlot);
    static void QueueSlots(eBANK_SLOT_TYPE Type, int NumSlots) {} // Decl: 204
    void RegisterSlots(eBANK_SLOT_TYPE Type, int NumSlots, int SizePerSlotSPU, int SizePerSlotMainMem, bool bDoPFSlot);
    stBankSlot *GetSlot(eBANK_SLOT_TYPE);

    void InitSPUram();

    void (*m_ExternalLoadCallback)(int);    // offset 0x4, size 0x4, Decl: 178
    int m_ExternalLoadCallbackParam;        // offset 0x8, size 0x4, Decl: 179
    int m_nBigFileOffset;                   // offset 0xC, size 0x4, Decl: 181
    char m_csTemp1[128];                    // offset 0x10, size 0x80, Decl: 182
    int m_NumBankLoadResolves;              // offset 0x90, size 0x4, Decl: 183
    char *m_pAsyncBuff;                     // offset 0x94, size 0x4, Decl: 184
    eTEMPALLOCLOCATION m_AsyncBuffLocation; // offset 0x98, size 0x4, Decl: 185
    int m_nCurLoadedBankIndex;              // offset 0x9C, size 0x4, Decl: 191
    int m_nEndOfList;                       // offset 0xA0, size 0x4, Decl: 192

    static const int m_SlotSizes[2][4]; // size: 0x20, address: 0x803D6B78
    static int m_RequiredSlots[4];      // size: 0x10, address: 0x804FE700

    BankSlotSystem mBankSlots;             // offset 0xA4, size 0x8, Decl: 211
    BankSlotSystem mPFBankSlot;            // offset 0xAC, size 0x8
    int m_SPUMainAllocsEnd;                // offset 0xB4, size 0x4, Decl: 213
    int m_SPU_UpperAddress;                // offset 0xB8, size 0x4, Decl: 214
    SndAssetQueue mWaitForResolve;         // offset 0xBC, size 0x8, Decl: 221
    bool bLoadingHalted;                   // offset 0xC4, size 0x1, Decl: 222
    AsyncQueue mWaitList;                  // offset 0xC8, size 0x30, Decl: 224
    EvtSystems m_pEvtSystems;              // offset 0xF8, size 0x10, Decl: 225
    int m_NumEvtSysLoaded;                 // offset 0x108, size 0x4, Decl: 226
    int mNumEvtSys;                        // offset 0x10C, size 0x4, Decl: 227
    int mAsyncBuffSize;                    // offset 0x110, size 0x4, Decl: 228
    int m_nCallbackEvtSys;                 // offset 0x114, size 0x4, Decl: 229
    stSndDataLoadParams *m_pCurLoadSDLP;   // offset 0x118, size 0x4, Decl: 230
    stSndDataLoadParams *m_pCurUNLOADSDLP; // offset 0x11C, size 0x4, Decl: 231
    stSndDataLoadParams *m_pAsyncLoadSDLP; // offset 0x120, size 0x4, Decl: 235
    int m_ItemsPendingAsyncResolve;        // offset 0x124, size 0x4, Decl: 236
    bool m_bBulkLoad;                      // offset 0x128, size 0x1, Decl: 237
    bool m_IsWaitingForFileCB;             // offset 0x12C, size 0x1, Decl: 239

  private:
    void InitiateLoad(); // Decl: 243
};

extern int m_RequiredSlots[4];      // size: 0x10, address: 0x804FE700, Decl: 205
extern const int m_SlotSizes[2][4]; // size: 0x20, address: 0x803D6B78, Decl: 206

extern EAXAemsManager gAEMSMgr; // size: 0x0, Decl: 248

#endif
