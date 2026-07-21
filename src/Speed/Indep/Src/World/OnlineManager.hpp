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

class SmartBitStream;
struct Client;
struct OnlineRacer;

enum ePosDataPriorityMask {
    PDP_MASK_NONE = 0,
    PDP_MASK_CRITICAL = 1,
    PDP_MASK_NORMAL = 2,
    PDP_MASK_LOW = 4,
    PDP_MASK_AESTHETIC = 8,
    PDP_MASK_INFLIGHT = 32,
    PDP_MASK_ALL = 47
};

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
    void StartSimFrame();
    void InitQuantizers();
    void Disconnect(bool force);
    void Update(bool networkThread);
    uint32 GetMasterTime();
    void DriverLeft(int driver_number, bool he_quit);
    void ExportPositionData(int driver_number, SmartBitStream &bitstream_data,
                            ePosDataPriorityMask priority_mask);
    OnlineRacer *GetOnlineRacer(int driver_number);
    void ImportPositionData(int driver_number, SmartBitStream &bitstream_data, float timestamp,
                            ePosDataPriorityMask priority_mask);
    void SetServerTime(uint32 time);
    void SignalScoreMessage(SmartBitStream &bitstream_data);
    void SignalSyncAnimationMessage(SmartBitStream &bitstream_data);
    void SignalDataCRCMessage(SmartBitStream &bitstream_data);
    void SignalGetAwayLeaderMessage(SmartBitStream &bitstream_data);
    void SignalGetAwayLostMessage(SmartBitStream &bitstream_data);
    void SignalDriverFinish(SmartBitStream &bitstream_data);
    void SignalRestartLoad(int driver_num);
    void ImportRaceParams(int driver_number, int server_driver_number,
                          SmartBitStream &bitstream_data);
    void ImportDriverInfo(int driver_number, SmartBitStream &bitstream_data);
    void ExportDriverInfo(int driver_number, SmartBitStream &bitstream_data);
    void SetStartRaceTime(uint32 tick, float time, float ping);
    bool GetRestartingRace();
    void RequestRestart();
    void SignalLoad();

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

  protected:
    eOnlineState State;
    OnlineRacer *pLocalRacer;
    OnlineRacer *pRacers[4];
    uint8 NumRacers;
    bool bOnlineRace;
    bool bHasStalled;
    uint32 mLastUpdateTime;
    uint32 mMasterTime;
    uint32 mStartTime;
    uint32 mServerTime;
    bool mStartRaceIsSet;
    uint32 mStartRaceTick;
    float mStartRaceTime;
    float mPostCountdownStartRaceTime;
    float mStartRacePing;
    float mPosUpdatePing;

    friend struct Client;
};

extern OnlineManager TheOnlineManager;

#endif
