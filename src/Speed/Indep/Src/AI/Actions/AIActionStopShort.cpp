#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

// total size: 0x48
class AIActionStopShort : public AIAction {
  public:
    static AIAction *Construct(struct AIActionParams *params);

    AIActionStopShort(AIActionParams *params, float score);

    // Virtual overrides
    // IUnknown
    ~AIActionStopShort() override {}

    // AIAction
    bool CanBeAttempted(float dT) override;

    void BeginAction(float dT) override {}

    bool IsFinished() override;

    void FinishAction(float dT) override {}

    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    IInput *mIInput;        // offset 0x48, size 0x4
    IPursuitAI *mPursuitAI; // offset 0x4C, size 0x4
};

AIAction *AIActionStopShort::Construct(AIActionParams *params) {
    return new AIActionStopShort(params, 0.0f);
}

AIActionStopShort::AIActionStopShort(AIActionParams *params, float score)
    : AIAction(params, score) //
{
    params->mOwner->QueryInterface(&mIInput);
    params->mOwner->QueryInterface(&mPursuitAI);
}

bool AIActionStopShort::CanBeAttempted(float dT) {
    return true;
}

void AIActionStopShort::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        GetOwner()->QueryInterface(&mPursuitAI);
    }
}

bool AIActionStopShort::IsFinished() {
    return mPursuitAI->GetBreaker() == false;
}

void AIActionStopShort::Update(float dT) {
    mIInput->SetControlGas(0.0f);
    mIInput->SetControlBrake(1.0f);
    mIInput->SetControlSteering(0.0f);
    mIInput->SetControlSteeringVertical(0.0f);
    mIInput->SetControlHandBrake(1.0f);
    mIInput->SetControlNOS(0.0f);
}
