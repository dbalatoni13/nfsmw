#ifndef ONLINE_MANAGER_HPP
#define ONLINE_MANAGER_HPP

// TODO check #if ONLINE_PLATFORM instead
#if defined(EA_PLATFORM_GAMECUBE)
#elif defined(EA_PLATFORM_XENON)
#define ONLINE_ENABLED
#elif defined(EA_PLATFORM_PLAYSTATION2)
#define ONLINE_ENABLED
#endif

#include <types.h>

enum eOnlineState {
    OLS_DISCONNECTED = 0,
    OLS_LOBBY_IN_LOBBY = 1,
    OLS_RACE_DATA_SYNC = 2,
    OLS_RACE_LOAD_TRACK = 3,
    OLS_RACE_START_LINE = 4,
    OLS_RACING = 5,
    OLS_RACE_END = 6,
    NUM_OLS_STATES = 7,
};

class OnlineManager {
  public:
    void InitForRace();

    void UninitForRace();

    void Disconnect(bool force);

    void Update(bool receive);

    float SyncWorldTimestep(float timestep);

    bool IsOnline();

    bool IsServer();

    uint32 GetMasterTime();

    uint32 GetServerTime();

    void DriverLeft(int driver_number, bool he_quit);

    void SetServerTime(uint32 time);

    void SignalRestartLoad(int driver_num);

    void SetStartRaceTime(uint32 tick, float time, float ping);

    float GetStartRaceTime(uint32 tick);

    bool GetRestartingRace();

    void RequestRestart();

    void SignalLoad();

    void PrintQuantizersUsageReport();

    void PrintCheatTallies(bool to_screen);

    void StartLobby();

    void StartOnlineRace();

    void ReadyStartLine();

    int AreAllPlayersFinishedRacing();

    void NotifyDiscEjected();

    void SendSyncAnimations();

    void InitAntiCheating(int num_players);

    bool HasAnyoneCheated();

    int GetGetAwayLeaderDriverNumber();

    Timer GetGetAwayFinishTime(int driver_num);

    Timer GetGetAwayLeaderTime(int driver_num);

    bool GetGetAwayRaceTimedOut();

    void SetPlayerIDs(const char (*personas)[16]);

    int GetDrift();

    bool RaceStartTimeSet();

    void SetPostCountdownStartRaceTime(float time);

    void SetRestartingRace(bool r);

    Timer GetRaceRestartTimer();

    void RestartLastRace();

    void RejectRestartRequest();

    bool IsRestartRequested();

    bool CanRestartRace();

    bool RaceStartAborted();

    bool IsAntiCheatingEnabled();

    void SetRaceEndDisconnect();

    void SetRaceTimeup();

    bool IsRaceTimeup();

    void UpdateOutgoing();

    void SetPosUpdatePing(float ping);

    void SetupRaceParams();

    void SetupLocalDriver(int driver_num);

    void ChangeState(eOnlineState new_state);

    void MapRacers2PlayerIDs();

    void StartRace();

    void PurgeDisconnectedRacers();

    int GetNumConnectedRacers();

    bool IsRaceOver();

    void FinalizeCheatDetection();

    void CalculateFinishOrder();

    void SendEndOfRaceResults();

    void ClearAnimWorldObjects();

    void BuildAnimWorldObjects();

    void SyncRunningAnimations(float delta_t, bool resetfirst);

    void SendLocalPlayerDataCRC();

    void CheckWorldTimerHacking();

    void CheckGetAwayLeaderChange();

    void SetupStartingPositions();

    void SetupRestartRace();

    void FinishGetAwayRace();

    void SelfDisconnect();

    Timer &GetTimeupStartTime();

    float GetTimeupLength();

    void UpdateIncoming();

    int GetNumRacers();

    void StartSimFrame();
    void InitQuantizers();

    OnlineManager() {
        InitQuantizers();
    }

    void EndSimFrame() {}

    void Initialize(int argc, char **argv) {}

    bool IsOnlineRace() {
#ifndef ONLINE_ENABLED
        return false;
#else
        // TODO
        return false;
#endif
    }

    void EndOnlineRace(bool bForced) {
        // TODO
    }

    void TrackLoaded() {
        // TODO
    }

    // TODO
    eOnlineState GetState() {
        return OLS_DISCONNECTED;
    }
};

extern OnlineManager TheOnlineManager;

#endif
