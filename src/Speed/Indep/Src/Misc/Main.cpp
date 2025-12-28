#include "BuildRegion.hpp"
#include "Config.h"
#include "DemoDisc.hpp"
#include "GameFlow.hpp"
#include "HandyBreakpoint.hpp"
#include "Hermes.h"
#include "Joylog.hpp"
#include "Platform.h"
#include "QueuedFile.hpp"
#include "ResourceLoader.hpp"
#include "Speed/Indep/Src/Animation/AnimCtrl.hpp"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/SmokeableInfo.hpp"
#include "Speed/Indep/Src/Render/RenderConn.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/World/DebugVehicleSelection.h"
#include "Speed/Indep/Src/World/DebugWorld.h"
#include "Speed/Indep/Src/World/EventManager.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/SpaceNode.hpp"
#include "Speed/Indep/Src/World/TimeOfDay.hpp"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/VehiclePartDamage.h"
#include "Speed/Indep/Src/World/WWorld.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Stomper.hpp"
#include "bFile.hpp"

// OUTSIDE ZMISC //
extern int ExitTheGameFlag;
extern int frames_elapsed;
extern int loop_ticker;

// zFoundation
extern void (*UFoundation_AssertMessage)(const char *, ...);

////

// IN ZMISC //
void InitializeEverything(int argc, char *argv[]);

void WriteFreekerBaseAddressBeacon();

bool bInitDisculatorDriver(const char *dir_filename, const char *data_filename);

class RaceStarter {
  public:
    static void StartSkipFERace();
};
extern int SkipFE;

Timer RealTimer;

extern bool twkDumpProfileMarks;
////

void FastMemEmergencyInitialization(unsigned int &bytes, const char *&name, unsigned int &expansionsize, unsigned int &trackingsize) {
    bytes = 0x11f400;
    name = "gFastMem";
    expansionsize = 0x2000;
    trackingsize = 0x200;
}

void SeedRandomNumber() {
    int seed;
    if (Joylog::IsReplaying()) {
        seed = Joylog::GetData(32, JOYLOG_CHANNEL_RANDOM);
    } else {
        seed = bGetTicker() + 0x12345678;
        Joylog::AddData(seed, 32, JOYLOG_CHANNEL_RANDOM);
    }
    bSetRandomSeed(seed, &bDefaultSeed);
}

void InitBigFiles() {
    if (bFileExists("NFS\\ZDIR.BIN")) {
        bInitDisculatorDriver("NFS\\ZDIR.BIN", "NFS\\ZZDATA");
    }
}

static void Main_MyAssert(const char *format, ...) {}

int InitializeEverythingTicks = 0;

void InitializeEverything(int argc, char **argv) {
    ResetCapturedLoadingTimes();
    CaptureLoadingTime("InitializeEverything");
    // ProfileManager::Init(); // TODO

    UFoundation_AssertMessage = Main_MyAssert;
    bReserveMemoryPool(7);
    bReserveMemoryPool(8);
    bMemoryCreatePersistentPool(0x1000);
    bInitTicker(60000.0f);
    InitializeEverythingTicks = bGetTicker();
    bInitSharedStringPool(0x8000);
    InitPlatform();
    FirstBreakpoint();
    bMathTimingTest();
    BuildRegion::IsAmerica();
    TheDemoDiscManager.Init(argc, argv);
    bPListInit(0x1800);
    InitConfig();
    InitJoylog();
    SeedRandomNumber();
    InitQueuedFiles();
    emEventManagerInit();
    eMathInit();
    eInitEngine();
    InitDisplaySystem();
    EmitterSystem::Init();
    InitResourceLoader();
    InitializeSoundDriver();
    FEPackageManager::Init();
    LoadGlobalAChunks();
    FEManager::Init();
    BootLoadingScreen();
    InitBigFiles();
    InitMemoryCard();
    LoadGlobalChunks();
    InitFrontendDatabase();

    TheOnlineManager.Initialize(argc, argv);
#ifdef ONLINE_SUPPORT
// TODO
// /* inlined from
//    d:/p4_apex1666_d1001856/mw/speed/indep/src/online/conditionsimulator.hpp */
// if (ConditionSimulator::m_simCondition == SIMCONDITION_NTSCINEUROPE) {
//     SetVideoMode(MODE_NTSC);
// }
#endif

    IOModule::GetIOModule().Initialize();
    FEManager::InitInput();
    IOModule::GetIOModule().EnableUpdating(true);
    EventManager::Init();
    Hermes::System::Init();
    Scheduler::Init(0.016666668f);
    WWorld::Init();
    EventSequencer::Init(0.0f);
#ifdef DEBUG
    InitScreenPrintf();
    DebugMenuInit();
#endif
    UnloadFrontEndVault();
    GManager::Init("GLOBAL\\GAMEPLAY.BIN");
    GRaceDatabase::Init();
    GManager::Get().InitializeRaceStreaming();
    GInfractionManager::Init();
    SmokeableSpawner::Init();
    InitCarRender();
    InitStandardModels();
    InitCarLoader();
    InitVehicleDamage();
    DebugVehicleSelection::Init();
    DebugWorld::Init();
    InitStomper();
    Physics::Info::Init();
    InitAnimCtrls();
    InitSpaceNodes();
    TheICEManager.Init();
    FastMem::Lock();

    PrintCapturedLoadingTime("InitializeEverything", "InitializeEverything");
}

void WriteFreekerBaseAddressBeacon() {}

void DisplayDebugScreenPrints() {}

int RealLoopCounter;

void VerifyJoylogChecksum() {
    if (!Joylog::IsCapturing() && !Joylog::IsReplaying()) {
        return;
    }
    if (Joylog::IsCapturing()) {
        Joylog::AddData(bDefaultSeed, 32, JOYLOG_CHANNEL_RANDOM);
    } else if (Joylog::IsReplaying()) {
        bDefaultSeed = Joylog::GetData(32, JOYLOG_CHANNEL_RANDOM);
    }
    uint16 world_checksum = 0;
    {
        const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_ALL);
        for (IVehicle::List::const_iterator iter = vehicles.begin(); iter != vehicles.end(); iter++) {
            IVehicle *vehicle = *iter;
            const UMath::Vector3 &position = vehicle->GetPosition();
            world_checksum += reinterpret_cast<const uint16 *>(&position)[1];
            world_checksum += reinterpret_cast<const uint16 *>(&position)[3];
            world_checksum += reinterpret_cast<const uint16 *>(&position)[5];
        }
    }
    uint16 menu_checksum = FEngGetActiveScreensChecksum();
    uint16 random_seed_checksum = bDefaultSeed & 0xffff;
    if (TheGameFlowManager.IsInGame()) {
        random_seed_checksum = 0;
    }
    uint16 real_loop_counter_checksum = RealLoopCounter;
    uint16 current_checksum;
    int checksum_error = 0;
    if (Joylog::IsReplaying()) {
        uint16 prev_world_checksum = Joylog::GetData(16, JOYLOG_CHANNEL_CHECKSUM);

        if (world_checksum != prev_world_checksum) {
            checksum_error = 1;
        }
        if (menu_checksum != Joylog::GetData(16, JOYLOG_CHANNEL_CHECKSUM)) {
            checksum_error++;
        }
        if (random_seed_checksum != Joylog::GetData(16, JOYLOG_CHANNEL_CHECKSUM)) {
            checksum_error++;
        }
        if (real_loop_counter_checksum != Joylog::GetData(16, JOYLOG_CHANNEL_CHECKSUM)) {
            checksum_error++;
        }
    } else {
        Joylog::AddData(world_checksum, 16, JOYLOG_CHANNEL_CHECKSUM);
        Joylog::AddData(menu_checksum, 16, JOYLOG_CHANNEL_CHECKSUM);
        Joylog::AddData(random_seed_checksum, 16, JOYLOG_CHANNEL_CHECKSUM);
        Joylog::AddData(real_loop_counter_checksum, 16, JOYLOG_CHANNEL_CHECKSUM);
    }
    if (checksum_error) {
        bBreak();
        Joylog::StopReplaying();
    }
}

int TweakerPauseCamera = 0;

static float Main_AnimateFrame(float real_dT) {
    ProfileNode profile_node;
    static float camera_dt = 0.0f;
    if (real_dT <= 0.0f) {
        if (TheGameFlowManager.IsInFrontend()) {
            const float fe_dt_fix = 0.01f;
            CameraAI::Update(fe_dt_fix);
            Camera::UpdateAll(fe_dt_fix);
        }
        return 0.0f;
    }
    float game_dT = 0.0f;
    if (TweakerPauseCamera == 0) {
        camera_dt += real_dT;
    }
    static unsigned int last_sim_tick_animated = 0;
    static unsigned int last_render_frame_animated = 0;
    if (Sim::Exists() && Sim::GetState() == Sim::STATE_ACTIVE && last_sim_tick_animated != Sim::GetTick() &&
        last_render_frame_animated != eGetFrameCounter()) {
        game_dT = Sim::GetFrameTimeElapsed();
        if (game_dT <= 0.0f) {
            return 0.0f;
        }
        last_sim_tick_animated = Sim::GetTick();
        last_render_frame_animated = eGetFrameCounter();

        profile_node.Begin("WorldConn::UpdateServices()", 0);
        WorldConn::UpdateServices(game_dT);

        profile_node.Begin("SoundConn::UpdateServices()", 0);
        SoundConn::UpdateServices(game_dT);

        profile_node.Begin("RenderConn::UpdateServices()", 0);
        RenderConn::UpdateServices(game_dT);
    }
    profile_node.Begin("CameraUpdate", 0);
    CameraAI::Update(camera_dt);
    Camera::UpdateAll(camera_dt);
    camera_dt = 0.0f;
    return game_dT;
}

int gFramesToSkip = 0;

// UNSOLVED
void Main_SkipFrame(int numToSkip) {
    gFramesToSkip = bMax(gFramesToSkip, numToSkip);
}

int RenderTimingStart;
int RenderTimingEnd;
int FrameTimingStartTime;
int FrameTimingEndTime;

extern float HackTime;

void Main_DisplayFrame() {
    ProfileNode profile_node;
    static float timeStep = 0.01666f;
    if (gFramesToSkip >= 1) {
        gFramesToSkip--;
    } else {
        FrameTimingStartTime = FrameTimingEndTime;
        FrameTimingEndTime = bGetTicker();
        RenderTimingStart = bGetTicker();
        profile_node.Begin("eDisplayFrame()", 0);
        eDisplayFrame();
        RenderTimingEnd = bGetTicker();
        HackTime += timeStep;
        DisplayDebugScreenPrints();
        EnableInterrupts();
    }
}

void CheckTweakerTriggers() {}

void MainLoopCheckForFatalDiscError() {}

float RealTimeElapsed;

void MiniMainLoop() {
    static int recursion_checker = 0;
    static int previous_ticks = 0;

    bThreadYield(8);
    Sim::Suspend();
    float dt = bGetTickerDifference(previous_ticks);
    previous_ticks = bGetTicker();
    PrepareRealTimestep(dt * 0.001f);
    ServiceResourceLoading();
    ServiceFileStats();
    MainLoopCheckForFatalDiscError();
    IOModule::GetIOModule().Update();
    FEManager::Get()->Update();
    TheTrackStreamer.ServiceGameState();
    TheTrackStreamer.ServiceNonGameState();
    if (g_pEAXSound) {
        g_pEAXSound->Update(RealTimeElapsed);
    }
    eDisplayFrame();
    AdvanceRealTime();
    recursion_checker--;
}

extern bool Tweak_FullSpeedMode; // TODO

static int last_frame_count;

volatile int FrameCounter;

void MainLoop(float hardware_ms) {
    int ticks;
    ProfileNode profile_node_entire_mainloop("MainLoop()", 0);
    ProfileNode profile_node;
    bGetTicker();
    MainLoopBreakpoint();
    PrepareRealTimestep(hardware_ms * 0.001f);
    bSyncTaskRun();
    profile_node_entire_mainloop.Begin("IOModule::Update()", 0);
    IOModule::GetIOModule().Update();
    profile_node_entire_mainloop.Begin("Scheduler::Run()", 0);
    float game_dT = Scheduler::Get().Run(Tweak_FullSpeedMode);
    if (TheOnlineManager.IsOnlineRace() && TheOnlineManager.GetState() == OLS_RACING) {
        TheOnlineManager.EndSimFrame();
    }
    Attrib::Database::Get().CollectGarbage();
    profile_node_entire_mainloop.Begin("Main_AnimateFrame()", 0);
    game_dT = Main_AnimateFrame(game_dT);
    PrepareWorldTimestep(game_dT);
    last_frame_count = FrameCounter;
    ServiceJoylog();
    CheckTweakerTriggers();
    profile_node_entire_mainloop.Begin("MainLoop - ServiceGameFlow", 0);
    profile_node.End();
    TheGameFlowManager.Service();
    TheGameFlowManager.CheckForDemoDiscTimeout();
    emProcessAllEvents();
    if (Sim::Exists()) {
        World_Service();
    }
    ServicePlatform();
    ServicePreculler();
    FEManager::Get()->Update();
    EventManager::RunEvents();
    if (g_pEAXSound) {
        profile_node_entire_mainloop.Begin("EAXSound::Update()", 0);
        g_pEAXSound->Update(RealTimeElapsed);
    }
    profile_node_entire_mainloop.Begin("ServiceResourceLoading etc.", 0);
    ServiceResourceLoading();
    MainLoopCheckForFatalDiscError();
    RenderConn::UpdateLoading();
    TheTrackStreamer.ServiceNonGameState();
    HandleTrackStreamerLoadingBar();
    TickOverTimeOfday();
    if (!Tweak_FullSpeedMode) {
        profile_node_entire_mainloop.Begin("Main_DisplayFrame()", 0);
        Main_DisplayFrame();
    }
    Sim::StartProfile();
    profile_node_entire_mainloop.Begin("MainLoop wind down", 0);
    VerifyJoylogChecksum();
    AdvanceWorldTime();
    MaybePrintUnusedTextures();
    MaybeDoMemoryProfile();
    AdvanceRealTime();
    profile_node_entire_mainloop.End();
    profile_node.End();

    static int PrintFreeMem = 0;
    if (PrintFreeMem != 0) {
        int free_memory = bCountFreeMemory(0);
#ifdef EA_PLATFORM_GAMECUBE
        OSReport("Free memory %dK\n", free_memory / 1024);
#else
#endif
    }
}

#ifndef EA_PLATFORM_XENON
int main(int argc, char **argv)
#else
int MainThreadFunction(int argc, char **argv)
#endif
{
#ifdef EA_PLATFORM_WINDOWS
    // PC version wants there to only be one instance at a time
    if (CheckProcessCount("speed.exe", 1)) // this function name is complete bs that I made up for now -Toru
        _exit(0);
#endif

    InitializeEverything(argc, argv);

    WriteFreekerBaseAddressBeacon();

    if (SkipFE)
        RaceStarter::StartSkipFERace();
    else
        TheGameFlowManager.LoadFrontend();

    frames_elapsed = 1;

    loop_ticker = bGetTicker();
    Scheduler::Get().Synchronize(RealTimer);

    while (!ExitTheGameFlag) {
        const float minumum_time_step = 0.25f;

        unsigned int current_tick = bGetTicker();
        float milliseconds = bGetTickerDifference(loop_ticker, current_tick);
        int milliseconds_fix;

        if (current_tick != loop_ticker && milliseconds <= 0.0f) {
            loop_ticker = current_tick;
        } else if (milliseconds > minumum_time_step) {
            loop_ticker = current_tick;
            if (milliseconds > 32000.0f)
                milliseconds = 32000.0f;

            milliseconds_fix = (int)(milliseconds * 65536.0f); // this is used on other platforms

            MainLoop(milliseconds);

            if (twkDumpProfileMarks)
                twkDumpProfileMarks = false;
        }
    }

    return 0;
}
