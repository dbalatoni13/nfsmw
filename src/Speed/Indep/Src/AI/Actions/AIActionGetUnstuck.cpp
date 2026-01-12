#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

// total size: 0x48
struct AIActionGetUnstuck : public AIAction {
  public:
    static AIAction *Construct(struct AIActionParams *params);

    AIActionGetUnstuck(AIActionParams *params, float score);

    // Virtual overrides
    // IUnknown
    ~AIActionGetUnstuck() override {}

    // AIAction
    bool CanBeAttempted(float dT) override;

    void BeginAction(float dT) override {}

    bool IsFinished() override;

    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    UMath::Vector3 mStuckPos; // offset 0x48, size 0xC
    float mStuckTimer;        // offset 0x54, size 0x4
    IInput *mIInput;          // offset 0x58, size 0x4
};

AIActionGetUnstuck::AIActionGetUnstuck(AIActionParams *params, float score) : AIAction(params, score) {
    mStuckTimer = 0.0f;
    params->mOwner->QueryInterface(&mIInput);
}

void AIActionGetUnstuck::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
}

AIAction *AIActionGetUnstuck::Construct(AIActionParams *params) {
    return new AIActionGetUnstuck(params, 1.0f);
}

bool AIActionGetUnstuck::CanBeAttempted(float dT) {
    if (!GetVehicle() || !GetAI() || !mIInput || GetAI()->GetReverseOverride()) {
        return false;
    }
    bool stuck = false;
    UMath::Vector3 position = GetActionParams().mOwner->GetPosition();
    if (mIInput->GetControls().fGas >= 0.5f || mIInput->GetControls().fSteeringVertical >= 0.5f) {
        if (GetVehicle()->IsStaging() || mStuckTimer <= 0.0f) {
            mStuckPos = position;
            mStuckTimer = dT;
        } else {
            mStuckTimer += dT;
            if (mStuckTimer >= 3.0f) {
                float dist = UMath::Distance(mStuckPos, position);
                mStuckTimer = 0.0f;
                if (dist < 3.0f) {
                    stuck = true;
                    GetAI()->SetReverseOverride(2.0f);
                    mStuckTimer = 0.0f;
                }
            }
        }
    } else {
        mStuckTimer = 0.0f;
    }
    if (stuck) {
        GetAI()->ResetDriveToNav(SELECT_CENTER_LANE);
    }
    return stuck;
}

void AIActionGetUnstuck::FinishAction(float dT) {
    WRoadNav *nav = GetAI()->GetDriveToNav();
    if (nav) {
        GetAI()->ResetDriveToNav(SELECT_VALID_LANE);
    }
}

bool AIActionGetUnstuck::IsFinished() {
    return !GetAI()->GetReverseOverride();
}

void AIActionGetUnstuck::Update(float dT) {
    GetAI()->SetDriveSpeed(15.0f);
    GetAI()->DoSteering();
    GetAI()->DoGasBrake();
}
