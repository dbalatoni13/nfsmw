#ifndef SPEECH_EAXCOP_H
#define SPEECH_EAXCOP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "EAXCharacter.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

// TODO move
namespace Csis {
enum Type_intensity {
    Type_intensity_Normal = 1,
    Type_intensity_High = 2,
};
}

// total size: 0x84
class EAXCop : public EAXCharacter {
  public:
    EAXCop(int speakerID, HSIMABLE handle, int bID, int cID);
    int GetBackupTypeFromDispatch(int type);

    // Virtual overrides
    // AudioMemBase
    ~EAXCop() override;

    //  EAXCharacter
    void Update() override;
    void SetActive(bool activity) override;
    void Reset() override;

    // Virtual functions
    virtual void AttemptVehicleStop();
    virtual void VehicleReport();
    virtual void InitiatePursuit();
    virtual void LocationReport();
    virtual void SelfStrategy(int type);
    virtual void InitialCallForBackup();
    virtual void CallForBackup(int type);
    virtual void UnitBackupReply();
    virtual void InitiateStrategy(int type);
    virtual void CallToPosition(EAXCop *cop);
    virtual void CallToPositionReminder();
    virtual void StrategyExecute();
    virtual void Collision(int collisionType, float force, EAXCop *spkr);
    virtual void AnticipateSuccess();
    virtual void AnticipateFail();
    virtual void OutcomeFail(short intensity);
    virtual void StrategyReset(bool new_strategy);
    virtual void SuspectBehavior();
    virtual void SuspectOutrun();
    virtual void SuspectUTurn();
    virtual void LostSuspect();
    virtual void LostVisual();
    virtual void RegainVisual();
    virtual void SwarmingReply();
    virtual void Arrest();
    virtual void PursuitUpdateReply();
    virtual void ReinitiatePursuit();
    virtual void NegativeBackupReply();
    virtual void BackupReminder(int type);
    virtual void BackupArrives();
    virtual void IntentToRam();
    virtual void CallforEV(unsigned int type);
    virtual void UnitDisabled(int other);
    virtual void Bailout();
    virtual void LoBailout();
    virtual void HiBailout();
    virtual void BailoutTraffic();
    virtual void BailoutBadRoad();
    virtual void Spotter();
    virtual void SpotterReply();
    virtual void Reply911();
    virtual void DenyBailout();
    virtual void PrimaryEngage();
    virtual void Bullhorn();
    virtual void PreBullhorn();
    virtual void BullhornArrest();
    virtual void SuspectConfirmed();
    virtual void FocusChange();
    virtual void Spotted();
    virtual void DirectionChange();
    virtual void CallForSwarming();
    virtual void SpotterWanted();
    virtual void Offroad(unsigned int id, bool subsequent);
    virtual void WeatherReport();
    virtual void CallForRB();
    virtual void RBReminder();
    virtual void NegRBReply();
    virtual void RBApproach();
    virtual void RBEngage(bool spikes_hit);
    virtual void PursuitApproaching();
    virtual void RBAverted();
    virtual void CallForSubRB();
    virtual void RearEnded(Csis::Type_intensity intensity);
    virtual void HeadOn(Csis::Type_intensity intensity);
    virtual void SideSwiped(Csis::Type_intensity intensity);
    virtual void TBoned(Csis::Type_intensity intensity);
    virtual void SuspectRollover(Csis::Type_intensity intensity);
    virtual void SuspectAirborne(Csis::Type_intensity intensity);
    virtual void SuspectSpunout(Csis::Type_intensity intensity);
    virtual void SuspectBrake();
    virtual void SwapVoices(EAXCop *cop);
    virtual bool IsPrimary();

    virtual bool IsHeli() {
        return false;
    }

    virtual bool IsCross() {
        return mSpeakerID == 9;
    }

    virtual void SetInFormation(bool yes) {
        *reinterpret_cast<unsigned int *>(&mInFormation) = yes;
    }

    virtual int GetInFormation() {
        return *reinterpret_cast<unsigned int *>(&mInFormation);
    }

    virtual void SetInPosition(bool yes) {
        *reinterpret_cast<unsigned int *>(&mInPosition) = yes;
    }

    virtual int GetInPosition() {
        return *reinterpret_cast<unsigned int *>(&mInPosition);
    }

    virtual void SetTgtOffset(const UMath::Vector3 &off) {
        mTgtOffset = off;
    }

    virtual const UMath::Vector3 GetTgtOffset() {
        return mTgtOffset;
    }

    virtual bool SetRank(int newrank);

    virtual bool GetRank() {
        return mRank != 0;
    }

    virtual void JustHitTraffic() {
        mTrafficHitCount++;
    }

    virtual void WasRammed() {
        mNumRammed++;
        mLastRammedTime = WorldTimer;
    }

    virtual int GetTimesRammed() {
        return mNumRammed;
    }

    virtual float GetTimeLastSeen() {
        return (WorldTimer - mTimeNoLOS).GetSeconds();
    }

    virtual float GetTimeAirborne() {
        return (WorldTimer - mTimeAirborne).GetSeconds();
    }

    virtual float GetTimeLastRammed() {
        return (WorldTimer - mLastRammedTime).GetSeconds();
    }

    virtual float GetTimeLastClosing() {
        return (WorldTimer - mT_closingDist).GetSeconds();
    }

    virtual float IsAhead() {
        return static_cast<float>(*reinterpret_cast<unsigned int *>(&mAhead));
    }

    virtual void SetAhead(bool ahead) {
        *reinterpret_cast<unsigned int *>(&mAhead) = ahead;
    }

    bool operator<(const struct EAXCop &from) const;

  private:
    virtual void Impact_Suspect_World();

    virtual void Impact_Suspect_Semi();

    virtual void Impact_Suspect_Train();

    virtual void Impact_Suspect_Guardrail();

    virtual void Impact_Suspect_GasStation();

    virtual void Impact_Suspect_Spikebelt();

    virtual void Impact_Suspect_Traffic(Csis::Type_intensity intensity);

    int mRank;                    // offset 0x40, size 0x4
    bool mInFormation;            // offset 0x44, size 0x1
    bool mInPosition;             // offset 0x48, size 0x1
    bool mAhead;                  // offset 0x4C, size 0x1
    float mPctTractiveTires;      // offset 0x50, size 0x4
    Timer mTimeAirborne;          // offset 0x54, size 0x4
    int mTrafficHitCount;         // offset 0x58, size 0x4
    struct Timer mTimeNoLOS;      // offset 0x5C, size 0x4
    int mNumRammed;               // offset 0x60, size 0x4
    struct Timer mLastRammedTime; // offset 0x64, size 0x4
    struct Timer mT_lastactivity; // offset 0x68, size 0x4
    struct Timer mT_closingDist;  // offset 0x6C, size 0x4
    RoadNames mOrigin;            // offset 0x70, size 0x4
    RoadNames mCurrRoad;          // offset 0x74, size 0x4
    UMath::Vector3 mTgtOffset;    // offset 0x78, size 0xC
};

#endif
