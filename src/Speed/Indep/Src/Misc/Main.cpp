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
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
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
#include "Speed/Indep/Src/Main/Common/Event.hpp"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Physics/SmokeableInfo.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/World/DebugVehicleSelection.h"
#include "Speed/Indep/Src/World/DebugWorld.h"
#include "Speed/Indep/Src/World/EventManager.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/SpaceNode.hpp"
#include "Speed/Indep/Src/World/VehiclePartDamage.h"
#include "Speed/Indep/Src/World/WWorld.h"
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

void MainLoop(float hardware_ms);
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
    CaptureLoadingTime("TODO");
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

    PrintCapturedLoadingTime("TODO", "TODO2");
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
