#include "GameFlow.hpp"
#include "AttribAlloc.h"
#include "AttribVaultPack.h"
#include "Config.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Animation/AnimPlayer.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Frontend/FECarLoader.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Render/RenderConn.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Generated/Events/ELoadingScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Events/ELoadingScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Misc/DemoDisc.hpp"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Misc/Platform.h"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WorldModel.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "EasterEggs.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Stomper.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/World/VisualTreatment.h"
#include "Speed/Indep/Src/World/SimpleModelAnim.hpp"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/TimeOfDay.hpp"
#include "Speed/Indep/Src/World/WWorld.h"
#include "Speed/Indep/Src/World/Sun.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/Src/World/VisibleSection.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "ResourceLoader.hpp"
#include "bFile.hpp"

// TODO why do these two end up below in .bss?
RegionLoader TheRegionLoader;
TrackLoader TheTrackLoader;

GameFlowManager TheGameFlowManager;
static Timer last_any_joy;

// Forward declarations
void BeginGameFlowLoadTrack();
void BeginWorldLoad();
void BeginGameFlowLoadingFrontEnd();
void BeginGameFlowUnloadingFrontEnd();

// TODO move these
extern int EnableCodeOverlay;
extern int EnableCodeOverlayDebuggingOnly;
extern int CodeOverlayMemoryPoolNumber;
extern int CodeOverlayFirstTime;

extern void (*CodeOverlayCallback)(int);
extern char _overlay_start[];
extern char _overlay_end[];

// TODO GET RID OF THESE
extern void *InGameMemoryFile;
extern VMFile *pFrontEndVirtualMemBundle;
extern const char *LoadingControllerScreenPackageName;
extern const char *LoadingBootName;
extern int WeHaveCheckedIfJR2ServerExists;
extern int DisableSoundUpdate;

// TODO GET RID OF THESE
// External functions
void eInitFEEnvMapPlat();
void eRemoveFEEnvMapPlat();
void InitCarEffects();
void CloseCarEffects();
void LoadAemsFrontEnd(void (*)(int), int);
void UnloadAemsFrontEnd();
void LoadLanguageResources(bool load_global, bool load_frontend, bool load_ingame, bool blocking);
void NotifySkyUnloader();
const char *GetLoadingScreenPackageName();
void EstablishRemoteCaffeineConnection();
void StartWorldAnimations();
void MiniMainLoop();
void eWaitUntilRenderingDone();
void CloseAllGarageDoors();
void DisableAllSceneryGroups();
void SoundPause(bool, int);
void SetSoundControlState(bool, int, const char *);
void ServiceSpaceNodes();
void CloseSound();
void CloseWorldModels();
extern unsigned int bDefaultSeed;
void GenerateMissingCarParts();
void InitializeSoundLoad();
extern int AllowCompressedStreamingTexturesInThisPoolNum;
extern int EmergencySaveMemory;
extern int CarLoaderPoolSizes[];
extern void *GlobalMemoryFile;
extern int FinishedLoadingGlobalSuccesful;
void LoadAemsInGame(void (*)(intptr_t), intptr_t);
void UnloadAemsInGame();
void InitSkyHash(void (*)(intptr_t), intptr_t);
void UnloadSkyTextures();
void bCacheCodeineDirs(char *, int, int);
void ResetCapturedLoadingTimes();
void CodeOverlayUnloadingGame();

// TODO move
class LoadingTips {
  public:
    static bool IsDoneShowingLoadingTips() {
        return mDoneShowingLoadingTips;
    }

    static void SetDoneLoading(bool done) {
        mDoneShowingLoadingTips = done;
    }

  private:
    static bool mDoneLoading;
    static bool mDoneShowingLoadingTips;
    static void *mLoadingTipsScreenPtr;
};

Attrib::Vault *gDatabaseVault = nullptr;

static Attrib::Vault *sFrontEndVault = nullptr;
static unsigned char *sFrontEndVaultData = nullptr;
static int sFrontEndVaultHigh = 0;

void RenderTrackMarkers(eView *view) {}

Sim::eUserMode CalculateSimMode() {
    Sim::eUserMode mode = Sim::USER_SINGLE;
    if (SkipFE) {
        if (SkipFESplitScreen) {
            mode = Sim::USER_SPLIT_SCREEN;
        }
    } else if (FEDatabase->IsSplitScreenMode()) {
        mode = Sim::USER_SPLIT_SCREEN;
    }
    return mode;
}

void GetBuildVersionName(char *build_version_name) {
#ifdef DEBUG_OPT
    *build_version_name = '\0';
#elif defined(MILESTONE_OPT)
    bStrCpy(build_version_name, "Milestone");
#else
    bStrCpy(build_version_name, "Release");
#endif
}

inline void CodeOverlayFlushCaches() {
    // TODO two FlushCache calls on PS2
    FlushCaches();
}

inline char *GetCodeOverlayStart() {
    return _overlay_start;
}

inline char *GetCodeOverlayEnd() {
    return _overlay_end;
}

inline int GetCodeOverlaySize() {
    return GetCodeOverlayEnd() - GetCodeOverlayStart();
}

void CodeOverlayLoadedFrontendCallback(intptr_t param, int error_status) {
    CodeOverlayFlushCaches();
    void (*callback)(intptr_t) = CodeOverlayCallback;
    if (CodeOverlayCallback) {
        CodeOverlayCallback = nullptr;
        callback(param);
    }
}

void CodeOverlayLoadingFrontend(void (*callback)(int), int param) {
    if (EnableCodeOverlay || EnableCodeOverlayDebuggingOnly) {
        char build_version_name[64];
        char filename[64];

        GetBuildVersionName(build_version_name);
        bSPrintf(filename, "%s.ovl", build_version_name);
        if (CodeOverlayFirstTime) {
            int file_size = 0;
            void *file_buf = nullptr;
            if (bFileExists(filename)) {
                file_buf = bGetFile(filename, &file_size, 0);
            }
            unsigned int file_checksum = bCalculateCrc32(file_buf, file_size, 0xFFFFFFFF);
            unsigned int code_checksum = bCalculateCrc32(GetCodeOverlayStart(), GetCodeOverlaySize(), 0xFFFFFFFF);
            if (file_checksum != code_checksum) {
                bool explained_by_breakpoints; // TODO
                EnableCodeOverlay = false;
                EnableCodeOverlayDebuggingOnly = false;
            }
            bFree(file_buf);
            CodeOverlayFirstTime = false;
        } else {
            CodeOverlayCallback = callback;
            AddQueuedFile(GetCodeOverlayStart(), filename, 0, GetQueuedFileSize(filename), CodeOverlayLoadedFrontendCallback, param, nullptr);
            return;
        }
    }
    if (callback) {
        callback(param);
    }
}

void CodeOverlayUnloadingFrontend() {}

void CodeOverlayLoadingGame() {
    if (!TheOnlineManager.IsOnlineRace() && (EnableCodeOverlayDebuggingOnly || EnableCodeOverlay)) {
        if (CodeOverlayFirstTime) {
            CodeOverlayFirstTime = false;
        }
        if (EnableCodeOverlayDebuggingOnly) {
            for (int n = 0; n < GetCodeOverlaySize() - 8; n += 8) {
                reinterpret_cast<int *>(&GetCodeOverlayStart()[n])[0] = 0xdeadbeef;
                reinterpret_cast<int *>(&GetCodeOverlayStart()[n])[1] = 0xdeadbeef;
            }
            CodeOverlayFlushCaches();
        } else if (EnableCodeOverlay) {
            struct {
                intptr_t bottom_address;
                intptr_t top_address;
            } packet = {
                reinterpret_cast<intptr_t>(GetCodeOverlayStart()),
                reinterpret_cast<intptr_t>(GetCodeOverlayEnd()),
            };
            // TODO magic
            bFunkCallSync("CODEINE", 0x37, &packet, sizeof(packet), nullptr, 0);
            CodeOverlayMemoryPoolNumber = bGetFreeMemoryPoolNum();
            bInitMemoryPool(CodeOverlayMemoryPoolNumber, GetCodeOverlayStart(), GetCodeOverlaySize(), "CodeOverlay");
            CodeOverlayFlushCaches();
            bMemorySetOverflowPoolNumber(0, CodeOverlayMemoryPoolNumber);
        }
    }
}

void CodeOverlayUnloadingGame() {
    if ((EnableCodeOverlay || EnableCodeOverlayDebuggingOnly) && CodeOverlayMemoryPoolNumber) {
        bMemorySetOverflowPoolNumber(0, -1);
        bCloseMemoryPool(CodeOverlayMemoryPoolNumber);
        CodeOverlayMemoryPoolNumber = 0;
    }
}

// TODO move?
int FreeMemoryEnteringGame = 0;
int RealTimeFramesEnteringGame = 0;
extern int RealTimeFrames;

void ActivateMemorySponge() {
    // TODO how is this the bMalloc inline?
    int amount_to_waste = bLargestMalloc(0);
    FreeMemoryEnteringGame = amount_to_waste;
    RealTimeFramesEnteringGame = RealTimeFrames;
}

void DeactivateMemorySponge() {
    FreeMemoryEnteringGame = 0;
    RealTimeFramesEnteringGame = 0;
}

void LoadMemoryFileCallback(intptr_t param, int error_status) {
    void *memory_file = reinterpret_cast<void *>(param);
    AddMemoryFile(memory_file);
}

void *LoadMemoryFile(const char *filename) {
    int file_size = GetQueuedFileSize(filename);
    // TODO magic
    void *memory_file = bMalloc(file_size, filename, 0, 0x1040);
    AddQueuedFile(memory_file, filename, 0, file_size, &LoadMemoryFileCallback, reinterpret_cast<intptr_t>(memory_file), nullptr);
    return memory_file;
}

void BlockUntilMemoryFileLoaded(void *mem) {
    if (mem) {
        while (IsQueuedFileBusy()) {
            DVDErrorTask(nullptr, 0);
            bThreadYield(8);
            ServiceQueuedFiles();
        }
    }
}

void UnloadMemoryFile(void *mem) {
    if (mem) {
        RemoveMemoryFile(mem);
        bFree(mem);
    }
}

int LeakDetectorFreeMemory = 0;
int LeakDetectorLargestAlloc = 0;
int LeakDetectorAllocationNumber = 0;

void SetLeakDetector() {
    LeakDetectorFreeMemory = bCountFreeMemory(0);
    LeakDetectorAllocationNumber = bMemoryGetAllocationNumber();
    LeakDetectorLargestAlloc = bLargestMalloc(0);
}

void CheckLeakDetector(const char *debug_name) {}

// TODO memory profile stuff stripped from here

void MaybeDoMemoryProfile() {}

void BeginGameFlowLoadRegion() {
    TheRegionLoader.BeginLoading();
}

void CheckForHolesInMemory() {
    int free_memory = bCountFreeMemory(0);
    int largest_malloc = bLargestMalloc(0);
    int holes;
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
    if (EmergencySaveMemory) {
        pool_size -= 256;
    }
    if (two_player) {
        GRaceCustom *startupRace = GRaceDatabase::Get().GetStartupRace();
        bool loadingDrag;
        if (startupRace) {
            loadingDrag = startupRace->GetRaceType() == GRace::kRaceType_Drag;
        } else {
            loadingDrag = false;
        }
        if (loadingDrag) {
            pool_size = bMax(1850, pool_size);
        }
    }
    TheCarLoader.SetMemoryPoolSize(pool_size << 10);
    TheCarLoader.SetLoadingMode(CarLoader::MODE_IN_GAME, two_player);
    new ELoadingScreenOn(LoadingScreen::LS_LOADING_GAME_FROM_FE);
    Phase = 0;
    LoadHandler();
}

void GetTODFilename(eTimeOfDay tod, const char *filename_in, char *filename_out, int bufsize_out) {
    bStrCpy(filename_out, filename_in);
    if (NeedsSeperateTODStreamingFile(bGetPlatformName())) {
        char *extension_in = bStrStr(filename_in, ".");
        char *extension_out = bStrStr(filename_out, ".");
        bSPrintf(extension_out, "_%s%s", GetTimeOfDaySuffix(tod), extension_in);
    }
}

void RegionLoader::LoadHandler() {
    Phase++;
    TheGameFlowManager.ClearWaitingForCallback();
    TheGameFlowManager.SetWaitingForCallback("RegionLoader::LoadHandler", Phase);
    if (Phase == 1) {
        bool load_global = true;
        bool load_frontend = false;
        bool load_ingame = true;
        LoadLanguageResources(load_global, load_frontend, load_ingame, false);

        int ingamea_allocation_params = 0x2000;
        pResourceInGameA = CreateResourceFile("Global\\InGameA.bun", RESOURCE_FILE_INGAME, 0, 0, 0);
        pResourceInGameA->SetAllocationParams(ingamea_allocation_params, "Global\\InGameA.bun");
        pResourceInGameA->BeginLoading();
        pResourceInGameB_VM = LoadFileIntoVirtualMemory("GLOBAL\\INGAMEB_VM_NGC.BUN", false, false);
        pResourceInGameSplitScreen = nullptr;

        load_frontend = FEDatabase->IsSplitScreenMode();
        if (load_frontend) {
            pResourceInGameSplitScreen = LoadResourceFile("Global\\InGameSplitScreen.bun", RESOURCE_FILE_INGAME, 0);
        }
        pResourceInGameB =
            LoadResourceFile("GLOBAL\\INGAMEB.LZC", RESOURCE_FILE_INGAME, 9, &RegionLoader::LoadHandler, reinterpret_cast<intptr_t>(this), 0, 0);
        pResourceInGameB->ChangeFilenameForHotChunking("GLOBAL\\INGAMEB.BUN");
    } else if (Phase == 2) {
        char basefilename[32];
        char filename[32];
        bSPrintf(basefilename, "TRACKS\\%s.BUN", TrackInfo::GetLoadedTrackInfo()->RegionName);
        GetTODFilename(GetCurrentTimeOfDay(), basefilename, filename, sizeof(filename));
        pResourceRegion = LoadResourceFile(filename, RESOURCE_FILE_TRACK, 1, &RegionLoader::LoadHandler, reinterpret_cast<intptr_t>(this), 0, 0);

        char vm_basefilename[48];
        char vm_filename[48];
        bSPrintf(vm_basefilename, "TRACKS\\%s_VM_NGC.BUN", TrackInfo::GetLoadedTrackInfo()->RegionName);
        GetTODFilename(GetCurrentTimeOfDay(), vm_basefilename, vm_filename, sizeof(vm_filename));
        pResourceRegion_VM = LoadFileIntoVirtualMemory(vm_filename, false, true);
    } else if (Phase == 3) {
        LoadPrecullerBooBooScripts();
        LoadAemsInGame(&RegionLoader::LoadHandler, reinterpret_cast<intptr_t>(this));
    } else if (Phase == 4) {
        InitSkyHash(&RegionLoader::LoadHandler, reinterpret_cast<intptr_t>(this));
    } else if (Phase == 5) {
        TheGameFlowManager.ClearWaitingForCallback();
        FinishedLoading();
    }
}

void RegionLoader::FinishedLoading() {
    char baseregion_filename[64];
    char region_filename[64];
    bSPrintf(baseregion_filename, "TRACKS\\STREAM%s.BUN", TrackInfo::GetLoadedTrackInfo()->RegionName);
    GetTODFilename(GetCurrentTimeOfDay(), baseregion_filename, region_filename, sizeof(region_filename));
    EstablishRemoteCaffeineConnection();
    TheTrackStreamer.InitRegion(region_filename, FEDatabase->IsSplitScreenMode());
    TheGameFlowManager.SetSingleFunction(BeginGameFlowLoadTrack, "LoadTrack");
}

const char *WheelsModelPackFilename = "CARS\\WHEELS\\GEOMETRY.BIN";
const char *SpoilerModelPackFilename = "CARS\\SPOILER\\GEOMETRY.BIN";
const char *SpoilerCarreraModelPackFilename = "CARS\\SPOILER_CARRERA\\GEOMETRY.BIN";
const char *SpoilerHatchModelPackFilename = "CARS\\SPOILER_HATCH\\GEOMETRY.BIN";
const char *SpoilerPorschesModelPackFilename = "CARS\\SPOILER_PORSCHES\\GEOMETRY.BIN";
const char *RoofScoopModelPackFilename = "CARS\\ROOF\\GEOMETRY.BIN";
const char *BrakesModelPackFilename = "CARS\\BRAKES\\GEOMETRY.BIN";
const char *BrakesTexturePackFilename = "CARS\\BRAKES\\TEXTURES.BIN";
const char *CarTexturePackFilename = "CARS\\TEXTURES.BIN";
const char *WheelsTexturePackFilename = "CARS\\WHEELS\\TEXTURES.BIN";
const char *DynamicTexturePackFilename = "GLOBAL\\DYNTEX.BIN";
const char *HudDragTexturePackFilename = "GLOBAL\\HUDTEXDRAG.BIN";
const char *HudSingleRaceTexturePackFilename = "GLOBAL\\HUDTEXRACE.BIN";
const char *HudSplitScreenTexturePackFilename = "GLOBAL\\HUDTEXSPLIT.BIN";
const char *HudDragSplitScreenTexturePackFilename = "GLOBAL\\HUDTEXDRAGSPLIT.BIN";
const char *LoadingBootName = "loading_boot.fng";
const char *LoadingControllerScreenPackageName = "Loading_Controller.fng";

void RegionLoader::Unload() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_UNLOADING_REGION);
    TheTrackStreamer.HibernateStreamingSections();
    TheTrackStreamer.UnloadEverything();
    gEmitterSystem.KillEverything();

    WWorld::Get().Close();

    while (g_pEAXSound->AreResourceLoadsPending()) {
        g_pEAXSound->Update(0.1f);
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
    if (pResourceInGameSplitScreen) {
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
    TheTrackStreamer.MakeSpaceInPool(1, true);

    bool load_global = true;
    bool load_frontend = false;
    bool load_ingame = false;
    LoadLanguageResources(load_global, load_frontend, load_ingame, false);

    TrackInfo::SetLoadedTrackInfo(0);
    TheSlotPoolManager.CleanupExpandedSlotPools();
    CodeOverlayUnloadingGame();
    bFileFlushCachedFiles();
    CheckLeakDetector("Region");

    static int dummy = 1;
    dummy++;
}

void BeginGameFlowLoadTrack() {
    TheTrackLoader.BeginLoading();
}

void TrackLoader::BeginLoading() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_LOADING_TRACK);
    Phase = 0;
    int track_number = TheRaceParameters.TrackNumber;
    TrackInfo::SetLoadedTrackInfo(track_number);
    LoadHandler();
}

void TrackLoader::LoadHandler() {
    Phase++;
    TheGameFlowManager.ClearWaitingForCallback();
    TheGameFlowManager.SetWaitingForCallback("TrackLoader::LoadHandler", Phase);
    if (Phase == 1) {
        TheGameFlowManager.SetState(GAMEFLOW_STATE_LOADING_TRACK);
        LoadHandler();
    } else if (Phase == 2) {
        LoadHandler();
    } else if (Phase == 3) {
        InitWorldModels();
        TheGameFlowManager.ClearWaitingForCallback();
        FinishedLoading();
    }
}

void TrackLoader::FinishedLoading() {
    TheGameFlowManager.SetSingleFunction(BeginWorldLoad, "BeginWorldLoad");
}

void TrackLoader::Unload() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_UNLOADING_TRACK);
    SoundPause(false, 8);
    SetSoundControlState(false, 0, "TrackLoader::Unload");
    cFEng::Get()->QueuePackagePop(0);
    cFEng::Get()->ServiceFengOnly();
    DeactivateMemorySponge();
    g_pEAXSound->StopSND11();
    TheAnimPlayer.KillWorldAnimScene(true, false);
    ServiceSpaceNodes();
    ServiceSpaceNodes();
    IVisualTreatment::Get()->SetState(IVisualTreatment::FE_LOOK);
    CloseVisualTreatment();
    if (TheOnlineManager.IsOnlineRace()) {
        TheOnlineManager.EndOnlineRace(true);
    }
    Sim::Shutdown();
    SoundConn::RestoreServices();
    RenderConn::RestoreServices();
    delete pCurrentWorld;
    pCurrentWorld = nullptr;
    TheCarLoader.UnloadEverything();
    eWaitUntilRenderingDone();
    CloseWorldModels();
    SunTrackUnloader();
    NotifySkyUnloader();
    CloseTopologyAndSceneryGroups();
    CloseSound();
    WorldConn::RestoreServices();
    TheSlotPoolManager.CleanupExpandedSlotPools();
    bWaitUntilAsyncDone(nullptr);
    TheTrackPathManager.Close();
    TheTrackPathManager.Close();
    SkipFE = 0;
    bDefaultSeed = Joylog::AddOrGetData(bDefaultSeed, 0x20, JOYLOG_CHANNEL_RANDOM);
    if (TheRaceParameters.AIDemoMode == 0 && TheRaceParameters.ReplayDemoMode == 0) {
        ResetRenderEggs();
        gEasterEggs.ClearNonPersistent();
    }
}

static const int PrintSceneryGroups = ENABLE_IN_DEBUG;

void EnableBarrierSceneryGroup(const char *group_name, bool flip_artwork) {
    VisibleGroupInfo *group_info = VisibleSectionManager::GetGroupInfo(group_name);
    if (group_info) {
        unsigned int group_name_hash = bStringHash(group_name);
        TheVisibleSectionManager.EnableGroup(group_name_hash);
        EnableSceneryGroup(group_name_hash, flip_artwork);
        if (PrintSceneryGroups) {
            // TODO from a debug build
        }
        TheTrackPathManager.EnableBarriers(group_name);
    }
}

void InitTopologyAndSceneryGroups() {
    TheTrackLoader.InitTopologyAndSceneryGroups();
}

void TrackLoader::InitTopologyAndSceneryGroups() {
    if (FindSceneryGroup(bStringHash("SCENERY_GROUP_DOOR"))) {
        EnableBarrierSceneryGroup("SCENERY_GROUP_DOOR", false);
    }
    CloseAllGarageDoors();
}

void CloseTopologyAndSceneryGroups() {
    TheTrackLoader.CloseTopologyAndSceneryGroups();
}

void TrackLoader::CloseTopologyAndSceneryGroups() {
    TheTrackPathManager.DisableAllBarriers();
    TheVisibleSectionManager.DisableAllGroups();
    DisableAllSceneryGroups();
}

void RedoTopologyAndSceneryGroups() {
    TheTrackLoader.CloseTopologyAndSceneryGroups();
    TheTrackLoader.InitTopologyAndSceneryGroups();
    for (Car *car = pCurrentWorld->CarList.GetHead(); car != pCurrentWorld->CarList.EndOfList(); car = car->GetNext()) {
    }
}

void BeginGameFlowUnloadTrack(int destination) {
    if (TheDemoDiscManager.IsActive()) {
        // TODO demo
    }
    TheTrackLoader.Unload();
    TheRegionLoader.Unload();
    switch (destination) {
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
    TheCarLoader.DefragmentPool();
    Scheduler::Get().Synchronize(RealTimer);
    const char *load_screen = GetLoadingScreenPackageName();
    if (cFEng::Get()->IsPackageInControl(LoadingControllerScreenPackageName) || cFEng::Get()->IsPackageInControl(load_screen)) {
        GameFlowClearFEngLoadingScreen();
    }
    TheDemoDiscManager.StartPlayTimer();
    for (IHud::List::const_iterator iter = IHud::GetList().begin(); iter != IHud::GetList().end(); ++iter) {
        (*iter)->JoyEnable();
    }
    if (CodeOverlayMemoryPoolNumber != 0) {
        bMemorySetOverflowPoolNumber(0, -1);
    }
    ActivateMemorySponge();
}

void WaitForSimulation() {
    int state = Sim::GetState();
    if (state < 2) {
        TheGameFlowManager.SetSingleFunction(WaitForSimulation, "WaitForSimulation");
    } else {
        TheGameFlowManager.SetSingleFunction(FinishedGameLoading, "FinishedGameLoading");
    }
}

void BeginWorldLoad() {
    if (SkipFE && cFEng::Get()->IsPackageInControl(LoadingBootName)) {
        cFEng::Get()->QueuePackagePop(1);
    }
    eFixUpTables();
    EstablishRemoteCaffeineConnection();
    TheTrackLoader.InitTopologyAndSceneryGroups();
    SunTrackLoader();
    WeHaveCheckedIfJR2ServerExists = false;
    if (TheRaceParameters.AIDemoMode == 0 && TheRaceParameters.ReplayDemoMode == 0) {
        ActivateAnyRenderEggs();
    }
    if (TheOnlineManager.IsOnlineRace()) {
        TheOnlineManager.TrackLoaded();
    }
    WorldConn::InitServices();
    RenderConn::InitServices();
    SoundConn::InitServices();
    g_pEAXSound->StartSND11();
    StartWorldAnimations();
    DisableSoundUpdate = 0;
    new ("World", 0) World();
    Sim::eUserMode mode = CalculateSimMode();

    const char *activity = TheOnlineManager.IsOnlineRace() ? "OnlineGame" : "QuickGame";
    if (!SkipFE && !FEDatabase->IsSplitScreenMode() && FEDatabase->IsCareerMode()) {
        activity = "CareerGame";
    }
    Sim::Init(activity, mode);
    SimpleModelAnim::Reset();
    OpenVisualTreatment();
    IVisualTreatment::Get()->SetState(IVisualTreatment::HEAT_LOOK);
    TheGameFlowManager.SetSingleFunction(WaitForSimulation, "WaitForSimulation");
}

GameFlowManager::GameFlowManager()
    : pSingleFunction(nullptr),      //
      SingleFunctionParam(0),        //
      pSingleFunctionName(nullptr),  //
      pLoopingFunction(nullptr),     //
      pLoopingFunctionName(nullptr), //
      WaitingForCallback(false),     //
      pCallbackName(nullptr),        //
      CallbackPhase(0) {}

void GameFlowManager::SetSingleFunction(void (*function)(int), const char *debug_name, int param) {
    if (pSingleFunction) {
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
    if (pSingleFunction) {
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
    if (WaitingForCallback) {
        WaitingForCallback = false;
    }
}

void GameFlowManager::Service() {
    while (pSingleFunction) {
        void (*function)(int) = pSingleFunction;
        int param = SingleFunctionParam;

        pSingleFunction = nullptr;
        SingleFunctionParam = 0;
        pSingleFunctionName = nullptr;
        function(param);

        if (pSingleFunction == function) {
            break;
        }
    }
    if (pLoopingFunction) {
        pLoopingFunction();
    }
    CheckForDemoDiscTimeout();
}

void GameFlowManager::SetState(GameFlowState state) {
    CurrentGameFlowState = state;
}

Attrib::Vault *InitializeSingleAttributeVault(void *buf, const char *name, unsigned char **outPermBuffer, unsigned int allocFlags) {
    int ticks = bGetTicker();
    AttribVaultPackImage *image = static_cast<AttribVaultPackImage *>(buf);
    image->EndianSwap();
    int vaultIndex = image->GetVaultIndex(name);
    AttribVaultPackEntry &entry = image->GetEntry(vaultIndex);

    char *vlt_buffer = reinterpret_cast<char *>(image->GetData(entry.mVltOffset));
    char *bin_buffer = reinterpret_cast<char *>(image->GetData(entry.mBinOffset));

    char text[64];
    bSPrintf(text, "Attributes - %s.bin", name);

    unsigned char *bin_buffer_perm = static_cast<unsigned char *>(bMalloc(entry.mBinSize, text, 0, allocFlags));
    bMemCpy(bin_buffer_perm, image->GetData(entry.mBinOffset), entry.mBinSize);

    bSPrintf(text, "%s.bin", name);
    bool success = AddDepFile(text, bin_buffer_perm, entry.mBinSize);
    bSPrintf(text, "%s.vlt", name);
    Attrib::Vault *vault = AddVault(text, vlt_buffer, entry.mVltSize);
    if (outPermBuffer) {
        *outPermBuffer = bin_buffer_perm;
    }
    return vault;
}

void LoadFrontEndVault(bool allocHigh) {
    if (sFrontEndVault != nullptr)
        return;

    unsigned int allocFlagsPack = 0x40;
    if (allocHigh) {
        allocFlagsPack = 0;
    }
    allocFlagsPack |= 0x1000;
    int buf_size;
    void *buf = bGetFile("GLOBAL\\FE_ATTRIB.BIN", &buf_size, allocFlagsPack);

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
    if (oldAllocator) {
        AttribAlloc::OverrideAllocator(oldAllocator);
    }
    bFree(buf);
}

void GameFlowManager::LoadFrontend() {
    SetLeakDetector();
    LoadFrontEndVault(false);
    SetSingleFunction(BeginGameFlowLoadingFrontEnd, "LoadingFrontEnd");
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
    if (oldAllocator) {
        AttribAlloc::OverrideAllocator(oldAllocator);
    }
    sFrontEndVaultHigh = 0;
}

void GameFlowManager::UnloadFrontend() {
    SetSingleFunction(BeginGameFlowUnloadingFrontEnd, "UnloadingFrontEnd");
}
void GameFlowManager::LoadTrack() {
    SetSingleFunction(BeginGameFlowLoadRegion, "LoadingRegion");
}

void GameFlowManager::ReloadTrack() {
    BeginGameFlowUnloadTrack(1);
}

void GameFlowManager::UnloadTrack() {
    BeginGameFlowUnloadTrack(0);
}

void GameFlowManager::CheckForDemoDiscTimeout() {}

bool GameFlowManager::IsPaused() {
    return TheOnlineManager.IsOnlineRace() ? false : Sim::GetState() == Sim::STATE_IDLE;
}

// TODO
extern unsigned char bin_globala_bun[];

void LoadGlobalAChunks() {
#define ALIGN(n, align) ((n + align) & ~(align - 1))
    int alignment = 0x80;
    unsigned char *dest = reinterpret_cast<unsigned char *>(ALIGN(reinterpret_cast<uintptr_t>(bin_globala_bun), alignment));
#undef ALIGN
    // TODO hardcoded size?
    bOverlappedMemCpy(dest, &bin_globala_bun[1], 0x15df4);
    LoadEmbeddedChunks(reinterpret_cast<bChunk *>(dest), 0x15df4, "Embedded GlobalA.bun");
    WaitForResourceLoadingComplete();
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

    ResourceFile *r = LoadResourceFile("GLOBAL\\GLOBALB.LZC", RESOURCE_FILE_GLOBAL, 9);
    r->ChangeFilenameForHotChunking("GLOBAL\\GLOBALB.BUN");

    LoadFileIntoVirtualMemory("GLOBAL\\GLOBALB_VM_NGC.LZC", true, false);
    LoadResourceFile(BrakesModelPackFilename, RESOURCE_FILE_CAR, 0);
    LoadResourceFile(BrakesTexturePackFilename, RESOURCE_FILE_CAR, 0);
    WaitForResourceLoadingComplete();

    eLoadStreamingTexturePack(DynamicTexturePackFilename);
    eWaitForStreamingTexturePackLoading(DynamicTexturePackFilename);
    LoadCurrentLanguage();

    if (eIsWidescreen()) {
        LoadResourceFile("GLOBAL\\WIDESCREEN_GLOBAL.BUN", RESOURCE_FILE_GLOBAL, 0);
    } else {
        LoadResourceFile("GLOBAL\\THINSCREEN_GLOBAL.BUN", RESOURCE_FILE_GLOBAL, 0);
    }
    // TODO magic
    if (!GetTextureInfo(0xab0e817d, 0, 0)) {
        eLoadStreamingTexture(0xab0e817d);
    }
    WaitForResourceLoadingComplete();
    eDisplayFrame();
    eLoadStreamingTexturePack(CarTexturePackFilename);
    eLoadStreamingTexturePack(WheelsTexturePackFilename);
    eLoadStreamingSolidPack(WheelsModelPackFilename);
    eLoadStreamingSolidPack(SpoilerModelPackFilename);
    eLoadStreamingSolidPack(SpoilerCarreraModelPackFilename);
    eLoadStreamingSolidPack(SpoilerHatchModelPackFilename);
    eLoadStreamingSolidPack(SpoilerPorschesModelPackFilename);
    eLoadStreamingSolidPack(RoofScoopModelPackFilename);
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
    TheTrackStreamer.InitMemoryPool(size + car_loader_size);
    AllowCompressedStreamingTexturesInThisPoolNum = 7;
    TheCarLoader.SetMemoryPoolSize(car_loader_size);
    InitializeSoundLoad();
    BlockWhileQueuedFileBusy();
    UnloadMemoryFile(GlobalMemoryFile);
    GlobalMemoryFile = nullptr;
    bool sunset_exists = bFileExists("TRACKS/L2RA_Sunset.BUN");
    if (Joylog::IsCapturing()) {
        Joylog::AddData(static_cast<int>(sunset_exists), 8, JOYLOG_CHANNEL_CONFIG);
    } else if (Joylog::IsReplaying()) {
        bool prev_sunset_exists = Joylog::GetData(8, JOYLOG_CHANNEL_CONFIG) != 0;
        if (prev_sunset_exists != sunset_exists) {
            bBreak();
        }
    }
    LoadFrontEndVault(true);
}

void EndGameFlowLoadingFrontEnd();
void FinishedGameLoading();

void DelayWaitForLoadingScreen() {
    if (LoadingTips::IsDoneShowingLoadingTips()) {
        TheGameFlowManager.SetSingleFunction(EndGameFlowLoadingFrontEnd, "EndGameFlowLoadingFrontEnd");
    } else {
        TheGameFlowManager.SetSingleFunction(DelayWaitForLoadingScreen, "Delay wait for Frontennd\n");
    }
}

void GameFlowLoadingFrontEndPart3(int) {
    TheGameFlowManager.ClearWaitingForCallback();
    LoadingTips::SetDoneLoading(true);
    TheGameFlowManager.SetSingleFunction(DelayWaitForLoadingScreen, "Delay wait for Frontennd\n");
}

void GameFlowLoadingFrontEndPart2(int) {
    TheGameFlowManager.ClearWaitingForCallback();
    TheGameFlowManager.SetWaitingForCallback("GameFlowLoadingFrontEndPart2", 0);
    LoadAemsFrontEnd(GameFlowLoadingFrontEndPart3, 0);
}

void GameFlowLoadGarageScreen(ASYNCFILE_CALLBACK callback, intptr_t param) {
    char filename[128];
    bStrCpy(filename, FEManager::Get()->GetGarageNameFromType());

    FEManager::Get()->SetGarageBackground(CreateResourceFile(filename, RESOURCE_FILE_FRONTEND, 0, 0, 0));
    // TODO
    bool compressed;
    int flags;
    int pool;
    FEManager::Get()->GetGarageBackground()->SetAllocationParams(0x2007, filename);
    FEManager::Get()->GetGarageBackground()->BeginLoading(callback, param);
}

void GameFlowLoadingFrontEndPart1(int) {
    TheGameFlowManager.ClearWaitingForCallback();
    TheGameFlowManager.SetWaitingForCallback("GameFlowLoadingFrontEndPart2", 0);
    GameFlowLoadGarageScreen(GameFlowLoadingFrontEndPart2, 0);
}

void BeginGameFlowLoadingFrontEnd() {
    TheGameFlowManager.SetState(GAMEFLOW_STATE_LOADING_FRONTEND);
    TheRaceParameters.InitWithDefaults();
    new ELoadingScreenOn(LoadingScreen::LS_LOADING_FE);
    FEManager::Get()->ClearControllerError(4);
    MiniMainLoop();
    TheCarLoader.SetLoadingMode(CarLoader::MODE_FRONT_END, 0);
    CodeOverlayLoadingFrontend(nullptr, 0);
    pFrontEndVirtualMemBundle = LoadFileIntoVirtualMemory("FRONTEND\\FRONTB_VM_NGC.LZC", true, true);

    bool load_global = true;
    bool load_frontend = true;
    bool load_ingame = false;
    LoadLanguageResources(load_global, load_frontend, load_ingame, 0);
    LoadResourceFile("FRONTEND\\FRONTA.BUN", RESOURCE_FILE_FRONTEND, 0);
    TheGameFlowManager.SetWaitingForCallback("GameFlowLoadingFrontEndPart1", 0);

    ResourceFile *r = LoadResourceFile("FRONTEND\\FRONTB.LZC", RESOURCE_FILE_FRONTEND, 9, GameFlowLoadingFrontEndPart1, 0, 0, 0);
    r->SetAllocationParams(0x2007, "FRONTEND\\FRONTB.LZC");
    r->ChangeFilenameForHotChunking("FRONTEND\\FRONTB.BUN");
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
    TheCarLoader.UnloadEverything();
    if (TheTrackStreamer.IsPermFileLoading()) {
        new EFadeScreenOn(true);
        while (TheTrackStreamer.IsPermFileLoading()) {
            MiniMainLoop();
        }
        if (cFEng::Get()->IsPackagePushed("FadeScreen.fng")) {
            cFEng::Get()->PopNoControlPackage("FadeScreen.fng");
        }
    }
    if (IsQueuedFileBusy()) {
        BlockWhileQueuedFileBusy();
    }
    NotifySkyUnloader();
    CloseCarEffects();
    UnloadAemsFrontEnd();
    while (true) {
        ResourceFile *r = FindResourceFile(RESOURCE_FILE_FRONTEND);
        if (!r)
            break;
        UnloadResourceFile(r);
    }
    UnloadFileFromVirtualMemory(pFrontEndVirtualMemBundle);
    pFrontEndVirtualMemBundle = nullptr;

    bool load_global = true;
    bool load_frontend = false;
    bool load_ingame = false;
    LoadLanguageResources(load_global, load_frontend, load_ingame, 0);

    gEmitterSystem.KillEverything();
    TheSlotPoolManager.CleanupExpandedSlotPools();
    UnloadFrontEndVault();
    CodeOverlayUnloadingFrontend();
    bWaitUntilAsyncDone(nullptr);
    bFileFlushCachedFiles();
    CheckLeakDetector("Front End");
    TheGameFlowManager.LoadTrack();
}

int TrackStreamerLoadingBarUp = false;
#ifndef EA_BUILD_A124
void HandleTrackStreamerLoadingBar() {
    if (!TrackStreamerLoadingBarUp) {
        if (Sim::GetState() == Sim::STATE_ACTIVE && TheTrackStreamer.CheckLoadingBar()) {
            TrackStreamerLoadingBarUp = true;
            FEManager::RequestPauseSimulation("TrackStreamerLoadingBar");
            new EFadeScreenOn(true);
        }
    } else {
        if (TheTrackStreamer.CheckLoadingBar() == 0) {
            TrackStreamerLoadingBarUp = false;
            // TODO magic
            new EFadeScreenOff(0x16a259);
            FEManager::RequestUnPauseSimulation("TrackStreamerLoadingBar");
        }
    }
}
#endif

void BootLoadingScreen() {
    if (!cFEng::Get()->IsPackagePushed(LoadingBootName)) {
        cFEng::Get()->QueuePackageSwitch(LoadingBootName, 0, 0, false);
        if (!BuildRegion::IsAmerica()) {
            const unsigned long FEObj_Legalblurb = 0x855f83ba;
            FEngSetInvisible(LoadingBootName, FEObj_Legalblurb);
        }
    }
    FEManager::Get()->Update();
    eDisplayFrame();
}
