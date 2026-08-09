#ifndef SPEECH_SOUNDAI_H
#define SPEECH_SOUNDAI_H

#include "EAXAirSupport.h"
#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/speechtune.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"
#include "Speed/Indep/Src/Generated/Messages/MRestartRace.h"
#include "Speed/Indep/Src/Generated/Messages/MUnspawnCop.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Speech/MWRoadNames.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"

DECLARE_CONTAINER_TYPE(IVehiclePtrs);

namespace Speech {

// total size: 0x8
// Decl: 59
struct copPair {
    inline bool operator<(const struct copPair &from) const {}

    HSIMABLE hsimable;  // offset 0x0, size 0x4
    struct EAXCop *cop; // offset 0x4, size 0x4
};

DECLARE_CONTAINER_TYPE(copMap);

class copMap : public UTL::Std::vector<copPair, _type_copMap> {
  public:
    copMap(int size) {}
};

DECLARE_CONTAINER_TYPE(copList);

class copList : public UTL::Std::vector<EAXCop *, _type_copList>, public AudioMemBase {};

DECLARE_CONTAINER_TYPE(voiceIDs);

class voiceIDs : public UTL::Std::vector<int, _type_voiceIDs> {};

// total size: 0x70
// Decl: 86
struct VoiceUsage {
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
    void Reset() {} // Decl: 109

    void Set(float dist, float vel) {} // Decl: 116

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
        // Members
        float value;                // offset 0x0, size 0x4
        Type_heat_level heat_level; // offset 0x4, size 0x4
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

    typedef Activity Base;
    typedef UTL::Std::vector<IVehicle *, _type_IVehiclePtrs> IVehicles;

    SoundAI();
    ~SoundAI() override;
    Sim::IActivity *Construct(Sim::Param params);

    void OnVehicleAdded(IVehicle *ivehicle);
    void OnVehicleRemoved(IVehicle *ivehicle);

    // void EnableObservations() {}

    // void DisableObservations() {}

    // void ObserveOnly(unsigned int m) {}

    // void EnableAI() {}

    // void DisableAI() {}

    // void Enable() {}

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

    // const int GetHavoc() {}

    // SpeechObservations GetLastObservation() {
    //     if (mObserver != nullptr) {
    //         return mObserver->GetLastEvent();
    //     }
    //     return Speech::None;
    // }

    const int GetFocus() {
        return this->mFocus;
    }

    void SetFocus(MachineState s) {
        this->mFocus = s;
    }

    IRoadBlock *GetRoadblock();

    // Observer *GetObserver() {}

    // RoadblockFlow *GetRBFlow() {
    //     return this->mRoadblockFlow;
    // }

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
        return (this->mFlags & RB_ENABLED) != 0;
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
        if (mPlayerCarCustom == nullptr) {
            return 0;
        }
        return mPlayerCarCustom->color;
    }

    unsigned int GetPlayerCarColor() {}

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
    unsigned char GetCustomized(IVehicle *vehicle, CarCustomizations &custrec);

    void AttemptReattachPursuit();

    void MessagePerpBusted(const MPerpBusted &message);
    void MessageAIPerpBusted(const MPerpBusted &message);
    void MessageInfraction(const MMiscSound &message);
    void MessageRestart(const MRestartRace &message);
    void MessageUnspawnCop(const MUnspawnCop &message);
    void MessageTireBlown(const MGamePlayMoment &message);

    // TODO these two have unknown visibility
    EAXCop *GetCopInRB();
    void Force911State();

  private:
    HSIMTASK mMainUpdate;          // offset 0x54, size 0x4
    HSIMTASK mProcessObservations; // offset 0x58, size 0x4

    static int mRefCount; // size: 0x4, address: 0x80435E9C

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
    Speech::BlowByRecord mRecentBlowby;       // offset 0x1E4, size 0xC
    int mInfraction;                          // offset 0x1F0, size 0x4
    int mNumCopsInWave;                       // offset 0x1F4, size 0x4
    int mNumActiveCopCars;                    // offset 0x1F8, size 0x4
    int mPlayerOffroadID;                     // offset 0x1FC, size 0x4
    unsigned char mCopsInView;                // offset 0x200, size 0x1
    struct PursuitFlow *mPursuitFlow;         // offset 0x204, size 0x4
    struct StrategyFlow *mStrategyFlow;       // offset 0x208, size 0x4
    struct Observer *mObserver;               // offset 0x20C, size 0x4
    struct RoadblockFlow *mRoadblockFlow;     // offset 0x210, size 0x4
    struct MusicFlow *mMusicFlow;             // offset 0x214, size 0x4
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

#endif
