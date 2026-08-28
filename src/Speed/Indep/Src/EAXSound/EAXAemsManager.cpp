#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/COP_SIREN_AEMS.h"
#include "Speed/Indep/Src/EAXSound/SND_GEN/ENGINES_AEMS2.h"
#include "Speed/Indep/Src/EAXSound/SND_GEN/ENVIRO_AEMS.h"
#include "Speed/Indep/Src/EAXSound/SND_GEN/STITCH_AEMS.h"
#include "Speed/Indep/Src/EAXSound/SND_GEN/TURBO.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "snd/sndo.h"

int PRINT_ASSET_WAITLIST = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 77

EAXAemsManager gAEMSMgr; // size: 0x130, address: 0x8045B3E0, Decl: 79

stSndDataLoadParams g_SndAssetList[48]; // size: 0x1380, address: 0x8045B510, Decl: 83

// Decl: 151
const int EAXAemsManager::m_SlotSizes[eBANK_SLOT_MAX_NUM][2] = {
    {157000, 15 * 1024},
    {0, 0},
    {0, 0},
    {0, 20 * 1024},
};

// Decl: 164
int EAXAemsManager::m_RequiredSlots[4];

static bool gSubscribedFlag = false; // size: 0x1, address: 0x804177CC, Decl: 170

int PRINT_SND_IO_DEBUG = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 171

static const bool PRINT_SPU_MEMORY = false; // size: 0x1, Decl: 172

int DISABLE_SLOT_LOADING = 0; // size: 0x4, address: 0x804177D4, Decl: 174

// size: 0x34, address: 0x804177D8, Decl: 176
char *g_DataPaths[MAX_SNDDATA_PATHS] = {
    "sound\\",         "sound\\Engine\\", "sound\\evt_sys\\", "sound\\FE\\",    "sound\\Global\\",   "sound\\IG_Global\\", "sound\\NOS\\",
    "sound\\PFData\\", "sound\\Skids\\",  "sound\\Speech\\",  "sound\\Turbo\\", "sound\\Shifting\\", "sound\\FXEdit\\",
};

stBankSlot *BankSlotSystem::GetFreeSlot(eBANK_SLOT_TYPE Type) {
    BankSlotSystem::iterator i = this->begin();
    for (; i != this->end(); i++) {
        if ((*i).Type == Type && (*i).pAssetParams == nullptr) {
            return &(*i);
        }
    }

    return nullptr;
}

void BankSlotSystem::DestroySlots() {
    BankSlotSystem::iterator i = this->begin();
    while (i != this->end()) {
        stBankSlot &curslot = *i;
        if (curslot.LoadFailed == 0 && curslot.pAssetParams != nullptr) {
            gAEMSMgr.UnloadSndData(curslot.pAssetParams->AssetDescription.FileName);
        }
        if (curslot.MAINmemLocation != nullptr) {
            gAudioMemoryManager.FreeMemory(curslot.MAINmemLocation);
        }
        i++;
    }

    this->clear();
}

EAXAemsManager::EAXAemsManager()
    : m_pAsyncBuff(nullptr),     //
      m_NumEvtSysLoaded(0),      //
      mNumEvtSys(-1),            //
      mAsyncBuffSize(64 * 1024), //
      m_nCallbackEvtSys(-1),     //
      m_pCurLoadSDLP(nullptr),   //
      m_pAsyncLoadSDLP(nullptr), //
      m_ItemsPendingAsyncResolve(0) {
    this->m_bBulkLoad = false;
    this->m_ExternalLoadCallback = nullptr;
    this->m_pEvtSystems.clear();
    this->mWaitForResolve.clear();
    this->mBankSlots.clear();
    this->mPFBankSlot.clear();
    this->m_SPUMainAllocsEnd = 0;
    this->m_SPU_UpperAddress = 0;
}

EAXAemsManager::~EAXAemsManager() {
    this->m_pEvtSystems.clear();

    if (this->m_AsyncBuffLocation == TMP_ALLOC_AUDIO) {
        gAudioMemoryManager.FreeMemory(this->m_pAsyncBuff);
    } else if (this->m_AsyncBuffLocation == TMP_ALLOC_MAIN) {
        bFree(this->m_pAsyncBuff);
    } else if (this->m_AsyncBuffLocation == TMP_ALLOC_TRACKSTREAMER) {
        TheTrackStreamer.FreeUserMemory(this->m_pAsyncBuff);
    }

    this->m_pAsyncBuff = nullptr;
    this->m_pCurLoadSDLP = nullptr;
}

bool EAXAemsManager::AreResourceLoadsPending() {
    if (this->m_pCurLoadSDLP != nullptr || this->m_ItemsPendingAsyncResolve != 0 || this->m_IsWaitingForFileCB ||
        this->m_nCurLoadedBankIndex < this->m_nEndOfList - 1) {
        return true;
    }

    return false;
}

void EAXAemsManager::Init() {
    this->m_NumBankLoadResolves = 0;
    this->mNumEvtSys = g_pEAXSound->GetAttributes().Num_EvtSys();
    if (this->mNumEvtSys > 0) {
        this->m_pEvtSystems.resize(this->mNumEvtSys);
    }

    for (int n = 0; n < 7; n++) {
        this->AddEventSystem(static_cast<eEVTSYS>(n), SNDPATH_EVTSYS);
    }

    this->ResetBankLoadParams();
}

void EAXAemsManager::InitSPUram() {
    int Size;
    int Start = -1;
    this->m_SPU_UpperAddress = Start + SNDmemlargestunused(&Start);
}

// STRIPPED
void EAXAemsManager::RemoveEventSystem(eEVTSYS eESIndex) {}

int EAXAemsManager::AddEventSystem(eEVTSYS eESIndex, eSNDDATAPATH eSDP) {
    Attrib::Gen::audiosystem &atr = g_pEAXSound->GetAttributes();

    bStrCat(this->m_csTemp1, g_DataPaths[eSDP], atr.EvtSys(eESIndex).GetString());

    int nfilesize = bFileSize(this->m_csTemp1);

    this->m_pEvtSystems[eESIndex] = gAudioMemoryManager.AllocateMemoryChar(nfilesize, atr.EvtSys(eESIndex).GetString(), false);

    AddQueuedFile(this->m_pEvtSystems[this->m_NumEvtSysLoaded], this->m_csTemp1, 0, nfilesize, EvtSysLoadCallback, this->m_NumEvtSysLoaded, nullptr);

    int nret = this->m_NumEvtSysLoaded;
    this->m_NumEvtSysLoaded = nret + 1;
    return nret;
}

void SubscribeEventSys();

void EAXAemsManager::EvtSysLoadCallback(int param, int error_status) {
    gAEMSMgr.m_nCallbackEvtSys = param;
    SubscribeEventSys();
}

void *EAXAemsManager::GetCallbackEventSys() {
    return this->m_pEvtSystems[this->m_nCallbackEvtSys];
}

void SubscribeEventSys() {
    if (IsSoundEnabled != 1) {
        return;
    }
    if (gAEMSMgr.GetCallbackEventSys() != nullptr) {
        Csis::Result status = Csis::System::Subscribe(gAEMSMgr.GetCallbackEventSys());
    }
}

void EAXAemsManager::UnloadSndData(Attrib::StringKey filename) {
    int index = this->IsAssetInList(filename);
    if (index != -1) {
        this->UnloadSndData(index);
    }
}

void EAXAemsManager::UnloadSndData(int Index) {
    // TODO enum value?
    if (static_cast<unsigned int>(Index) > 47) {
        return;
    }

    while (this->AreResourceLoadsPending()) {
        ServiceQueuedFiles();
        g_pEAXSound->Update(0.1f);
    }

    this->m_pCurUNLOADSDLP = g_SndAssetList + Index;
    if (!this->m_pCurUNLOADSDLP->bResolvedSync) {
        return;
    }

    this->m_pCurUNLOADSDLP->bResolvedAsync = false;
    this->m_pCurUNLOADSDLP->bResolvedSync = false;
    if (this->m_pCurUNLOADSDLP->AssetDescription.eDataType < SDT_GENERIC_DATA) {
        this->RemoveAEMSBank();
    }

    if (this->m_pCurUNLOADSDLP->plocmem != nullptr) {
        if (this->m_pCurUNLOADSDLP->AssetDescription.eDataType == SDT_AEMS_MAINMEM) {
            bFree(this->m_pCurUNLOADSDLP->plocmem);
        } else {
            gAudioMemoryManager.FreeMemory(this->m_pCurUNLOADSDLP->plocmem);
        }
        this->m_pCurUNLOADSDLP->plocmem = nullptr;
    }

    if (this->m_pCurUNLOADSDLP->pmem != nullptr) {
        if (this->m_pCurUNLOADSDLP->AssetDescription.eDataType == SDT_AEMS_MAINMEM) {
            bFree(this->m_pCurUNLOADSDLP->pmem);
        } else {
            gAudioMemoryManager.FreeMemory(this->m_pCurUNLOADSDLP->pmem);
        }
        this->m_pCurUNLOADSDLP->pmem = nullptr;
    }

    if (this->m_pCurUNLOADSDLP->mBankSlot != nullptr) {
        this->m_pCurUNLOADSDLP->mBankSlot->pAssetParams = nullptr;
        this->m_pCurUNLOADSDLP->mBankSlot->pLastAlloc = this->m_pCurUNLOADSDLP->mBankSlot->MAINmemLocation;
        bMemSet(this->m_pCurUNLOADSDLP->mBankSlot->pLastAlloc, 0, this->m_pCurUNLOADSDLP->mBankSlot->MAINmemSize);
        this->m_pCurUNLOADSDLP->mBankSlot = nullptr;
    }

    while (!this->m_pCurUNLOADSDLP->resallocs.empty()) {
        void *presalloc = reinterpret_cast<void *>(this->m_pCurUNLOADSDLP->resallocs.back());
        this->m_pCurUNLOADSDLP->resallocs.pop_back();
        gAudioMemoryManager.FreeMemory(presalloc);
    }

    this->RemoveBankListing(Index);
    this->m_pCurUNLOADSDLP = nullptr;
}

void EAXAemsManager::RemoveBankListing(int Index) {
    g_SndAssetList[Index].Clear();
    for (int n = Index; n < NUM_ELEMENTS(g_SndAssetList) - 1; n++) {
        g_SndAssetList[n] = g_SndAssetList[n + 1];
        g_SndAssetList[n + 1].Clear();
    }

    this->m_nCurLoadedBankIndex--;
    this->m_nEndOfList--;
}

void EAXAemsManager::RemoveAEMSBank() {
    if (this->m_pCurUNLOADSDLP->Handle != -1) {
        SNDAEMS_removemodulebank(this->m_pCurUNLOADSDLP->Handle);
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

void EAXAemsManager::SetupNextLoad() {
    this->m_pCurLoadSDLP = nullptr;

    if (this->m_nCurLoadedBankIndex < this->m_nEndOfList - 1) {
        this->m_nCurLoadedBankIndex++;
        this->m_pCurLoadSDLP = g_SndAssetList + this->m_nCurLoadedBankIndex;

        if (this->InitiateLoad() < 0) {
            this->mWaitForResolve.DeleteRefToAsset(this->m_pCurLoadSDLP->AssetDescription.FileName);
            this->RemoveBankListing(this->m_nCurLoadedBankIndex);
            this->SetupNextLoad();
        }
    }
}

void EAXAemsManager::ResetBankLoadParams() {
    this->m_nCurLoadedBankIndex = -1;
    this->m_nEndOfList = 0;
    this->mWaitForResolve.clear();
    this->DestroySlots(true);
}

int EAXAemsManager::InitiateLoad() {
    bStrCat(this->m_csTemp1, g_DataPaths[this->m_pCurLoadSDLP->AssetDescription.DataPath],
            this->m_pCurLoadSDLP->AssetDescription.FileName.GetString());

    this->m_pCurLoadSDLP->nSize = bFileSize(this->m_csTemp1);
    if (this->m_pCurLoadSDLP->nSize < 1) {
        return -1;
    }

    if (this->m_pCurLoadSDLP->AssetDescription.eDataType < SDT_GENERIC_DATA) {
        if (this->m_pAsyncBuff == nullptr) {
            this->m_AsyncBuffLocation = TMP_ALLOC_AUDIO;
            bLargestMalloc(AudioMemoryPool);

            int nlargestbuff = bLargestMalloc(0);

            if (nlargestbuff <= 128 * 1024) {
                this->m_pAsyncBuff = static_cast<char *>(TheTrackStreamer.AllocateUserMemory(64 * 1024, "EAXAemsManager::m_pAsyncBuff", 0));
                this->m_AsyncBuffLocation = TMP_ALLOC_TRACKSTREAMER;

                if (this->m_pAsyncBuff == nullptr) {
                    if (this->m_pCurLoadSDLP->mBankSlot != nullptr) {
                        this->m_pCurLoadSDLP->mBankSlot->LoadFailed = 1;
                    }
                    return -2;
                }
            } else {
                this->m_AsyncBuffLocation = TMP_ALLOC_MAIN;
                this->m_pAsyncBuff = static_cast<char *>(bMalloc(64 * 1024, "EAXAemsManager::m_pAsyncBuff", 0, 0));
            }
        }

        if (this->m_pCurLoadSDLP->mBankSlot != nullptr) {
            this->m_pCurLoadSDLP->mBankSlot->LoadFailed = 0;
        }

        this->m_pCurLoadSDLP->MemLocation = TMP_ALLOC_NONE;
        this->m_pCurLoadSDLP->AssetDescription.eDataType = SDT_AEMS_ASYNCSPU;
    } else {
        this->m_pCurLoadSDLP->MemLocation = TMP_ALLOC_AUDIO;
    }

    QueuedFileParams queued_file_params;
    queued_file_params.Priority = QueuedFileDefaultPriority - 2;

    switch (this->m_pCurLoadSDLP->MemLocation) {
        case TMP_ALLOC_MAIN:
            if (bLargestMalloc(0) < this->m_pCurLoadSDLP->nSize) {
                return -2;
            }

            this->m_pCurLoadSDLP->pmem = bMalloc(this->m_pCurLoadSDLP->nSize, this->m_pCurLoadSDLP->AssetDescription.FileName.GetString(), 0, 0x1040);

            AddQueuedFile(this->m_pCurLoadSDLP->pmem, this->m_csTemp1, 0, this->m_pCurLoadSDLP->nSize, DataLoadCB,
                          reinterpret_cast<intptr_t>(this->m_pCurLoadSDLP), &queued_file_params);

            this->m_IsWaitingForFileCB = true;
            break;

        case TMP_ALLOC_TRACKSTREAMER:
            if (TheTrackStreamer.GetMemoryPoolSize() == 0) {
                return -3;
            }

            this->m_pCurLoadSDLP->pmem =
                TheTrackStreamer.AllocateUserMemory(this->m_pCurLoadSDLP->nSize, this->m_pCurLoadSDLP->AssetDescription.FileName.GetString(), 0);

            if (this->m_pCurLoadSDLP->pmem == nullptr) {
                return -3;
            }

            AddQueuedFile(this->m_pCurLoadSDLP->pmem, this->m_csTemp1, 0, this->m_pCurLoadSDLP->nSize, DataLoadCB,
                          reinterpret_cast<intptr_t>(this->m_pCurLoadSDLP), &queued_file_params);

            this->m_IsWaitingForFileCB = true;
            break;

        case TMP_ALLOC_AUDIO:
            if (this->m_pCurLoadSDLP->AssetDescription.eDataType == SDT_GENERIC_DATA && this->m_pCurLoadSDLP->mBankSlot != nullptr) {
                if (this->m_pCurLoadSDLP->nSize > this->m_pCurLoadSDLP->mBankSlot->MAINmemSize) {
                    return -4;
                }

                AddQueuedFile(this->m_pCurLoadSDLP->mBankSlot->MAINmemLocation, this->m_csTemp1, 0, this->m_pCurLoadSDLP->nSize, DataLoadCB,
                              reinterpret_cast<intptr_t>(this->m_pCurLoadSDLP), &queued_file_params);

                this->m_IsWaitingForFileCB = true;
                break;
            }

            if (bLargestMalloc(AudioMemoryPool) < this->m_pCurLoadSDLP->nSize) {
                return -4;
            }

            this->m_pCurLoadSDLP->pmem =
                gAudioMemoryManager.AllocateMemory(this->m_pCurLoadSDLP->nSize, this->m_pCurLoadSDLP->AssetDescription.FileName.GetString(),
                                                   this->m_pCurLoadSDLP->AssetDescription.bLoadToTop);

            AddQueuedFile(this->m_pCurLoadSDLP->pmem, this->m_csTemp1, 0, this->m_pCurLoadSDLP->nSize, DataLoadCB,
                          reinterpret_cast<intptr_t>(this->m_pCurLoadSDLP), &queued_file_params);

            this->m_IsWaitingForFileCB = true;
            break;

        case TMP_ALLOC_NONE:
            this->m_IsWaitingForFileCB = false;
            break;

        default:
            break;
    }

    if (this->m_pCurLoadSDLP->AssetDescription.eDataType == SDT_AEMS_ASYNCSPU) {
        int Result;
        if (this->m_pCurLoadSDLP->mBankSlot != nullptr) {
            Result = SNDmemlimits(this->m_pCurLoadSDLP->mBankSlot->BANKmemLocation,
                                  this->m_pCurLoadSDLP->mBankSlot->BANKmemLocation + this->m_pCurLoadSDLP->mBankSlot->BANKMemSize);
        } else {
            Result = SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
        }

        int nhandle = SNDAEMS_asyncloadmodulebank(this->m_csTemp1, 0, nullptr, 0, this->m_pAsyncBuff, this->mAsyncBuffSize, AsyncResidentAllocCB);
        this->m_pCurLoadSDLP->Handle = nhandle;
    }

    return 0;
}

void EAXAemsManager::CheckForCompleteAsyncLoad() {
    if (!this->m_pCurLoadSDLP->bResolvedAsync) {
        switch (this->m_pCurLoadSDLP->AssetDescription.eDataType) {
            case SDT_AEMS_ASYNCSPU:
                if (SNDAEMS_asyncloadmodulebankdone() > 0) {
                    this->CompleteAsyncLoad();
                }
                break;
            case SDT_AEMS_ASYNCSPUMEM:
                if (SNDAEMS_asyncloadmodulebankmemdone() > 0 && !this->m_IsWaitingForFileCB) {
                    this->CompleteAsyncLoad();
                }
                break;
            case SDT_AEMS_SYNCSPU:
            default:
                break;
        }
    }
}

int StartBankLoadTicks = 0; // Decl: 1336

void EAXAemsManager::CompleteAsyncLoad() {
    stSndDataLoadParams *m_pCurrentlyLoading;
    if (gAEMSMgr.m_pAsyncLoadSDLP != nullptr) {
        m_pCurrentlyLoading = gAEMSMgr.m_pAsyncLoadSDLP;
    } else {
        m_pCurrentlyLoading = gAEMSMgr.m_pCurLoadSDLP;
    }
    m_pCurrentlyLoading->bResolvedAsync = true;
    int Result = SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    m_pCurrentlyLoading->t_load = WorldTimer;
    float delta = (m_pCurrentlyLoading->t_load - m_pAsyncLoadSDLP->t_req).GetSeconds();
    if (PRINT_SND_IO_DEBUG) {
        // TODO printf from Undercover
    }
}

const int DEBUG_PERFORM_MEMORY_PROFILING = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 1365
int PerformingProfile;                        // size: 0x4, address: 0xFFFFFFFF, Decl: 1366
void *testBankLoading;                        // size: 0x4, address: 0xFFFFFFFF, Decl: 1367

// STRIPPED
void *testcallback(void *pbank, int residentsize, int totalsize) {
    return nullptr;
}

void EAXAemsManager::ResolvePendingAsyncLoads() {}

void EAXAemsManager::Update() {
RestartLoop:
    SndAssetQueue::iterator i = this->mWaitForResolve.begin();
    while (i != this->mWaitForResolve.end()) {
        stSndAssetQueue currequst = *i;
        int index = this->IsAssetLoaded(currequst.Asset.FileName);
        if (index != -1) {
            bool callinit = true;
            i++;
            SndAssetQueue::iterator othercallbacks = this->mWaitForResolve.begin();
            while (othercallbacks != this->mWaitForResolve.end()) {
                stSndAssetQueue futureitems = *othercallbacks;
                if (futureitems == currequst) {
                    othercallbacks++;
                    continue;
                }
                if (futureitems.pThis == currequst.pThis) {
                    callinit = false;
                }
                othercallbacks++;
            }

            if (callinit && currequst.pThis != nullptr) {
                currequst.pThis->InitSFX();
            }

            this->mWaitForResolve.remove(currequst);
            goto RestartLoop;
        }
        i++;
    }

    if (this->m_ItemsPendingAsyncResolve != 0 && this->m_bBulkLoad) {
        this->ResolvePendingAsyncLoads();
    }

    if (this->m_pCurLoadSDLP != nullptr) {
        if (!this->m_pCurLoadSDLP->bResolvedAsync) {
            this->CheckForCompleteAsyncLoad();
        }

        this->m_pCurLoadSDLP->bResolvedSync =
            static_cast<bool>(Joylog::AddOrGetData(static_cast<unsigned int>(this->m_pCurLoadSDLP->bResolvedAsync), 8, JOYLOG_CHANNEL_SOUND_LOADING));

        if (Joylog::IsReplaying() != 0 && this->m_pCurLoadSDLP->bResolvedSync) {
            while (!this->m_pCurLoadSDLP->bResolvedAsync) {
                SNDSYS_service();
                this->CheckForCompleteAsyncLoad();
            }
        }

        if (this->m_pCurLoadSDLP->bResolvedSync) {
            if (this->m_pCurLoadSDLP->AssetDescription.eDataType < SDT_GENERIC_DATA) {
                this->ResolveCurrentDataMemory();
            }
            this->SetupNextLoad();
        }
    } else {
        this->SetupNextLoad();
        if (this->m_pCurLoadSDLP == nullptr && this->m_ItemsPendingAsyncResolve == 0) {
            if (this->m_pAsyncBuff != nullptr) {
                if (this->m_AsyncBuffLocation == TMP_ALLOC_AUDIO) {
                    gAudioMemoryManager.FreeMemory(this->m_pAsyncBuff);
                } else if (this->m_AsyncBuffLocation == TMP_ALLOC_MAIN) {
                    bFree(this->m_pAsyncBuff);
                } else if (this->m_AsyncBuffLocation == TMP_ALLOC_TRACKSTREAMER) {
                    TheTrackStreamer.FreeUserMemory(this->m_pAsyncBuff);
                }
                this->m_pAsyncBuff = nullptr;
            }

            if (m_ExternalLoadCallback != nullptr) {
                Csis::CacheHandlesMAIN_AEMS();
                Csis::CacheHandlesENGINES_AEMS2();
                Csis::CacheHandlesENVIRO_AEMS();
                Csis::CacheHandlesTURBO();
                Csis::CacheHandlesFE_AEMS();
                Csis::CacheHandlesCOP_SIREN_AEMS();
                Csis::CacheHandlesSTITCH_AEMS();

                this->m_ExternalLoadCallback(this->m_ExternalLoadCallbackParam);
                this->m_ExternalLoadCallback = nullptr;
                this->m_ExternalLoadCallbackParam = 0;
                this->m_bBulkLoad = false;
            }
        }
    }
}

void *EAXAemsManager::AsyncResidentAllocCB(int size) {
    stSndDataLoadParams *m_pCurrentlyLoading;
    if (gAEMSMgr.m_pAsyncLoadSDLP != nullptr) {
        m_pCurrentlyLoading = gAEMSMgr.m_pAsyncLoadSDLP;
    } else {
        m_pCurrentlyLoading = gAEMSMgr.m_pCurLoadSDLP;
    }

    void *newresalloc;
    if (m_pCurrentlyLoading->mBankSlot != nullptr) {
        newresalloc = m_pCurrentlyLoading->mBankSlot->pLastAlloc;
        m_pCurrentlyLoading->mBankSlot->pLastAlloc += size;
    } else {
        char dbgstring[64];
        bStrCat(dbgstring, m_pCurrentlyLoading->AssetDescription.FileName.GetString(), ": resident allocation");
        newresalloc = gAudioMemoryManager.AllocateMemory(size, dbgstring, true);
        uintptr_t newresallocaddr = reinterpret_cast<uintptr_t>(newresalloc);
        m_pCurrentlyLoading->resallocs.push_back(newresallocaddr);
    }
    return newresalloc;
}

void *EAXAemsManager::ResidentAllocCB(void *pbank, int residentsize, int totalsize) {
    if (residentsize != totalsize) {
        if (gAEMSMgr.m_pCurLoadSDLP->AssetDescription.eDataType == SDT_AEMS_MAINMEM) {
            gAEMSMgr.m_pCurLoadSDLP->plocmem = bMalloc(residentsize, gAEMSMgr.m_pCurLoadSDLP->AssetDescription.FileName.GetString(), 0, 0x1040);
            gAEMSMgr.m_NumBankLoadResolves++;
            return gAEMSMgr.m_pCurLoadSDLP->plocmem;
        } else {
            if (gAEMSMgr.m_pCurLoadSDLP->mBankSlot != nullptr) {
                gAEMSMgr.m_pCurLoadSDLP->mBankSlot->pLastAlloc += residentsize;
                return gAEMSMgr.m_pCurLoadSDLP->mBankSlot->MAINmemLocation;
            }

            gAEMSMgr.m_pCurLoadSDLP->plocmem =
                gAudioMemoryManager.AllocateMemory(residentsize, gAEMSMgr.m_pCurLoadSDLP->AssetDescription.FileName.GetString(), false);
            gAEMSMgr.m_NumBankLoadResolves++;
            return gAEMSMgr.m_pCurLoadSDLP->plocmem;
        }
    } else {
        return gAEMSMgr.m_pCurLoadSDLP->pmem;
    }
}

void EAXAemsManager::DataLoadCB(intptr_t param, int error_status) {
    StartBankLoadTicks = bGetTicker();
    gAEMSMgr.m_pCurLoadSDLP = reinterpret_cast<stSndDataLoadParams *>(param);
    gAEMSMgr.m_pCurLoadSDLP->t_load = WorldTimer;
    eSNDDATATYPE eDataType = gAEMSMgr.m_pCurLoadSDLP->AssetDescription.eDataType;

    switch (eDataType) {
        case SDT_AEMS_AUDIOMEM:
        case SDT_AEMS_MAINMEM:
        case SDT_AEMS_SYNCSPU:
            AddAemsBank();
            gAEMSMgr.ResolveCurrentDataMemory();
            gAEMSMgr.m_pCurLoadSDLP->bResolvedAsync = true;
            gAEMSMgr.m_pCurLoadSDLP->bResolvedSync = true;
            break;

        case SDT_AEMS_ASYNCSPU:
            break;

        case SDT_AEMS_ASYNCSPUMEM:
            gAEMSMgr.m_IsWaitingForFileCB = false;
            if (gAEMSMgr.m_bBulkLoad) {
                gAEMSMgr.m_pCurLoadSDLP = nullptr;
                gAEMSMgr.m_ItemsPendingAsyncResolve++;
                break;
            } else {
                stBankSlot *pBankSlot = gAEMSMgr.m_pCurLoadSDLP->mBankSlot;
                if (pBankSlot != nullptr) {
                    SNDmemlimits(pBankSlot->BANKmemLocation, pBankSlot->BANKmemLocation + pBankSlot->BANKMemSize);
                } else {
                    SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
                }
            }

            gAEMSMgr.m_pCurLoadSDLP->Handle = SNDAEMS_asyncloadmodulebankmem(gAEMSMgr.m_pCurLoadSDLP->pmem, nullptr, 0, AsyncResidentAllocCB);
            break;

        case SDT_GENERIC_DATA:
            gAEMSMgr.m_pCurLoadSDLP->bResolvedAsync = true;
            gAEMSMgr.m_pCurLoadSDLP->bResolvedSync = true;
            break;

        default:
            break;
    }

    gAEMSMgr.m_IsWaitingForFileCB = false;
}

void EAXAemsManager::ResolveCurrentDataMemory() {
    switch (gAEMSMgr.m_pCurLoadSDLP->MemLocation) {
        case TMP_ALLOC_NONE:
            break;

        case TMP_ALLOC_MAIN:
            bFree(gAEMSMgr.m_pCurLoadSDLP->pmem);
            gAEMSMgr.m_pCurLoadSDLP->pmem = nullptr;
            break;

        case TMP_ALLOC_TRACKSTREAMER:
            TheTrackStreamer.FreeUserMemory(gAEMSMgr.m_pCurLoadSDLP->pmem);
            gAEMSMgr.m_pCurLoadSDLP->pmem = nullptr;
            break;

        case TMP_ALLOC_AUDIO:
            if (gAEMSMgr.m_pCurLoadSDLP->plocmem == nullptr) {
                if (gAEMSMgr.m_pCurLoadSDLP->mBankSlot == nullptr) {
                    return;
                }
            }

            gAudioMemoryManager.FreeMemory(gAEMSMgr.m_pCurLoadSDLP->pmem);
            gAEMSMgr.m_pCurLoadSDLP->pmem = nullptr;
            break;

        default:
            break;
    }
}

// STRIPPED
void EAXAemsManager::DebugPrints() {}

int EAXAemsManager::AddBankListing(stAssetDescription &asset) {
    g_SndAssetList[this->m_nEndOfList].Clear();
    g_SndAssetList[this->m_nEndOfList].AssetDescription = asset;

    // TODO fake match
    int FAKE = this->m_nEndOfList;
    this->m_nEndOfList++;
    return FAKE;
}

int EAXAemsManager::IsAssetInList(Attrib::StringKey filename) {
    for (int n = 0; n < this->m_nEndOfList; n++) {
        if (filename == g_SndAssetList[n].AssetDescription.FileName) {
            return n;
        }
    }
    return -1;
}

int EAXAemsManager::IsAssetLoaded(Attrib::StringKey filename) {
    int n = this->IsAssetInList(filename);
    if (n == -1) {
        return -1;
    }
    if (!g_SndAssetList[n].bResolvedSync) {
        return -1;
    }
    return n;
}

void EAXAemsManager::QueueFileLoad(stSndAssetQueue &queueitem, eBANK_SLOT_TYPE SlotType) {
    int BankIndex = this->IsAssetInList(queueitem.Asset.FileName);

    if (BankIndex == -1) {
        if (DISABLE_SLOT_LOADING != 0) {
            SlotType = eBANK_SLOT_NONE;
        }

        stBankSlot *Slot = nullptr;

        if (SlotType != eBANK_SLOT_NONE) {
            if (SlotType == eBANK_SLOT_PATHFINDER) {
                Slot = this->mPFBankSlot.GetFreeSlot(eBANK_SLOT_PATHFINDER);
            } else {
                Slot = this->mBankSlots.GetFreeSlot(SlotType);
            }

            if (Slot == nullptr) {
                if (queueitem.pThis != nullptr) {
                    this->mWaitForResolve.DeleteRefToAsset(queueitem.pThis);
                }
                return;
            }
        }

        BankIndex = this->AddBankListing(queueitem.Asset);

        if (Slot != nullptr) {
            Slot->pAssetParams = &g_SndAssetList[BankIndex];
            g_SndAssetList[BankIndex].mBankSlot = Slot;
        }
    }

    this->mWaitForResolve.push_back(queueitem);
}

void EAXAemsManager::InitializeSlots(bool bDoPFSlot) {
    if (DISABLE_SLOT_LOADING == 0) {
        if (bDoPFSlot) {
            this->mPFBankSlot.DestroySlots();
        }
        this->mBankSlots.DestroySlots();
        this->m_SPUMainAllocsEnd = this->m_SPU_UpperAddress;
        for (int n = 0; n < eBANK_SLOT_MAX_NUM; n++) {
            this->RegisterSlots(static_cast<eBANK_SLOT_TYPE>(n), m_RequiredSlots[n], m_SlotSizes[n][0], m_SlotSizes[n][1], bDoPFSlot);
        }
        SNDmemlimits(-1, this->m_SPUMainAllocsEnd);
    }
}

void EAXAemsManager::DestroySlots(bool bDoPFSlot) {
    if (bDoPFSlot == true) {
        this->mPFBankSlot.DestroySlots();
    }
    this->mBankSlots.DestroySlots();
    this->m_SPUMainAllocsEnd = this->m_SPU_UpperAddress;
    SNDmemlimits(-1, this->m_SPU_UpperAddress);
    bMemSet(m_RequiredSlots, 0, sizeof(m_RequiredSlots));
}

// STRIPPED
char *GetSlotName(eBANK_SLOT_TYPE Type) {
    return nullptr;
}

void EAXAemsManager::RegisterSlots(eBANK_SLOT_TYPE Type, int NumSlots, int SizePerSlotSPU, int SizePerSlotMainMem, bool bDoPFSlot) {
    stBankSlot NewSlot;

    for (int n = 0; n < NumSlots; n++) {
        char DebugString[1];
        DebugString[0] = '\0';

        NewSlot.Index = static_cast<uint8>(n);
        NewSlot.Type = Type;
        NewSlot.MAINmemLocation = static_cast<char *>(gAudioMemoryManager.AllocateMemory(SizePerSlotMainMem, DebugString, false));
        NewSlot.pLastAlloc = NewSlot.MAINmemLocation;
        NewSlot.MAINmemSize = SizePerSlotMainMem;

        this->m_SPUMainAllocsEnd -= SizePerSlotSPU;
        NewSlot.BANKmemLocation = this->m_SPUMainAllocsEnd;

        NewSlot.BANKMemSize = SizePerSlotSPU;
        NewSlot.pAssetParams = nullptr;

        if (Type == eBANK_SLOT_PATHFINDER && bDoPFSlot) {
            this->mPFBankSlot.push_back(NewSlot);
        } else {
            this->mBankSlots.push_back(NewSlot);
        }
    }
}
