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
    this->mStuckTimer = 0.0f;
    params->mOwner->QueryInterface(&this->mIInput);
}

void AIActionGetUnstuck::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mIInput);
    }
}

AIAction *AIActionGetUnstuck::Construct(AIActionParams *params) {
    return new AIActionGetUnstuck(params, 1.0f);
}

bool AIActionGetUnstuck::CanBeAttempted(float dT) {
    if (!this->GetVehicle() || !this->GetAI() || !this->mIInput || this->GetAI()->GetReverseOverride()) {
        return false;
    }
    bool stuck = false;
    UMath::Vector3 position = this->GetActionParams().mOwner->GetPosition();
    if (this->mIInput->GetControls().fGas >= 0.5f || this->mIInput->GetControls().fSteeringVertical >= 0.5f) {
        if (this->GetVehicle()->IsStaging() || this->mStuckTimer <= 0.0f) {
            this->mStuckPos = position;
            this->mStuckTimer = dT;
        } else {
            this->mStuckTimer += dT;
            if (this->mStuckTimer >= 3.0f) {
                float dist = UMath::Distance(this->mStuckPos, position);
                this->mStuckTimer = 0.0f;
                if (dist < 3.0f) {
                    stuck = true;
                    this->GetAI()->SetReverseOverride(2.0f);
                    this->mStuckTimer = 0.0f;
                }
            }
        }
    } else {
        this->mStuckTimer = 0.0f;
    }
    if (stuck) {
        this->GetAI()->ResetDriveToNav(SELECT_CENTER_LANE);
    }
    return stuck;
}

void AIActionGetUnstuck::FinishAction(float dT) {
    WRoadNav *nav = this->GetAI()->GetDriveToNav();
    if (nav) {
        this->GetAI()->ResetDriveToNav(SELECT_VALID_LANE);
    }
}

bool AIActionGetUnstuck::IsFinished() {
    return !this->GetAI()->GetReverseOverride();
}

void AIActionGetUnstuck::Update(float dT) {
    this->GetAI()->SetDriveSpeed(15.0f);
    this->GetAI()->DoSteering();
    this->GetAI()->DoGasBrake();
}
