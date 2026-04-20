#include "Observer.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifySpeechStatus.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace Speech {

Observer::Observer()
    : mObservations(), //
      mObserveMask(Braking | Outcome), //
      mLastEvent(kNone), //
      mNumCopsWithLOS(0), //
      mT_bullhorn(WorldTimer), //
      mT_unstable(WorldTimer), //
      mT_airborne(WorldTimer), //
      mT_flipped(WorldTimer), //
      mT_trackingOutcome(WorldTimer), //
      mDotTrack(0.0f), //
      mOffroadHistory(0), //
      mCurrOffroadID(-1), //
      mTracking(0), //
      mT_gasstationexpl(WorldTimer), //
      mPrevPursuitState(2), //
      mWeather(false), //
      mTunnel(false), //
      mOutcomeIntensity(Csis::Type_intensity_Normal), //
      mRamCop(0), //
      mAirborneHeight(0.0f), //
      mAirborneLength(0.0f), //
      mGasStationPos(UMath::Vector3::kZero), //
      mFwPlayer(UMath::Vector3::kZero), //
      mFwRoad(UMath::Vector3::kZero), //
      mMsgBlewPastCop(0), //
      mMsgGamePlayMoment(0), //
      mMsgNotifyEventCompletion(0), //
      mMsgTunnelUpdate(0) {
    mMsgBlewPastCop = Hermes::Handler::Create<MGamePlayMoment, Observer, Observer>(this, &Observer::MessageBlewPastCop, "BlewByCop", 0);
    mMsgGamePlayMoment = Hermes::Handler::Create<MGamePlayMoment, Observer, Observer>(this, &Observer::MessageGamePlayMoment, "MomentStrm", 0);
    mMsgNotifyEventCompletion =
        Hermes::Handler::Create<MNotifySpeechStatus, Observer, Observer>(this, &Observer::MessageEventComplete, UCrc32(0x20d60dbf), 0);
    mMsgTunnelUpdate = Hermes::Handler::Create<MMiscSound, Observer, Observer>(this, &Observer::MessageTunnelUpdate, "TunnelUpdate", 0);
}

Observer::~Observer() {
    if (mMsgBlewPastCop) {
        Hermes::Handler::Destroy(mMsgBlewPastCop);
        mMsgBlewPastCop = 0;
    }
    if (mMsgGamePlayMoment) {
        Hermes::Handler::Destroy(mMsgGamePlayMoment);
        mMsgGamePlayMoment = 0;
    }
    if (mMsgNotifyEventCompletion) {
        Hermes::Handler::Destroy(mMsgNotifyEventCompletion);
        mMsgNotifyEventCompletion = 0;
    }
    if (mMsgTunnelUpdate) {
        Hermes::Handler::Destroy(mMsgTunnelUpdate);
        mMsgTunnelUpdate = 0;
    }
}

void Observer::Update() {
    switch (mState) {
    case kTransition:
        return;
    case kCullCheck:
        CullCheck();
        return;
    default:
        ChangeStateTo(kCullCheck);
        return;
    }
}

void Observer::CullCheck() {
    Process();
}

bool Observer::IsTransitionable() {
    return mState == kTransition;
}

void Observer::Reset() {
    mObservations.clear();
    mObserveMask = Braking | Outcome;
    mLastEvent = kNone;
    mNumCopsWithLOS = 0;
    mT_bullhorn = WorldTimer;
    mT_unstable = WorldTimer;
    mT_airborne = WorldTimer;
    mT_flipped = WorldTimer;
    mT_trackingOutcome = WorldTimer;
    mDotTrack = 0.0f;
    mOffroadHistory = 0;
    mCurrOffroadID = -1;
    mTracking = 0;
    mT_gasstationexpl = WorldTimer;
    mPrevPursuitState = 2;
    mWeather = false;
    mTunnel = false;
    mOutcomeIntensity = Csis::Type_intensity_Normal;
    mRamCop = 0;
    mAirborneHeight = 0.0f;
    mAirborneLength = 0.0f;
    mGasStationPos = UMath::Vector3::kZero;
    mFwPlayer = UMath::Vector3::kZero;
    mFwRoad = UMath::Vector3::kZero;
    mBusy = 0;
    mState = kCullCheck;
    mLastState = kTransition;
}

void Observer::Observe(int currobsrvation, int speaker, float score) {
    SpeechObservation obs;
    obs.observation = currobsrvation;
    obs.speaker = speaker;
    obs.score = score;
    mLastEvent = static_cast<SpeechObservations>(currobsrvation);
    mObservations.push_back(obs);
}

void Observer::Process() {
    observations::iterator it = mObservations.begin();
    while (it != mObservations.end()) {
        const SpeechObservation &obs = *it;
        if ((obs.observation >= kCollision_Cop_Cop) && (obs.observation <= kCollision_Suspect_Spikebelt)) {
            SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
            if (ai && ai->GetLeader()) {
                ai->GetLeader()->Collision(obs.observation, obs.score, mRamCop);
            }
        }
        ++it;
    }
    mObservations.clear();
    AssessLOS();
    AssessOutcome();
    AssessBraking();
    AssessOutrun();
    AssessOffroad();
    AssessArrest();
}

float Observer::CalcFWVec_Road_Car() {
    return UMath::Dot(mFwRoad, mFwPlayer);
}

void Observer::GasStationAftermath() {
    if ((WorldTimer - mT_gasstationexpl).GetSeconds() > 5.0f) {
        mGasStationPos = UMath::Vector3::kZero;
    }
}

void Observer::AssessArrest() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }
    if (ai->GetPursuitState() == SoundAI::kInactive) {
        mTracking |= Arrest;
    }
}

void Observer::AssessLOS() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    int prev = mNumCopsWithLOS;
    mNumCopsWithLOS = ai->NumCopsWithLOS();
    if (ai->GetLeader()) {
        if ((prev > 0) && (mNumCopsWithLOS <= 0)) {
            ai->GetLeader()->LostVisual();
        } else if ((prev <= 0) && (mNumCopsWithLOS > 0)) {
            ai->GetLeader()->RegainVisual();
        }
    }
}

void Observer::NotifyAirborne(float alt, float t) {
    mAirborneHeight = alt;
    mAirborneLength = t;
    mT_airborne = WorldTimer;
}

void Observer::AssessFlippage() {
    if (mAirborneHeight > 1.0f) {
        mTracking |= UTurnResult;
        mT_flipped = WorldTimer;
    }
}

void Observer::Assess180() {
    float dot = CalcFWVec_Road_Car();
    mDotTrack = dot;
    if (dot < -0.2f) {
        mTracking |= UTurn;
    }
}

void Observer::AssessOutcome() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }

    int cur = static_cast<int>(ai->GetPursuitState());
    if (cur != mPrevPursuitState) {
        mTracking |= Outcome;
        mT_trackingOutcome = WorldTimer;
        if (cur == SoundAI::kSearching) {
            mOutcomeIntensity = Csis::Type_intensity_Normal;
        } else if (cur == SoundAI::kInactive) {
            mOutcomeIntensity = Csis::Type_intensity_High;
        }
        mPrevPursuitState = cur;
    }
}

void Observer::AssessBraking() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai) {
        return;
    }
    if ((ai->GetPlayerSpeed() < 7.0f) && ai->GetLeader()) {
        mTracking |= Braking;
        ai->GetLeader()->SuspectBrake();
    }
}

void Observer::AssessOutrun() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai || !ai->GetLeader()) {
        return;
    }
    if (ai->GetPursuitState() == SoundAI::kSearching) {
        mTracking |= Lost;
        ai->GetLeader()->SuspectOutrun();
    }
}

void Observer::AssessOffroad() {
    SoundAI *ai = UTL::Collections::Singleton<SoundAI>::Get();
    if (!ai || !ai->GetLeader()) {
        return;
    }

    unsigned int id = ai->GetPlayerOffroadID();
    if (static_cast<int>(id) != mCurrOffroadID) {
        mCurrOffroadID = static_cast<int>(id);
        mOffroadHistory <<= 1;
        mOffroadHistory |= (id != 0) ? 1 : 0;
        ai->GetLeader()->Offroad(id, (mOffroadHistory & 1) != 0);
    }
}

void Observer::MessageEventComplete(const MNotifySpeechStatus &) {
    mBusy = 0;
}

void Observer::MessageBlewPastCop(const MGamePlayMoment &) {
    Observe(kCollision_Suspect_Traffic, -1, 1.0f);
}

void Observer::MessageTunnelUpdate(const MMiscSound &message) {
    mTunnel = message.GetSoundID() != 0;
}

void Observer::MessageGamePlayMoment(const MGamePlayMoment &message) {
    if (message.GetAttribKey() != 0) {
        Observe(kCollision_Cop_Suspect, -1, 1.0f);
    }
}

void Observer::DetachRamCop() {
    mRamCop = 0;
}

EAXCop *Observer::GetRamCop() {
    return mRamCop;
}

bool Observer::WeatherExists() {
    return mWeather;
}

bool Observer::PlayerInTunnel() {
    return mTunnel;
}

void Observer::SetMask(unsigned int mask) {
    mObserveMask = mask;
}

SpeechObservations Observer::GetLastEvent() {
    return mLastEvent;
}

} // namespace Speech
