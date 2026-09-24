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
#include "Stomper.hpp"
#include "bFile.hpp"
#include "Speed/Indep/Src/Animation/AnimCtrl.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Debug/Dcyclecount.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
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
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/VehiclePartDamage.h"
#include "Speed/Indep/Src/World/WWorld.h"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Libs/Support/Utility/UDebug.h"

FastMem gFastMem; // Decl: 227
void FastMemEmergencyInitialization(unsigned int &bytes, const char *&name, unsigned int &expansionsize, unsigned int &trackingsize) {
    bytes = 0x11f400;
    name = "gFastMem";
    expansionsize = 0x2000;
    trackingsize = 0x200;
}

// STRIPPED
void GetTheMiliseconds() {}

// STRIPPED
void GetTheSeconds() {}

int ExitTheGameFlag = 0; // Decl: 258

int frames_elapsed;                // Decl: 260 TODO probably extern
int loop_ticker;                   // Decl: 261 TODO probably extern
static int32 last_frame_count = 0; // Decl: 262
static int32 num_race_iterations;  // Decl: 263

static const int PrintAllSlotPools = 0; // Decl: 266
static const int DumpSharedStrings = 0; // Decl: 267

int g_discErrorNumber = 0;  // Decl: 270
int g_discErrorOccured = 0; // Decl: 271
unsigned int CurrentTicksDiff;
int32 CurrentLoopCounter = 0;
int SaveTheLoopCounter1;
int SaveTheLoopCounter2;

uint32 TimeDifferenceInMicroseconds = 0;
float TimeDifferenceInMiliseconds = 0.0f;
float TimeDifferenceInSeconds = 0.0f;
float MicrosecondsToMiliseconds = 0.001f;
float MilisecondsToSeconds = 1000.0f;

static const float PLATFORM_TARGET_FPS = 60.0f; // Decl: 313
static const float PLATFORM_WARN_FPS = 25.0f;   // Decl: 314

static const float PLATFORM_TARGET_TIME_SLICE = (1.0f / PLATFORM_TARGET_FPS) * 1000.0f; // Decl: 316
static const float PLATFORM_TARGET_SIM_SLICE = PLATFORM_TARGET_TIME_SLICE / 2.0f;       // Decl: 317
static const float PLATFORM_TARGET_RENDER_SLICE = PLATFORM_TARGET_TIME_SLICE / 2.0f;    // Decl: 318
static const unsigned int PLATFORM_MINIMUM_SIMFRAME = 1;                                // Decl: 319 TODO use in DisplayDebugScreenPrints

// Decl: 353
// STRIPPED
void PrintTimingStat(const char *name, int *ticks, int *start_tick) {}

static const bool TweakShowUTLWarnings = false; // Decl: 364

// Decl: 365
// STRIPPED
void ShowUTLWarnings(const char *who, const char *warning) {}

static const int DoProfileAttribSearches = 0; // Decl: 377

// Decl: 393
// STRIPPED
void ProfileAttribSearch(unsigned int key, bool collections) {}

static const bool DoAttribOverrunCheck = false; // Decl: 463

// Decl: 464
// STRIPPED
void AttribOverrunCheck(int currententries, int overrun) {}

// Decl: 501
// STRIPPED
int DoWithMainMemoryStack(void *function, int arg1, int arg2) {}

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

void InitScreenPrintf();
void DebugMenuInit();

static const bool Events_Verbose = false; // Decl: 792

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
#ifdef MILESTONE_BUILD
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

float PreviousCpuFrameTime = 0.0f;    // Decl: 1585
float PreviousGpuFrameTime = 0.0f;    // Decl: 1586
float PreviousCpuFrameRate = 0.0f;    // Decl: 1587
float PreviousGpuFrameRate = 0.0f;    // Decl: 1588
float PreviousSimFrameTime = 0.0f;    // Decl: 1589
float PreviousRenderFrameTime = 0.0f; // Decl: 1590
int PreviousSimCallCount = 0.0f;      // Decl: 1591
float PreviousProfileTime = 0.0f;     // Decl: 1592

static const int nPolCountX = 30;    // Decl: 1594
static const int nPolCountY = 0;     // Decl: 1595
static const int nPolCountXPC = 170; // Decl: 1596
static const int nPolCountXMC = 260; // Decl: 1597

static const int nTimerInfoX = -300; // Decl: 1599
static const int nTimerInfoY = -245; // Decl: 1600

static const int nFrameRateX = -300; // Decl: 1603
static const int nFrameRateY = -60;  // Decl: 1604

static const int nWorldPositionX = -300; // Decl: 1607
static const int nWorldPositionY = 15;   // Decl: 1608

static const int nTimeY = -90; // Decl: 1610

int DisplayPolyCount = 0;         // Decl: 1612
bool gDoFrameRateSummary = false; // Decl: 1613

static const int DisplayFrameRateDetailed = 0; // Decl: 1615
static const int DisplayChangelist = 1;        // Decl: 1616
static const int DisplayWalkPathTime = 1;      // Decl: 1617

int RenderTimingStart = 0; // Decl: 1619
int RenderTimingEnd = 0;   // Decl: 1620

int FrameTimingStartTime = 0;              // Decl: 1622
int FrameTimingEndTime = 0;                // Decl: 1623
eProfMeter MainCycleCounter = kPROFRender; // Decl: 1624
static const bool Tweak_PrintTime = false; // Decl: 1625

float tframe = 0.0f;   // Decl: 1630
float tframe30 = 0.0f; // Decl: 1631
float over30 = 0.0f;   // Decl: 1632
int DisplayOver30 = 0; // Decl: 1633
int ResetOver30 = 0;   // Decl: 1634

void DisplayDebugScreenPrints() {}

extern int DoScreenPrintf; // Decl: 1824

static const int StopOnJoylogChecksumError = 1; // Decl: 2207
static const int DoDetailedJoylogChecksum = 1;  // Decl: 2208

void VerifyJoylogChecksum() {
    if (!Joylog::IsCapturing() && !Joylog::IsReplaying()) {
        return;
    }
#ifndef EA_BUILD_A124
    if (Joylog::IsCapturing()) {
        Joylog::AddData(bDefaultSeed, 32, JOYLOG_CHANNEL_RANDOM);
    } else if (Joylog::IsReplaying()) {
        bDefaultSeed = Joylog::GetData(32, JOYLOG_CHANNEL_RANDOM);
    }
#endif
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
        // TODO warning print
        if (StopOnJoylogChecksumError) {
            bBreak();
            Joylog::StopReplaying();
        }
    }
}

int TweakerPauseCamera = 0; // Decl: 2337

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
    static uint32 last_sim_tick_animated = 0;
    static uint32 last_render_frame_animated = 0;
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

static const int StopOnRealLoopCounter = -1; // Decl: 2346

// STRIPPED
void Main_LimitFrameRate() {}

int gFramesToSkip = 0; // Decl: 2376

void Main_SkipFrame(int numToSkip) {
    gFramesToSkip = bMax(numToSkip, gFramesToSkip);
}

void Main_DisplayFrame() {
    ProfileNode profile_node;
    extern float HackTime;
    static float timeStep = 0.01666f;
    if (gFramesToSkip >= 1) {
        gFramesToSkip--;
    } else {
        FrameTimingStartTime = FrameTimingEndTime;
        FrameTimingEndTime = bGetTicker();
        RenderTimingStart = bGetTicker();
        profile_node.Begin("eDisplayFrame()", 0);

        void eDisplayFrame(); // Decl: 2403
        eDisplayFrame();
        RenderTimingEnd = bGetTicker();
        HackTime += timeStep;
        DisplayDebugScreenPrints();
        EnableInterrupts();
    }
}

void CheckTweakerTriggers() {}

void MainLoopCheckForFatalDiscError() {}

bool Tweak_FullSpeedMode = false;                      // Decl: 2585
bool twkDumpProfileMarks = false;                      // Decl: 2586
static const float DumpProfileOnMicropauseTime = 0.0f; // Decl: 2587

void MiniMainLoop() {
    static int recursion_checker = 0;
    static int previous_ticks = 0;

    // TODO are these ifdefs right?
#ifdef EA_BUILD_A124
    bMonitorService();
#endif
    bThreadYield(8);
#ifndef EA_BUILD_A124
    Sim::Suspend();
#endif
    float dt = bGetTickerDifference(previous_ticks);
    previous_ticks = bGetTicker();
    PrepareRealTimestep(dt * 0.001f);
    ServiceResourceLoading();
#ifndef EA_BUILD_A124
    void ServiceFileStats(); // Decl: 2621
    ServiceFileStats();
    MainLoopCheckForFatalDiscError();
#endif
    IOModule::GetIOModule().Update();
    FEManager::Get()->Update();
    TheTrackStreamer.ServiceGameState();
    TheTrackStreamer.ServiceNonGameState();
#ifndef EA_BUILD_A124
    if (g_pEAXSound != nullptr) {
        g_pEAXSound->Update(RealTimeElapsed);
    }
#endif
    void eDisplayFrame(); // Decl: 2649
    eDisplayFrame();
    AdvanceRealTime();
    recursion_checker--;
}

extern bool LOCK_TO_30; // Decl: 2662

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
    if (g_pEAXSound != nullptr) {
        profile_node_entire_mainloop.Begin("EAXSound::Update()", 0);
        g_pEAXSound->Update(RealTimeElapsed);
    }
    profile_node_entire_mainloop.Begin("ServiceResourceLoading etc.", 0);
    ServiceResourceLoading();
    MainLoopCheckForFatalDiscError();
    RenderConn::UpdateLoading();
    TheTrackStreamer.ServiceNonGameState();
#ifndef EA_BUILD_A124
    HandleTrackStreamerLoadingBar();
#endif
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

static const float Tweak_MinimumRealTimeStep = 0.25f; // Decl: 3123

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

    if (SkipFE) {
        RaceStarter::StartSkipFERace();
    } else {
        TheGameFlowManager.LoadFrontend();
    }

    frames_elapsed = 1;

    loop_ticker = bGetTicker();
    Scheduler::Get().Synchronize(RealTimer);

    while (!ExitTheGameFlag) {
        const float minumum_time_step = Tweak_MinimumRealTimeStep;

        uint32 current_tick = bGetTicker();
        float milliseconds = bGetTickerDifference(loop_ticker, current_tick);
        bFix milliseconds_fix;

        if (current_tick != loop_ticker && milliseconds <= 0.0f) {
            loop_ticker = current_tick;
        } else if (milliseconds > minumum_time_step) {
            loop_ticker = current_tick;
            if (milliseconds > 32000.0f)
                milliseconds = 32000.0f;

            milliseconds_fix = (bFix)(milliseconds * 65536.0f); // this is used on other platforms

            MainLoop(milliseconds);

            if (twkDumpProfileMarks) {
                twkDumpProfileMarks = false;
            }
        }
    }

    return 0;
}
