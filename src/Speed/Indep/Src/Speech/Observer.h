#ifndef __OBSERVER_H
#define __OBSERVER_H 1 // Decl: 15

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/COPSPEECH.hpp"
#include "Speed/Indep/Src/Speech/SpeechFlow.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

namespace Speech {

// Decl: 44
enum SpeechObservations {
    None = 0,
    Collision_Cop_Cop = 1,
    Collision_Cop_Traffic = 2,
    Collision_Cop_Suspect = 3,
    Collision_Cop_World = 4,
    Collision_Suspect_World = 5,
    Collision_Suspect_Suspect = 6,
    Collision_Suspect_Traffic = 7,
    Collision_Suspect_Structure = 8,
    Collision_Suspect_Tree = 9,
    Collision_Suspect_Guardrail = 10,
    Collision_Suspect_Train = 11,
    Collision_Suspect_Semi = 12,
    Collision_Suspect_GasStation = 13,
    Collision_Suspect_Spikebelt = 14,
    Behavior_Suspect_Spunout = 15,
    Behavior_Suspect_Airborne = 16,
    Behavior_Suspect_Rollover = 17,
    NUM_OBSERVATIONS = 18,
};

// total size: 0x10
// Decl: 68
struct SpeechObservation {
    SpeechObservation(SpeechObservations obs, int spkr, float f) {
        this->speakerID = spkr;
        this->time = WorldTimer;
        this->force = f;
        this->observation = obs;
    }
    SpeechObservation(SpeechObservations obs, int spkr) : observation(obs), time(WorldTimer), force(1.0f), speakerID(spkr) {}

    SpeechObservations observation; // offset 0x0, size 0x4, Decl: 78
    Timer time;                     // offset 0x4, size 0x4, Decl: 79
    float force;                    // offset 0x8, size 0x4, Decl: 80
    int speakerID;                  // offset 0xC, size 0x4, Decl: 81
};

DECLARE_CONTAINER_TYPE(observations);

// total size: 0x14
// Decl: 85
// total size: 0xC
class observations : public UTL::Std::list<Speech::SpeechObservation, Speech::_type_observations>, public AudioMemBase {};

// total size: 0xA0
// Decl: 92
class Observer : public SpeechFlow {
  public:
    enum State {
        kTransition = -1,
        kCullCheck = 0,
    };
    enum TrackBehavior {
        UTurn = 1,
        Braking = 2,
        Outcome = 4,
        CarRam = 8,
        UTurnResult = 16,
        Lost = 32,
        Arrest = 64,
    };

    Observer(); // Decl: 145
    ~Observer() override;

    // Overrides: SpeechFlow
    void Update() override;
    bool IsTransitionable() override;

    void Observe(int currobsrvation, int speaker, float f);

    void Process();

    // Overrides: SpeechFlow
    void Reset() override;

    // Decl: 106
    void DetachRamCop() {
        this->mRamCop = nullptr;
    }
    // Decl: 107
    EAXCop *GetRamCop() {
        return this->mRamCop;
    }
    // Decl: 108
    bool WeatherExists() {
        return this->mWeather;
    }
    // Decl: 109
    bool PlayerInTunnel() {
        return this->mTunnel;
    }

    void NotifyAirborne(float alt, float t);

    // Decl: 112
    // void SetMask(unsigned int mask) {}
    // Decl: 113
    Speech::SpeechObservations GetLastEvent() {
        return this->mLastEvent;
    }

  private:
    Observer(const Observer &);
    const Observer &operator=(const Observer &);

    float CalcFWVec_Road_Car();

    void AssessArrest();

    void AssessLOS();
    void AssessFlippage();
    void Assess180();
    void AssessOutcome();
    void AssessBraking();
    void AssessOutrun();
    void AssessOffroad();
    void GasStationAftermath();
    void AssessWeather();

    virtual void CullCheck();

    void MessageBlewPastCop(const MGamePlayMoment &message);
    void MessageGamePlayMoment(const MGamePlayMoment &message);
    void MessageEventComplete(const MNotifySpeechStatus &message);
    void MessageTunnelUpdate(const MMiscSound &message);

    observations mObservations;    // offset 0x10, size 0xC, Decl: 148
    unsigned int mObserveMask;     // offset 0x1C, size 0x4, Decl: 149
    SpeechObservations mLastEvent; // offset 0x20, size 0x4, Decl: 150

    int mNumCopsWithLOS; // offset 0x24, size 0x4, Decl: 166
    Timer mT_bullhorn;   // offset 0x28, size 0x4, Decl: 167
    Timer mT_unstable;   // offset 0x2C, size 0x4, Decl: 168
    Timer mT_airborne;   // offset 0x30, size 0x4, Decl: 169
    Timer mT_flipped;    // offset 0x34, size 0x4, Decl: 170

    Timer mT_trackingOutcome; // offset 0x38, size 0x4, Decl: 172

    float mDotTrack;                        // offset 0x3C, size 0x4, Decl: 175
    unsigned int mOffroadHistory;           // offset 0x40, size 0x4, Decl: 176
    int mCurrOffroadID;                     // offset 0x44, size 0x4, Decl: 177
    unsigned int mTracking;                 // offset 0x48, size 0x4, Decl: 178
    Timer mT_gasstationexpl;                // offset 0x4C, size 0x4, Decl: 179
    int mPrevPursuitState;                  // offset 0x50, size 0x4, Decl: 180
    bool mWeather;                          // offset 0x54, size 0x1, Decl: 181
    bool mTunnel;                           // offset 0x58, size 0x1, Decl: 182
    Csis::Type_intensity mOutcomeIntensity; // offset 0x5C, size 0x4, Decl: 183
    EAXCop *mRamCop;                        // offset 0x60, size 0x4, Decl: 184
    float mAirborneHeight;                  // offset 0x64, size 0x4, Decl: 185
    float mAirborneLength;                  // offset 0x68, size 0x4, Decl: 186

    UMath::Vector3 mGasStationPos; // offset 0x6C, size 0xC, Decl: 195
    UMath::Vector3 mFwPlayer;      // offset 0x78, size 0xC, Decl: 196
    UMath::Vector3 mFwRoad;        // offset 0x84, size 0xC, Decl: 197

    Hermes::HHANDLER mMsgBlewPastCop; // offset 0x90, size 0x4, Decl: 199

    Hermes::HHANDLER mMsgGamePlayMoment; // offset 0x94, size 0x4, Decl: 202

    Hermes::HHANDLER mMsgNotifyEventCompletion; // offset 0x98, size 0x4, Decl: 208

    Hermes::HHANDLER mMsgTunnelUpdate; // offset 0x9C, size 0x4, Decl: 214
};

}; // namespace Speech

#endif
