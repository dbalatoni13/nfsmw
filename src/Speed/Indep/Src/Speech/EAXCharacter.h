//
//
//
//
//
//
//
//
//
//
//
//
//
#ifndef __EAXCHARACTER_H
#define __EAXCHARACTER_H 1

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/EAXSound/SND_GEN/COPSPEECH.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/P2temp.h"
#include "Speed/Indep/Src/Speech/MWRoadNames.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

namespace Speech {

// total size: 0x8
// Decl: 34
struct History {
    // Decl: 35
    History() {
        this->time = Timer(0);
        this->count = 0;
        this->speakers = 0;
    }

    // Decl: 42
    void Touch(unsigned short spkrID) {
        this->time = WorldTimer;
        this->count = this->count + 1;
        if (spkrID < 10 && ((this->speakers >> spkrID & 1) == 0)) {
            this->speakers = this->speakers | (1 << spkrID);
        }
    }

    Timer time;              // offset 0x0, size 0x4, Decl: 50
    unsigned short count;    // offset 0x4, size 0x2, Decl: 51
    unsigned short speakers; // offset 0x6, size 0x2, Decl: 52
};

// total size: 0xC
// Decl: 58
class HistoryPair {
  public:
    HistoryPair() : id(kSPCH1_EventID_MaxEventID) {} // Decl: 59

    SPCHType_1_EventID id; // offset 0x0, size 0x4, Decl: 61
    History history;       // offset 0x4, size 0x8, Decl: 62

    // Decl: 63
    bool operator<(const Speech::HistoryPair &from) const {
        return this->id < from.id;
    }
};

}; // namespace Speech

// total size: 0x1
// Decl: 68
class MiscSpeech {
  public:
    static void ForcedRhinoArrival(int speakerid);
    static void CrossDialog(int ID);
    static void PlayE3Specific();
    static bool GetSPAMLocation(int SPAMID, Csis::Type_offroad_moment_id &id);
    static bool GetLocation(RoadNames id, Csis::Type_location_region &region, Csis::Type_location &location);
    static void RBWarning();
    static void RBPosition(int pos);
    static void RBEngaged(bool spikes_hit);
    static void RBAverted();
    static void SwarmingReply();
    static void SuperPursuitReply();
    static void SwarmingReplyFollow();
    static void QuadrantForming();
    static void SuspectPossiblyGone();
    static void QuadrantMoving();
    static void OtherLead();
    static void PossibleSuspect();
    static void WrongSuspect();
    static void SuspectGone();
    static void D_Day();
    static void DispIntroRace();
    static int MoreDetails(int spkrID);
    static int Unit911Reply(int spkrID);
    static int LostSuspect(int spkrID);
    static int Bailout(int spkrID);
    static void SMSCellCall(int SMS_ID);
    static bool MapSMSToSPCHEnums(int SMS_ID, Csis::CellCallStruct &data);
    static bool IsSMSValid(int SMS_ID);
    static bool IsVehicleTypeOK();
};

// total size: 0x40
// Decl: 100
class EAXCharacter : public AudioMemBase {
  public:
    void *operator new(size_t obj_size);
    void operator delete(void *ptr);
    EAXCharacter(int sID, HSIMABLE wID, int bID, int cID);
    ~EAXCharacter();

    virtual void Ack();
    virtual void Deny();
    virtual void InterruptStatic();
    virtual void InterruptExpletive();
    virtual void InterruptViolent();
    virtual void InterruptComposedLow();
    virtual void InterruptComposedHigh();
    virtual void DriverHistory();
    virtual void HeatJump(Csis::Type_heat_level heat);

    virtual HSIMABLE GetHandle() {
        return this->mHandle;
    }
    virtual void SetHandle(HSIMABLE handle) {
        this->mHandle = handle;
    }

    virtual int GetSpeakerID() {
        return this->mSpeakerID;
    }

    virtual int GetCallsign() {
        return this->mCallsign.name;
    }

    virtual int GetUnitNumber() {
        return this->mCallsign.number;
    }

    virtual void SetCallsign(int callsign) {
        this->mCallsign.name = callsign;
    }

    virtual void SetUnitNumber(int unitnum) {
        this->mCallsign.number = unitnum;
    }

    virtual void SetSpeakerID(int spkrID) {
        this->mSpeakerID = spkrID;
    }

    virtual void SetPosition(const UMath::Vector3 &v) {
        UMath::Copy(v, this->mPos);
    }

    virtual const UMath::Vector3 GetPosition() {
        return this->mPos;
    }
    virtual void SetSpeed(const float speed) {
        this->mSpeed = speed;
    }

    virtual void Update();

    virtual float GetDistance() {
        return this->mDistance;
    }
    virtual float GetHealth() {
        return this->mHealth;
    }
    virtual bool IsActive() {
        return this->mActive;
    }
    virtual void SetActive(bool active) {
        this->mActive = active;
    }
    virtual float GetSpeed() {
        return this->mSpeed;
    }
    virtual bool IsDead() {
        return this->mDestroyed;
    }
    virtual bool HasLOS() {
        return this->mSuspectLOS;
    }
    virtual void SetLOS(bool yes) {
        this->mSuspectLOS = yes;
    }

    virtual Csis::Type_code GetRandomizedCode() {
        return Csis::Type_code_dont_use_10_code;
    }

    virtual void Reset();

  protected:
    int mTimeLastSpoken; // offset 0x4, size 0x4, Decl: 144
    int mLastEvent;      // offset 0x8, size 0x4

    int mSpeakerID;              // offset 0xC, size 0x4
    HSIMABLE mHandle;            // offset 0x10, size 0x4
    Speech::Battalion mCallsign; // offset 0x14, size 0x8
    UMath::Vector3 mPos;         // offset 0x1C, size 0xC
    float mSpeed;                // offset 0x28, size 0x4
    float mDistance;             // offset 0x2C, size 0x4
    float mHealth;               // offset 0x30, size 0x4
    bool mDestroyed;             // offset 0x34, size 0x1
    bool mActive;                // offset 0x38, size 0x1
    bool mSuspectLOS;            // offset 0x3C, size 0x1, Decl: 156
};

// total size: 0x84
// Decl: 161
class EAXCop : public EAXCharacter {
  public:
    EAXCop(int speakerID, HSIMABLE handle, int bID, int cID);
    ~EAXCop() override;

    bool operator<(const EAXCop &from) const {}

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
        return this->mSpeakerID == Speech::Cross;
    }
    virtual void SetInFormation(bool yes) {
        this->mInFormation = yes;
    }
    virtual bool GetInFormation() {
        return this->mInFormation;
    }
    virtual void SetInPosition(bool yes) {
        this->mInPosition = yes;
    }
    virtual bool GetInPosition() {
        return this->mInPosition;
    }
    virtual void SetTgtOffset(const UMath::Vector3 &off) {
        UMath::Copy(off, this->mTgtOffset);
    }

    virtual const UMath::Vector3 GetTgtOffset() {
        return this->mTgtOffset;
    }

    virtual bool SetRank(int newrank);

    virtual bool GetRank() {
        return this->mRank != 0;
    }

    // Overrides: EAXCharacter
    void Update() override;
    void Reset() override;
    void SetActive(bool activity) override;

    virtual void JustHitTraffic() {
        this->mTrafficHitCount++;
    }

    virtual void WasRammed() {
        this->mNumRammed++;
        this->mLastRammedTime = WorldTimer;
    }

    virtual int GetTimesRammed() {
        return this->mNumRammed;
    }

    virtual float GetTimeLastSeen() {
        return (WorldTimer - this->mTimeNoLOS).GetSeconds();
    }
    virtual float GetTimeAirborne() {
        return (WorldTimer - this->mTimeAirborne).GetSeconds();
    }
    virtual float GetTimeLastRammed() {
        return (WorldTimer - this->mLastRammedTime).GetSeconds();
    }

    virtual float GetTimeLastClosing() {
        return (WorldTimer - this->mT_closingDist).GetSeconds();
    }

    // TODO bug?
    virtual float IsAhead() {
        return static_cast<float>(this->mAhead);
    }

    virtual void SetAhead(bool ahead) {
        this->mAhead = ahead;
    }

  private:
    virtual void Impact_Suspect_World();
    virtual void Impact_Suspect_Semi();
    virtual void Impact_Suspect_Train();
    virtual void Impact_Suspect_Guardrail();
    virtual void Impact_Suspect_GasStation();
    virtual void Impact_Suspect_Spikebelt();
    virtual void Impact_Suspect_Traffic(Csis::Type_intensity intensity);

    int GetBackupTypeFromDispatch(int type);

    int mRank;                 // offset 0x40, size 0x4
    bool mInFormation;         // offset 0x44, size 0x1
    bool mInPosition;          // offset 0x48, size 0x1
    bool mAhead;               // offset 0x4C, size 0x1
    float mPctTractiveTires;   // offset 0x50, size 0x4
    Timer mTimeAirborne;       // offset 0x54, size 0x4
    int mTrafficHitCount;      // offset 0x58, size 0x4
    Timer mTimeNoLOS;          // offset 0x5C, size 0x4
    int mNumRammed;            // offset 0x60, size 0x4
    Timer mLastRammedTime;     // offset 0x64, size 0x4
    Timer mT_lastactivity;     // offset 0x68, size 0x4
    Timer mT_closingDist;      // offset 0x6C, size 0x4
    RoadNames mOrigin;         // offset 0x70, size 0x4
    RoadNames mCurrRoad;       // offset 0x74, size 0x4
    UMath::Vector3 mTgtOffset; // offset 0x78, size 0xC
};

// total size: 0x40
// Decl: 312
class EAXDispatch : public EAXCharacter {
  public:
    friend class SoundAI;
    ~EAXDispatch() override;

    // Overrides: EAXCharacter
    void Update() override;

    void PursuitUpdate(EAXCop *cop);

    void BackupReply(EAXCop *cop, int yes, int type);

    void ArrestReply();

    void BackupUpdate(EAXCop *cop, int yes);

    void BreakAway();

    void GoAhead();

    void Report911(Csis::Type_pursuit_type infraction);

    void TimeExpired();

    void PursuitEscalation();

    void PursuitEscalationGeneric();

    void EVReply();

    void JurisShift(Csis::Type_jurisdiction jurisdiction);

    void BackupETA();

    void VehicleDescription();

    void NoVehicleDescription();

    void VehicleVinyls();

    void VehicleCustomPaint();

    void RBReply(EAXCop *cop, int8 true_false, unsigned int type);

    void RBUpdate(EAXCop *cop, int8 true_false);

    void SubRBReply();

  private:
    EAXDispatch(int sID) : EAXCharacter(sID, nullptr, 0, 0) {}
    // const EAXCharacter & operator=(const EAXCharacter &) {}
};

// total size: 0x84
// Decl: 351
class EAXAirSupport : public EAXCop {
  public:
    EAXAirSupport(int speakerID, HSIMABLE handle) : EAXCop(speakerID, handle, 64, 1) {}
    ~EAXAirSupport() override;

    void SelfStrategy(int type) override;
    virtual void JoinRB();
    void LostVisual() override;
    virtual void IntentToBail();
    void Bailout() override;
    virtual void Swarming();
    void Spotter() override;
    virtual void HazardAlert(Csis::Type_heli_hazard_alert_type type);
    void BullhornArrest() override;
    virtual void Quadrant();
    virtual void QuadrantMoving();

    // Overrides: EAXCharacter
    void Update() override;

    // Overrides: EAXCop
    bool IsHeli() override {
        return true;
    }

  private:
    EAXAirSupport();
    const EAXAirSupport &operator=(const EAXAirSupport &);

    Csis::Type_heli_bailout_type GetCauseOfBailout();
};

#endif
