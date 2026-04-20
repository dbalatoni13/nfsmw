#ifndef SPEECH_OBSERVER_H
#define SPEECH_OBSERVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "EAXCop.h"
#include "SpeechFlow.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct MGamePlayMoment;
struct MMiscSound;
struct MNotifySpeechStatus;

namespace Speech {

enum SpeechObservations {
    kNone = 0,
    kCollision_Cop_Cop = 1,
    kCollision_Cop_Traffic = 2,
    kCollision_Cop_Suspect = 3,
    kCollision_Cop_World = 4,
    kCollision_Suspect_World = 5,
    kCollision_Suspect_Suspect = 6,
    kCollision_Suspect_Traffic = 7,
    kCollision_Suspect_Structure = 8,
    kCollision_Suspect_Tree = 9,
    kCollision_Suspect_Guardrail = 10,
    kCollision_Suspect_Train = 11,
    kCollision_Suspect_Semi = 12,
    kCollision_Suspect_GasStation = 13,
    kCollision_Suspect_Spikebelt = 14,
};

struct SpeechObservation {
    int observation;
    int speaker;
    float score;
};

DECLARE_CONTAINER_TYPE(observations);

struct observations : public UTL::Std::list<SpeechObservation, _type_observations>, public AudioMemBase {};

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

    Observer();
    ~Observer() override;
    void Update() override;
    void CullCheck();
    bool IsTransitionable() override;
    void Reset() override;
    void Observe(int currobsrvation, int speaker, float score);
    void Process();
    float CalcFWVec_Road_Car();
    void GasStationAftermath();
    void AssessArrest();
    void AssessLOS();
    void NotifyAirborne(float alt, float t);
    void AssessFlippage();
    void Assess180();
    void AssessOutcome();
    void AssessBraking();
    void AssessOutrun();
    void AssessOffroad();
    void MessageEventComplete(const MNotifySpeechStatus &message);
    void MessageBlewPastCop(const MGamePlayMoment &message);
    void MessageTunnelUpdate(const MMiscSound &message);
    void MessageGamePlayMoment(const MGamePlayMoment &message);
    void DetachRamCop();
    EAXCop *GetRamCop();
    bool WeatherExists();
    bool PlayerInTunnel();
    void SetMask(unsigned int mask);
    SpeechObservations GetLastEvent();

  private:
    observations mObservations;                 // offset 0x10, size 0xC
    unsigned int mObserveMask;                  // offset 0x1C, size 0x4
    SpeechObservations mLastEvent;              // offset 0x20, size 0x4
    int mNumCopsWithLOS;                        // offset 0x24, size 0x4
    Timer mT_bullhorn;                          // offset 0x28, size 0x4
    Timer mT_unstable;                          // offset 0x2C, size 0x4
    Timer mT_airborne;                          // offset 0x30, size 0x4
    Timer mT_flipped;                           // offset 0x34, size 0x4
    Timer mT_trackingOutcome;                   // offset 0x38, size 0x4
    float mDotTrack;                            // offset 0x3C, size 0x4
    unsigned int mOffroadHistory;               // offset 0x40, size 0x4
    int mCurrOffroadID;                         // offset 0x44, size 0x4
    unsigned int mTracking;                     // offset 0x48, size 0x4
    Timer mT_gasstationexpl;                    // offset 0x4C, size 0x4
    int mPrevPursuitState;                      // offset 0x50, size 0x4
    bool mWeather;                              // offset 0x54, size 0x1
    bool mTunnel;                               // offset 0x58, size 0x1
    Csis::Type_intensity mOutcomeIntensity;     // offset 0x5C, size 0x4
    EAXCop *mRamCop;                            // offset 0x60, size 0x4
    float mAirborneHeight;                      // offset 0x64, size 0x4
    float mAirborneLength;                      // offset 0x68, size 0x4
    UMath::Vector3 mGasStationPos;              // offset 0x6C, size 0xC
    UMath::Vector3 mFwPlayer;                   // offset 0x78, size 0xC
    UMath::Vector3 mFwRoad;                     // offset 0x84, size 0xC
    Hermes::HHANDLER mMsgBlewPastCop;           // offset 0x90, size 0x4
    Hermes::HHANDLER mMsgGamePlayMoment;        // offset 0x94, size 0x4
    Hermes::HHANDLER mMsgNotifyEventCompletion; // offset 0x98, size 0x4
    Hermes::HHANDLER mMsgTunnelUpdate;          // offset 0x9C, size 0x4
};

} // namespace Speech

#endif
