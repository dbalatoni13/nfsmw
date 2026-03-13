#include "GameFlow.hpp"
#include "AttribAlloc.h"
#include "AttribVaultPack.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/cFEng.hpp"
#include "Speed/Indep/Src/Generated/Events/ELoadingScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/TimeOfDay.hpp"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "ResourceLoader.hpp"
#include "bFile.hpp"

GameFlowManager TheGameFlowManager; // size: 0x24

void RenderTrackMarkers(eView *view) {}

// Forward declarations
void GetBuildVersionName(char *build_version_name);
void BeginGameFlowLoadingFrontEnd();
void BeginGameFlowUnloadingFrontEnd();

// todo
extern int SkipFE;
extern int SkipFESplitScreen;

extern int EnableCodeOverlay;
extern int EnableCodeOverlayDebuggingOnly;
extern int CodeOverlayMemoryPoolNumber;
extern int CodeOverlayFirstTime;
extern int _11LoadingTips_mDoneShowingLoadingTips;
extern int _11LoadingTips_mDoneLoading;

extern void (*CodeOverlayCallback)(int);
extern char _overlay_start[];
extern char _overlay_end[];

extern RaceParameters TheRaceParameters;
extern CarLoader TheCarLoader;

class EmitterSystem;
struct SlotPoolManager;
extern EmitterSystem gEmitterSystem;
extern SlotPoolManager TheSlotPoolManager;
void HibernateStreamingSections__13TrackStreamer(TrackStreamer *);
void UnloadEverything__13TrackStreamer(TrackStreamer *);
void MakeSpaceInPool__13TrackStreamerib(TrackStreamer *, int, bool);
void Close__6WWorld(WWorld *);
extern WWorld *_6WWorld_fgWorld;
void InitRegion__13TrackStreamerPCcb(TrackStreamer *, const char *, bool);
void KillEverything__13EmitterSystem(EmitterSystem *);
void CleanupExpandedSlotPools__15SlotPoolManager(SlotPoolManager *);

extern TrackPathManager TheTrackPathManager;
extern Scheduler *_9Scheduler_fgScheduler;
extern Timer _RealTimer;
extern void *InGameMemoryFile;
extern VMFile *pFrontEndVirtualMemBundle;
extern const char *LoadingControllerScreenPackageName;
extern const char *LoadingBootName;
extern int WeHaveCheckedIfJR2ServerExists;
extern int DisableSoundUpdate;
extern int iRam8049d7dc;
extern int iRam804a73bc;
extern int iRam804a5fa0;
extern int iRam804a5fa4;
extern unsigned int uRam8048df4c;
extern int **__Q33UTL11Collectionst8Listable2Z4IHudi2__mTable;
extern int iRam80481afc;

// External functions
void eFixUpTables();
void eInitFEEnvMapPlat();
void eRemoveFEEnvMapPlat();
void InitCarEffects();
void CloseCarEffects();
void InitGarageCarLoaders();
void CleanUpGarageCarLoaders();
void FlushCaches();
void SetLoadingMode__9CarLoaderQ29CarLoader12eLoadingModei(CarLoader *, int, int);
void UnloadEverything__9CarLoader(CarLoader *);
void DefragmentPool__9CarLoader(CarLoader *);
void bCloseMemoryPool(int pool);
void bMemorySetOverflowPoolNumber(int pool, int overflow);
unsigned int bCalculateCrc32(const void *, int, unsigned int);
void *bGetFile(const char *, int *, int);
void DVDErrorTask(void *, int);
void bThreadYield(int);
void AddMemoryFile(void *);
void RemoveMemoryFile(void *);
void LoadAemsFrontEnd(void (*)(int), int);
void UnloadAemsFrontEnd();
int IsQueuedFileBusy();
void BlockWhileQueuedFileBusy();
void LoadLanguageResources(bool, bool, int, int);
void NotifySkyUnloader();
void bWaitUntilAsyncDone(bFile *);
void bFileFlushCachedFiles();
int ServiceResourceLoading();
ResourceFile *LoadResourceFile(const char *, int, int, void (*)(void *), void *, int, int);
ResourceFile *CreateResourceFile(const char *, int, int, int, int);
int FindResourceFile(int);
void UnloadResourceFile(ResourceFile *);
VMFile *LoadFileIntoVirtualMemory(const char *, bool, bool);
void UnloadFileFromVirtualMemory(VMFile *);
const char *GetLoadingScreenPackageName();
void SunTrackLoader();
void EstablishRemoteCaffeineConnection();
void ActivateAnyRenderEggs();
void StartWorldAnimations();
void OpenVisualTreatment();
void MiniMainLoop();
void eWaitUntilRenderingDone();
int FindSceneryGroup(unsigned int);
void CloseAllGarageDoors();
void DisableAllSceneryGroups();
unsigned int bStringHash(const char *);
unsigned int stringhash32(const char *);

void Synchronize__9SchedulerG5Timer(Scheduler *, Timer *);
void SoundPause(bool, int);
void SetSoundControlState(bool, int, const char *);
void ServiceSpaceNodes();
void SunTrackUnloader();
void CloseSound();
void CloseWorldModels();
void CloseVisualTreatment();
void ResetRenderEggs();
void KillWorldAnimScene__11CAnimPlayerbT1(void *, bool, bool);
void InitServices__9WorldConnv();
void InitServices__10RenderConnv();
void InitServices__9SoundConnv();
void RestoreServices__9WorldConnv();
void RestoreServices__10RenderConnv();
void RestoreServices__9SoundConnv();
extern unsigned int bDefaultSeed;
void GenerateMissingCarParts();
void InitializeSoundLoad();
void LoadCurrentLanguage();
void InitLocalization();
extern int AllowCompressedStreamingTexturesInThisPoolNum;
extern int EmergencySaveMemory;
extern int CarLoaderPoolSizes[];
void InitMemoryPool__13TrackStreameri(void *, int);
void SetMemoryPoolSize__9CarLoaderi(CarLoader *, int);
extern void *GlobalMemoryFile;
extern int FinishedLoadingGlobalSuccesful;
extern char TheAnimPlayer[];
void WaitForResourceLoadingComplete();
unsigned int GetVirtualMemoryAllocParams();
void ServiceQueuedFiles();
void LoadPrecullerBooBooScripts();
void LoadAemsInGame(void (*)(int), int);
void UnloadAemsInGame();
void InitSkyHash(void (*)(int), int);
void UnloadSkyTextures();
void eUnloadAllStreamingTextures(const char *);
int bFileSize(const char *);
void bCacheCodeineDirs(char *, int, int);
void ResetCapturedLoadingTimes();
void CodeOverlayUnloadingGame();
void CheckLeakDetector(const char *);
int AreResourceLoadsPending__8EAXSound(EAXSound *);
void Update__8EAXSoundf(EAXSound *, float);
extern const char *HudDragTexturePackFilename;
extern const char *HudSingleRaceTexturePackFilename;
extern const char *HudSplitScreenTexturePackFilename;
extern const char *HudDragSplitScreenTexturePackFilename;
extern const char *DynamicTexturePackFilename;
extern const char *CarTexturePackFilename;
extern const char *WheelsTexturePackFilename;
extern const char *WheelsModelPackFilename;
extern const char *SpoilerModelPackFilename;
extern const char *SpoilerCarreraModelPackFilename;
extern const char *SpoilerHatchModelPackFilename;
extern const char *SpoilerPorschesModelPackFilename;
extern const char *RoofScoopModelPackFilename;
extern const char *BrakesModelPackFilename;
extern const char *BrakesTexturePackFilename;
extern int dummy_32338;

void eLoadStreamingTexturePack(const char *, void (*)(void *), void *, int);
void eLoadStreamingSolidPack(const char *, void (*)(void *), void *, int);
void eWaitForStreamingTexturePackLoading(const char *);
void eWaitForStreamingSolidPackLoading(const char *);
void eLoadStreamingTexture(unsigned int *, int, void (*)(void *), void *, int);
int eIsWidescreen();
void bBreak();

extern Attrib::Vault *gDatabaseVault;

static Attrib::Vault *sFrontEndVault;
static unsigned char *sFrontEndVaultData;
static int sFrontEndVaultHigh;

bool RemoveDepFile(const char *);
void RemoveVault(const char *);
bool AddDepFile(const char *, void *, unsigned int);
Attrib::Vault *AddVault(const char *, void *, unsigned int);

#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
extern EasterEggs gEasterEggs;

class IVisualTreatment {
  public:
    static IVisualTreatment *Get();
    void Reset();
};

class ELoadingScreenOn : public Event {
  public:
    ELoadingScreenOn(int);
    int fParam;
};

class EFadeScreenOn : public Event {
  public:
    EFadeScreenOn(bool);
    bool fLoadingBarOnly;
};

class SimpleModelAnim {
  public:
    static void Reset();
};

// UNSOLVED
Sim::eUserMode CalculateSimMode() {
    Sim::eUserMode mode = Sim::USER_SINGLE;
    if (SkipFE) {
        if (SkipFESplitScreen) {
            mode = Sim::USER_SPLIT_SCREEN;
        }
    } else {
        int isSplit = 0;
        if (FEDatabase->IsSplitScreenMode()) {
            isSplit = (FEDatabase->iNumPlayers == 2);
        }
        if (isSplit) {
            mode = Sim::USER_SPLIT_SCREEN;
        }
    }
    return mode;
}

void CodeOverlayLoadedFrontendCallback(int, int);
void CodeOverlayLoadingFrontend(void (*callback)(int), int param);
void CodeOverlayLoadingGame();

void CodeOverlayLoadedFrontendCallback(int param1, int) {
    FlushCaches();
    void (*cb)(int) = CodeOverlayCallback;
    if (CodeOverlayCallback != nullptr) {
        CodeOverlayCallback = nullptr;
        cb(param1);
    }
}

void CodeOverlayLoadingFrontend(void (*callback)(int), int param) {
    char build_version[64];
    char overlay_path[64];

    if (EnableCodeOverlay != 0 || EnableCodeOverlayDebuggingOnly != 0) {
        GetBuildVersionName(build_version);
        bSPrintf(overlay_path, "%s.ovl", build_version);
        if (CodeOverlayFirstTime != 0) {
            int filesize[1];
            filesize[0] = 0;
            void *data = nullptr;
            if (bFileExists(overlay_path)) {
                data = bGetFile(overlay_path, filesize, 0);
            }
            int crc1 = bCalculateCrc32(data, filesize[0], 0xFFFFFFFF);
            int crc2 = bCalculateCrc32(_overlay_start, _overlay_end - _overlay_start, 0xFFFFFFFF);
            if (crc1 != crc2) {
                EnableCodeOverlay = 0;
                EnableCodeOverlayDebuggingOnly = 0;
            }
            bFree(data);
            CodeOverlayFirstTime = 0;
        } else {
            CodeOverlayCallback = callback;
            int size = GetQueuedFileSize(overlay_path);
            AddQueuedFile(_overlay_start, overlay_path, 0, size,
                          reinterpret_cast<void (*)(void *, int)>(CodeOverlayLoadedFrontendCallback),
                          reinterpret_cast<void *>(param), nullptr);
            return;
        }
    }
    if (callback != nullptr) {
        callback(param);
    }
}

void CodeOverlayUnloadingFrontend() {}

void CheckLeakDetector(const char *) {}

void MaybeDoMemoryProfile() {}

extern int LeakDetectorFreeMemory;
extern int LeakDetectorAllocationNumber;
extern int LeakDetectorLargestAlloc;
int bCountFreeMemory(int);
int bMemoryGetAllocationNumber();
int bLargestMalloc(int);

void SetLeakDetector() {
    LeakDetectorFreeMemory = bCountFreeMemory(0);
    LeakDetectorAllocationNumber = bMemoryGetAllocationNumber();
    LeakDetectorLargestAlloc = bLargestMalloc(0);
}

// Forward declarations (first block already at top)
void BeginGameFlowUnloadingFrontEnd();
void BeginGameFlowLoadRegion();
void BeginGameFlowUnloadTrack(int reload);
void LoadFrontEndVault(bool high_alloc);

GameFlowManager::GameFlowManager()
    : pSingleFunction(nullptr),    //
      SingleFunctionParam(0),      //
      pSingleFunctionName(nullptr), //
      pLoopingFunction(nullptr),   //
      pLoopingFunctionName(nullptr), //
      WaitingForCallback(false),   //
      pCallbackName(nullptr),      //
      CallbackPhase(0) {}

void GameFlowManager::SetSingleFunction(void (*function)(int), const char *debug_name, int param) {
    if (pSingleFunction != nullptr) {
        pSingleFunction = nullptr;
    }
    if (WaitingForCallback) {
        WaitingForCallback = false;
    }
    pSingleFunction = function;
    pSingleFunctionName = debug_name;
    SingleFunctionParam = param;
}

void GameFlowManager::SetWaitingForCallback(const char *name, int phase) {
    if (pSingleFunction != nullptr) {
        pSingleFunction = nullptr;
    }
    if (WaitingForCallback) {
        WaitingForCallback = false;
    }
    WaitingForCallback = true;
    pCallbackName = name;
    CallbackPhase = phase;
}

void GameFlowManager::ClearWaitingForCallback() {
    if (!WaitingForCallback) {
        return;
    }
    WaitingForCallback = false;
}

void GameFlowManager::Service() {
    void (*func)(int);
    void (*saved)(int);
    func = pSingleFunction;
    if (func != nullptr) {
        do {
            saved = pSingleFunction;
            int param = SingleFunctionParam;
            pSingleFunction = nullptr;
            SingleFunctionParam = 0;
            pSingleFunctionName = nullptr;
            saved(param);
            func = pSingleFunction;
        } while (func != saved && func != nullptr);
    }
    if (pLoopingFunction != nullptr) {
        pLoopingFunction();
    }
    CheckForDemoDiscTimeout();
}

void GameFlowManager::SetState(GameFlowState state) {
    CurrentGameFlowState = state;
}

Attrib::Vault *InitializeSingleAttributeVault(void *buf, const char *name,
                                              unsigned char **outPermBuffer,
                                              unsigned int allocFlags) {
    char nameBuf[64];
    bGetTicker();
    AttribVaultPackImage *pack = static_cast<AttribVaultPackImage *>(buf);
    pack->EndianSwap();
    int index = pack->GetVaultIndex(name);
    AttribVaultPackEntry &entry = pack->GetEntry(index);
    unsigned char *vltData = pack->GetData(entry.mVltOffset);
    bSPrintf(nameBuf, "%s.bin", name);
    unsigned char *binData = static_cast<unsigned char *>(bMalloc(entry.mBinSize, allocFlags));
    bMemCpy(binData, pack->GetData(entry.mBinOffset), entry.mBinSize);
    bSPrintf(nameBuf, "%s.dep", name);
    AddDepFile(nameBuf, binData, entry.mBinSize);
    bSPrintf(nameBuf, "%s", name);
    Attrib::Vault *result = AddVault(nameBuf, vltData, entry.mVltSize);
    if (outPermBuffer != nullptr) {
        *outPermBuffer = binData;
    }
    return result;
}

void LoadFrontEndVault(bool allocHigh) {
    if (sFrontEndVault != nullptr) return;
    int buf_size;
    void *buf = bGetFile("GLOBAL\\FE_ATTRIB.BIN", &buf_size, (-(int)(!allocHigh) & 0x40) | 0x1000);
    unsigned int allocFlagsBin = 0;
    if (allocHigh) {
        allocFlagsBin = 0x40;
    }
    allocFlagsBin |= GetVirtualMemoryAllocParams();
    HighAttribAlloc highAllocator;
    sFrontEndVaultHigh = allocHigh;
    IAttribAllocator *oldAllocator = nullptr;
    if (allocHigh) {
        oldAllocator = AttribAlloc::OverrideAllocator(&highAllocator);
    }
    sFrontEndVault = InitializeSingleAttributeVault(buf, "frontend", &sFrontEndVaultData, allocFlagsBin);
    if (oldAllocator != nullptr) {
        AttribAlloc::OverrideAllocator(oldAllocator);
    }
    bFree(buf);
}

void UnloadFrontEndVault() {
    HighAttribAlloc highAllocator;
    IAttribAllocator *oldAllocator = nullptr;
    if (sFrontEndVaultHigh != 0) {
        oldAllocator = AttribAlloc::OverrideAllocator(&highAllocator);
    }
    gDatabaseVault->Clean();
    sFrontEndVault->Deinitialize();
    Attrib::Database::Get().CollectGarbage();
    sFrontEndVault->Release();
    sFrontEndVault = nullptr;
    RemoveDepFile("frontend.bin");
    RemoveVault("frontend.vlt");
    bFree(sFrontEndVaultData);
    sFrontEndVaultData = nullptr;
    if (oldAllocator != nullptr) {
        AttribAlloc::OverrideAllocator(oldAllocator);
    }
    sFrontEndVaultHigh = 0;
}

void GameFlowManager::LoadFrontend() {
    SetLeakDetector();
    LoadFrontEndVault(false);
    SetSingleFunction(reinterpret_cast<void (*)(int)>(BeginGameFlowLoadingFrontEnd), "LoadFrontend", 0);
}

void GameFlowManager::UnloadFrontend() {
    SetSingleFunction(reinterpret_cast<void (*)(int)>(BeginGameFlowUnloadingFrontEnd), "UnloadFrontend", 0);
}

void GameFlowManager::LoadTrack() {
    SetSingleFunction(reinterpret_cast<void (*)(int)>(BeginGameFlowLoadRegion), "LoadTrack", 0);
}

void GameFlowManager::ReloadTrack() {
    BeginGameFlowUnloadTrack(1);
}

void GameFlowManager::UnloadTrack() {
    BeginGameFlowUnloadTrack(0);
}

void GameFlowManager::CheckForDemoDiscTimeout() {}

bool GameFlowManager::IsPaused() {
    return Sim::GetState() == Sim::STATE_IDLE;
}

extern unsigned char bin_globala_bun[];
void bOverlappedMemCpy(void *dst, const void *src, int size);
void LoadEmbeddedChunks(void *data, int size, const char *name);
void WaitForResourceLoadingComplete();

void LoadGlobalAChunks() {
    char *base = reinterpret_cast<char *>(bin_globala_bun);
    void *dest = reinterpret_cast<void *>((reinterpret_cast<unsigned int>(base) + 0x80) & ~0x7F);
    bOverlappedMemCpy(dest, base + 1, 0x15df4);
    LoadEmbeddedChunks(dest, 0x15df4, "globala");
    WaitForResourceLoadingComplete();
}

// TODO
void GetBuildVersionName(char *build_version_name) {
#ifdef MILESTONE_OPT
    bStrCpy(build_version_name, "Milestone");
#else
    bStrCpy(build_version_name, "Release");
#endif
}

extern int FreeMemoryEnteringGame;
extern int RealTimeFramesEnteringGame;
extern int RealTimeFrames;

void ActivateMemorySponge() {
    int amount_to_waste = bLargestMalloc(0);
    FreeMemoryEnteringGame = amount_to_waste;
    RealTimeFramesEnteringGame = RealTimeFrames;
}

void DeactivateMemorySponge() {
    FreeMemoryEnteringGame = 0;
    RealTimeFramesEnteringGame = 0;
}

void LoadMemoryFileCallback(int param, int) {
    AddMemoryFile(reinterpret_cast<void *>(param));
}

void *LoadMemoryFile(const char *filename) {
    int size = GetQueuedFileSize(filename);
    void *buf = bMalloc(size, 0x1040);
    AddQueuedFile(buf, filename, 0, size,
                  reinterpret_cast<void (*)(void *, int)>(LoadMemoryFileCallback),
                  buf, nullptr);
    return buf;
}

void BlockUntilMemoryFileLoaded(void *mem) {
    if (mem != nullptr) {
        while (IsQueuedFileBusy()) {
            DVDErrorTask(nullptr, 0);
            bThreadYield(8);
            ServiceQueuedFiles();
        }
    }
}

void UnloadMemoryFile(void *mem) {
    if (mem != nullptr) {
        RemoveMemoryFile(mem);
        bFree(mem);
    }
}

void LoadGlobalChunks() {
    GlobalMemoryFile = LoadMemoryFile("Global\\GlobalMemoryFile.bin");
    BlockUntilMemoryFileLoaded(GlobalMemoryFile);
    InitLocalization();
    int buf_size;
    void *buf = bGetFile("GLOBAL\\ATTRIBUTES.BIN", &buf_size, 0x1040);
    unsigned int allocFlags = GetVirtualMemoryAllocParams();
    gDatabaseVault = InitializeSingleAttributeVault(buf, "db", nullptr, allocFlags);
    bFree(buf);
    ResourceFile *r = LoadResourceFile("GLOBAL\\GLOBALB.LZC", 1, 9, nullptr, nullptr, 0, 0);
    r->ChangeFilenameForHotChunking("GLOBAL\\GLOBALB.BUN");
    LoadFileIntoVirtualMemory("GLOBAL\\GLOBALB_VM_NGC.LZC", true, false);
    LoadResourceFile(BrakesModelPackFilename, 6, 0, nullptr, nullptr, 0, 0);
    LoadResourceFile(BrakesTexturePackFilename, 6, 0, nullptr, nullptr, 0, 0);
    WaitForResourceLoadingComplete();
    eLoadStreamingTexturePack(DynamicTexturePackFilename, nullptr, nullptr, 0);
    eWaitForStreamingTexturePackLoading(DynamicTexturePackFilename);
    LoadCurrentLanguage();
    if (eIsWidescreen() != 0) {
        LoadResourceFile("GLOBAL\\WIDESCREEN_GLOBAL.BUN", 1, 0, nullptr, nullptr, 0, 0);
    } else {
        LoadResourceFile("GLOBAL\\THINSCREEN_GLOBAL.BUN", 1, 0, nullptr, nullptr, 0, 0);
    }
    if (GetTextureInfo(0xab0e817d, 0, 0) == nullptr) {
        unsigned int hash = 0xab0e817d;
        eLoadStreamingTexture(&hash, 1, nullptr, nullptr, 0);
    }
    WaitForResourceLoadingComplete();
    eDisplayFrame();
    eLoadStreamingTexturePack(CarTexturePackFilename, nullptr, nullptr, 0);
    eLoadStreamingTexturePack(WheelsTexturePackFilename, nullptr, nullptr, 0);
    eLoadStreamingSolidPack(WheelsModelPackFilename, nullptr, nullptr, 0);
    eLoadStreamingSolidPack(SpoilerModelPackFilename, nullptr, nullptr, 0);
    eLoadStreamingSolidPack(SpoilerCarreraModelPackFilename, nullptr, nullptr, 0);
    eLoadStreamingSolidPack(SpoilerHatchModelPackFilename, nullptr, nullptr, 0);
    eLoadStreamingSolidPack(SpoilerPorschesModelPackFilename, nullptr, nullptr, 0);
    eLoadStreamingSolidPack(RoofScoopModelPackFilename, nullptr, nullptr, 0);
    eWaitForStreamingTexturePackLoading(nullptr);
    eWaitForStreamingSolidPackLoading(nullptr);
    WaitForResourceLoadingComplete();
    FinishedLoadingGlobalSuccesful = 1;
    GenerateMissingCarParts();
    int size = 0x4cf400;
    int car_loader_size = CarLoaderPoolSizes[0] * 0x400;
    if (EmergencySaveMemory != 0) {
        car_loader_size -= 0x40000;
        size = 0x48f400;
    }
    InitMemoryPool__13TrackStreameri(&TheTrackStreamer, size + car_loader_size);
    AllowCompressedStreamingTexturesInThisPoolNum = 7;
    SetMemoryPoolSize__9CarLoaderi(&TheCarLoader, car_loader_size);
    InitializeSoundLoad();
    BlockWhileQueuedFileBusy();
    UnloadMemoryFile(GlobalMemoryFile);
    GlobalMemoryFile = nullptr;
    bool sunset_exists = bFileExists("TRACKS/L2RA_Sunset.BUN") != 0;
    if (Joylog::IsCapturing() != 0) {
        Joylog::AddData(static_cast<int>(sunset_exists), 8, static_cast<JoylogChannel>(1));
    } else if (Joylog::IsReplaying() != 0) {
        bool prev_sunset_exists = Joylog::GetData(8, static_cast<JoylogChannel>(1)) != 0;
        if (prev_sunset_exists != sunset_exists) {
            bBreak();
        }
    }
    LoadFrontEndVault(true);
}

RegionLoader TheRegionLoader;
TrackLoader TheTrackLoader;
int TrackStreamerLoadingBarUp;

void BeginGameFlowLoadRegion() {
    TheRegionLoader.BeginLoading();
}

void CheckForHolesInMemory() {
    bCountFreeMemory(0);
    bLargestMalloc(0);
}

void RegionLoader::BeginLoading() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_LOADING_REGION);
    TrackInfo::SetLoadedTrackInfo(TheRaceParameters.TrackNumber);
    SetLeakDetector();
    CheckForHolesInMemory();
    InGameMemoryFile = LoadMemoryFile("InGame.mem");
    CodeOverlayLoadingGame();
    WWorld::Init();
    bool two_player = CalculateSimMode() == Sim::USER_SPLIT_SCREEN;
    int pool_size = CarLoaderPoolSizes[0];
    if (two_player) {
        pool_size = CarLoaderPoolSizes[1];
    }
    if (EmergencySaveMemory != 0) {
        pool_size -= 0x100;
    }
    if (two_player) {
        GRaceCustom *startupRace = GRaceDatabase::Get().GetStartupRace();
        bool loadingDrag;
        if (startupRace != nullptr) {
            loadingDrag = startupRace->GetRaceType() == GRace::kRaceType_Drag;
        } else {
            loadingDrag = false;
        }
        if (loadingDrag && pool_size < 0x73a) {
            pool_size = 0x73a;
        }
    }
    SetMemoryPoolSize__9CarLoaderi(&TheCarLoader, pool_size << 10);
    SetLoadingMode__9CarLoaderQ29CarLoader12eLoadingModei(&TheCarLoader, 2, two_player);
    new ELoadingScreenOn(1);
    Phase = 0;
    LoadHandler();
}

int NeedsSeperateTODStreamingFile(const char *);
const char *GetTimeOfDaySuffix(eTimeOfDay);
char *bStrStr(const char *, const char *);

void GetTODFilename(eTimeOfDay tod, const char *base, char *out, int) {
    bStrCpy(out, base);
    if (NeedsSeperateTODStreamingFile(base) != 0) {
        char *dot = bStrStr(base, ".");
        char *out_dot = bStrStr(out, ".");
        const char *suffix = GetTimeOfDaySuffix(tod);
        bSPrintf(out_dot, "_%s%s", suffix, dot);
    }
}

void RegionLoader::LoadHandler(int) {
    TheRegionLoader.LoadHandler();
}

extern "C" void LoadHandler__12RegionLoaderi(int);

void RegionLoader::LoadHandler() {
    Phase++;
    TheGameFlowManager.ClearWaitingForCallback();
    TheGameFlowManager.SetWaitingForCallback("RegionLoader::LoadHandler", Phase);
    if (Phase == 1) {
        LoadLanguageResources(true, false, true, false);
        int ingamea_allocation_params = 0x2000;
        if (TheOnlineManager.IsOnlineRace()) {
            int filesize = bFileSize("InGameA_online_gc.bun");
            ingamea_allocation_params = 0x2047;
            MakeSpaceInPool__13TrackStreamerib(&TheTrackStreamer, filesize, true);
        }
        ResourceFile *res = CreateResourceFile("InGameA_online_gc.bun", 3, 0, 0, 0);
        pResourceInGameA = res;
        res->SetAllocationParams(ingamea_allocation_params, "InGameA_online_gc.bun");
        pResourceInGameA->BeginLoading(nullptr, nullptr);
        pResourceInGameB_VM = LoadFileIntoVirtualMemory("GLOBAL\\INGAMEB_VM_NGC.BUN", false, false);
        pResourceInGameSplitScreen = nullptr;
        bool load_frontend = FEDatabase->IsSplitScreenMode() && FEDatabase->iNumPlayers == 2;
        if (load_frontend) {
            pResourceInGameSplitScreen = LoadResourceFile("InGameSplitScreen_gc.bun", 3, 0, nullptr, nullptr, 0, 0);
        }
        ResourceFile *ingameB = LoadResourceFile("InGameB_gc.bun", 3, 9, reinterpret_cast<void (*)(void *)>(LoadHandler__12RegionLoaderi), reinterpret_cast<void *>(this), 0, 0);
        pResourceInGameB = ingameB;
        ingameB->ChangeFilenameForHotChunking("InGameB_gc_hot.bun");
    } else if (Phase == 2) {
        char basefilename[32];
        char filename[32];
        bSPrintf(basefilename, "TRACKS\\%s\\", TrackInfo::GetLoadedTrackInfo()->RegionName);
        GetTODFilename(GetCurrentTimeOfDay(), basefilename, filename, 0x20);
        pResourceRegion = LoadResourceFile(filename, 4, 1, reinterpret_cast<void (*)(void *)>(LoadHandler__12RegionLoaderi), reinterpret_cast<void *>(this), 0, 0);
        char vm_basefilename[48];
        char vm_filename[48];
        bSPrintf(vm_basefilename, "TRACKS\\%s\\_VM", TrackInfo::GetLoadedTrackInfo()->RegionName);
        GetTODFilename(GetCurrentTimeOfDay(), vm_basefilename, vm_filename, 0x30);
        pResourceRegion_VM = LoadFileIntoVirtualMemory(vm_filename, false, true);
    } else if (Phase == 3) {
        LoadPrecullerBooBooScripts();
        LoadAemsInGame(LoadHandler__12RegionLoaderi, reinterpret_cast<int>(this));
    } else if (Phase == 4) {
        InitSkyHash(LoadHandler__12RegionLoaderi, reinterpret_cast<int>(this));
    } else if (Phase == 5) {
        TheGameFlowManager.ClearWaitingForCallback();
        FinishedLoading();
    }
}

void BeginGameFlowLoadTrack();

void RegionLoader::FinishedLoading() {
    char baseregion_filename[64];
    char region_filename[64];
    bSPrintf(baseregion_filename, "TRACKS\\%s\\", TrackInfo::GetLoadedTrackInfo()->RegionName);
    GetTODFilename(GetCurrentTimeOfDay(), baseregion_filename, region_filename, 0x40);
    EstablishRemoteCaffeineConnection();
    bool load_frontend = FEDatabase->IsSplitScreenMode() && FEDatabase->iNumPlayers == 2;
    InitRegion__13TrackStreamerPCcb(&TheTrackStreamer, region_filename, load_frontend);
    TheGameFlowManager.SetSingleFunction(reinterpret_cast<void (*)(int)>(BeginGameFlowLoadTrack), "LoadTrack", 0);
}

void RegionLoader::Unload() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_UNLOADING_REGION);
    HibernateStreamingSections__13TrackStreamer(&TheTrackStreamer);
    UnloadEverything__13TrackStreamer(&TheTrackStreamer);
    KillEverything__13EmitterSystem(&gEmitterSystem);
    Close__6WWorld(_6WWorld_fgWorld);
    while (AreResourceLoadsPending__8EAXSound(g_pEAXSound)) {
        Update__8EAXSoundf(g_pEAXSound, 0.1f);
        ServiceQueuedFiles();
    }
    UnloadAemsInGame();
    UnloadSkyTextures();
    eUnloadAllStreamingTextures(HudDragTexturePackFilename);
    eUnloadAllStreamingTextures(HudSingleRaceTexturePackFilename);
    eUnloadAllStreamingTextures(HudSplitScreenTexturePackFilename);
    eUnloadAllStreamingTextures(HudDragSplitScreenTexturePackFilename);
    eUnloadAllStreamingTextures(DynamicTexturePackFilename);
    UnloadResourceFile(pResourceRegion);
    UnloadResourceFile(pResourceInGameB);
    if (pResourceInGameSplitScreen != nullptr) {
        UnloadResourceFile(pResourceInGameSplitScreen);
    }
    UnloadResourceFile(pResourceInGameA);
    pResourceRegion = nullptr;
    pResourceInGameB = nullptr;
    pResourceInGameSplitScreen = nullptr;
    pResourceInGameA = nullptr;
    UnloadFileFromVirtualMemory(pResourceRegion_VM);
    UnloadFileFromVirtualMemory(pResourceInGameB_VM);
    pResourceRegion_VM = nullptr;
    pResourceInGameB_VM = nullptr;
    MakeSpaceInPool__13TrackStreamerib(&TheTrackStreamer, 1, true);
    LoadLanguageResources(true, false, false, false);
    TrackInfo::SetLoadedTrackInfo(0);
    CleanupExpandedSlotPools__15SlotPoolManager(&TheSlotPoolManager);
    CodeOverlayUnloadingGame();
    bFileFlushCachedFiles();
    CheckLeakDetector("In-Game");
    dummy_32338++;
}

void BeginGameFlowLoadTrack() {
    TheTrackLoader.BeginLoading();
}

void BeginGameFlowUnloadTrack(int reload) {
    TheTrackLoader.Unload();
    TheRegionLoader.Unload();
    switch (reload) {
        case 0:
            TheGameFlowManager.LoadFrontend();
            break;
        case 1:
            SetLeakDetector();
            TheGameFlowManager.LoadTrack();
            break;
    }
}

void GameFlowClearFEngLoadingScreen() {
    new ELoadingScreenOff();
}

extern RaceParameters TheRaceParameters;
TrackInfo *LoadedTrackInfo;

void InitWorldModels();
void BeginWorldLoad();

void TrackLoader::BeginLoading() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_LOADING_TRACK);
    Phase = 0;
    TrackInfo::SetLoadedTrackInfo(TheRaceParameters.TrackNumber);
    LoadHandler();
}

void TrackLoader::LoadHandler() {
    Phase = Phase + 1;
    TheGameFlowManager.ClearWaitingForCallback();
    TheGameFlowManager.SetWaitingForCallback("TrackLoader", Phase);
    if (Phase == 1) {
        TheGameFlowManager.SetState(GAMEFLOW_STATE_LOADING_TRACK);
    } else if (Phase == 2) {
        LoadHandler();
        return;
    } else if (Phase == 3) {
        InitWorldModels();
        TheGameFlowManager.ClearWaitingForCallback();
        FinishedLoading();
        return;
    } else {
        return;
    }
    LoadHandler();
}

void TrackLoader::FinishedLoading() {
    TheGameFlowManager.SetSingleFunction(reinterpret_cast<void (*)(int)>(BeginWorldLoad), "BeginWorldLoad", 0);
}

void TrackLoader::Unload() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_UNLOADING_TRACK);
    SoundPause(false, 8);
    SetSoundControlState(false, 0, "TrackLoaderUnload");
    cFEng::mInstance->QueuePackagePop(0);
    cFEng::mInstance->ServiceFengOnly();
    DeactivateMemorySponge();
    g_pEAXSound->StopSND11();
    KillWorldAnimScene__11CAnimPlayerbT1(reinterpret_cast<void *>(TheAnimPlayer), true, false);
    ServiceSpaceNodes();
    ServiceSpaceNodes();
    IVisualTreatment *vt = IVisualTreatment::Get();
    vt->Reset();
    *reinterpret_cast<int *>(vt) = 2;
    CloseVisualTreatment();
    Sim::Shutdown();
    RestoreServices__9SoundConnv();
    RestoreServices__10RenderConnv();
    if (pCurrentWorld != nullptr) {
        delete pCurrentWorld;
    }
    pCurrentWorld = nullptr;
    UnloadEverything__9CarLoader(&TheCarLoader);
    eWaitUntilRenderingDone();
    CloseWorldModels();
    SunTrackUnloader();
    NotifySkyUnloader();
    CloseTopologyAndSceneryGroups();
    CloseSound();
    RestoreServices__9WorldConnv();
    CleanupExpandedSlotPools__15SlotPoolManager(&TheSlotPoolManager);
    bWaitUntilAsyncDone(nullptr);
    SkipFE = 0;
    bDefaultSeed = Joylog::AddOrGetData(bDefaultSeed, 0x20, JOYLOG_CHANNEL_RANDOM);
    if (TheRaceParameters.AIDemoMode == 0 && TheRaceParameters.ReplayDemoMode == 0) {
        ResetRenderEggs();
        gEasterEggs.ClearNonPersistent();
    }
}

void EnableSceneryGroup(unsigned int hash, bool enable);

void EnableBarrierSceneryGroup(const char *name, bool enable) {
    if (VisibleSectionManager::GetGroupInfo(name) != 0) {
        unsigned int hash = bStringHash(name);
        TheVisibleSectionManager.EnableGroup(hash);
        EnableSceneryGroup(hash, enable);
        TheTrackPathManager.EnableBarriers(name);
    }
}

void InitTopologyAndSceneryGroups() {
    TheTrackLoader.InitTopologyAndSceneryGroups();
}

void TrackLoader::InitTopologyAndSceneryGroups() {
    unsigned int hash = bStringHash("Barriers");
    if (FindSceneryGroup(hash) != 0) {
        EnableBarrierSceneryGroup("Barriers", false);
    }
    CloseAllGarageDoors();
}

void CloseTopologyAndSceneryGroups() {
    TheTrackLoader.CloseTopologyAndSceneryGroups();
}

void TrackLoader::CloseTopologyAndSceneryGroups() {
    TheTrackPathManager.DisableAllBarriers();
    bMemSet(TheVisibleSectionManager.EnabledGroups, 0, 0x400);
    DisableAllSceneryGroups();
}

void RedoTopologyAndSceneryGroups() {
    TheTrackLoader.CloseTopologyAndSceneryGroups();
    TheTrackLoader.InitTopologyAndSceneryGroups();
    int *p = reinterpret_cast<int *>(pCurrentWorld);
    do {
        p = reinterpret_cast<int *>(*p);
    } while (p != reinterpret_cast<int *>(pCurrentWorld));
}

void EndGameFlowLoadingFrontEnd();
void FinishedGameLoading();

void CodeOverlayLoadingGame() {
    if (EnableCodeOverlayDebuggingOnly != 0 || EnableCodeOverlay != 0) {
        if (CodeOverlayFirstTime != 0) {
            CodeOverlayFirstTime = 0;
        }
        if (EnableCodeOverlayDebuggingOnly != 0) {
            int size = _overlay_end - _overlay_start;
            int n = 0;
        overlay_loop:
            if (n >= size - 8) goto overlay_done;
            *reinterpret_cast<int *>(_overlay_start + n) = static_cast<int>(0xdeadbeef);
            *reinterpret_cast<int *>(_overlay_start + n + 4) = static_cast<int>(0xdeadbeef);
            n += 8;
            goto overlay_loop;
        overlay_done:
            FlushCaches();
        } else if (EnableCodeOverlay != 0) {
            void *overlay_addr[2] = {0, 0};
            overlay_addr[0] = _overlay_start;
            overlay_addr[1] = _overlay_end;
            bFunkCallSync("_overlay_start", 0x37, overlay_addr, 8, nullptr, 0);
            CodeOverlayMemoryPoolNumber = bGetFreeMemoryPoolNum();
            bInitMemoryPool(CodeOverlayMemoryPoolNumber, _overlay_start, _overlay_end - _overlay_start, "Overlay");
            FlushCaches();
            bMemorySetOverflowPoolNumber(0, CodeOverlayMemoryPoolNumber);
        }
    }
}

void CodeOverlayUnloadingGame() {
    if ((EnableCodeOverlay != 0 || EnableCodeOverlayDebuggingOnly != 0) && CodeOverlayMemoryPoolNumber != 0) {
        bMemorySetOverflowPoolNumber(0, -1);
        bCloseMemoryPool(CodeOverlayMemoryPoolNumber);
        CodeOverlayMemoryPoolNumber = 0;
    }
}

void WaitForSimulation() {
    int state = Sim::GetState();
    if (state < 2) {
        TheGameFlowManager.SetSingleFunction(reinterpret_cast<void (*)(int)>(WaitForSimulation), "WaitForSimulation", 0);
    } else {
        TheGameFlowManager.SetSingleFunction(reinterpret_cast<void (*)(int)>(FinishedGameLoading), "FinishedGameLoading", 0);
    }
}

void FinishedGameLoading() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_RACING);
    if (TheCarLoader.IsLoadingInProgress()) {
        while (TheCarLoader.IsLoadingInProgress()) {
            bThreadYield(8);
            ServiceResourceLoading();
        }
    }
    UnloadMemoryFile(InGameMemoryFile);
    InGameMemoryFile = nullptr;
    DefragmentPool__9CarLoader(&TheCarLoader);
    Timer t = _RealTimer;
    Synchronize__9SchedulerG5Timer(_9Scheduler_fgScheduler, &t);
    const char *loadingPackage = GetLoadingScreenPackageName();
    if (cFEng::mInstance->IsPackageInControl(LoadingControllerScreenPackageName) ||
        cFEng::mInstance->IsPackageInControl(loadingPackage)) {
        GameFlowClearFEngLoadingScreen();
    }
    TheDemoDiscManager.StartPlayTime = _RealTimer;
    TheDemoDiscManager.SuspendedPlayTime.ResetLow();
    int **table = __Q33UTL11Collectionst8Listable2Z4IHudi2__mTable;
    for (int *p = *table; p != *(table + iRam80481afc); p++) {
        int *obj = reinterpret_cast<int *>(*p);
        int *vtbl = reinterpret_cast<int *>(*(obj + 1));
        void (*func)(void *) = reinterpret_cast<void (*)(void *)>(*(vtbl + 0x54 / 4));
        func(reinterpret_cast<void *>(reinterpret_cast<char *>(obj) + *reinterpret_cast<short *>(reinterpret_cast<char *>(vtbl) + 0x50)));
    }
    if (CodeOverlayMemoryPoolNumber != 0) {
        bMemorySetOverflowPoolNumber(0, -1);
    }
    ActivateMemorySponge();
}

void BeginWorldLoad() {
    if (SkipFE != 0 && cFEng::mInstance->IsPackageInControl(LoadingBootName)) {
        cFEng::mInstance->QueuePackagePop(1);
    }
    eFixUpTables();
    EstablishRemoteCaffeineConnection();
    TheTrackLoader.InitTopologyAndSceneryGroups();
    SunTrackLoader();
    WeHaveCheckedIfJR2ServerExists = 0;
    if (TheRaceParameters.AIDemoMode == 0 && TheRaceParameters.ReplayDemoMode == 0) {
        ActivateAnyRenderEggs();
    }
    InitServices__9WorldConnv();
    InitServices__10RenderConnv();
    InitServices__9SoundConnv();
    g_pEAXSound->StartSND11();
    StartWorldAnimations();
    DisableSoundUpdate = 0;
    new char[0xe8];
    Sim::eUserMode simMode = CalculateSimMode();
    const char *trackName = "open";
    if (SkipFE == 0) {
        bool isSplitScreen = FEDatabase->IsSplitScreenMode();
        if (!isSplitScreen && FEDatabase->iNumPlayers == 2) {
            trackName = "open_split";
        }
    }
    UCrc32 trackCrc(stringhash32(trackName));
    Sim::Init(trackCrc, simMode);
    SimpleModelAnim::Reset();
    OpenVisualTreatment();
    IVisualTreatment *vt = IVisualTreatment::Get();
    vt->Reset();
    *reinterpret_cast<int *>(vt) = 0;
    TheGameFlowManager.SetSingleFunction(reinterpret_cast<void (*)(int)>(WaitForSimulation), "WaitForSimulation", 0);
}

void DelayWaitForLoadingScreen() {
    if (_11LoadingTips_mDoneShowingLoadingTips != 0) {
        TheGameFlowManager.SetSingleFunction(reinterpret_cast<void (*)(int)>(EndGameFlowLoadingFrontEnd), "EndGameFlowLoadingFrontEnd", 0);
    } else {
        TheGameFlowManager.SetSingleFunction(reinterpret_cast<void (*)(int)>(DelayWaitForLoadingScreen), "DelayWaitForLoadingScreen", 0);
    }
}

void GameFlowLoadingFrontEndPart3(int) {
    TheGameFlowManager.ClearWaitingForCallback();
    _11LoadingTips_mDoneLoading = 1;
    TheGameFlowManager.SetSingleFunction(reinterpret_cast<void (*)(int)>(DelayWaitForLoadingScreen), "DelayWaitForLoadingScreen", 0);
}

void GameFlowLoadingFrontEndPart2(int) {
    TheGameFlowManager.ClearWaitingForCallback();
    TheGameFlowManager.SetWaitingForCallback("LoadAemsFrontEnd", 0);
    LoadAemsFrontEnd(GameFlowLoadingFrontEndPart3, 0);
}

void GameFlowLoadGarageScreen(void (*callback)(int), int param) {
    char garage_name[128];
    const char *name = FEManager::Get()->GetGarageNameFromType();
    bStrCpy(garage_name, name);
    int mgr_addr = reinterpret_cast<int>(FEManager::Get());
    ResourceFile *res = CreateResourceFile(garage_name, 2, 0, 0, 0);
    *reinterpret_cast<ResourceFile **>(mgr_addr + 0x3c) = res;
    mgr_addr = reinterpret_cast<int>(FEManager::Get());
    (*reinterpret_cast<ResourceFile **>(mgr_addr + 0x3c))->SetAllocationParams(0x2007, garage_name);
    mgr_addr = reinterpret_cast<int>(FEManager::Get());
    (*reinterpret_cast<ResourceFile **>(mgr_addr + 0x3c))->BeginLoading(reinterpret_cast<void (*)(void *)>(callback), reinterpret_cast<void *>(param));
}

void GameFlowLoadingFrontEndPart1(int) {
    TheGameFlowManager.ClearWaitingForCallback();
    TheGameFlowManager.SetWaitingForCallback("LoadAemsFrontEnd", 0);
    GameFlowLoadGarageScreen(GameFlowLoadingFrontEndPart2, 0);
}

void BeginGameFlowLoadingFrontEnd() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_LOADING_FRONTEND);
    TheRaceParameters.InitWithDefaults();
    new ELoadingScreenOn(0);
    FEManager *mgr = FEManager::Get();
    for (int i = 0; i < 8; i++) {
        reinterpret_cast<int *>(reinterpret_cast<char *>(mgr) + 8)[i] = 0;
    }
    MiniMainLoop();
    SetLoadingMode__9CarLoaderQ29CarLoader12eLoadingModei(&TheCarLoader, 0, 0);
    CodeOverlayLoadingFrontend(nullptr, 0);
    pFrontEndVirtualMemBundle = LoadFileIntoVirtualMemory("FrontEnd.bun", true, true);
    LoadLanguageResources(true, true, 0, 0);
    LoadResourceFile("FrontB.bun", 2, 0, nullptr, nullptr, 0, 0);
    TheGameFlowManager.SetWaitingForCallback("LoadGlobalChunks", 0);
    ResourceFile *res = LoadResourceFile("FrontA.bun", 2, 9,
                                         reinterpret_cast<void (*)(void *)>(GameFlowLoadingFrontEndPart1),
                                         nullptr, 0, 0);
    res->SetAllocationParams(0x2007, "FrontA.bun");
    bFreeSharedString(res->GetFilename());
    *reinterpret_cast<const char **>(reinterpret_cast<char *>(res) + 0x1c) = bAllocateSharedString("FrontA_gc.bun");
}

void EndGameFlowLoadingFrontEnd() {
    eFixUpTables();
    eInitFEEnvMapPlat();
    InitCarEffects();
    InitGarageCarLoaders();
    new ELoadingScreenOff();
    FEManager::Get()->StartFE();
    TheGameFlowManager.SetState(GAMEFLOW_STATE_IN_FRONTEND);
    g_pEAXSound->StartSND11();
}

void BeginGameFlowUnloadingFrontEnd() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_UNLOADING_FRONTEND);
    g_pEAXSound->StopSND11();
    eWaitUntilRenderingDone();
    FEManager::Get()->StopFE();
    eRemoveFEEnvMapPlat();
    CleanUpGarageCarLoaders();
    UnloadEverything__9CarLoader(&TheCarLoader);
    if (iRam8049d7dc != 0) {
        new EFadeScreenOn(true);
        while (iRam8049d7dc != 0) {
            MiniMainLoop();
        }
        if (cFEng::mInstance->IsPackagePushed("ScreenFade.fng")) {
            cFEng::mInstance->PopNoControlPackage("ScreenFade.fng");
        }
    }
    if (IsQueuedFileBusy()) {
        BlockWhileQueuedFileBusy();
    }
    NotifySkyUnloader();
    CloseCarEffects();
    UnloadAemsFrontEnd();
    while (true) {
        ResourceFile *res = reinterpret_cast<ResourceFile *>(FindResourceFile(2));
        if (res == nullptr) break;
        UnloadResourceFile(res);
    }
    UnloadFileFromVirtualMemory(pFrontEndVirtualMemBundle);
    pFrontEndVirtualMemBundle = nullptr;
    LoadLanguageResources(true, false, 0, 0);
    KillEverything__13EmitterSystem(&gEmitterSystem);
    CleanupExpandedSlotPools__15SlotPoolManager(&TheSlotPoolManager);
    UnloadFrontEndVault();
    CodeOverlayUnloadingFrontend();
    bWaitUntilAsyncDone(nullptr);
    bFileFlushCachedFiles();
    CheckLeakDetector("Unload Frontend");
    TheGameFlowManager.LoadTrack();
}

void eDisplayFrame();
FEObject *FEngFindObject(const char *, unsigned int);
void FEngSetInvisible(FEObject *);
int IsAmerica();

void HandleTrackStreamerLoadingBar() {
    if (TrackStreamerLoadingBarUp == 0) {
        if (Sim::GetState() == Sim::STATE_ACTIVE && TheTrackStreamer.CheckLoadingBar() != 0) {
            TrackStreamerLoadingBarUp = 1;
            FEManager::RequestPauseSimulation("TrackStreamer");
            new EFadeScreenOn(true);
        }
    } else {
        if (TheTrackStreamer.CheckLoadingBar() == 0) {
            TrackStreamerLoadingBarUp = 0;
            new EFadeScreenOff(0x16a259);
            FEManager::RequestUnPauseSimulation("TrackStreamer");
        }
    }
}

void BootLoadingScreen() {
    if (!cFEng::mInstance->IsPackagePushed(LoadingBootName)) {
        cFEng::mInstance->QueuePackageSwitch(LoadingBootName, 0, 0, false);
        if (IsAmerica() == 0) {
            FEObject *obj = FEngFindObject(LoadingBootName, 0x855f83ba);
            FEngSetInvisible(obj);
        }
    }
    FEManager::Get()->Update();
    eDisplayFrame();
}
