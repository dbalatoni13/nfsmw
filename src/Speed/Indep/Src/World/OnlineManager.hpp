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

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Online/FloatQuantizer.hpp"
#include "Speed/Indep/Src/Online/IntQuantizer.hpp"
#include "Speed/Indep/Src/Online/NetworkMutex.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

class SmartBitStream;
struct Client;
struct WorldObject;
class IVehicle;

enum eOnlineRacerState {
    OPS_DISCONNECTED = 0,
    OPS_CONNECTED = 1,
    OPS_RACING = 2,
    OPS_FINISHED = 3,
    OPS_LOST_CONNECTION = 4,
    OPS_DISCERROR = 5,
    OPS_QUIT = 6,
    NUM_OPS_STATES = 7
};

enum eOnlineRaceEndReason {
    OEND_RACE_FINISHED = 0,
    OEND_NO_PLAYERS = 1,
    OEND_DISCONNECTED = 2,
    OEND_REJECTED = 3
};

enum ePosDataPriorityMask {
    PDP_MASK_NONE = 0,
    PDP_MASK_CRITICAL = 1,
    PDP_MASK_NORMAL = 2,
    PDP_MASK_LOW = 4,
    PDP_MASK_AESTHETIC = 8,
    PDP_MASK_INFLIGHT = 32,
    PDP_MASK_ALL = 47
};

enum ePosDataPriority {
    PDP_UNSPECIFIED = -1,
    PDP_CRITICAL = 0,
    PDP_NORMAL = 1,
    PDP_LOW = 2,
    PDP_AESTHETIC = 3,
    NUM_POSDATA_PRIORITIES = 4,
    PDP_INFLIGHT = 5
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

struct ExtrapolatedCar : Debugable {
    struct ALIGN_16 State {
      private:
        UMath::Vector3 mPosition;
        float mSteering;
        UMath::Vector3 mLinearVelocity;
        float mGas;
        UMath::Vector3 mLinearAcceleration;
        float mBrake;
        UMath::Vector4 mRotation;
        UMath::Vector4 mAngularVelocity;
        UMath::Vector4 mAngularAcceleration;
        float mHandBrake;
        float mBlendRate;
        float mBlend;
        float mTime;
        bool mInFlight;
        bool mNOS;
        uint8 mGear;
        uint8 mPriority;

      public:
        friend struct ExtrapolatedCar;
        State();
        void Import(const ISimable *simable, float simtime);
        void Export(SmartBitStream &data, ePosDataPriorityMask priority_mask,
                    uint8 repositioncount);
        uint8 Import(float time, SmartBitStream &data, ePosDataPriorityMask priority_mask);
        void Extrapolate(float simtime);
        bool Blend(State &blended, float t);
        bool IsBlending() const;
        void Export(ISimable *simable) const;
        void SetOnGround(IVehicle *vehicle);
        bool IsValidPosition();
        float SquaredDistanceTo(State &target) const;
        float GetTime() const { return mTime; }
        UMath::Vector3 &GetPosition();
        void ExtractDirection(UMath::Vector3 &direction) const;
        ISimable *SpawnVehicle(Attrib::Key cartype);
        bool HasPriority() const { return (mPriority & 4) != 0; }
    };

    ExtrapolatedCar(Attrib::Key cartype);
    ~ExtrapolatedCar();

    bool IsStateListEmpty() const;
    void ExportSimable(ISimable *simable);
    void SaveToStream(SmartBitStream &data, ePosDataPriorityMask priority_mask);
    void ExportStream(SmartBitStream &data, ePosDataPriorityMask priority_mask);
    void ImportStream(SmartBitStream &data, ePosDataPriorityMask priority_mask);
    void ImportSimable(ISimable *simable, float t, float simtime);
    bool WantsDriverAI() const;
    void ExtractExtrapolatedPosition(UMath::Vector3 &position) const;
    void ExtractExtrapolatedDirection(UMath::Vector3 &direction) const;
    void Reset(ISimable *simable);
    float GetLastTime() const;
    float GetLatency() const;
    float GetPercentReceived() const;
    void IncreaseRepositionCount();
    void Pause();
    void UnPause();
    ISimable *SpawnVehicle();
    bool HasHeadset() const;
    bool IsAbleToSee(ExtrapolatedCar &target);
    Attrib::Key &GetCarType() { return mCarType; }

  private:
    struct CopMap;
    State mStateArray[32];
    State mSaved;
    State mBlended;
    Attrib::Key mCarType;
    CopMap *mCops;
    ALIGN_16 UMath::Vector3 mLastPosition;
    State *mLast;
    int mHead;
    int mTail;
    NetworkMutex *mMutex;
    float mCollisionTime;
    uint32 mUpdateTime;
    bool mActive;
    uint8 mRepositionCount;
    bool mHasHeadset;
    bool mPaused;
    int mUseDriverAI;

    static int Next(int c) {
        int n = c + 1 > 31 ? 0 : c + 1;
        return n;
    }
};

struct ExtrapolatedCar::CopMap : public UTL::Std::map<HSIMABLE, ExtrapolatedCar *, _type_map> {
    USE_FASTALLOC(CopMap)
    ~CopMap();
};

struct OnlineRacer : ExtrapolatedCar {
    friend class OnlineManager;

    OnlineRacer(int8 driver_number, bool is_server, const char *persona);
    ~OnlineRacer();
    static void operator delete(void *mem);

    int8 GetDriverNumber() const { return DriverNumber; }
    eOnlineRacerState GetState() { return State; }
    int8 GetPlayerID() const { return PlayerID; }
    void SetPlayerID(int8 player_id) { PlayerID = player_id; }
    char *GetPersona() { return Persona; }
    int GetRaceScore() const { return RaceScore; }
    void SetRaceScore(int score);
    bool IsConnected() {
        bool connected = false;
        if (GetState() != OPS_LOST_CONNECTION) {
            if (GetState() != OPS_QUIT) {
                if (GetState() != OPS_DISCONNECTED) {
                    connected = true;
                    if (GetState() == OPS_DISCERROR) {
                        connected = false;
                    }
                }
            }
        }
        return connected;
    }
    bool IsServer() const { return bIsServer; }
    bool IsFinishedRacing();
    bool IsSameSideOfRestart();
    float GetBadnessCountdown(int *r_reason);
    float GetEndRaceCountdown();
    FinishedRaceStatsEntry *GetFinishedRaceStats() { return &FinishedRaceStats; }
    uint16 GetCheatTally();
    uint8 GetCheatScore();
    bool IsCheating();

    void SetLastSpamRealTime(float time) { LastSpamRealTime = time; }
    float GetLastSpamRealTime() const { return LastSpamRealTime; }
    void ChangeState(eOnlineRacerState new_state);
    void SetPersona(const char *persona);
    void UpdateLocal(float t);
    void Finish(int nRank, bool bBlinkBlinkPoof, int raceFinishReason);
    void SignalFinish(SmartBitStream &data);
    void UpdateEndRaceStats();
    void DriverDisconnect(eOnlineRacerState new_state, int finish_reason);
    void SetDisconnectTime(float time) { DisconnectTime.SetTime(time); }
    float GetDisconnectTime() { return DisconnectTime.GetSeconds(); }
    void ClearCheatInfo();
    void DetectedCheat(int reason) { BadnessReason = reason; }
    uint32 GetDataCRC(bool recalc);

  protected:
    Attrib::Key CarTypeKey;
    bool IsCustomCar;
    FECustomizationRecord CarCustomization;
    eOnlineRacerState State;
    int8 DriverNumber;
    int8 PlayerID;
    uint32 Reputation;
    int RaceScore;
    bool bIsServer;
    bool bShouldRestart;
    char Persona[16];
    FinishedRaceStatsEntry FinishedRaceStats;
    uint32 PhysicsDataCRC;
    uint16 CheatTally[16];
    float LastSpamRealTime;
    uint32 SyncScoreMsgID;
    Timer GraceCountdown;
    Timer BadnessCountdown;
    Timer DisconnectTime;
    int BadnessReason;
    float EndRaceCountdown;
};

class OnlineManager {
  public:
    friend struct OnlineRacer;

    OnlineManager();
    ~OnlineManager();
    void Initialize(int argc, char **argv);
    void InitForRace();
    void UninitForRace();
    void Disconnect(bool force);
    void Update(bool receive);
    float SyncWorldTimestep(float timestep);
    void StartSimFrame();
    void InitQuantizers();
    void EndSimFrame();
    eOnlineState GetState() { return State; }
    bool IsOnlineRace();
    bool IsOnline();
    bool IsServer();
    uint32 GetMasterTime();
    uint32 GetServerTime();
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
    void ExportRaceParams(int driver_number, SmartBitStream &bitstream_data);
    void SetStartRaceTime(uint32 tick, float time, float ping);
    float GetStartRaceTime(uint32 tick);
    bool GetRestartingRace();
    void RequestRestart();
    void SignalLoad();

    void PrintQuantizersUsageReport();
    void PrintCheatTallies(bool to_screen);
    void StartLobby();
    void StartOnlineRace();
    void EndOnlineRace(bool bForced);
    void TrackLoaded();
    void ReadyStartLine();
    int AreAllPlayersFinishedRacing();
    void NotifyDiscEjected();
    bool FinishDriver(int driver_number, int nRank, bool bBlinkBlinkPoof,
                      int raceFinishReason);
    void SendSyncAnimations();
    void InitAntiCheating(int num_players);
    bool HasAnyoneCheated();
    int GetGetAwayLeaderDriverNumber();
    Timer GetGetAwayFinishTime(int driver_num);
    Timer GetGetAwayLeaderTime(int driver_num);
    bool GetGetAwayRaceTimedOut();
    void SetPlayerIDs(const char (*personas)[16]);
    void SetGameSeed(unsigned int seed) { GameSeed = seed; }
    unsigned int GetGameSeed() const { return GameSeed; }
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
    eOnlineRaceEndReason GetRaceEndReason();
    void SetRaceEndDisconnect();
    void SetRaceTimeup();
    bool IsRaceTimeup();
    void UpdateOutgoing();

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

    WorldObject *pAnimWorldObjects[8];
    uint8 NumWorldObjects;
    Timer CountdownSyncAnims;
    Timer CountdownSendDataCRC;
    Timer LastSyncedWTTimeStamp;
    Timer TimeRaceFinished;
    Timer TimeTrackLoaded;
    char PersonaMap[4][16];
    unsigned int GameSeed;
    eOnlineRaceEndReason RaceEndReason;
    Timer LastAntiCheatWorldTimer;
    Timer LastAntiCheatRealTimer;
    uint32 SavedLocalPhysicsCRC;
    int8 GetAwayLeaderDriverNumber;
    Timer GetAwayLeaderCheckTimer;
    Timer GetAwayFinishTime[4];
    Timer GetAwayLeaderTime[4];
    Timer LastGetAwayLeaderChangeTime;
    Timer RaceRestartTimer;
    bool GetAwayRaceTimedOut;
    bool FrameRateIsTooLow;
    Timer TimeupStartTime;
    float TimeupLength;
    bool RaceTimeup;
    bool RestartingRace;
    bool RestartRequested;
    int32 m_ticker;

  public:
    IntQuantizer QuantCarSlotID;
    IntQuantizer QuantCarType;
    IntQuantizer QuantCarPartIndex;
    IntQuantizer QuantInt2Bit;
    IntQuantizer QuantInt3Bit;
    IntQuantizer QuantInt4Bit;
    FloatQuantizer QuantFloatTime;
    FloatQuantizer QuantFloatSimTime;
    FloatQuantizer PositionQuantizerX;
    FloatQuantizer PositionQuantizerY;
    FloatQuantizer PositionQuantizerZ;
    FloatQuantizer VelocityQuantizer;
    FloatQuantizer MatrixQuantizer;
    FloatQuantizer AngleQuantizer;
    FloatQuantizer AccelerationQuantizer;
    FloatQuantizer AVelocityQuantizer;
    FloatQuantizer ControlQuantizer;

  protected:
    void SetPosUpdatePing(float ping);
    uint32 PackRideInfo(RideInfo &ride, SmartBitStream &data, bool pack_parts);
    uint32 UnpackRideInfo(RideInfo &ride, SmartBitStream &data, int skin_number);
    void SetupRaceParams();
    void SetupLocalDriver(int driver_num);
    void ChangeState(eOnlineState new_state);
    void MapRacers2PlayerIDs();
    void StartRace();
    void CreateOnlineRacer(int driver_num, SmartBitStream *pdata, bool is_server,
                           const char *persona);
    void PurgeDisconnectedRacers();
    int GetNumConnectedRacers();
    OnlineRacer *GetServerRacer();
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

  public:
    OnlineRacer *GetOnlineRacer(const char *racerName);
    OnlineRacer *GetLocalRacer() { return pLocalRacer; }
    bool IsValidRacer(int driver_number) {
        return pRacers[driver_number] != nullptr;
    }
    int GetNumRacers();

    friend struct Client;
    friend struct Server;
};

extern OnlineManager TheOnlineManager;

#endif
