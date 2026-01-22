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
    mIsAirborne = false;
    mAirborneTimer = 0.0f;
    params->mOwner->QueryInterface(&mISuspension);
    params->mOwner->QueryInterface(&mIInput);
    params->mOwner->QueryInterface(&mRBComplex);
}

void AIActionAirborne::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mISuspension);
    } else if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    } else if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mRBComplex);
    }
}

bool AIActionAirborne::CanBeAttempted(float dT) {
    if (!mISuspension || !mIInput || !mRBComplex) {
        return false;
    }
    if (mRBComplex->IsModeling()) {
        if (mIsAirborne) {
            mAirborneTimer += dT;
            if (mAirborneTimer > 1.5f) {
                return true;
            }
        }
        mIsAirborne = mISuspension->GetNumWheelsOnGround() == 0;
    }
    return false;
}

bool AIActionAirborne::IsFinished() {
    return !mIsAirborne;
}

void AIActionAirborne::Update(float dT) {
    mIInput->SetControlGas(0.0f);
    mIInput->SetControlBrake(0.0f);
    mIInput->SetControlSteering(0.0f);
    mIInput->SetControlSteeringVertical(0.0f);
    mIInput->SetControlHandBrake(0.0f);

    mIsAirborne = mISuspension->GetNumWheelsOnGround() == 0;
}
