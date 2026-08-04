#include "OnlineManager.hpp"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Animation/AnimPlayer.hpp"
#include "Speed/Indep/Src/Online/OnlineCfg.hpp"
#include "Speed/Indep/Src/Online/SmartBitstream.hpp"
#include "Speed/Indep/Src/Online/VoiceCore.hpp"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

namespace Online {
bool IsInitialized();
void Close();
void Init();
void SignalStartClockSync();
void SignalReady();
void ReadIncomingPackets();
void SendUpdates();
void SignalSyncAnimationMessage(SmartBitStream &payload_data);
}

extern int OnlineIsServer;
extern int OnlineEnabled;
extern int bSuperBenderConnected;
extern int NetworkUseLobbies;
extern void *pCurrentRace;
extern const char *SkipFEPlayerCar;

int SuperBenderGetCommandLineArgc();
char **SuperBenderGetCommandLineArgv();
uint32 fptoui(float value);
inline void *operator new(size_t size, const char *name, int line, int allocation_params) {
    return bMalloc(size, name, line, allocation_params);
}

class RaceStarter {
  public:
    static void SetupOnlineRace();
};

void SmartBitStream::AddQuantizedInt(int value, IntQuantizer &iq) {
    AddBits(iq.Pack(value), iq.GetNumBits());
}

void SmartBitStream::AddQuantizedFloat(float value, FloatQuantizer &qf) {
    AddBits(qf.Pack(value), qf.GetNumBits());
}

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

void OnlineManager::StartOnlineRace() {
    if (NetworkUseLobbies != 0 && State != OLS_LOBBY_IN_LOBBY) {
        return;
    }

    ChangeState(OLS_LOBBY_IN_LOBBY);
    CreateOnlineRacer(0, nullptr, IsServer(), FEDatabase->OnlineSettings.GetLobbyPersona());
    if (SkipFE == 0) {
        if (IsServer()) {
            SetupRaceParams();
        }
    }

    mStartRaceIsSet = false;
    mStartRaceTime = 0.0f;
    mStartRaceTick = bGetTicker();
    mMasterTime += GetMasterTime();
    uint32 master_time = GetMasterTime();
    SetServerTime(master_time);
    mLastUpdateTime = NetworkCore::Instance().GetTime();
    TimeupStartTime.UnSet();
    Online::Init();
    bOnlineRace = true;
    ChangeState(OLS_RACE_DATA_SYNC);
}

void OnlineManager::SetupRestartRace() {
    TheRaceParameters.RaceType = RACE_TYPE_SINGLE_RACE;

    int driver_number = 0;
    OnlineRacer **racer = pRacers;
    do {
        OnlineRacer *oracer = GetOnlineRacer(driver_number);
        ++driver_number;
        if (*racer && oracer != pLocalRacer) {
            if (!oracer->bShouldRestart) {
                TheRaceParameters.RemoveDriverInfo(oracer->GetDriverNumber());
                delete *racer;
                *racer = nullptr;
                --NumRacers;
            } else {
                oracer->bShouldRestart = false;
                oracer->SetRaceScore(0);
            }
        }
        ++racer;
    } while (driver_number < 4);
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

void OnlineManager::SetPlayerIDs(const char (*personas)[16]) {
    bMemCpy(PersonaMap, personas, 0x40);
    RestartingRace = false;
    RestartRequested = false;
}

bool OnlineManager::GetRestartingRace() {
    return RestartingRace;
}

void OnlineManager::StartLobby() {
    if (State == OLS_DISCONNECTED) {
        ChangeState(OLS_LOBBY_IN_LOBBY);
    }
}

void OnlineManager::SetupStartingPositions() {}

void OnlineManager::SetupRaceParams() {
    RaceStarter::SetupOnlineRace();
    g_tweakDriftPhysics = 0;
}

void OnlineManager::SetupLocalDriver(int driver_num) {
    OnlineRacer **racer;
    if (driver_num != 0) {
        racer = &pRacers[driver_num];
        *racer = pRacers[0];
        pRacers[0] = nullptr;
    } else {
        racer = pRacers;
    }
    pLocalRacer = *racer;
    pLocalRacer->DriverNumber = driver_num;
}

void OnlineManager::CreateOnlineRacer(int driver_num, SmartBitStream *pdata, bool is_server,
                                      const char *persona) {
    PurgeDisconnectedRacers();
    OnlineRacer **racer_slot = pRacers + driver_num;
    if (*racer_slot == nullptr) {
        OnlineRacer *racer = new ("OnlineRacer", 0, 8)
            OnlineRacer(static_cast<int8>(driver_num), is_server, persona);
        *racer_slot = racer;
        racer->ChangeState(OPS_CONNECTED);
        ++NumRacers;
    } else {
        if (GetOnlineRacer(driver_num)->IsServer()) {
            GetOnlineRacer(driver_num)->SetPersona(persona);
            return;
        }
    }
}

void OnlineManager::ExportRaceParams(int server_driver_number, SmartBitStream &data) {
    if (!pLocalRacer) {
        SetupLocalDriver(server_driver_number);
    }

    cOnlineSettings *settings = &FEDatabase->OnlineSettings;
    OnlineRaceParameters *race_settings = settings->GetRaceSettings();
    data.AddByte(settings->RaceMode);
    data.AddBool(settings->CollisionDetection);
    data.AddByte(race_settings->TrackDirection);
    data.AddShort(race_settings->NumLaps);
    data.AddByte(race_settings->CopDensity);
    data.AddByte(0);
}

void OnlineManager::ImportRaceParams(int local_driver_number, int server_driver_number,
                                     SmartBitStream &data) {
    cFrontendDatabase *database = FEDatabase;
    cOnlineSettings *settings;
    OnlineRaceParameters *race_settings;
    GRace::Type race_mode;
    uint32 collision_detection;
    uint32 track_direction;
    uint32 num_laps;
    uint32 cop_density;
    uint32 temp;

    race_mode = GRace::kRaceType_P2P;
    data.GetBits(reinterpret_cast<uint32 &>(race_mode), 8);
    settings = &database->OnlineSettings;
    race_settings = settings->GetRaceSettings();
    settings->RaceMode = static_cast<GRace::Type>(static_cast<uint8>(race_mode));
    collision_detection = 0;
    data.GetBits(collision_detection, 1);
    settings->CollisionDetection = collision_detection == 1;
    track_direction = 0;
    data.GetBits(track_direction, 8);
    race_settings->TrackDirection = track_direction;
    num_laps = 0;
    data.GetBits(num_laps, 16);
    race_settings->NumLaps = num_laps;
    cop_density = 0;
    data.GetBits(cop_density, 8);
    race_settings->CopDensity = static_cast<uint8>(cop_density);
    SetupRaceParams();
    SetupLocalDriver(local_driver_number);
    CreateOnlineRacer(server_driver_number, nullptr, true, nullptr);
    temp = 0;
    data.GetBits(temp, 8);
}

void OnlineManager::ImportDriverInfo(int driver_number, SmartBitStream &data) {
    char persona[16];
    OnlineRacer *racer;
    uint32 v;

    data.GetTerminatedString(persona, 0x10);
    CreateOnlineRacer(driver_number, nullptr, false, persona);
    racer = GetOnlineRacer(driver_number);
    data.GetRawData(reinterpret_cast<char *>(&racer->CarTypeKey), 4);
    v = 0;
    data.GetBits(v, 1);
    racer->IsCustomCar = v != 0;
    if (racer->IsCustomCar) {
        data.GetRawData(reinterpret_cast<char *>(&racer->CarCustomization), 0x198);
    }

    Attrib::Gen::pvehicle vehicle(racer->CarTypeKey, 0, nullptr);
}

void OnlineManager::ExportDriverInfo(int driver_number, SmartBitStream &data) {
    OnlineRacer *racer = GetOnlineRacer(driver_number);
    char *carType;

    data.AddTerminatedString(racer->Persona);
    racer->IsCustomCar = false;
    carType = const_cast<char *>(SkipFEPlayerCar);
    if (SkipFE == 0 || carType == nullptr || *carType == '\0') {
        RaceSettings *race_settings =
            FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes);
        FEPlayerCarDB *stable = &FEDatabase->CurrentUserProfiles[0]->PlayersCarStable;
        FECarRecord *record = stable->GetCarRecordByHandle(race_settings->SelectedCar[0]);
        carType = record->GetDebugName();
        FECustomizationRecord *customization =
            stable->GetCustomizationRecordByHandle(record->Customization);
        if (customization) {
            racer->IsCustomCar = true;
            racer->CarCustomization = *customization;
        }
    }

    racer->CarTypeKey = Attrib::StringToKey(carType);
    data.AddRawData(reinterpret_cast<char *>(&racer->CarTypeKey), 4);
    data.AddBool(racer->IsCustomCar);
    if (racer->IsCustomCar) {
        data.AddRawData(reinterpret_cast<char *>(&racer->CarCustomization), 0x198);
    }
}

void OnlineManager::MapRacers2PlayerIDs() {
    int playerID = 0;
    do {
        if (PersonaMap[playerID][0] != '\0') {
            int driver_num = 0;
            do {
                OnlineRacer *racer = GetOnlineRacer(driver_num);
                if (racer && bStrICmp(PersonaMap[playerID], racer->GetPersona()) == 0) {
                    racer->SetPlayerID(playerID);
                }
                ++driver_num;
            } while (driver_num < 4);
        }
        ++playerID;
    } while (playerID < 4);
    bMemSet(PersonaMap, 0, 0x40);
}

void OnlineManager::ImportPositionData(int driver_number, SmartBitStream &data, float timestamp,
                                       ePosDataPriorityMask priority_mask) {
    if (pRacers[driver_number]) {
        if (data.GetByteLengthRemaining() > 1) {
            GetOnlineRacer(driver_number)->ImportStream(data, priority_mask);
        }
    }
}

void OnlineManager::ExportPositionData(int driver_number, SmartBitStream &data,
                                       ePosDataPriorityMask priority_mask) {
    if (pRacers[driver_number]) {
        OnlineRacer *racer = GetOnlineRacer(driver_number);
        if (pCurrentWorld) {
            racer->ExportStream(data, priority_mask);
        }
    }
}

void OnlineManager::CheckGetAwayLeaderChange() {}

OnlineRacer *OnlineManager::GetServerRacer() {
    int driver_number = 0;
    OnlineRacer **racer = pRacers;
    do {
        if (*racer && GetOnlineRacer(driver_number)->IsServer()) {
            return GetOnlineRacer(driver_number);
        }
        ++driver_number;
        ++racer;
    } while (driver_number < 4);
    return nullptr;
}

void OnlineManager::PurgeDisconnectedRacers() {
    if (pLocalRacer && !pLocalRacer->IsConnected()) {
        pLocalRacer = nullptr;
    }

    int driver_number = 0;
    OnlineRacer **racer = pRacers;
    do {
        if (*racer && !GetOnlineRacer(driver_number)->IsConnected()) {
            delete *racer;
            *racer = nullptr;
            --NumRacers;
        }
        ++driver_number;
        ++racer;
    } while (driver_number < 4);
}

void OnlineManager::SignalDriverFinish(SmartBitStream &data) {
    uint32 v = 0;
    uint8 drivernum;
    data.GetBits(v, 8);
    drivernum = v;
    if (pRacers[drivernum]) {
        GetOnlineRacer(drivernum)->SignalFinish(data);
    }
}

void OnlineManager::SignalSyncAnimationMessage(SmartBitStream &) {}

void OnlineManager::SignalScoreMessage(SmartBitStream &) {}

void OnlineManager::SignalRestartLoad(int driver_num) {
    if (pRacers[driver_num]) {
        GetOnlineRacer(driver_num)->ChangeState(OPS_CONNECTED);
    }
}

void OnlineManager::NotifyDiscEjected() {
    if (pLocalRacer && pCurrentRace) {
        pLocalRacer->DriverDisconnect(OPS_DISCERROR, 0xc);
        int driver_number = 0;
        do {
            OnlineRacer *racer = GetOnlineRacer(driver_number);
            if (racer && racer != pLocalRacer) {
                racer->DriverDisconnect(OPS_DISCONNECTED, 0xc);
            }
            ++driver_number;
        } while (driver_number < 4);
        EndOnlineRace(false);
        Online::Close();
    }
}

int OnlineManager::AreAllPlayersFinishedRacing() {
    if (State == OLS_RACE_END) {
        if (pLocalRacer->IsConnected() || pLocalRacer->GetState() == OPS_QUIT) {
            return 1;
        }

    int time_finished = TimeRaceFinished.GetPackedTime();
    int finished = 0;
    if (TimeRaceFinished.IsSet() &&
        static_cast<float>(WorldTimer.GetPackedTime() - time_finished) * 0.00025f > 3.0f) {
            finished = 1;
        }
        return finished;
    }
    return State == OLS_DISCONNECTED;
}

void OnlineManager::SignalDataCRCMessage(SmartBitStream &data) {
    char driver_number;
    uint32 expected_crc;
    {
        uint32 temp = 0;
        data.GetBits(temp, 8);
        driver_number = static_cast<char>(temp);
    }
    {
        uint32 temp = 0;
        data.GetBits(temp, 32);
        expected_crc = temp;
    }

    if (pRacers[driver_number]) {
        OnlineRacer *racer = GetOnlineRacer(static_cast<int>(driver_number));
        uint32 actual_crc = racer->GetDataCRC(false);
        bool anti_cheating = false;
        if (FEDatabase->OnlineSettings.RankedGame || SkipFE) {
            anti_cheating = FrameRateIsTooLow == false;
        }
        if (anti_cheating && expected_crc != actual_crc && IsAntiCheatingEnabled() &&
            racer->CheatTally[5] < 0xfe01) {
            ++racer->CheatTally[5];
        }
    }
}

void OnlineManager::SendSyncAnimations() {
    if (NumWorldObjects) {
        SmartBitStream data;
        data.AddByte(NumWorldObjects);
        data.AddQuantizedFloat(WorldTimer.GetSeconds(), QuantFloatTime);
        if (Online::IsInitialized()) {
            Online::SignalSyncAnimationMessage(data);
        }
    }
}

bool OnlineManager::HasAnyoneCheated() {
    bool has_cheated = false;
    int driver_number = 0;
    OnlineRacer **racer = pRacers;
    for (; driver_number < 4; ++racer, ++driver_number) {
        if (*racer && GetOnlineRacer(driver_number)->GetCheatScore()) {
            has_cheated = true;
            break;
        }
    }
    return has_cheated;
}

void OnlineManager::UpdateIncoming() {
    if (Online::IsInitialized()) {
        Online::ReadIncomingPackets();
    }
}

void OnlineManager::UpdateOutgoing() {
    if (Online::IsInitialized()) {
        Online::SendUpdates();
    }
}

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

void OnlineManager::ClearAnimWorldObjects() {
    NumWorldObjects = 0;
    int i = 7;
    WorldObject **object = pAnimWorldObjects + 7;
    do {
        *object = nullptr;
        --i;
        --object;
    } while (i > -1);
}

void OnlineManager::BuildAnimWorldObjects() {
    ClearAnimWorldObjects();
}

bool OnlineManager::IsAntiCheatingEnabled() {
    bool enabled = false;
    if (FEDatabase->OnlineSettings.RankedGame || SkipFE) {
        enabled = FrameRateIsTooLow == false;
    }
    return enabled;
}

void OnlineManager::ReadyStartLine() {
    if (State == OLS_RACE_LOAD_TRACK) {
        m_ticker = bGetTicker();
        ChangeState(OLS_RACE_START_LINE);
        TheAnimPlayer.PauseAll();
        if (pCurrentWorld) {
            pCurrentWorld->BeginOnlinePause();
            OnlineTimer::ServerTimer = WorldTimer;
            BuildAnimWorldObjects();
        }
        if (Online::IsInitialized()) {
            Online::SignalReady();
        }
    }
}

void OnlineManager::StartRace() {
    TheAnimPlayer.UnPauseAll();
    RaceEndReason = OEND_RACE_FINISHED;

    OnlineRacer **racer = pRacers;
    int driver_number = 0;
    do {
        if (*racer) {
            OnlineRacer *online_racer = GetOnlineRacer(driver_number);
            eOnlineRacerState state = online_racer->State;
            bool connected = false;
            if (state != OPS_LOST_CONNECTION && state != OPS_QUIT &&
                state != OPS_DISCONNECTED) {
                connected = true;
                if (state == OPS_DISCERROR) {
                    connected = false;
                }
            }
            if (connected) {
                GetOnlineRacer(driver_number)->ChangeState(OPS_RACING);
            }
        }
        ++driver_number;
        ++racer;
    } while (driver_number < 4);

    mPostCountdownStartRaceTime = 0.0f;
    mStartTime = mMasterTime;
    ChangeState(OLS_RACING);
}
