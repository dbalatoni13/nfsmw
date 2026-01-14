#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"

// total size: 0x48
class AIActionHeliExit : public AIAction, public Debugable {
  public:
    enum kExitMode {
        kSeekUp = 0,
        kSeekCar = 1,
        kFlyOut = 2,
    };

    static AIAction *Construct(struct AIActionParams *params);

    AIActionHeliExit(AIActionParams *params, float score);

    // Virtual functions
    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AIActionHeliExit() override {}

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

    // Inlines
    // WRoadNav *GetPursuitNav() {}

  private:
    IVehicleAI *mIVehicleAI;       // offset 0x4C, size 0x4
    IVehicle *mIVehicle;           // offset 0x50, size 0x4
    IRigidBody *mIRigidBody;       // offset 0x54, size 0x4
    IAIHelicopter *mIAIHelicopter; // offset 0x58, size 0x4
    IPursuitAI *mIPursuitAI;       // offset 0x5C, size 0x4
    float mExitTime;               // offset 0x60, size 0x4
    bool mBuildingPath;            // offset 0x64, size 0x1
    UMath::Vector3 mSeekPosition;  // offset 0x68, size 0xC
    kExitMode mExitMode;           // offset 0x74, size 0x4
};

AIActionHeliExit::AIActionHeliExit(AIActionParams *params, float score)
    : AIAction(params, score), //
      mExitTime(0.0f),         //
      mExitMode(kSeekUp),      //
      mBuildingPath(false) {
    MakeDebugable(DBG_AI);
    params->mOwner->QueryInterface(&mIVehicleAI);
    params->mOwner->QueryInterface(&mIPursuitAI);
    params->mOwner->QueryInterface(&mIVehicle);
    params->mOwner->QueryInterface(&mIAIHelicopter);
    mIRigidBody = params->mOwner->GetRigidBody();
}

AIAction *AIActionHeliExit::Construct(AIActionParams *params) {
    return new AIActionHeliExit(params, 0.0f);
}

float Exit_Height;

bool AIActionHeliExit::IsFinished() {
    IRigidBody *player_rigid_body = IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody();
    const UMath::Vector3 &heliPosition = mIRigidBody->GetPosition();
    const UMath::Vector3 &playerPosition = player_rigid_body->GetPosition();

    if (mExitMode == kFlyOut && heliPosition.y - playerPosition.y > Exit_Height) {
        UMath::Vector3 playerForward;
        player_rigid_body->GetForwardVector(playerForward);

        UMath::Vector3 player2heli;
        UMath::Sub(heliPosition, playerPosition, player2heli);
        float dotForward = UMath::Dot(player2heli, playerForward);

        if (dotForward < 0.0f) {
            float dSquared = DistanceSquare(heliPosition, playerPosition);
            if (dSquared > 22500.0f) {
                return true;
            }
        }
    }

    return false;
}

void AIActionHeliExit::FinishAction(float dT) {
    mExitMode = kSeekUp;
    mExitTime = 0.0f;
}

void AIActionHeliExit::BeginAction(float dT) {
    mExitMode = kSeekUp;
    mSeekPosition = mIRigidBody->GetPosition();
    mIAIHelicopter->RestrictPointToRoadNet(mSeekPosition);
    mSeekPosition.y += Exit_Height;
}

void AIActionHeliExit::Update(float dT) {
    IRigidBody *player_rigid_body;
    UMath::Vector3 playerPosition;
    UMath::Vector3 myPosition;
    UMath::Vector3 direction;

    mExitTime = mExitTime + dT;

    player_rigid_body = IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody();
    playerPosition = player_rigid_body->GetPosition();
    myPosition = mIRigidBody->GetPosition();
    player_rigid_body->GetForwardVector(direction);

    float flySpeed = 100.0f;
    switch (mExitMode) {
        case kSeekUp:
            if (myPosition.y - playerPosition.y > 15.0f) {
                UMath::ScaleAdd(direction, 85.0f, playerPosition, mSeekPosition);
                mSeekPosition.y += Exit_Height;
                mExitMode = kSeekCar;
            }
            break;
        case kSeekCar: {
            float dSquared = UMath::DistanceSquare(mSeekPosition, myPosition);
            if (dSquared < 25.0f) {
                UMath::Vector3 playerRightVec;
                player_rigid_body->GetRightVector(playerRightVec);
                float rScale = 3.0f;
                if (UMath::Dot(playerRightVec, mIRigidBody->GetLinearVelocity()) >= 0.0f) {
                    rScale *= -3.0f;
                }
                UMath::ScaleAdd(playerRightVec, rScale, direction, direction);
                UMath::ScaleAdd(direction, -200.0f, playerPosition, mSeekPosition);
                mExitMode = kFlyOut;
                mSeekPosition.y = playerPosition.y + Exit_Height + 5.0f;
            }
        } break;
        case kFlyOut:
            break;
    }

    mIAIHelicopter->SetLookAtPosition(mSeekPosition);
    mIVehicleAI->SetDriveSpeed(flySpeed);

    mIAIHelicopter->SetDestinationVelocity(mIRigidBody->GetLinearVelocity());

    mIVehicleAI->SetDriveTarget(mSeekPosition);

    mIVehicleAI->DoDriving(7);
}

void AIActionHeliExit::OnDebugDraw() {}
