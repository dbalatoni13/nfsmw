#ifndef SPEECH_SOUNDAI_H
#define SPEECH_SOUNDAI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "EAXAirSupport.h"
#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/speechtune.h"
#include "Speed/Indep/Src/Generated/Messages/MUnspawnCop.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"

DECLARE_VECTOR_TYPE(IVehiclePtrs);

// total size: 0x260
class SoundAI : public Sim::Activity, public Sim::Collision::IListener, public UTL::Collections::Singleton<SoundAI> {
  public:
    // total size: 0x8
    struct CarHeading {
        // Members
        unsigned int direction; // offset 0x0, size 0x4
        RoadNames roadID;       // offset 0x4, size 0x4
    };
    enum PursuitState {
        kActive = 0,
        kSearching = 1,
        kInactive = 2,
        kOtherTarget = 3,
    };
    enum QuadrantState {
        kInitial = 0,
        kForming = 1,
        kFiction1 = 2,
        kFiction2 = 3,
        kExpired = 4,
        kReset = 5,
    };
    enum CarCustomFlags {
        VINYLS = 1,
        PAINT = 2,
        RACING_NUMS = 4,
        DECALS = 8,
    };
    // total size: 0x8
    struct CarCustomizations {
        // Members
        // Type_car_color color; // offset 0x0, size 0x4
        unsigned int flags; // offset 0x4, size 0x4
    };
    // total size: 0x8
    struct HeatCutoffs {
        // Members
        float value;                // offset 0x0, size 0x4
        Type_heat_level heat_level; // offset 0x4, size 0x4
    };
    enum MachineState {
        kPursuitFlow = 1,
        kStrategyFlow = 2,
        kBackupFlow = 3,
        kOutcomeFlow = 4,
        kRoadblockFlow = 5,
        kSwarmingFlow = 6,
        kTransition = -1,
        kWaiting = 333,
        kLost = 666,
        kTerminal = 999,
        kCullCheck = 0,
    };
    enum BailoutType {
        kOutrunBail = 0,
        kForcedBail = 1,
    };
    enum VehicleImpactType {
        kCopREperp = 0,
        kPerpRECop = 1,
        kCopTBPerp = 2,
        kPerpTBCop = 3,
        kCopHOPerp = 4,
        kPerpHOCop = 5,
        kCopSSPerp = 6,
        kPerpSSCop = 7,
        kUnknown = 8,
    };
    enum SoundAIFlags {
        RB_ENABLED = 1,
        HELIRB_ENABLED = 2,
        SPIKES_ENABLED = 4,
        LOWSPEEDTIMER = 8,
        PATH_WAITING = 16,
        COPS_ARE_AHEAD = 32,
        HELI_INTRO_REQ = 64,
        BUSTED = 128,
        DISP911_ACTIVE = 256,
        SETUP_RESTARTED = 512,
        COPS_IMMUNE = 1024,
        RACERS_PROXIMAL = 2048,
        PURSUIT_EXPIRED = 4096,
    };

    typedef struct Activity Base;
    typedef struct UTL::Std::vector<IVehicle *, _type_IVehiclePtrs> IVehicles;

    SoundAI();
    ~SoundAI() override;

    void MessagePerpBusted(const struct MPerpBusted &message);
    void MessageAIPerpBusted(const struct MPerpBusted &message);
    void MessageInfraction(const struct MMiscSound &message);
    void MessageRestart(const struct MRestartRace &message);
    void MessageUnspawnCop(const MUnspawnCop &message);
    void MessageTireBlown(const struct MGamePlayMoment &message);
    void OnVehicleAdded(IVehicle *ivehicle);
    void OnVehicleRemoved(IVehicle *ivehicle);
    EAXCop *GetCopInRB();
    EAXCop *GetRandomActiveCop(int type, bool reqLOS);
    EAXCop *GetRandomCop(int type);
    Sim::IActivity *Construct(Sim::Param params);
    IRoadBlock *GetRoadblock();
    bool IsMusicActive();
    void DealWithDeadAir();
    void UpdateStateMachines();
    void AttemptReattachPursuit();
    void SyncPursuit();
    void TerminatePursuit(BailoutType type);
    void ResetPursuit(bool including_music);
    void ShuffleActors();
    bool IsHeadingValid();
    void SyncPlayers();
    void Force911State();
    void SyncCarsToActors();
    void SyncFormations();
    EAXCop *FindFurthestCop(bool includeHeli);
    EAXCop *FindClosestCop(bool enforceLOS, bool includeHeli);
    void RemoveCop(HSIMABLE seeya);
    void AddNewHeli(IVehicle *heli);
    EAXCop *SpawnCop();
    int GetBattalionFromRoadID(int roadID);
    int GetBattalionFromKey(unsigned int theKey);
    void AddNewCop(IVehicle *newcop);
    bool MakeLeader(EAXCop *newprim);
    // int GetCallsign(Type_speaker_battalion battalion);
    // void RandomizeCallsign(struct voiceIDs &cs, Type_speaker_call_sign_id start, Type_speaker_call_sign_id finish);
    int GetVoice(int type);
    EAXCop *GetCop(int speaker);
    void RandomBailoutDeny(EAXCop *wimp);
    unsigned int CalcPlayerDirection(bool force_set);
    void ForceGlobalVoiceChange();
    unsigned char GetCustomized(IVehicle *vehicle, CarCustomizations &custrec);
    bool IsHighIntensity();
    const float GetTimeLastNailedCop();

    // Virtual overrides
    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;

    //  IAttachable
    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    // IActivity
    void Release() override;

    // void EnableObservations() {}

    // void DisableObservations() {}

    // void ObserveOnly(unsigned int m) {}

    // void EnableAI() {}

    // void DisableAI() {}

    // void Enable() {}

    // void Disable() {}

    // const struct copMap &GetActors() {}

    // EAXCop *GetLeader() {}

    EAXAirSupport *GetHeli() {
        return mHeli;
    }

    // struct EAXDispatch *GetDispatch() {}

    // struct IPursuit *GetPursuit() {}

    // enum PursuitState GetPursuitState() {}

    // const int GetHeat() {}

    // EAXCop *GetLatestCop() {}

    // const struct copList &GetCopsInFormation() {}

    // const float GetPursuitDistance() {}

    // const float GetPlayerSpeed() {}

    // const struct Vector3 &GetPlayerPos() {}

    // const int NumCopsWithLOS() {}

    // const int NumTrafficHits() {}

    // const signed char NumRoadBlocks() {}

    // const int NumPursuits() {}

    // const struct pvehicle &GetPlayerSpecs() {}

    // const struct speechtune &GetTune() {}

    const Attrib::Gen::pursuitlevels &GetPursuitSpecs() {
        return mPursuitLevel;
    }

    // const int GetHavoc() {}

    // enum SpeechObservations GetLastObservation() {}

    // const int GetFocus() {}

    // void SetFocus(enum MachineState s) {}

    // struct Observer *GetObserver() {}

    // struct RoadblockFlow *GetRBFlow() {}

    // enum RoadNames GetAIRacerRoadID(int n) {}

    // unsigned int GetAIRacerDirection(int n) {}

    // unsigned int GetLastKnownAIDirection() {}

    // enum RoadNames GetLastKnownAIRoad() {}

    // enum RoadNames GetPlayerRoadID(int n) {}

    // unsigned int GetPlayerDirection(int n) {}

    // unsigned int GetLastKnownDirection() {}

    // enum RoadNames GetLastKnownRoad() {}

    // const float GetPursuitDuration() {}

    // const float GetPlayerStopTime() {}

    // const float GetPerpLostTime() {}

    // const float GetTimeLastCrashed() {}

    // const float GetTimeSinceLastChase() {}

    // const float GetTimeInView() {}

    // struct BlowByRecord &GetRecentBlowby() {}

    // struct SlotPool *GetActorPool() {}

    // void MakeCopsImmune() {}

    // void ClearImmunity() {}

    // const unsigned char GetRacerCount() {}

    // bool RoadblocksEnabled() {}

    // bool HeliRoadblocksEnabled() {}

    // bool SpikesEnabled() {}

    // bool AreCopsAhead() {}

    // bool Is911Active() {}

    // bool AreRacersNearby() {}

    // int GetLastInfraction() {}

    // int GetNumCopsInWave() {}

    // unsigned int GetPlayerOffroadID() {}

    // unsigned int GetPlayerCarColor() {}

    // unsigned int GetPlayerCustom() {}

    // const unsigned char GetNumCopsInView() {}

    // int GetNumActiveCopCars() {}

    static const HeatCutoffs heat_cutoffs[4]; // size: 0x20, address: 0x80407A80
    static int mRefCount;                     // size: 0x4, address: 0x80435E9C

  private:
    HSIMTASK mMainUpdate;                     // offset 0x54, size 0x4
    HSIMTASK mProcessObservations;            // offset 0x58, size 0x4
    unsigned int mFlags;                      // offset 0x5C, size 0x4
    Speech::copMap mActors;                   // offset 0x60, size 0x10
    Speech::VoiceUsage mUsage;                // offset 0x70, size 0x70
    struct EAXDispatch *mDispatch;            // offset 0xE0, size 0x4
    EAXCop *mLeader;                          // offset 0xE4, size 0x4
    EAXAirSupport *mHeli;                     // offset 0xE8, size 0x4
    Speech::copList mCopsInFormation;         // offset 0xEC, size 0x14
    float mDeadAir;                           // offset 0x100, size 0x4
    EAXCop *mLastCopInFormation;              // offset 0x104, size 0x4
    EAXCop *mLatestCop;                       // offset 0x108, size 0x4
    int mPlayerHeat;                          // offset 0x10C, size 0x4
    float mPlayerSpeed;                       // offset 0x110, size 0x4
    UMath::Vector3 mPlayerPos;                // offset 0x114, size 0xC
    UMath::Vector3 mPlayerFW;                 // offset 0x120, size 0xC
    UMath::Vector3 mSmoothedFWRoad;           // offset 0x12C, size 0xC
    IPursuit *mPursuit;                       // offset 0x138, size 0x4
    IPursuit *mAIPursuit;                     // offset 0x13C, size 0x4
    int mFocus;                               // offset 0x140, size 0x4
    float mPursuitDist;                       // offset 0x144, size 0x4
    float mPursuitDuration;                   // offset 0x148, size 0x4
    float mT_PerpLastSeen;                    // offset 0x14C, size 0x4
    short mLOSCount;                          // offset 0x150, size 0x2
    int mTrafficHits911;                      // offset 0x154, size 0x4
    int mCTS911;                              // offset 0x158, size 0x4
    int mHavoc;                               // offset 0x15C, size 0x4
    int mPursuitCount;                        // offset 0x160, size 0x4
    char mNumRoadBlocks;                      // offset 0x164, size 0x1
    char mRacerCount;                         // offset 0x165, size 0x1
    float mClosestRacerDist;                  // offset 0x168, size 0x4
    float mTimeSinceLastChase;                // offset 0x16C, size 0x4
    Attrib::Gen::pvehicle mPVehicle;          // offset 0x170, size 0x14
    Attrib::Gen::speechtune mTune;            // offset 0x184, size 0x14
    Attrib::Gen::pursuitlevels mPursuitLevel; // offset 0x198, size 0x14
    CarHeading mPlayerCurrent[2];             // offset 0x1AC, size 0x10
    CarHeading mAICurrent[2];                 // offset 0x1BC, size 0x10
    CarHeading mLastKnown;                    // offset 0x1CC, size 0x8
    CarHeading mAILastKnown;                  // offset 0x1D4, size 0x8
    PursuitState mPursuitState;               // offset 0x1DC, size 0x4
    QuadrantState mQuadrantState;             // offset 0x1E0, size 0x4
    // BlowByRecord mRecentBlowby;               // offset 0x1E4, size 0xC
    int mInfraction;                        // offset 0x1F0, size 0x4
    int mNumCopsInWave;                     // offset 0x1F4, size 0x4
    int mNumActiveCopCars;                  // offset 0x1F8, size 0x4
    int mPlayerOffroadID;                   // offset 0x1FC, size 0x4
    unsigned char mCopsInView;              // offset 0x200, size 0x1
    struct PursuitFlow *mPursuitFlow;       // offset 0x204, size 0x4
    struct StrategyFlow *mStrategyFlow;     // offset 0x208, size 0x4
    struct Observer *mObserver;             // offset 0x20C, size 0x4
    struct RoadblockFlow *mRoadblockFlow;   // offset 0x210, size 0x4
    struct MusicFlow *mMusicFlow;           // offset 0x214, size 0x4
    Timer mT_outofFormation;                // offset 0x218, size 0x4
    Timer mT_reallylowspeed;                // offset 0x21C, size 0x4
    Timer mT_noLOS;                         // offset 0x220, size 0x4
    Timer mT_LOS;                           // offset 0x224, size 0x4
    Timer mT_lastCrashed;                   // offset 0x228, size 0x4
    Timer mT_lastCopNailed;                 // offset 0x22C, size 0x4
    Timer mT_pursuitStart;                  // offset 0x230, size 0x4
    Timer mT_sinceLastPursuit;              // offset 0x234, size 0x4
    CarCustomizations *mPlayerCarCustom;    // offset 0x238, size 0x4
    CarCustomizations *mAICarCustom;        // offset 0x23C, size 0x4
    SlotPool *mActorPool;                   // offset 0x240, size 0x4
    Hermes::HHANDLER mMsgPerpBusted;        // offset 0x244, size 0x4
    Hermes::HHANDLER mMsgAIPerpBusted;      // offset 0x248, size 0x4
    Hermes::HHANDLER mMsgForcePursuitStart; // offset 0x24C, size 0x4
    Hermes::HHANDLER mMsgRestartRace;       // offset 0x250, size 0x4
    Hermes::HHANDLER mMsgInfraction;        // offset 0x254, size 0x4
    Hermes::HHANDLER mMsgUnspawnCop;        // offset 0x258, size 0x4
    Hermes::HHANDLER mMsgTireBlown;         // offset 0x25C, size 0x4
};

#endif
