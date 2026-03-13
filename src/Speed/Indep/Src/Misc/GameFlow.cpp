#include "GameFlow.hpp"
#include "AttribAlloc.h"
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
};

class EFadeScreenOn : public Event {
  public:
    EFadeScreenOn(bool);
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
        if (TheOnlineManager.IsOnlineRace()) {
            mode = Sim::USER_ONLINE;
        } else if (FEDatabase->IsSplitScreenMode() && FEDatabase->iNumPlayers == 2) {
            mode = Sim::USER_SPLIT_SCREEN;
        } else {
            mode = Sim::USER_SINGLE;
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
        if (CodeOverlayFirstTime == 0) {
            CodeOverlayCallback = callback;
            int size = GetQueuedFileSize(overlay_path);
            AddQueuedFile(_overlay_start, overlay_path, 0, size,
                          reinterpret_cast<void (*)(void *, int)>(CodeOverlayLoadedFrontendCallback),
                          reinterpret_cast<void *>(param), nullptr);
            return;
        }
        int filesize[1];
        filesize[0] = 0;
        void *data = nullptr;
        if (bFileExists(overlay_path)) {
            data = bGetFile(overlay_path, filesize, 0);
        }
        int crc1 = bCalculateCrc32(data, filesize[0], 0xFFFFFFFF);
        int crc2 = bCalculateCrc32(_overlay_start, 0x24998, 0xFFFFFFFF);
        if (crc1 != crc2) {
            EnableCodeOverlay = 0;
            EnableCodeOverlayDebuggingOnly = 0;
        }
        bFree(data);
        CodeOverlayFirstTime = 0;
    }
    if (callback != nullptr) {
        callback(param);
    }
}

void CodeOverlayUnloadingFrontend() {}

void CheckLeakDetector(const char *) {}

void MaybeDoMemoryProfile() {}

void SetLeakDetector() {}

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
    bOverlappedMemCpy(reinterpret_cast<void *>(0x8041f900), bin_globala_bun, 0x15df4);
    LoadEmbeddedChunks(reinterpret_cast<void *>(0x8041f900), 0x15df4, "globala");
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

void LoadMemoryFileCallback(int, int) {
    AddMemoryFile(nullptr);
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
    LoadedTrackInfo = TrackInfo::GetTrackInfo(TheRaceParameters.TrackNumber);
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
    VisibleSectionManager *vsm = &TheVisibleSectionManager;
    if (vsm->GetGroupInfo(name) != 0) {
        unsigned int hash = bStringHash(name);
        vsm->EnableGroup(hash);
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
    bMemSet(reinterpret_cast<void *>(0x804a48b0), 0, 0x400);
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
        if (EnableCodeOverlayDebuggingOnly == 0) {
            if (EnableCodeOverlay != 0) {
                void *overlay_addr[2];
                overlay_addr[0] = _overlay_start;
                overlay_addr[1] = _overlay_end;
                bFunkCallSync("_overlay_start", 0x37, overlay_addr, 8, nullptr, 0);
                CodeOverlayMemoryPoolNumber = bGetFreeMemoryPoolNum();
                bInitMemoryPool(CodeOverlayMemoryPoolNumber, _overlay_start, 0x24998, "Overlay");
                FlushCaches();
                bMemorySetOverflowPoolNumber(0, CodeOverlayMemoryPoolNumber);
            }
        } else {
            for (int i = 0; i < 0x24990; i += 8) {
                *reinterpret_cast<int *>(_overlay_start + i) = 0xdeadbeef;
                *reinterpret_cast<int *>(_overlay_start + i + 4) = 0xdeadbeef;
            }
            FlushCaches();
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
    if (iRam804a73bc != 0) {
        while (iRam804a73bc != 0) {
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
    TheDemoDiscManager.LastActivityTime = _RealTimer;
    uRam8048df4c = 0;
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
    if (_11LoadingTips_mDoneShowingLoadingTips == 0) {
        TheGameFlowManager.SetSingleFunction(reinterpret_cast<void (*)(int)>(DelayWaitForLoadingScreen), "DelayWaitForLoadingScreen", 0);
    } else {
        TheGameFlowManager.SetSingleFunction(reinterpret_cast<void (*)(int)>(EndGameFlowLoadingFrontEnd), "EndGameFlowLoadingFrontEnd", 0);
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
    FEManager::Get();
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
