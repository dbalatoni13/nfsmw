#ifndef __SOUNDAI_H_
#define __SOUNDAI_H_ 1 // Decl: 13

#include "Speed/Indep/Src/Generated/AttribSys/Classes/speechtune.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"
#include "Speed/Indep/Src/Generated/Messages/MRestartRace.h"
#include "Speed/Indep/Src/Generated/Messages/MUnspawnCop.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/Speech/MWRoadNames.h"
#include "Speed/Indep/Src/Speech/MusicFlow.h"
#include "Speed/Indep/Src/Speech/PursuitFlow.h"
#include "Speed/Indep/Src/Speech/StrategyFlow.h"
#include "Speed/Indep/Src/Speech/RoadblockFlow.h"
#include "Speed/Indep/Src/Speech/Observer.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"

#define SOUNDAI_MAIN_UPDATE_RATE 0.1f      // Decl: 25
#define SOUNDAI_OBSERVER_UPDATE_RATE 0.25f // Decl: 26
#define MAX_VOICE_ACTORS 20                // Decl: 27

DECLARE_CONTAINER_TYPE(IVehiclePtrs);

namespace Speech {

// total size: 0x8
// Decl: 59
struct copPair {
    bool operator<(const copPair &from) const {
        return this->hsimable < from.hsimable;
    }

    HSIMABLE hsimable; // offset 0x0, size 0x4
    EAXCop *cop;       // offset 0x4, size 0x4
};

DECLARE_CONTAINER_TYPE(copMap);

class copMap : public UTL::Std::vector<copPair, _type_copMap> {
  public:
    copMap(int size) {
        this->reserve(size);
    }

    void Add(HSIMABLE hsimable, EAXCop *cop);
    EAXCop *Remove(HSIMABLE hsimable);
    void ModifyHandle(HSIMABLE hsimable, HSIMABLE newhandle);
    EAXCop *Find(HSIMABLE hsimable) const;
};

DECLARE_CONTAINER_TYPE(copList);

class copList : public UTL::Std::vector<EAXCop *, _type_copList>, public AudioMemBase {};

DECLARE_CONTAINER_TYPE(voiceIDs);

class voiceIDs : public UTL::Std::vector<int, _type_voiceIDs> {};

// total size: 0x70
// Decl: 86
struct VoiceUsage {
    VoiceUsage() {
        voices.reserve(8);
        cs_Rhino.reserve(6);
        cs_SuperPursuit.reserve(6);
        cs_City.reserve(20);
        cs_Coastal.reserve(10);
        cs_Rosewood.reserve(10);
        cs_Alpine.reserve(10);
    }

    voiceIDs voices;          // offset 0x0, size 0x10
    voiceIDs cs_Rhino;        // offset 0x10, size 0x10
    voiceIDs cs_SuperPursuit; // offset 0x20, size 0x10
    voiceIDs cs_City;         // offset 0x30, size 0x10
    voiceIDs cs_Coastal;      // offset 0x40, size 0x10
    voiceIDs cs_Rosewood;     // offset 0x50, size 0x10
    voiceIDs cs_Alpine;       // offset 0x60, size 0x10
};

// total size: 0xC
// Decl: 108
struct BlowByRecord {
    // Decl: 109
    void Reset() {
        this->distance = 32767.0f;
        this->speed = 0.0f;
        this->timestamp = Timer(0);
    }

    // Decl: 116
    void Set(float dist, float vel) {
        this->distance = dist;
        this->speed = vel;
        this->timestamp = WorldTimer;
    }

    float distance;  // offset 0x0, size 0x4, Decl: 123
    float speed;     // offset 0x4, size 0x4, Decl: 124
    Timer timestamp; // offset 0x8, size 0x4, Decl: 125
};

}; // namespace Speech

// total size: 0x260
// Decl: 133
class SoundAI : public Sim::Activity, public Sim::Collision::IListener, public UTL::Collections::Singleton<SoundAI> {
  public:
    enum CarCustomFlags {
        VINYLS = 1,
        PAINT = 2,
        RACING_NUMS = 4,
        DECALS = 8,
    };
    // total size: 0x8
    // Decl: 159
    struct CarCustomizations {
        Csis::Type_car_color color; // offset 0x0, size 0x4
        unsigned int flags;         // offset 0x4, size 0x4
    };
    // total size: 0x8
    // Decl: 165
    struct HeatCutoffs {
        float value;                      // offset 0x0, size 0x4
        Csis::Type_heat_level heat_level; // offset 0x4, size 0x4
    };
    // total size: 0x8
    // Decl: 173
    struct CarHeading {
        unsigned int direction; // offset 0x0, size 0x4
        RoadNames roadID;       // offset 0x4, size 0x4
    };
    // Decl: 178
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
    // Decl: 193
    enum BailoutType {
        kOutrunBail = 0,
        kForcedBail = 1,
    };
    // Decl: 199
    enum PursuitState {
        kActive = 0,
        kSearching = 1,
        kInactive = 2,
        kOtherTarget = 3,
    };
    // Decl: 207
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
    // Decl: 236
    enum QuadrantState {
        kInitial = 0,
        kForming = 1,
        kFiction1 = 2,
        kFiction2 = 3,
        kExpired = 4,
        kReset = 5,
    };
    enum SoundAIFlags {
        RB_ENABLED = 1 << 0,
        HELIRB_ENABLED = 1 << 1,
        SPIKES_ENABLED = 1 << 2,
        LOWSPEEDTIMER = 1 << 3,
        PATH_WAITING = 1 << 4,
        COPS_ARE_AHEAD = 1 << 5,
        HELI_INTRO_REQ = 1 << 6,
        BUSTED = 1 << 7,
        DISP911_ACTIVE = 1 << 8,
        SETUP_RESTARTED = 1 << 9,
        COPS_IMMUNE = 1 << 10,
        RACERS_PROXIMAL = 1 << 11,
        PURSUIT_EXPIRED = 1 << 12,
    };

    typedef Activity Base;
    typedef UTL::Std::vector<IVehicle *, _type_IVehiclePtrs> IVehicles;

    SoundAI();
    ~SoundAI() override;
    static Sim::IActivity *Construct(Sim::Param params);

    void OnVehicleAdded(IVehicle *ivehicle);
    void OnVehicleRemoved(IVehicle *ivehicle);

    void EnableObservations() {
        this->ModifyTask(this->mProcessObservations, SOUNDAI_OBSERVER_UPDATE_RATE);
    }

    // void DisableObservations() {}

    // void ObserveOnly(unsigned int m) {}

    void EnableAI() {
        this->ModifyTask(this->mMainUpdate, SOUNDAI_MAIN_UPDATE_RATE);
    }

    // void DisableAI() {}

    void Enable() {
        this->EnableAI();
        this->EnableObservations();
    }

    // void Disable() {}

    // IActivity
    void Release() override;

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    const Speech::copMap &GetActors() {
        return this->mActors;
    }

    EAXCop *GetLeader() {
        return this->mLeader;
    }

    EAXAirSupport *GetHeli() {
        return mHeli;
    }

    struct EAXDispatch *GetDispatch() {
        return this->mDispatch;
    }

    IPursuit *GetPursuit() {
        return this->mPursuit;
    }

    PursuitState GetPursuitState() {
        return mPursuitState;
    }

    bool IsMusicActive();

    const int GetHeat() {
        return this->mPlayerHeat;
    }

    EAXCop *GetLatestCop() {
        return this->mLatestCop;
    }

    const Speech::copList &GetCopsInFormation() {
        return this->mCopsInFormation;
    }

    const float GetPursuitDistance() {
        return this->mPursuitDist;
    }

    EAXCop *FindClosestCop(bool enforceLOS, bool includeHeli);
    EAXCop *FindFurthestCop(bool includeHeli);

    const float GetPlayerSpeed() {
        return this->mPlayerSpeed;
    }

    const UMath::Vector3 &GetPlayerPos() {
        return this->mPlayerPos;
    }

    const int NumCopsWithLOS() {
        return this->mLOSCount;
    }

    const int NumTrafficHits() {
        return this->mTrafficHits911;
    }

    const signed char NumRoadBlocks() {
        return this->mNumRoadBlocks;
    }

    const int NumPursuits() {
        return this->mPursuitCount;
    }

    const Attrib::Gen::pvehicle &GetPlayerSpecs() {
        return this->mPVehicle;
    }

    const Attrib::Gen::speechtune &GetTune() {
        return this->mTune;
    }

    const Attrib::Gen::pursuitlevels &GetPursuitSpecs() {
        return this->mPursuitLevel;
    }

    const int GetHavoc() {
        return this->mCTS911; // TODO BUG? why not mHavoc?
    }

    Speech::SpeechObservations GetLastObservation() {
        if (this->mObserver != nullptr) {
            return this->mObserver->GetLastEvent();
        }
        return Speech::None;
    }

    const int GetFocus() {
        return this->mFocus;
    }

    void SetFocus(MachineState s) {
        this->mFocus = s;
    }

    IRoadBlock *GetRoadblock();

    Speech::Observer *GetObserver() {
        return this->mObserver;
    }

    Speech::RoadblockFlow *GetRBFlow() {
        return this->mRoadblockFlow;
    }

    unsigned int CalcPlayerDirection(bool force_set);

    // RoadNames GetAIRacerRoadID(int n) {}

    // unsigned int GetAIRacerDirection(int n) {}

    // unsigned int GetLastKnownAIDirection() {}

    // RoadNames GetLastKnownAIRoad() {}

    RoadNames GetPlayerRoadID(int n) {
        return this->mPlayerCurrent[n].roadID;
    }

    unsigned int GetPlayerDirection(int n) {
        return this->mPlayerCurrent[n].direction;
    }

    unsigned int GetLastKnownDirection() {
        return this->mLastKnown.direction;
    }

    RoadNames GetLastKnownRoad() {
        return this->mLastKnown.roadID;
    }

    bool IsHeadingValid();

    const float GetPursuitDuration() {
        return this->mPursuitDuration;
    }

    const float GetPlayerStopTime() {
        return (WorldTimer - this->mT_reallylowspeed).GetSeconds();
    }

    const float GetPerpLostTime() {
        return (WorldTimer - this->mT_noLOS).GetSeconds();
    }

    const float GetTimeLastCrashed() {
        return (WorldTimer - this->mT_lastCrashed).GetSeconds();
    }

    const float GetTimeLastNailedCop();

    const float GetTimeSinceLastChase() {
        return this->mTimeSinceLastChase;
    }

    const float GetTimeInView() {
        return (WorldTimer - this->mT_LOS).GetSeconds();
    }

    // const unsigned char GetRacerCount() {}

    void RandomBailoutDeny(EAXCop *wimp);

    bool RoadblocksEnabled() {
        if ((this->mFlags & RB_ENABLED) != 0) {
            return true;
        }
        return false;
    }

    bool HeliRoadblocksEnabled() {
        return (this->mFlags & HELIRB_ENABLED) != 0;
    }

    bool SpikesEnabled() {
        return (this->mFlags & SPIKES_ENABLED) != 0;
    }

    bool AreCopsAhead() {
        return (this->mFlags & COPS_ARE_AHEAD) != 0;
    }

    bool Is911Active() {
        return (this->mFlags & DISP911_ACTIVE) != 0;
    }

    bool AreRacersNearby() {
        return (this->mFlags & RACERS_PROXIMAL) != 0;
    }

    int GetLastInfraction() {
        return this->mInfraction;
    }

    int GetNumCopsInWave() {
        return this->mNumCopsInWave;
    }

    unsigned int GetPlayerOffroadID() {
        return static_cast<unsigned int>(this->mPlayerOffroadID);
    }

    unsigned int GetPlayerCarColor() {
        if (this->mPlayerCarCustom == nullptr) {
            return 0;
        }
        return this->mPlayerCarCustom->color;
    }

    unsigned int GetPlayerCustom() {
        if (mPlayerCarCustom == nullptr) {
            return 0;
        }
        return mPlayerCarCustom->flags;
    }

    const unsigned char GetNumCopsInView() {
        return this->mCopsInView;
    }

    int GetNumActiveCopCars() {
        return this->mNumActiveCopCars;
    }

    void Force911State();

  protected:
    //  IAttachable
    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;

    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;

  public:
    void AddNewCop(IVehicle *newcop);
    void AddNewHeli(IVehicle *heli);
    void RemoveCop(HSIMABLE seeya);

    void SyncCarsToActors();
    void SyncFormations();
    void SyncPursuit();
    void SyncPlayers();

    EAXCop *GetCop(int speaker);
    EAXCop *GetRandomCop(int type);
    EAXCop *GetRandomActiveCop(int type, bool reqLOS);
    EAXCop *GetCopInRB();

    void UpdateStateMachines();
    void DealWithDeadAir();

    void ShuffleActors();
    bool MakeLeader(EAXCop *newprim);
    void TerminatePursuit(BailoutType type);
    void ResetPursuit(bool including_music);

    Speech::BlowByRecord &GetRecentBlowby() {
        return this->mRecentBlowby;
    }

    EAXCop *SpawnCop();

    SlotPool *GetActorPool() {
        return this->mActorPool;
    }

    void MakeCopsImmune() {
        this->mFlags |= COPS_IMMUNE;
    }

    void ClearImmunity() {
        this->mFlags &= ~COPS_IMMUNE;
    }

    bool IsHighIntensity();

    static const HeatCutoffs heat_cutoffs[4]; // size: 0x20, address: 0x80407A80

  private:
    int GetVoice(int type);
    void RandomizeCallsign(Speech::voiceIDs &cs, Csis::Type_speaker_call_sign_id start, Csis::Type_speaker_call_sign_id finish);
    int GetCallsign(Csis::Type_speaker_battalion battalion);
    int GetBattalionFromRoadID(int roadID);
    int GetBattalionFromKey(unsigned int theKey);

    void ForceGlobalVoiceChange();
    uint8 GetCustomized(IVehicle *vehicle, CarCustomizations &custrec);

    void AttemptReattachPursuit();

    void MessagePerpBusted(const MPerpBusted &message);
    void MessageAIPerpBusted(const MPerpBusted &message);
    void MessageInfraction(const MMiscSound &message);
    void MessageRestart(const MRestartRace &message);
    void MessageUnspawnCop(const MUnspawnCop &message);
    void MessageTireBlown(const MGamePlayMoment &message);

  private:
    HSIMTASK mMainUpdate;          // offset 0x54, size 0x4
    HSIMTASK mProcessObservations; // offset 0x58, size 0x4

    static int mRefCount; // size: 0x4, address: 0x80435E9C

    unsigned int mFlags;                      // offset 0x5C, size 0x4
    Speech::copMap mActors;                   // offset 0x60, size 0x10
    Speech::VoiceUsage mUsage;                // offset 0x70, size 0x70
    EAXDispatch *mDispatch;                   // offset 0xE0, size 0x4
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
    int8 mNumRoadBlocks;                      // offset 0x164, size 0x1
    int8 mRacerCount;                         // offset 0x165, size 0x1
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
    Speech::BlowByRecord mRecentBlowby;       // offset 0x1E4, size 0xC
    int mInfraction;                          // offset 0x1F0, size 0x4
    int mNumCopsInWave;                       // offset 0x1F4, size 0x4
    int mNumActiveCopCars;                    // offset 0x1F8, size 0x4
    int mPlayerOffroadID;                     // offset 0x1FC, size 0x4
    uint8 mCopsInView;                        // offset 0x200, size 0x1
    Speech::PursuitFlow *mPursuitFlow;        // offset 0x204, size 0x4
    Speech::StrategyFlow *mStrategyFlow;      // offset 0x208, size 0x4
    Speech::Observer *mObserver;              // offset 0x20C, size 0x4
    Speech::RoadblockFlow *mRoadblockFlow;    // offset 0x210, size 0x4
    Speech::MusicFlow *mMusicFlow;            // offset 0x214, size 0x4
    Timer mT_outofFormation;                  // offset 0x218, size 0x4
    Timer mT_reallylowspeed;                  // offset 0x21C, size 0x4
    Timer mT_noLOS;                           // offset 0x220, size 0x4
    Timer mT_LOS;                             // offset 0x224, size 0x4
    Timer mT_lastCrashed;                     // offset 0x228, size 0x4
    Timer mT_lastCopNailed;                   // offset 0x22C, size 0x4
    Timer mT_pursuitStart;                    // offset 0x230, size 0x4
    Timer mT_sinceLastPursuit;                // offset 0x234, size 0x4
    CarCustomizations *mPlayerCarCustom;      // offset 0x238, size 0x4
    CarCustomizations *mAICarCustom;          // offset 0x23C, size 0x4
    SlotPool *mActorPool;                     // offset 0x240, size 0x4
    Hermes::HHANDLER mMsgPerpBusted;          // offset 0x244, size 0x4
    Hermes::HHANDLER mMsgAIPerpBusted;        // offset 0x248, size 0x4
    Hermes::HHANDLER mMsgForcePursuitStart;   // offset 0x24C, size 0x4
    Hermes::HHANDLER mMsgRestartRace;         // offset 0x250, size 0x4
    Hermes::HHANDLER mMsgInfraction;          // offset 0x254, size 0x4
    Hermes::HHANDLER mMsgUnspawnCop;          // offset 0x258, size 0x4
    Hermes::HHANDLER mMsgTireBlown;           // offset 0x25C, size 0x4
};

extern const bool SPEECHFLOW_DISPLAY; // TODO const and move?

#endif
