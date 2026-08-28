#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

// total size: 0x48
class AIActionAirborne : public AIAction {
  public:
    static AIAction *Construct(AIActionParams *params);

    AIActionAirborne(AIActionParams *params, float score);

    // Virtual overrides
    // IUnknown
    ~AIActionAirborne() override {}

    // AIAction
    bool CanBeAttempted(float dT) override;

    bool IsFinished() override;

    void BeginAction(float dT) override {}

    void FinishAction(float dT) override {}

    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    ISuspension *mISuspension;  // offset 0x48, size 0x4
    IInput *mIInput;            // offset 0x4C, size 0x4
    ICollisionBody *mRBComplex; // offset 0x50, size 0x4
    bool mIsAirborne;           // offset 0x54, size 0x1
    float mAirborneTimer;       // offset 0x58, size 0x4
};

AIAction *AIActionAirborne::Construct(AIActionParams *params) {
    return new AIActionAirborne(params, 0.0f);
}

AIActionAirborne::AIActionAirborne(AIActionParams *params, float score) : AIAction(params, score) {
    this->mIsAirborne = false;
    this->mAirborneTimer = 0.0f;
    params->mOwner->QueryInterface(&this->mISuspension);
    params->mOwner->QueryInterface(&this->mIInput);
    params->mOwner->QueryInterface(&this->mRBComplex);
}

void AIActionAirborne::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&this->mISuspension);
    } else if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mIInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mRBComplex);
    }
}

bool AIActionAirborne::CanBeAttempted(float dT) {
    if (!this->mISuspension || !this->mIInput || !this->mRBComplex) {
        return false;
    }
    if (this->mRBComplex->IsModeling()) {
        if (this->mIsAirborne) {
            this->mAirborneTimer += dT;
            if (this->mAirborneTimer > 1.5f) {
                return true;
            }
        }
        this->mIsAirborne = this->mISuspension->GetNumWheelsOnGround() == 0;
    }
    return false;
}

bool AIActionAirborne::IsFinished() {
    return !this->mIsAirborne;
}

void AIActionAirborne::Update(float dT) {
    this->mIInput->SetControlGas(0.0f);
    this->mIInput->SetControlBrake(0.0f);
    this->mIInput->SetControlSteering(0.0f);
    this->mIInput->SetControlSteeringVertical(0.0f);
    this->mIInput->SetControlHandBrake(0.0f);

    this->mIsAirborne = this->mISuspension->GetNumWheelsOnGround() == 0;
}
