#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

extern "C" int SNDAEMS_removemodulebank(int handle);
extern "C" int SNDAEMS_addmodulebank(void *pmem, char *chunk, int offset, void *(*cb)(void *, int, int));
extern "C" int SNDAEMS_asyncloadmodulebank(char *filename, int unk0, char *chunk, int offset, void *pmem, int size,
                                           void *(*cb)(int));
extern "C" int SNDAEMS_asyncloadmodulebankmem(void *pmem, char *chunk, int offset, void *(*cb)(int));
extern "C" int SNDAEMS_asyncloadmodulebankdone();
extern "C" int SNDAEMS_asyncloadmodulebankmemdone();
void SubscribeEventSys();
char *g_DataPaths[] = {"sound\\", "sound\\Engine\\", "sound\\evt_sys\\", "sound\\FE\\", "sound\\Global\\", "sound\\IG_Global\\", "sound\\NOS\\", "sound\\PFData\\", "sound\\Skids\\", "sound\\Speech\\", "sound\\Turbo\\", "sound\\Shifting\\", "sound\\FXEdit\\"};
int DISABLE_SLOT_LOADING = 0;
extern "C" int SNDmemlargestunused(int *start);
extern "C" int SNDmemlimits(int lower, int upper);
extern void *bMalloc(int size, int allocation_params);
extern void *bMalloc(int size, const char *name, int line, int pool);
extern char *bStrCat(char *destString, const char *s1, const char *s2);
extern void bFree(void *ptr);
extern "C" void SNDSYS_service();
extern int QueuedFileDefaultPriority;
extern stSndDataLoadParams g_SndAssetList[];
extern Timer WorldTimer;
int StartBankLoadTicks = 0;

// .rodata:0x803D6B78, 0x20 B. La declara EAXAemsManager.h:248 y no la definia
// nadie. Valores leidos del zEAXSound extraido.
const int EAXAemsManager::m_SlotSizes[4][2] = {
    {157000, 15360},
    {0, 0},
    {0, 0},
    {0, 20480},
};
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

stBankSlot *BankSlotSystem::GetFreeSlot(eBANK_SLOT_TYPE Type) {
    for (BankSlotSystem::iterator i = this->begin(); i != this->end(); i++) {
        if ((*i).Type == Type && (*i).pAssetParams == nullptr) {
            return &(*i);
        }
    }

    return nullptr;
}

void BankSlotSystem::DestroySlots() {
    BankSlotSystem::iterator i = this->begin();
    while (i != this->end()) {
        {
            stBankSlot &curslot = *i;
            if (curslot.LoadFailed == 0 && curslot.pAssetParams != nullptr) {
                gAEMSMgr.UnloadSndData(curslot.pAssetParams->AssetDescription.FileName);
            }
            if (curslot.MAINmemLocation != nullptr) {
                gAudioMemoryManager.FreeMemory(curslot.MAINmemLocation);
            }
            i++;
        }
    }

    this->clear();
}

EAXAemsManager::EAXAemsManager() : m_pAsyncBuff(nullptr) {
    this->m_NumEvtSysLoaded = 0;
    this->mNumEvtSys = -1;
    this->mAsyncBuffSize = 0x10000;
    this->m_nCallbackEvtSys = -1;
    this->m_pCurLoadSDLP = nullptr;
    this->m_pAsyncLoadSDLP = nullptr;
    this->m_ItemsPendingAsyncResolve = 0;
    this->m_bBulkLoad = false;
    m_ExternalLoadCallback = nullptr;
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
    if (this->m_pCurLoadSDLP != nullptr || this->m_ItemsPendingAsyncResolve != 0 ||
        this->m_IsWaitingForFileCB != 0 || this->m_nCurLoadedBankIndex < (this->m_nEndOfList - 1)) {
        return true;
    }
    return false;
}

void EAXAemsManager::Init() {
    this->m_NumBankLoadResolves = 0;
    unsigned int numEvtSys = g_pEAXSound->GetAttributes().Num_EvtSys();
    this->mNumEvtSys = numEvtSys;
    if (static_cast<int>(numEvtSys) > 0) {
        this->m_pEvtSystems.resize(numEvtSys);
    }

    int n = 0;
    do {
        this->AddEventSystem(static_cast<eEVTSYS>(n), SNDPATH_EVTSYS);
        n++;
    } while (n < 7);

    this->ResetBankLoadParams();
}

void EAXAemsManager::InitSPUram() {
    int Size;
    int Start = -1;
    Size = SNDmemlargestunused(&Start);
    this->m_SPU_UpperAddress = Start + Size;
}

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
    if (static_cast<unsigned int>(Index) > 0x2F) {
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
        bMemSet(this->m_pCurUNLOADSDLP->mBankSlot->pLastAlloc, '\0', this->m_pCurUNLOADSDLP->mBankSlot->MAINmemSize);
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
    {
        for (int n = Index; n < 0x2F; n++) {
            g_SndAssetList[n] = g_SndAssetList[n + 1];
            g_SndAssetList[n + 1].Clear();
        }
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

// Iter6: 1008 B exactos junto al reinicio de DeleteRefToAsset de la cabecera.
// Tras borrar una referencia se vuelve a buscar sobre el mismo SndBase.
// Las cuatro entradas reales de this elevan su prioridad RTL a 16 refs;
// endIndex y bankIndex estan inicializados y conservan su valor en cada asm.
// Dos cadenas separadas mantienen el reparto sin mover los argumentos.
// r67b: IRREDUCIBLES las cuatro barreras (base zEAXSound 9e92525a5436ae95). El DWARF del original
// no trae esta funcion ("Failed to process tag E5B398"); el mapa de lineas da la forma SIN locales
// (912 puntero a NULL, 913 if, 915 ++, 916 puntero, 919 DeleteRefToAsset, 922 RemoveBankListing,
// 925 la recursion). Escrita asi y sin barreras: a03b55692ad0849f, 17 filas (`this` en r27, no r29).
// Quitando solo las barreras y dejando las locales: el MISMO digest. En el .greg, `this` (pseudo 82)
// tiene 12 refs en 241 insns -> prioridad 1493, por debajo de las dos givs de deleteCount del inline
// DeleteRefToAsset de EAXAemsManager.h (6 refs en 58 insns -> 2068), que se quedan r29 y r28. Con 16
// refs floor_log2 sube de 3 a 4 y la prioridad a 2612 (la de la base): son justo las cuatro entradas
// "r"(this). Desde este .cpp no hay cuatro referencias reales mas; si hay palanca, esta en la vida o
// en las refs de esas givs, o sea en la cabecera.
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
    int result;
    int nbytes;
    char *to = this->m_csTemp1;

    bStrCat(to, g_DataPaths[this->m_pCurLoadSDLP->AssetDescription.DataPath],
            this->m_pCurLoadSDLP->AssetDescription.FileName.GetString());
    result = bFileSize(to);
    this->m_pCurLoadSDLP->nSize = result;
    if (this->m_pCurLoadSDLP->nSize < 1) {
        return -1;
    }

    if (this->m_pCurLoadSDLP->AssetDescription.eDataType < SDT_GENERIC_DATA) {
        if (this->m_pAsyncBuff == nullptr) {
            this->m_AsyncBuffLocation = TMP_ALLOC_AUDIO;
            bLargestMalloc(AudioMemoryPool);
            {
                int nlargestbuff = bLargestMalloc(0);
                if (nlargestbuff <= 0x20000) {
                    this->m_pAsyncBuff = static_cast<char *>(TheTrackStreamer.AllocateUserMemory(0x10000, "EAXAemsManager::m_pAsyncBuff", 0));
                    this->m_AsyncBuffLocation = TMP_ALLOC_TRACKSTREAMER;
                    if (this->m_pAsyncBuff == nullptr) {
                        if (this->m_pCurLoadSDLP->mBankSlot != nullptr) {
                            this->m_pCurLoadSDLP->mBankSlot->LoadFailed = 1;
                        }
                        return -2;
                    }
                } else {
                    this->m_AsyncBuffLocation = TMP_ALLOC_MAIN;
                    this->m_pAsyncBuff = static_cast<char *>(bMalloc(0x10000, "EAXAemsManager::m_pAsyncBuff", 0, 0));
                }
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
    {
        QueuedFileParams queuedFileParams;
        queuedFileParams.Priority = QueuedFileDefaultPriority - 2;
        eTEMPALLOCLOCATION memLocation = this->m_pCurLoadSDLP->MemLocation;
        switch (memLocation) {
        case TMP_ALLOC_MAIN:
            result = bLargestMalloc(0);
            if (result < this->m_pCurLoadSDLP->nSize) {
                return -2;
            }
            this->m_pCurLoadSDLP->pmem = bMalloc(this->m_pCurLoadSDLP->nSize,
                                                 this->m_pCurLoadSDLP->AssetDescription.FileName.GetString(), 0, 0x1040);
            AddQueuedFile(this->m_pCurLoadSDLP->pmem, this->m_csTemp1, 0, this->m_pCurLoadSDLP->nSize, DataLoadCB,
                          reinterpret_cast<int>(this->m_pCurLoadSDLP), &queuedFileParams);
            this->m_IsWaitingForFileCB = true;
            break;
        case TMP_ALLOC_TRACKSTREAMER:
            result = TheTrackStreamer.GetMemoryPoolSize();
            if (result == 0) {
                return -3;
            }
            this->m_pCurLoadSDLP->pmem =
                TheTrackStreamer.AllocateUserMemory(this->m_pCurLoadSDLP->nSize,
                                                    this->m_pCurLoadSDLP->AssetDescription.FileName.GetString(),
                                                    0);
            if (this->m_pCurLoadSDLP->pmem == nullptr) {
                return -3;
            }
            nbytes = this->m_pCurLoadSDLP->nSize;
            AddQueuedFile(this->m_pCurLoadSDLP->pmem, this->m_csTemp1, 0, nbytes, DataLoadCB,
                          reinterpret_cast<int>(this->m_pCurLoadSDLP), &queuedFileParams);
            this->m_IsWaitingForFileCB = true;
            break;
        case TMP_ALLOC_AUDIO:
            if (this->m_pCurLoadSDLP->AssetDescription.eDataType == SDT_GENERIC_DATA) {
                if (this->m_pCurLoadSDLP->mBankSlot != nullptr) {
                    nbytes = this->m_pCurLoadSDLP->nSize;
                    if (nbytes > this->m_pCurLoadSDLP->mBankSlot->MAINmemSize) {
                        return -4;
                    }
                    AddQueuedFile(this->m_pCurLoadSDLP->mBankSlot->MAINmemLocation, this->m_csTemp1, 0, nbytes, DataLoadCB,
                                  reinterpret_cast<int>(this->m_pCurLoadSDLP), &queuedFileParams);
                    this->m_IsWaitingForFileCB = true;
                    break;
                }
            }
            result = bLargestMalloc(AudioMemoryPool);
            if (result < this->m_pCurLoadSDLP->nSize) {
                return -4;
            }
            this->m_pCurLoadSDLP->pmem = gAudioMemoryManager.AllocateMemory(
                this->m_pCurLoadSDLP->nSize,
                this->m_pCurLoadSDLP->AssetDescription.FileName.GetString(),
                this->m_pCurLoadSDLP->AssetDescription.bLoadToTop);
            nbytes = this->m_pCurLoadSDLP->nSize;
            AddQueuedFile(this->m_pCurLoadSDLP->pmem, this->m_csTemp1, 0, nbytes, DataLoadCB,
                          reinterpret_cast<int>(this->m_pCurLoadSDLP), &queuedFileParams);
            this->m_IsWaitingForFileCB = true;
            break;
        case TMP_ALLOC_NONE:
            this->m_IsWaitingForFileCB = false;
            break;
        default:
            break;
        }
        if (this->m_pCurLoadSDLP->AssetDescription.eDataType == SDT_AEMS_ASYNCSPU) {
            if (this->m_pCurLoadSDLP->mBankSlot != nullptr) {
                SNDmemlimits(this->m_pCurLoadSDLP->mBankSlot->BANKmemLocation,
                             this->m_pCurLoadSDLP->mBankSlot->BANKmemLocation + this->m_pCurLoadSDLP->mBankSlot->BANKMemSize);
            } else {
                SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
            }
            result = SNDAEMS_asyncloadmodulebank(this->m_csTemp1, 0, nullptr, 0, this->m_pAsyncBuff, this->mAsyncBuffSize,
                                                 AsyncResidentAllocCB);
            this->m_pCurLoadSDLP->Handle = result;
        }
        result = 0;
    }
    return result;
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
            if (SNDAEMS_asyncloadmodulebankmemdone() > 0 && this->m_IsWaitingForFileCB == 0) {
                this->CompleteAsyncLoad();
            }
            break;
        case SDT_AEMS_SYNCSPU:
        default:
            break;
        }
    }
}

void EAXAemsManager::CompleteAsyncLoad() {
    stSndDataLoadParams *m_pCurrentlyLoading;
    int Result;
    float delta;
    if (gAEMSMgr.m_pAsyncLoadSDLP != nullptr) {
        m_pCurrentlyLoading = gAEMSMgr.m_pAsyncLoadSDLP;
    } else {
        m_pCurrentlyLoading = gAEMSMgr.m_pCurLoadSDLP;
    }
    m_pCurrentlyLoading->bResolvedAsync = 1;
    SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
    m_pCurrentlyLoading->t_load = WorldTimer;
    delta = (WorldTimer - Timer(static_cast<int>(StartBankLoadTicks))).GetSeconds();
}

void EAXAemsManager::ResolvePendingAsyncLoads() {}

void EAXAemsManager::Update() {
ReprocessQueue:
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
            goto ReprocessQueue;
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
            Joylog::AddOrGetData(static_cast<unsigned int>(this->m_pCurLoadSDLP->bResolvedAsync), 8, JOYLOG_CHANNEL_SOUND_LOADING) != 0;

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

                m_ExternalLoadCallback(this->m_ExternalLoadCallbackParam);
                m_ExternalLoadCallback = nullptr;
                this->m_ExternalLoadCallbackParam = 0;
                this->m_bBulkLoad = false;
            }
        }
    }
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

    if (m_pCurrentlyLoading->mBankSlot != nullptr) {
        newresalloc = m_pCurrentlyLoading->mBankSlot->pLastAlloc;
        m_pCurrentlyLoading->mBankSlot->pLastAlloc += size;
    } else {
        const char *filename = m_pCurrentlyLoading->AssetDescription.FileName.GetString();
        char dbgstring[64];
        bStrCat(dbgstring, filename, ": resident allocation");
        newresalloc = gAudioMemoryManager.AllocateMemory(size, dbgstring, true);
        unsigned int newresallocaddr = reinterpret_cast<unsigned int>(newresalloc);
        m_pCurrentlyLoading->resallocs.push_back(newresallocaddr);
    }
    return newresalloc;
}

void *EAXAemsManager::ResidentAllocCB(void *pbank, int residentsize, int totalsize) {
    (void)pbank;
    if (residentsize != totalsize) {
        EAXAemsManager &mgr = gAEMSMgr;
        void *resmem;
        if (mgr.m_pCurLoadSDLP->AssetDescription.eDataType == SDT_AEMS_MAINMEM) {
            resmem = bMalloc(residentsize, 0x1040);
        } else {
            stBankSlot *pBankSlot = mgr.m_pCurLoadSDLP->mBankSlot;
            if (pBankSlot != nullptr) {
                pBankSlot->pLastAlloc += residentsize;
                return mgr.m_pCurLoadSDLP->mBankSlot->MAINmemLocation;
            }

            AudioMemoryManager &amm = gAudioMemoryManager;
            char *filename = const_cast<char *>(mgr.m_pCurLoadSDLP->AssetDescription.FileName.GetString());
            resmem = amm.AllocateMemory(residentsize, filename, false);
        }

        mgr.m_pCurLoadSDLP->plocmem = resmem;
        mgr.m_NumBankLoadResolves++;
        return mgr.m_pCurLoadSDLP->plocmem;
    }

    return gAEMSMgr.m_pCurLoadSDLP->pmem;
}

void EAXAemsManager::DataLoadCB(int param, int error_status) {
    (void)error_status;
    StartBankLoadTicks = bGetTicker();
    gAEMSMgr.m_pCurLoadSDLP = reinterpret_cast<stSndDataLoadParams *>(param);
    gAEMSMgr.m_pCurLoadSDLP->t_load = WorldTimer;

    switch (gAEMSMgr.m_pCurLoadSDLP->AssetDescription.eDataType) {
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
        }

        {
            int Result;
            int nhandle;

            if (gAEMSMgr.m_pCurLoadSDLP->mBankSlot != nullptr) {
                Result = SNDmemlimits(gAEMSMgr.m_pCurLoadSDLP->mBankSlot->BANKmemLocation,
                                      gAEMSMgr.m_pCurLoadSDLP->mBankSlot->BANKmemLocation + gAEMSMgr.m_pCurLoadSDLP->mBankSlot->BANKMemSize);
            } else {
                Result = SNDmemlimits(-1, gAEMSMgr.m_SPUMainAllocsEnd);
            }

            nhandle = SNDAEMS_asyncloadmodulebankmem(gAEMSMgr.m_pCurLoadSDLP->pmem, nullptr, 0, AsyncResidentAllocCB);
            gAEMSMgr.m_pCurLoadSDLP->Handle = nhandle;
            (void)Result;
        }
        break;
    case SDT_GENERIC_DATA:
        gAEMSMgr.m_pCurLoadSDLP->bResolvedAsync = true;
        gAEMSMgr.m_pCurLoadSDLP->bResolvedSync = true;
        break;
    }

    gAEMSMgr.m_IsWaitingForFileCB = false;
}

void EAXAemsManager::ResolveCurrentDataMemory() {
    switch (gAEMSMgr.m_pCurLoadSDLP->MemLocation) {
    case TMP_ALLOC_MAIN:
        bFree(gAEMSMgr.m_pCurLoadSDLP->pmem);
        gAEMSMgr.m_pCurLoadSDLP->pmem = nullptr;
        break;
    case TMP_ALLOC_TRACKSTREAMER:
        TheTrackStreamer.FreeUserMemory(gAEMSMgr.m_pCurLoadSDLP->pmem);
        gAEMSMgr.m_pCurLoadSDLP->pmem = nullptr;
        break;
    case TMP_ALLOC_AUDIO:
        if (gAEMSMgr.m_pCurLoadSDLP->plocmem != nullptr || gAEMSMgr.m_pCurLoadSDLP->mBankSlot != nullptr) {
            gAudioMemoryManager.FreeMemory(gAEMSMgr.m_pCurLoadSDLP->pmem);
            gAEMSMgr.m_pCurLoadSDLP->pmem = nullptr;
        }
        break;
    case TMP_ALLOC_NONE:
    default:
        break;
    }
}

int EAXAemsManager::AddBankListing(stAssetDescription &asset) {
    g_SndAssetList[this->m_nEndOfList].Clear();

    g_SndAssetList[this->m_nEndOfList].AssetDescription = asset;

    int index = this->m_nEndOfList;
    this->m_nEndOfList = index + 1;

    return index;
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
    int bankIndex = this->IsAssetInList(queueitem.Asset.FileName);
    if (bankIndex == -1) {
        if (DISABLE_SLOT_LOADING != 0) {
            SlotType = eBANK_SLOT_NONE;
        }
        stBankSlot *slot = nullptr;

        if (SlotType != eBANK_SLOT_NONE) {
            if (SlotType == eBANK_SLOT_PATHFINDER) {
                slot = this->mPFBankSlot.GetFreeSlot(eBANK_SLOT_PATHFINDER);
            } else {
                slot = this->mBankSlots.GetFreeSlot(SlotType);
            }

            if (slot == nullptr) {
                SndBase *owner = queueitem.pThis;
                if (!owner) {
                    return;
                }

                this->mWaitForResolve.DeleteRefToAsset(owner);
                return;
            }
        }

        bankIndex = this->AddBankListing(queueitem.Asset);
        if (slot != nullptr) {
            slot->pAssetParams = &g_SndAssetList[bankIndex];
            g_SndAssetList[bankIndex].mBankSlot = slot;
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
        for (int n = 0; n < 4; n++) {
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
    bMemSet(EAXAemsManager::m_RequiredSlots, 0, 0x10);
}

void EAXAemsManager::RegisterSlots(eBANK_SLOT_TYPE Type, int NumSlots, int SizePerSlotSPU, int SizePerSlotMainMem, bool bDoPFSlot) {
    stBankSlot NewSlot;
    for (int n = 0; n < NumSlots; n++) {
        char DebugString[1];
        DebugString[0] = '\0';
        NewSlot.Index = static_cast<unsigned char>(n);
        NewSlot.Type = Type;
        NewSlot.MAINmemLocation = static_cast<char *>(
            gAudioMemoryManager.AllocateMemory(SizePerSlotMainMem, DebugString, false));
        NewSlot.MAINmemSize = SizePerSlotMainMem;
        NewSlot.pLastAlloc = NewSlot.MAINmemLocation;
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

inline void stBankSlot::Clear() {
    this->BANKmemLocation = 0;
    this->MAINmemLocation = nullptr;
    this->pLastAlloc = nullptr;
    this->Index = 0;
    this->pAssetParams = nullptr;
    this->BANKMemSize = 0;
    this->MAINmemSize = 0;
    this->Type = static_cast<eBANK_SLOT_TYPE>(-1);
    this->LoadFailed = 0;
}
