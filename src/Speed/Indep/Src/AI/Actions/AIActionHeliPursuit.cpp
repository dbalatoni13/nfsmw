#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IHelicopter.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

// total size: 0x48
class AIActionHeliPursuit : public AIAction, public Debugable, public Sim::Collision::IListener {
  public:
    enum kPursuitMode {
        kStraight_Line = 0,
        kSearch_Pattern = 1,
        kSkid_Hit_Approach = 2,
        kSkid_Hit_Strike = 3,
    };

    static AIAction *Construct(struct AIActionParams *params);

    AIActionHeliPursuit(AIActionParams *params, float score);
    void StartPathToPlayerCar();
    float UpdateNavPos(float lookAheadDistance);
    void StraightLinePursuit();
    void SkidHitPursuit();
    void SetNextPerpSearchDest();
    void SearchForPerp();

    // Virtual functions
    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AIActionHeliPursuit() override {
        Sim::Collision::RemoveListener(this);
    }

    // AIAction
    bool CanBeAttempted(float dT) override {
        return true;
    }
    void BeginAction(float dT) override;
    bool IsFinished() override;
    void FinishAction(float dT) override;
    void Update(float dT) override;

    void OnBehaviorChange(const UCrc32 &mechanic) override {}

    bool ShouldRestartWhenFinished() override {
        return true;
    }

    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;

    WRoadNav *GetPursuitNav() {}

  private:
    IVehicleAI *mIVehicleAI;                  // offset 0x50, size 0x4
    IVehicle *mIVehicle;                      // offset 0x54, size 0x4
    IRigidBody *mIRigidBody;                  // offset 0x58, size 0x4
    IAIHelicopter *mIAIHelicopter;            // offset 0x5C, size 0x4
    IPursuitAI *mIPursuitAI;                  // offset 0x60, size 0x4
    float mPursuitTime;                       // offset 0x64, size 0x4
    float mSkidKnockTimer;                    // offset 0x68, size 0x4
    float mPathTime;                          // offset 0x6C, size 0x4
    bool mBuildingPath;                       // offset 0x70, size 0x1
    float mSearchPatternAngle;                // offset 0x74, size 0x4
    ALIGN_16 UMath::Vector3 mSearchDestPoint; // offset 0x78, size 0xC
    IRigidBody *mPlayerRigidBody;             // offset 0x84, size 0x4
    ALIGN_16 UMath::Vector3 mPlayerPosition;  // offset 0x88, size 0xC
    ALIGN_16 UMath::Vector3 mSkidHitOffset;   // offset 0x94, size 0xC
    int mCollisionAbort;                      // offset 0xA0, size 0x4
    float mPlayerSpeed;                       // offset 0xA4, size 0x4
    kPursuitMode mPursuitMode;                // offset 0xA8, size 0x4
};

AIActionHeliPursuit::AIActionHeliPursuit(AIActionParams *params, float score)
    : AIAction(params, score),                 //
      mSearchDestPoint(UMath::Vector3::kZero), //
      mPursuitTime(0.0f),                      //
      mSkidKnockTimer(0.0f),                   //
      mPathTime(0.0f),                         //
      mSearchPatternAngle(0.0f),               //
      mBuildingPath(false),                    //
      mPlayerSpeed(0.0f),                      //
      mPursuitMode(kStraight_Line),            //
      mCollisionAbort(0) {
    this->MakeDebugable(DBG_AI);

    params->mOwner->QueryInterface(&this->mIVehicleAI);
    params->mOwner->QueryInterface(&this->mIPursuitAI);
    params->mOwner->QueryInterface(&this->mIVehicle);
    params->mOwner->QueryInterface(&mIAIHelicopter);
    this->mIRigidBody = params->mOwner->GetRigidBody();
    Sim::Collision::AddListener(this, this->mIRigidBody, "AIActionHeliPursuit");
}

void AIActionHeliPursuit::OnCollision(const COLLISION_INFO &cinfo) {
    if (this->mSkidKnockTimer <= 0.0f) {
        return;
    }
    if (cinfo.type == COLLISION_INFO::GROUND) {
        this->mCollisionAbort += 2;
    } else if (cinfo.type == COLLISION_INFO::OBJECT) {
        ISimable *obja = ISimable::FindInstance(cinfo.objA);
        if (obja == nullptr || !obja->IsPlayer()) {
            ISimable *objb = ISimable::FindInstance(cinfo.objB);
            if (objb != nullptr && objb->IsPlayer()) {
                this->mCollisionAbort += 5;
            }
        } else {
            this->mCollisionAbort += 5;
        }
    }

    if (this->mCollisionAbort > 20) {
        this->mSkidKnockTimer = 0.0f;
    }
}

AIAction *AIActionHeliPursuit::Construct(AIActionParams *params) {
    return new AIActionHeliPursuit(params, AIACTION_SCORE_LOW);
}

bool AIActionHeliPursuit::IsFinished() {
    return this->mPursuitTime > 10.0f;
}

void AIActionHeliPursuit::FinishAction(float dT) {
    this->mPursuitTime = 0.0f;
    this->mSkidKnockTimer = 0.0f;
}

void AIActionHeliPursuit::StartPathToPlayerCar() {
    IRigidBody *mPlayerRigidBody = IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody();
    UMath::Vector3 seekPosition = mPlayerRigidBody->GetPosition();
    mIAIHelicopter->StartPathToPoint(seekPosition);
    this->mBuildingPath = true;
    this->mPathTime = this->mPursuitTime;
}

void AIActionHeliPursuit::BeginAction(float dT) {
    this->StartPathToPlayerCar();
}

void AIActionHeliPursuit::StraightLinePursuit() {
    UMath::Vector3 myPosition = this->mIRigidBody->GetPosition();
    UMath::Vector3 seekPosition;
    UMath::Vector3 direction;
    UMath::Vector3 perp2Me;

    UMath::Vector3 perpUnitVel = this->mPlayerRigidBody->GetLinearVelocity();
    UMath::Unit(perpUnitVel, perpUnitVel);
    UMath::Sub(myPosition, this->mPlayerPosition, perp2Me);

    bool SkidHitEnabled = false;
    if (this->mIVehicleAI->GetPursuit()->SkidHitEnabled()) {
        SkidHitEnabled = this->mSkidKnockTimer < -5.0f;
    }
    float heliAheadOfPerpDot = UMath::Length(perp2Me);
    float distToPerp;
    if (heliAheadOfPerpDot < 35.0f && heliAheadOfPerpDot > 5.0f) {
        float ydif = perp2Me.y;
        perp2Me.y = 0.0f;
        perpUnitVel.y = 0.0f;
        if (UMath::Dot(perp2Me, perpUnitVel) / heliAheadOfPerpDot > 0.707f && this->mIVehicleAI != nullptr && SkidHitEnabled && ydif < 13.0f) {
            this->mPursuitMode = kSkid_Hit_Approach;
            this->mSkidKnockTimer = 8.0f;
            this->mSkidHitOffset = UMath::Vector3::kZero;
            SoundAI *copspeech = SoundAI::Get();
            if (copspeech != nullptr && copspeech->GetHeli() != nullptr) {
                int parm = bRandom(2) != 0 ? 2 : 4;
                copspeech->GetHeli()->SelfStrategy(parm);
            }
        }
    }

    this->mPlayerRigidBody->GetForwardVector(direction);
    UMath::Unit(direction, direction);

    float leadDist = this->mPlayerSpeed * 0.4f + 30.0f;
    if (leadDist > 45.0f) {
        leadDist = 45.0f;
    }
    if (SkidHitEnabled) {
        leadDist *= 0.75f;
    }

    UMath::ScaleAdd(direction, leadDist, this->mPlayerPosition, seekPosition);

    if (this->mSkidKnockTimer > 0.0f) {
        seekPosition.y += 2.0f;
    } else if (heliAheadOfPerpDot > 55.0f || this->mSkidKnockTimer > -5.0f) {
        seekPosition.y += 12.0f;
    } else {
        seekPosition.y += 6.0f;
    }

    float chaseHeight = seekPosition.y;
    seekPosition.y = 0.0f;

    UMath::Vector3 meToSeek;
    UMath::Sub(seekPosition, myPosition, meToSeek);
    meToSeek.y = 0.0f;
    float distToSeekPos = UMath::Normalize(meToSeek);

    float adjustDriving = UMath::Dot(meToSeek, this->mPlayerRigidBody->GetLinearVelocity());
    if (adjustDriving < 0.0f) {
        adjustDriving *= 0.5f;
    }
    float driveSpeed;
    float desiredClosingSpeed;
    UMath::Vector3 *lookAtPosition = &seekPosition;
    if (distToSeekPos > 15.0f) {
        desiredClosingSpeed = 30.0f;
        if (distToSeekPos > 300.0f) {
            desiredClosingSpeed = 50.0f;
        }
    } else {
        if (distToSeekPos > 3.0f) {
            desiredClosingSpeed = 20.0f;
        } else {
            desiredClosingSpeed = distToSeekPos;
        }
        if (distToSeekPos < 5.0f && this->mPlayerSpeed < 15.0f) {
            lookAtPosition = &this->mPlayerPosition;
        }
    }
    driveSpeed = adjustDriving + desiredClosingSpeed;
    this->mIVehicleAI->SetDriveSpeed(driveSpeed);

    mIAIHelicopter->SetDestinationVelocity(mPlayerRigidBody->GetLinearVelocity());

    seekPosition.y = chaseHeight;
    this->mIVehicleAI->SetDriveTarget(seekPosition);

    mIAIHelicopter->SetLookAtPosition(*lookAtPosition);

    this->mIVehicleAI->DoDriving(7);
}

void AIActionHeliPursuit::SkidHitPursuit() {
    UMath::Vector3 seekPosition;
    UMath::Vector3 myPosition = this->mIRigidBody->GetPosition();
    UMath::Vector3 perp2Me;
    UMath::Sub(myPosition, this->mPlayerPosition, perp2Me);

    UMath::Vector3 perpForward;
    this->mPlayerRigidBody->GetForwardVector(perpForward);

    UMath::Vector3 perpLinVel;
    perpLinVel = this->mPlayerRigidBody->GetLinearVelocity();

    UMath::Vector3 perpRightVec;
    this->mPlayerRigidBody->GetRightVector(perpRightVec);

    float dot = UMath::Dot(perpForward, perp2Me);
    float dist2PerpSQ = UMath::LengthSquare(perp2Me);
    float dotWithRight = UMath::Dot(perp2Me, perpRightVec);

    if (this->mPursuitMode == kSkid_Hit_Approach) {
        float rscale = 6.0f;
        if (dotWithRight < 0.0f) {
            rscale = -6.0f;
        }
        UMath::Scale(perpRightVec, rscale, this->mSkidHitOffset);
        UMath::Vector3 localOffset;
        UMath::ScaleAdd(perpLinVel, 0.23f, this->mSkidHitOffset, localOffset);
        localOffset.y += 1.8f;
        if (perp2Me.y < 2.0f) {
            localOffset.y += 3.0f;
        }
        UMath::Add(localOffset, this->mPlayerPosition, seekPosition);

        float dToStrikeStart = UMath::Distancexz(seekPosition, myPosition);
        if (dToStrikeStart < 4.0f && UMath::Abs(dotWithRight) > 1.9f) {
            this->mPursuitMode = kSkid_Hit_Strike;
            if (this->mSkidKnockTimer < 2.0f) {
                this->mSkidKnockTimer = 2.0f;
            }
            SoundAI *copspeech = SoundAI::Get();
            if (copspeech != nullptr && copspeech->GetHeli() != nullptr) {
                copspeech->GetHeli()->IntentToRam();
            }
        }
    } else {
        UMath::Vector3 localOffset;
        UMath::Scale(perpLinVel, 0.092f, localOffset);
        UMath::ScaleAdd(this->mSkidHitOffset, -0.5f, localOffset, localOffset);
        localOffset.y += 0.4f;
        UMath::Add(localOffset, this->mPlayerPosition, seekPosition);
    }

    UMath::Vector3 lookPosition;
    UMath::Add(this->mPlayerPosition, perpLinVel, lookPosition);
    mIAIHelicopter->SetLookAtPosition(lookPosition);
    this->mIVehicleAI->SetDriveSpeed(6.25f);

    UMath::Vector3 destVel = this->mPlayerRigidBody->GetLinearVelocity();
    mIAIHelicopter->SetDestinationVelocity(destVel);
    this->mIVehicleAI->SetDriveTarget(seekPosition);
    this->mIVehicleAI->DoDriving(7);

    if (dot > 0.0f) {
        if (dist2PerpSQ > 1600.0f) {
            this->mSkidKnockTimer = 0.0f;
        }
        if (myPosition.y < this->mPlayerPosition.y) {
            this->mSkidKnockTimer = 0.0f;
        }
    } else if (dist2PerpSQ > 144.0f) {
        this->mSkidKnockTimer = 0.0f;
    }

    if (this->mSkidKnockTimer <= 0.0f) {
        this->mPursuitMode = kStraight_Line;
    }
}

void AIActionHeliPursuit::SetNextPerpSearchDest() {
    IPursuit *ip = this->mIVehicleAI->GetPursuit();
    if (ip == nullptr) {
        return;
    }
    const UMath::Vector3 &centre = ip->GetLastKnownLocation();
    AITarget *target = this->mIVehicleAI->GetTarget();
    IPerpetrator *iperp;
    target->QueryInterface(&iperp);

    Attrib::Gen::pursuitlevels *pl = iperp->GetPursuitLevelAttrib();
    if (pl == nullptr) {
        return;
    }
    float radius = pl->SearchModeRoadblockRadius();
    float workingRadius = Sim::GetRandom()._SimRandom_FloatRange(radius * 0.7f) + radius * 0.2f;
    UMath::Vector3 baseVector = UMath::Vector3Make(workingRadius, 0.0f, 0.0f);

    this->mSearchPatternAngle += 0.125f;
    if (this->mSearchPatternAngle >= 1.0f) {
        this->mSearchPatternAngle -= 1.0f;
    }
    UMath::RotateInXZ(this->mSearchPatternAngle, baseVector, this->mSearchDestPoint);
    UMath::Add(this->mSearchDestPoint, centre, this->mSearchDestPoint);
    mIAIHelicopter->FilterHeliAltitude(mSearchDestPoint);
    this->mSearchDestPoint.y += 5.0f;
}

void AIActionHeliPursuit::SearchForPerp() {
    UMath::Vector3 myPosition = this->mIRigidBody->GetPosition();

    float dToDest = UMath::Distancexz(this->mSearchDestPoint, myPosition);
    if (dToDest < 30.0f) {
        this->SetNextPerpSearchDest();
    }

    this->mIVehicleAI->SetDriveSpeed(70.0f);

    UMath::Vector3 destVel = this->mIRigidBody->GetLinearVelocity();
    this->mIAIHelicopter->SetDestinationVelocity(destVel);
    this->mIVehicleAI->SetDriveTarget(this->mSearchDestPoint);
    this->mIAIHelicopter->SetLookAtPosition(this->mSearchDestPoint);
    this->mIVehicleAI->DoDriving(7);
}

bool bIgnoreHeliSheet;
bool NeverIgnoreHeliSheet;

void AIActionHeliPursuit::Update(float dT) {
    this->mPursuitTime += dT;
    this->mSkidKnockTimer -= dT;
    if (this->mCollisionAbort != 0) {
        this->mCollisionAbort--;
    }
    this->mPlayerRigidBody = IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody();
    this->mPlayerPosition = this->mPlayerRigidBody->GetPosition();
    this->mPlayerSpeed = this->mPlayerRigidBody->GetSpeed();
    IPursuit *ip = this->mIVehicleAI->GetPursuit();
    if (ip != nullptr && !ip->IsPerpInSight()) {
        if (this->mPursuitMode != kSearch_Pattern) {
            this->SetNextPerpSearchDest();
            this->mPursuitMode = kSearch_Pattern;
        }
    } else {
        if (this->mPursuitMode == kSearch_Pattern) {
            this->mPursuitMode = kStraight_Line;
        }
    }
    bIgnoreHeliSheet = false;
    switch (this->mPursuitMode) {
        case kStraight_Line:
            this->StraightLinePursuit();
            break;
        case kSkid_Hit_Approach:
        case kSkid_Hit_Strike:
            if (!NeverIgnoreHeliSheet) {
                bIgnoreHeliSheet = true;
            }
            this->SkidHitPursuit();
            break;
        case kSearch_Pattern:
            this->SearchForPerp();
            break;
    }
}

void AIActionHeliPursuit::OnDebugDraw() {}
