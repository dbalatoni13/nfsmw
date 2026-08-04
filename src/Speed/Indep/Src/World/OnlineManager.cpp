#include "OnlineManager.hpp"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Online/OnlineCfg.hpp"
#include "Speed/Indep/Src/Online/VoiceCore.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

namespace Online {
bool IsInitialized();
void Close();
void SignalStartClockSync();
}

extern int OnlineIsServer;
extern int OnlineEnabled;
extern int bSuperBenderConnected;
extern int NetworkUseLobbies;

int SuperBenderGetCommandLineArgc();
char **SuperBenderGetCommandLineArgv();
uint32 fptoui(float value);

struct ConnectionCore {
    static ConnectionCore &Instance();
    void LeaveSession();
};

class NetworkCore {
  public:
    static NetworkCore &Instance() { return mInstance; }
    static void DoNetworkProcessing();
    uint32 GetTime();

  private:
    static NetworkCore mInstance;
};

struct OnlineTimer {
    static Timer ServerTimer;
};

OnlineManager TheOnlineManager;

OnlineManager::OnlineManager()
    : RestartingRace(false) //
    , RestartRequested(false) {
    State = OLS_DISCONNECTED;
    bMemSet(PersonaMap, 0, sizeof(PersonaMap));
    InitQuantizers();

    pLocalRacer = nullptr;
    NumRacers = 0;
    bOnlineRace = false;
    mMasterTime = 0;
    mServerTime = 0;
    CountdownSyncAnims.UnSet();
    CountdownSendDataCRC.UnSet();
    LastSyncedWTTimeStamp.UnSet();
    LastAntiCheatWorldTimer.UnSet();
    LastAntiCheatRealTimer.UnSet();
    TimeRaceFinished.UnSet();
    RaceTimeup = false;

    for (int i = 3; i >= 0; --i) {
        pRacers[i] = nullptr;
    }
    ClearAnimWorldObjects();
}

void OnlineManager::InitQuantizers() {
    float min_value;
    float max_value;

    PositionQuantizerX.Init("PositionQuantizerX", -4800.0f, 1792.0f, 0.01f);
    PositionQuantizerY.Init("PositionQuantizerY", -15.0f, 300.0f, 0.01f);
    max_value = 1.0f;
    PositionQuantizerZ.Init("PositionQuantizerZ", -1664.0f, 4224.0f, 0.01f);
    min_value = 0.0f;
    VelocityQuantizer.Init("VelocityQuantizer", -125.0f, 125.0f, 0.1f);
    MatrixQuantizer.Init("MatrixQuantizer", -1.0f, max_value, 0.01f);
    AngleQuantizer.Init("AngleQuantizer", -0.5f, 0.5f, 0.0025f);
    AccelerationQuantizer.Init("AccelerationQuantizer", -25.0f, 25.0f, 0.1f);
    AVelocityQuantizer.Init("AVelocityQuantizer", -5.0f, 5.0f, 0.01f);
    ControlQuantizer.Init("ControlQuantizer", min_value, max_value, 0.15f);
    QuantFloatSimTime.Init("QuantFloatSimTime", min_value, 5400.0f, 0.001f);
    QuantCarSlotID.Init("QuantCarSlotID", 0, 0x8b);
    QuantCarPartIndex.Init("QuantPartIndex", 0, 16000);
    QuantCarType.Init("QuantCarType", 0, 0x4e);
    QuantInt2Bit.Init("QuantInt2Bit", 0, 3);
    QuantInt3Bit.Init("QuantInt3Bit", 0, 7);
    QuantInt4Bit.Init("QuantInt4Bit", 0, 0xf);
    QuantFloatTime.Init("QuantFloatTime", min_value, 14400.0f, 0x18);
}

void OnlineManager::PrintQuantizersUsageReport() {
    PositionQuantizerX.PrintUsageReport();
    PositionQuantizerY.PrintUsageReport();
    PositionQuantizerZ.PrintUsageReport();
    VelocityQuantizer.PrintUsageReport();
    AccelerationQuantizer.PrintUsageReport();
    AngleQuantizer.PrintUsageReport();
    AVelocityQuantizer.PrintUsageReport();
    MatrixQuantizer.PrintUsageReport();
    ControlQuantizer.PrintUsageReport();
}

void OnlineManager::PrintCheatTallies(bool) {}

OnlineManager::~OnlineManager() {}

bool OnlineManager::IsOnlineRace() {
    bool online_race = false;
    if (bOnlineRace || Online::IsInitialized()) {
        online_race = true;
    }
    return online_race;
}

bool OnlineManager::IsServer() {
    return OnlineIsServer != 0;
}

bool OnlineManager::RaceStartAborted() {
    return GetNumConnectedRacers() == 0;
}

void OnlineManager::ChangeState(eOnlineState new_state) {
    eOnlineState old_state = State;
    if (new_state != old_state) {
        State = new_state;
        if (new_state == OLS_RACING || old_state == OLS_RACING) {
            float temp = TheOnlineManager.GetMasterTime() * 0.001f;
            Timer time(static_cast<int>(temp * 4000.0f + 0.5f));
            Scheduler::Get().Synchronize(time);
        }
    }
}

void OnlineManager::Initialize(int argc, char **argv) {
    if (bSuperBenderConnected) {
        argc = SuperBenderGetCommandLineArgc();
        argv = SuperBenderGetCommandLineArgv();
    }

    char *filename = nullptr;
    for (int i = 1; i < argc; ++i) {
        int len = bStrLen(argv[i]);
        if (len > 4 && bStrICmp(argv[i] + len - 4, ".cfg") == 0) {
            filename = argv[i];
        }
    }

    OnlineCfg::ReadConfigFile(filename);
    VoiceCore::Construct();
    if (SkipFE == 0) {
        OnlineEnabled = 1;
    }
}

void OnlineManager::Disconnect(bool force_disconnect) {
    if (Online::IsInitialized()) {
        Online::Close();
    }

    int driver_number = 0;
    ConnectionCore::Instance().LeaveSession();
    bOnlineRace = false;

    for (OnlineRacer **racer = pRacers; driver_number < 4; ++driver_number, ++racer) {
        if (*racer) {
            GetOnlineRacer(driver_number)->ChangeState(OPS_DISCONNECTED);
        }
    }

    if (State == OLS_RACE_DATA_SYNC && NetworkUseLobbies != 0 && !force_disconnect) {
        ChangeState(OLS_LOBBY_IN_LOBBY);
    } else {
        ChangeState(OLS_DISCONNECTED);
    }
    PurgeDisconnectedRacers();
}

int OnlineManager::GetNumConnectedRacers() {
    int num_connected = 0;
    OnlineRacer **racer = pRacers;

    for (int driver_number = 0; driver_number < 4; ++driver_number, ++racer) {
        if (*racer && GetOnlineRacer(driver_number) != pLocalRacer) {
            if (GetOnlineRacer(driver_number)->IsConnected()) {
                ++num_connected;
            }
        }
    }
    return num_connected;
}

int OnlineManager::GetNumRacers() {
    int num_racers = 0;
    OnlineRacer **racer = pRacers;
    int i = 3;
    do {
        if (*racer) {
            ++num_racers;
        }
        ++racer;
        --i;
    } while (i >= 0);
    return num_racers;
}

OnlineRacer *OnlineManager::GetOnlineRacer(int driver_number) {
    return pRacers[driver_number];
}

OnlineRacer *OnlineManager::GetOnlineRacer(const char *racer_name) {
    int driver_number = 0;
    OnlineRacer **racer = pRacers;

    for (; driver_number < 4; ++driver_number, ++racer) {
        if (*racer && bStrCmp((*racer)->GetPersona(), racer_name) == 0) {
            return *racer;
        }
    }
    return nullptr;
}

uint32 OnlineManager::GetMasterTime() {
    return NetworkCore::Instance().GetTime() - mMasterTime;
}

void OnlineManager::SetServerTime(uint32 time) {
    mServerTime = time - NetworkCore::Instance().GetTime();
}

uint32 OnlineManager::GetServerTime() {
    return mServerTime + NetworkCore::Instance().GetTime();
}

float OnlineManager::GetStartRaceTime(uint32 from_tick) {
    if (mStartRaceIsSet) {
        return mStartRaceTime - bGetTickerDifference(mStartRaceTick, from_tick) * 0.001f;
    }
    return 2.0f;
}

void OnlineManager::StartSimFrame() {
    NetworkCore::DoNetworkProcessing();
}

void OnlineManager::SetStartRaceTime(uint32 from_tick, float time, float ping) {
    if (!mStartRaceIsSet || ping < mStartRacePing) {
        RestartingRace = false;
        RestartRequested = false;
        mStartRaceTime = time;
        mStartRaceTick = from_tick;
        mStartRacePing = ping;

        uint32 master_time = GetMasterTime();
        uint32 time_milliseconds = fptoui(time * 1000.0f);
        mMasterTime += master_time + time_milliseconds;
        SetServerTime(GetMasterTime());
        mStartRaceIsSet = true;
    }
}

void OnlineManager::RequestRestart() {}

bool OnlineManager::GetRestartingRace() {
    return RestartingRace;
}

void OnlineManager::StartLobby() {
    if (State == OLS_DISCONNECTED) {
        ChangeState(OLS_LOBBY_IN_LOBBY);
    }
}

void OnlineManager::SetupStartingPositions() {}

void OnlineManager::SignalLoad() {
    if (NetworkUseLobbies != 0) {
        MapRacers2PlayerIDs();
    }
    ChangeState(OLS_RACE_LOAD_TRACK);
    if (TheGameFlowManager.IsInFrontend()) {
        TheGameFlowManager.UnloadFrontend();
        return;
    }
    TheGameFlowManager.LoadTrack();
}

void OnlineManager::TrackLoaded() {
    if (State == OLS_RACE_LOAD_TRACK) {
        CountdownSyncAnims.UnSet();
        CountdownSendDataCRC.UnSet();
        LastSyncedWTTimeStamp.UnSet();
        LastAntiCheatRealTimer.UnSet();
        LastAntiCheatWorldTimer.UnSet();
        TimeRaceFinished.UnSet();
        FrameRateIsTooLow = false;
        OnlineTimer::ServerTimer = WorldTimer;
        TimeTrackLoaded = RealTimer;
        if (Online::IsInitialized()) {
            Online::SignalStartClockSync();
            SetupStartingPositions();
        }
    }
}
