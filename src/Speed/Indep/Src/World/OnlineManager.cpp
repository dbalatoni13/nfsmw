#include "OnlineManager.hpp"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Animation/AnimPlayer.hpp"
#include "Speed/Indep/Src/Online/OnlineCfg.hpp"

class PlatformNetworkCore {
  public:
    PlatformNetworkCore();
    virtual ~PlatformNetworkCore();

    bool IsOnline();
    void ExecNetGui();

  protected:
    bool Init();
    void Destroy();

  private:
    void LoadIRXs();
    void PrepareForExec();
    char *FilenameFromSlus();
};

#include "Speed/Indep/Src/Online/LobbyGameSessions.hpp"
#include "Speed/Indep/Src/Online/InGame/Client.hpp"
#include "Speed/Indep/Src/Online/InGame/Server.hpp"
#include "Speed/Indep/Src/Online/SmartBitstream.hpp"
#include "Speed/Indep/Src/Online/VoiceCore.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyOnlineRaceOver.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/SpeedScript.hpp"

namespace Online {
bool IsInitialized();
void Close();
void Init();
void SignalStartClockSync();
void SignalReady();
void ReadIncomingPackets();
void SendUpdates();
void SignalSyncAnimationMessage(SmartBitStream &payload_data);
    void SignalDataCRCMessage(SmartBitStream &payload_data);
    void SignalRestart();
void ShowDiagnostics();
}

extern int OnlineIsServer;
extern int OnlineEnabled;
extern int bSuperBenderConnected;
extern int NetworkUseLobbies;
extern void *pCurrentRace;
extern void FEOnlineDisconnectMsg(OnlineRacer *racer);
extern const char *SkipFEPlayerCar;
extern int ONLINE_CHEAT_NOSEND_CRC;
extern int ONLINE_CHEAT_IGNORE_TIMEUP;
extern int ONLINE_CHEAT_FALSESTART;
extern int ONLINE_SCREENPRINT_CHEAT_REPORT;
extern int DoScreenPrintf;
extern float MinCheatFreq[16];
extern int ONLINE_CHEATSCORE_THRESHOLD;
extern "C" char *TagFieldFind(const char *record, const char *name);
extern "C" int TagFieldGetString(const char *field, char *buffer, int bufferSize,
                                  const char *defaultValue);

struct IOnlinePlayer : UTL::COM::IUnknown {
  protected:
    IOnlinePlayer();

  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual void SetOnlineRacer();
    virtual OnlineRacer *GetOnlineRacer();
    virtual void Reposition();
};

struct Base64 {
    static unsigned long Encode(const void *source, unsigned long source_buflen, void *dest,
                                unsigned long dest_buflen, bool terminate);
};

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

struct CUIOnlineDisconnect {
    static bool mIsHostInGameDisconnect;
};

extern "C" int *NetGameLinkStatus(NetGameLinkRefT *pLinkRef);

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

void OnlineManager::Update(bool receive) {
    Online::ShowDiagnostics();

    uint32 time = NetworkCore::Instance().GetTime();
    uint32 elapsed = time - mLastUpdateTime;
    if (elapsed < 0x65) {
        mLastUpdateTime = time;
    } else {
        GetMasterTime();
        mLastUpdateTime = time;
    }

    bool host_disconnected = false;
    if (IsOnlineRace() && State != OLS_RACE_END && !IsServer() &&
        !CUIOnlineDisconnect::mIsHostInGameDisconnect) {
        ConnApiClientT *host = ConnectionCore::Instance().GetPlayer(0);
        if (!host) {
            host_disconnected = true;
        } else {
            host = ConnectionCore::Instance().GetPlayer(0);
            if (host->GameInfo.eStatus != CONNAPI_STATUS_ACTV) {
                host_disconnected = true;
            }
        }
    }

    if (host_disconnected) {
        if (TheGameFlowManager.GetState() == GAMEFLOW_STATE_RACING) {
            CUIOnlineDisconnect::mIsHostInGameDisconnect = true;
            cFEng::Get()->QueuePackagePush("UI_OL_Disconnect_BG.fng", 0, 0, false);
        }
    }

    if (State == OLS_RACING) {
        if (IsServer()) {
        } else {
            uint32 master = GetMasterTime();
            uint32 server = GetServerTime();
            int delta = server - master;
            if (delta < 0x65) {
                if (delta < 4) {
                    if (delta < -1) {
                        ++mMasterTime;
                    } else {
                        mMasterTime -= delta;
                    }
                } else {
                    mMasterTime -= 3;
                }
            } else {
                mMasterTime += master - server;
            }
        }
    }

    if (State > OLS_RACE_END) {
        if (receive) {
            UpdateIncoming();
        }
        return;
    }

    switch (State) {
    case OLS_DISCONNECTED:
    case OLS_RACE_DATA_SYNC:
    case OLS_RACE_LOAD_TRACK:
        break;

    case OLS_LOBBY_IN_LOBBY:
        if (NetworkUseLobbies == 0 && SkipFE &&
            static_cast<PlatformNetworkCore &>(NetworkCore::Instance()).IsOnline() == true) {
            FEDatabase->OnlineSettings.RankedGame = false;
            StartOnlineRace();
        }
        break;

    case OLS_RACE_START_LINE:
        if (!IsServer()) {
            uint32 ticker = bGetTicker();
            if (bGetTickerDifference(m_ticker, ticker) > 125.0f) {
                Client::SendStartRaceSyncMessage();
                m_ticker = ticker;
            }
        }

        if (TimeTrackLoaded.IsSet() && !mStartRaceIsSet && !RaceStartAborted()) {
            if (IsServer()) {
                if (static_cast<float>(RealTimer.GetPackedTime() -
                                       TimeTrackLoaded.GetPackedTime()) * 0.00025f >
                    120.0f) {
                    Server::DisconnectLaggers();
                    TimeTrackLoaded.UnSet();
                }
            } else if (static_cast<float>(RealTimer.GetPackedTime() -
                                          TimeTrackLoaded.GetPackedTime()) * 0.00025f >
                       140.0f) {
                Online::Close();
                TimeTrackLoaded.UnSet();
            }
        }

        if (ONLINE_CHEAT_FALSESTART == 0 &&
            GetStartRaceTime(bGetTicker()) <= 0.0f && !RaceStartAborted()) {
            StartRace();
        }
        break;

    case OLS_RACING: {
        if (TheGameFlowManager.GetState() != GAMEFLOW_STATE_RACING) {
            break;
        }

        if (GetNumConnectedRacers() == 0 && !pLocalRacer->IsFinishedRacing()) {
            pLocalRacer->DriverDisconnect(OPS_DISCONNECTED, 0xc);
        }

        bool all_finished = true;
        bool all_disconnected = true;
        const IPlayer::List &players = IPlayer::GetList(PLAYER_ALL);
        for (IPlayer::List::const_iterator iter = players.begin(); iter != players.end();
             ++iter) {
            IPlayer *player = *iter;
            IOnlinePlayer *online_player = nullptr;
            if (!player->QueryInterface(&online_player)) {
                continue;
            }

            OnlineRacer *racer = online_player->GetOnlineRacer();
            GRacerInfo *racer_info = GRaceStatus::Get().GetRacerInfo(player->GetSimable());
            if (racer_info->mFinishedRacing || racer->IsFinishedRacing()) {
                if (racer->IsConnected()) {
                    all_disconnected = false;
                }
            } else {
                all_finished = false;
                break;
            }
        }

        if (all_finished) {
            if (all_disconnected && RaceEndReason == OEND_RACE_FINISHED) {
                RaceEndReason = OEND_NO_PLAYERS;
            }
            EndOnlineRace(false);
            if (all_disconnected && !CUIOnlineDisconnect::mIsHostInGameDisconnect) {
                CUIOnlineDisconnect::mIsHostInGameDisconnect = true;
                cFEng::Get()->QueuePackagePush("UI_OL_Disconnect_BG.fng", 0, 0, false);
            }
        } else {
            if (IsServer()) {
                if (!CountdownSyncAnims.CountDown(RealTimeElapsed)) {
                    SendSyncAnimations();
                    CountdownSyncAnims = Timer(1.0f);
                }
            }
            if (!CountdownSendDataCRC.CountDown(RealTimeElapsed)) {
                SendLocalPlayerDataCRC();
                CountdownSendDataCRC = Timer(5.0f);
            }

            if (static_cast<float>(RealTimer.GetPackedTime() -
                                   LastAntiCheatRealTimer.GetPackedTime()) * 0.00025f >
                2.0f) {
                CheckWorldTimerHacking();
                LastAntiCheatRealTimer = RealTimer;
            }

            if (IsServer() && TheRaceParameters.RaceType != RACE_TYPE_GET_AWAY) {
                CheckGetAwayLeaderChange();
            }
        }

        if (pLocalRacer->IsConnected()) {
            pLocalRacer->UpdateLocal(WorldTimeElapsed);
        }

        if ((FEDatabase->OnlineSettings.RankedGame || SkipFE) && !FrameRateIsTooLow &&
            pLocalRacer->GetEndRaceCountdown() < -3.0f) {
            for (int i = 0; i < 4; ++i) {
                OnlineRacer *racer = GetOnlineRacer(i);
                if (racer && !racer->IsFinishedRacing()) {
                    if (IsAntiCheatingEnabled() && racer->CheatTally[5] < 0xfe00) {
                        ++racer->CheatTally[5];
                    }
                    if (racer != pLocalRacer && ONLINE_CHEAT_IGNORE_TIMEUP == 0) {
                        racer->DriverDisconnect(OPS_FINISHED, 0xb);
                    }
                }
            }
        }

        if (ONLINE_SCREENPRINT_CHEAT_REPORT && HasAnyoneCheated()) {
            DoScreenPrintf = 1;
            PrintCheatTallies(true);
        }
        break;
    }

    case OLS_RACE_END:
        if (!TimeRaceFinished.IsSet()) {
            RaceTimeup = false;
            TimeRaceFinished = WorldTimer;
        }
        PrintCheatTallies(true);
        break;

    default:
        break;
    }

    if (receive) {
        UpdateIncoming();
    }
}

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

void OnlineManager::InitAntiCheating(int num_players) {
    if (NetworkUseLobbies != 0) {
        char *server_config = LobbyCore::Instance().GetServerConfig();
        if (server_config) {
            char buff1[128];
            char buff2[255];

            TagFieldGetString(TagFieldFind(server_config, "RESPONSE_GRAPH"), buff1, 0x80, "");
            bSPrintf(buff2, "%s: %s", "GRAPH", buff1);
            SpeedScript parse("GRAPH_SCRIPT", buff2, 0);
            if (parse.GetNextCommand()) {
                int i = 0;
                float *frequency = MinCheatFreq;
                while (i < 16) {
                    bool has_argument = parse.IsAnotherArgument();
                    if (!has_argument) {
                        break;
                    }
                    ++i;
                    *frequency = parse.GetNextArgumentFloat();
                    ++frequency;
                }
            }

            TagFieldGetString(TagFieldFind(server_config, "MIN_CHEAT_THRESHOLD"), buff1,
                              0x80, "51");
            ONLINE_CHEATSCORE_THRESHOLD = bStrToLong(buff1);
            bSPrintf(buff2, "RESPONSE_REMAP%d", num_players);
            TagFieldGetString(TagFieldFind(server_config, buff2), buff1, 0x80, "");
            if (bStrLen(buff1) > 0) {
                bSPrintf(buff2, "%s: %s", "REMAP", buff1);
                SpeedScript parse("REMAP_SCRIPT", buff2, 0);
                if (parse.GetNextCommand()) {
                    int i = 0;
                    float *frequency = Client::m_sendCarFrequencyHz;
                    while (i < 4) {
                        bool has_argument = parse.IsAnotherArgument();
                        if (!has_argument) {
                            break;
                        }
                        ++i;
                        *frequency = static_cast<float>(parse.GetNextArgumentInt());
                        ++frequency;
                    }
                }
            }
        }
    }
}

void OnlineManager::SendEndOfRaceResults() {
    GRace::Type ugmMode;
    int8 race_mode;
    ugmMode = FEDatabase->OnlineSettings.RaceMode;
    race_mode = -1;
    if (ugmMode == GRace::kRaceType_Circuit) {
        race_mode = 0;
    } else if (ugmMode == GRace::kRaceType_P2P) {
        race_mode = 1;
    } else if (ugmMode == GRace::kRaceType_Drag) {
        race_mode = 2;
    }

    SmartBitStream data;
    data.AddByte(3);
    data.AddByte(GetNumRacers());
    data.AddByte(race_mode);
    data.AddByte(pLocalRacer->PlayerID);
    data.AddShort(static_cast<short>(TheRaceParameters.TrackNumber));
    data.AddByte(TheRaceParameters.TrackDirection);
    data.AddByte(TheRaceParameters.NumLapsInRace);

    const IPlayer::List &players = IPlayer::GetList(PLAYER_ALL);
    for (IPlayer::List::const_iterator iter = players.begin(); iter != players.end(); ++iter) {
        IPlayer *player = *iter;
        IOnlinePlayer *online_player = nullptr;
        if (!player->QueryInterface(&online_player)) {
            break;
        }

        OnlineRacer *racer = online_player->GetOnlineRacer();
        GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(player->GetSimable());
        FinishedRaceStatsEntry local_frs;
        FinishedRaceStatsEntry &frs = racer->FinishedRaceStats;

        data.AddByte(racer->PlayerID);
        data.AddByte(static_cast<uint8>(racerInfo->mRanking));
        data.AddByte(static_cast<uint8>(frs.FinishReason));
        data.AddByte(0);
        data.AddFloat(static_cast<float>(racerInfo->mLapsCompleted));
        data.AddFloat(racerInfo->mRaceTimer.GetTime());
        if (racerInfo->mLapsCompleted != 0) {
            data.AddFloat(GRaceStatus::Get().GetBestLapTime(racerInfo->mIndex));
        } else {
            data.AddFloat(0.0f);
        }
        data.AddFloat(racerInfo->CalcAverageSpeed());
        data.AddFloat(racerInfo->mTopSpeed);
        data.AddFloat(racerInfo->mZeroToSixtyTime);
        data.AddFloat(racerInfo->mQuarterMileTime);
        data.AddFloat(frs.QuarterMileSpeed);
        data.AddFloat(frs.LongestJump);
        data.AddFloat(0.0f);
        data.AddFloat(racerInfo->mPoundsNOSUsed);

        uint8 lostconnection;
        if (racer->State == OPS_DISCERROR) {
            lostconnection = 3;
        } else if (racer->State < OPS_QUIT) {
            lostconnection = 0;
            if (racer->State == OPS_LOST_CONNECTION) {
                lostconnection = 1;
            }
        } else {
            lostconnection = 0;
            if (racer->State == OPS_QUIT) {
                lostconnection = 2;
            }
        }
        data.AddByte(lostconnection);

        uint16 cheat_bitmask = 0;
        for (int i = 0; i < 16; ++i) {
            if (racer->CheatTally[i] != 0) {
                cheat_bitmask |= static_cast<uint16>(1 << i);
            }
        }
        data.AddShort(static_cast<short>(cheat_bitmask));
        for (int i = 15; i >= 0; --i) {
            if (racer->CheatTally[15 - i] != 0) {
                data.AddShort(static_cast<short>(racer->CheatTally[15 - i]));
            }
        }
    }

    float byte_length = bCeil(static_cast<float>(data.GetBitLength()) /
                              static_cast<float>(BitStream::BIT_DEPTH));
    char b64encoded[1024];
    if (NetworkUseLobbies != 0) {
        Base64::Encode(&data, static_cast<unsigned long>(static_cast<int>(byte_length)), b64encoded,
                       0x400, true);
        LobbyCore::Instance().SaveRaceResults(b64encoded);
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

void OnlineManager::EndOnlineRace(bool bForced) {
    MNotifyOnlineRaceOver(false).Post(UCrc32(0x20d60dbf));

    if (pCurrentWorld) {
        pCurrentWorld->EndOnlinePause();
    }

    if (State == OLS_RACING || State == OLS_RACE_LOAD_TRACK || State == OLS_RACE_START_LINE) {
        if (bForced) {
            if (pLocalRacer) {
                pLocalRacer->DriverDisconnect(OPS_QUIT, 0xc);
            }
            for (int i = 0; i < 4; ++i) {
                if (pRacers[i] && GetOnlineRacer(i) != pLocalRacer) {
                    OnlineRacer *racer = GetOnlineRacer(i);
                    if (!racer->IsFinishedRacing()) {
                        GetOnlineRacer(i)->DriverDisconnect(OPS_DISCONNECTED, 0xc);
                    }
                }
            }
        }

        for (int i = 0; i < 4; ++i) {
            if (pRacers[i]) {
                OnlineRacer *racer = GetOnlineRacer(i);
                bool connected = racer->IsConnected();
                racer->bShouldRestart = connected;
            }
        }

        if (IsServer()) {
            int num_disc_at_current_time = 0;
            int index = -1;
            for (int i = 0; i < 4; ++i) {
                if (pRacers[i]) {
                    OnlineRacer *racer = GetOnlineRacer(i);
                    if (racer && racer != pLocalRacer &&
                        bAbs(racer->DisconnectTime.GetSeconds() - WorldTimer.GetSeconds()) <= 0.3f) {
                        ++num_disc_at_current_time;
                        if (num_disc_at_current_time > 1) {
                            racer->ChangeState(OPS_DISCONNECTED);
                            if (index > -1) {
                                GetOnlineRacer(index)->ChangeState(OPS_DISCONNECTED);
                                index = -1;
                            }
                        } else {
                            index = i;
                        }
                    }
                }
            }
            if (num_disc_at_current_time > 1) {
                pLocalRacer->ChangeState(OPS_LOST_CONNECTION);
            }
        }

        CalculateFinishOrder();
        SendEndOfRaceResults();
        ClearAnimWorldObjects();
        PrintCheatTallies(false);
        PrintQuantizersUsageReport();
    }

    if (bForced || State == OLS_RACE_END) {
        if (GetRestartingRace()) {
            bRandom(2, &GameSeed);
            SetupRestartRace();
            ChangeState(OLS_RACE_LOAD_TRACK);
            Online::SignalRestart();
        } else {
            Disconnect(false);
        }
    } else {
        ChangeState(OLS_RACE_END);
    }
}

int OnlineManager::GetNumConnectedRacers() {
    int num_connected = 0;
    int driver_number = 0;
    OnlineRacer **racer = pRacers;
    do {
        if (*racer && GetOnlineRacer(driver_number) != pLocalRacer) {
            OnlineRacer *online_racer = GetOnlineRacer(driver_number);
            int connected = 0;
            eOnlineRacerState state = online_racer->State;
            if (state != OPS_LOST_CONNECTION) {
                if (state == OPS_QUIT) {
                    ++driver_number;
                    ++racer;
                    continue;
                }
                if (state != OPS_DISCONNECTED) {
                    connected = state != OPS_DISCERROR;
                }
            }
            num_connected += connected;
        }
        ++driver_number;
        ++racer;
    } while (driver_number <= 3);
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

void OnlineManager::EndSimFrame() {
    uint32 time = NetworkCore::Instance().GetTime();
    if (time - mLastUpdateTime >= 0x65) {
        if (!bHasStalled) {
            GetMasterTime();
        }
        bHasStalled = true;
    } else {
        bHasStalled = false;
    }

    if (IsServer()) {
        LobbyGameSessions::Instance().SetSessionLatency(0);
    } else {
        ConnApiClientT *player = ConnectionCore::Instance().GetPlayer(0);
        if (!player) {
            LobbyGameSessions::Instance().SetSessionLatency(-1);
        } else {
            player = ConnectionCore::Instance().GetPlayer(0);
            if (player->GameInfo.eStatus != CONNAPI_STATUS_ACTV) {
                LobbyGameSessions::Instance().SetSessionLatency(-1);
            } else {
                LobbyGameSessions *sessions = &LobbyGameSessions::Instance();
                player = ConnectionCore::Instance().GetPlayer(0);
                int *status = NetGameLinkStatus(player->pGameLinkRef);
                sessions->SetSessionLatency(status[3]);
            }
        }
    }
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

void OnlineManager::DriverLeft(int driver_number, bool he_quit) {
    if (pRacers[driver_number]) {
        OnlineRacer *racer = GetOnlineRacer(driver_number);
        racer->DisconnectTime.SetTime(WorldTimer.GetSeconds());

        if (State == OLS_RACE_DATA_SYNC) {
            racer->ChangeState(OPS_DISCONNECTED);
            TheRaceParameters.RemoveDriverInfo(racer->DriverNumber);
            if (racer->IsServer()) {
                for (int i = 0; i < 4; ++i) {
                    if (pRacers[i] && GetOnlineRacer(i) != racer) {
                        OnlineRacer *other = GetOnlineRacer(i);
                        other->ChangeState(OPS_DISCONNECTED);
                        other = GetOnlineRacer(i);
                        TheRaceParameters.RemoveDriverInfo(other->DriverNumber);
                    }
                }
            }
        } else if (!pCurrentRace) {
            eOnlineRacerState state = he_quit ? OPS_QUIT : OPS_LOST_CONNECTION;
            racer->ChangeState(state);
            if (racer->IsServer()) {
                for (int i = 0; i < 4; ++i) {
                    if (pRacers[i] && GetOnlineRacer(i) != racer) {
                        GetOnlineRacer(i)->ChangeState(OPS_DISCONNECTED);
                    }
                }
            }
        } else {
            FEOnlineDisconnectMsg(racer);
            if (State == OLS_RACE_END) {
                racer->DriverDisconnect(OPS_DISCONNECTED, 0xc);
            } else {
                eOnlineRacerState state = he_quit ? OPS_QUIT : OPS_LOST_CONNECTION;
                racer->DriverDisconnect(state, 0xc);
            }

            if (racer->IsServer()) {
                for (int i = 0; i < 4; ++i) {
                    if (pRacers[i] && GetOnlineRacer(i) != racer) {
                        GetOnlineRacer(i)->DriverDisconnect(OPS_DISCONNECTED, 0xc);
                    }
                }
            }
        }
    }
}

void OnlineManager::CreateOnlineRacer(int driver_num, SmartBitStream *pdata, bool is_server,
                                      const char *persona) {
    PurgeDisconnectedRacers();
    if (pRacers[driver_num] == nullptr) {
        OnlineRacer *racer = new ("OnlineRacer", 0, 8)
            OnlineRacer(static_cast<int8>(driver_num), is_server, persona);
        pRacers[driver_num] = racer;
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
    volatile uint8 drivernum;
    uint32 v = 0;
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

bool OnlineManager::FinishDriver(int driver_number, int nRank, bool bBlinkBlinkPoof,
                                 int raceFinishReason) {
    GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(driver_number);
    OnlineRacer *racer = GetOnlineRacer(racerInfo.GetName());
    bool finished = racer->IsFinishedRacing();

    if (!finished && racer == pLocalRacer) {
        OnlineManager &manager = TheOnlineManager;
        bool anti_cheating = false;
        if (FEDatabase->OnlineSettings.RankedGame || SkipFE) {
            anti_cheating = manager.FrameRateIsTooLow == false;
        }
        if (anti_cheating && raceFinishReason == 1 &&
            (TheRaceParameters.IsDriftRace() || TheRaceParameters.bDragRaceFlag)) {
        }

        if (raceFinishReason != 0xb || ONLINE_CHEAT_IGNORE_TIMEUP == 0) {
            racer->Finish(nRank, bBlinkBlinkPoof, raceFinishReason);
            finished = true;

            if (raceFinishReason == 0xb && racer->CheatTally[2] != 0) {
                for (int i = 0; i < 4; ++i) {
                    OnlineRacer *other_racer = GetOnlineRacer(i);
                    if (other_racer && other_racer != racer) {
                        other_racer->DriverDisconnect(OPS_DISCONNECTED, 0xc);
                    }
                }
            }
        }
    }

    if (raceFinishReason == 1 && nRank == 1 && !TimeupStartTime.IsSet()) {
        TimeupStartTime.SetTime(mPostCountdownStartRaceTime + racerInfo.GetRaceTime());
        TimeupLength = 30.0f;
    }
    return finished;
}

void OnlineManager::CalculateFinishOrder() {
    int num_racers = 0;
    OnlineRacer *racers[4];
    for (int i = 0; i < 4; ++i) {
        if (pRacers[i]) {
            racers[num_racers++] = GetOnlineRacer(i);
        }
    }

    bool swap;
    RaceParameters &race_parameters = TheRaceParameters;
    while (true) {
        int j = 0;
        swap = false;
        if (num_racers - 1 > 0) {
            do {
                OnlineRacer *racer[2];
                int sort_score[2];
                FinishedRaceStatsEntry *frs[2];

                racer[0] = racers[j];
                racer[1] = racers[j + 1];
                bMemSet(sort_score, 0, 8);
                frs[1] = racer[1]->GetFinishedRaceStats();
                frs[0] = racer[0]->GetFinishedRaceStats();

                for (int i = 0; i < 2; ++i) {
                    if (frs[i]->FinishReason == 0xc) {
                        sort_score[i] += 2;
                    } else if (frs[i]->FinishReason != 1) {
                        sort_score[i] += 1;
                    }
                }

                if (sort_score[0] == sort_score[1]) {
                    int behind_index = -1;
                    if (!race_parameters.IsDriftRace()) {
                        if (sort_score[0] == 0) {
                            if (frs[0]->RaceTime.GetPackedTime() !=
                                frs[1]->RaceTime.GetPackedTime()) {
                                if (frs[0]->RaceTime.GetPackedTime() <
                                    frs[1]->RaceTime.GetPackedTime()) {
                                    behind_index = 1;
                                } else {
                                    behind_index = 0;
                                }
                            }
                        } else if (frs[0]->NumLapsCompletedExact !=
                                   frs[1]->NumLapsCompletedExact) {
                            behind_index = frs[1]->NumLapsCompletedExact <
                                            frs[0]->NumLapsCompletedExact ?
                                        1 : 0;
                        }
                    }
                    if (behind_index > -1) {
                        ++sort_score[behind_index];
                    }
                }

                if (sort_score[1] < sort_score[0]) {
                    racers[j] = racer[1];
                    swap = true;
                    racers[j + 1] = racer[0];
                }
                ++j;
            } while (j < num_racers - 1);
        }
        if (!swap) {
            break;
        }
    }

    for (int i = 0; i < num_racers; ++i) {
        OnlineRacer *racer = racers[i];
        racer->FinishedRaceStats.FinishPosition = i + 1;
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

void OnlineManager::CheckWorldTimerHacking() {
    OnlineManager &manager = TheOnlineManager;
    bool anti_cheating = false;
    if (FEDatabase->OnlineSettings.RankedGame || SkipFE) {
        anti_cheating = manager.FrameRateIsTooLow == false;
    }
    if (anti_cheating) {
        int last_real = LastAntiCheatRealTimer.GetPackedTime();
        if (LastAntiCheatRealTimer.IsSet()) {
            float result =
                static_cast<float>(WorldTimer.GetPackedTime() - LastAntiCheatWorldTimer.GetPackedTime()) *
                    0.00025f -
                static_cast<float>(RealTimer.GetPackedTime() - last_real) * 0.00025f;
            result = bAbs(result);
            if (result > 0.5f) {
                OnlineRacer *server = GetServerRacer();
                if (server && manager.IsAntiCheatingEnabled() && server->CheatTally[1] < 0xfe01) {
                    ++server->CheatTally[1];
                }
            }
        }
        LastAntiCheatRealTimer = RealTimer;
        LastAntiCheatWorldTimer = WorldTimer;
    }
}

void OnlineManager::SendLocalPlayerDataCRC() {
    if (pLocalRacer && ONLINE_CHEAT_NOSEND_CRC == 0) {
        SmartBitStream data;
        data.AddByte(pLocalRacer->GetDriverNumber());
        data.AddInt(pLocalRacer->GetDataCRC(true));
        if (Online::IsInitialized()) {
            Online::SignalDataCRCMessage(data);
        }

        OnlineManager &manager = TheOnlineManager;
        bool anti_cheating = false;
        if (FEDatabase->OnlineSettings.RankedGame || SkipFE) {
            anti_cheating = manager.FrameRateIsTooLow == false;
        }
        if (anti_cheating && pLocalRacer->GetDataCRC(false) != SavedLocalPhysicsCRC) {
            OnlineRacer *racer = pLocalRacer;
            if (manager.IsAntiCheatingEnabled() && racer->CheatTally[5] < 0xfe01) {
                ++racer->CheatTally[5];
            }
        }
    }
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

    const int lost_connection = OPS_LOST_CONNECTION;
    const int quit = OPS_QUIT;
    const int racer_count = 4;
    int i = 0;
    do {
        if (pRacers[i]) {
            OnlineRacer *online_racer = GetOnlineRacer(i);
            eOnlineRacerState state = online_racer->State;
            bool connected = false;
            if (state != lost_connection) {
                if (state != quit) {
                    if (state != OPS_DISCONNECTED) {
                        connected = true;
                        if (state == OPS_DISCERROR) {
                            connected = false;
                        }
                    }
                }
            }
            if (connected) {
                GetOnlineRacer(i)->ChangeState(OPS_RACING);
            }
        }
        ++i;
    } while (i < racer_count);

    mPostCountdownStartRaceTime = 0.0f;
    mStartTime = mMasterTime;
    ChangeState(OLS_RACING);
}
