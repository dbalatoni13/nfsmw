#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

// total size: 0x48
class AIActionStopShort : public AIAction {
  public:
    AIActionStopShort(AIActionParams *params, float score);
    ~AIActionStopShort() override {}

    static AIAction *Construct(AIActionParams *params);

    // AIAction
    bool CanBeAttempted(float dT) override;
    bool IsFinished() override;

    void BeginAction(float dT) override {}

    void FinishAction(float dT) override {}

    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    IInput *mIInput;        // offset 0x48, size 0x4
    IPursuitAI *mPursuitAI; // offset 0x4C, size 0x4
};

BIND_AIACTION_FACTORY(AIActionStopShort);

AIAction *AIActionStopShort::Construct(AIActionParams *params) {
    return new AIActionStopShort(params, AIACTION_SCORE_LOW);
}

AIActionStopShort::AIActionStopShort(AIActionParams *params, float score) : AIAction(params, score) {
    params->mOwner->QueryInterface(&this->mIInput);
    params->mOwner->QueryInterface(&this->mPursuitAI);
}

bool AIActionStopShort::CanBeAttempted(float dT) {
    return true;
}

void AIActionStopShort::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_AI) {
        this->GetOwner()->QueryInterface(&this->mPursuitAI);
    }
}

bool AIActionStopShort::IsFinished() {
    return this->mPursuitAI->GetBreaker() == false;
}

void AIActionStopShort::Update(float dT) {
    this->mIInput->SetControlGas(0.0f);
    this->mIInput->SetControlBrake(1.0f);
    this->mIInput->SetControlSteering(0.0f);
    this->mIInput->SetControlSteeringVertical(0.0f);
    this->mIInput->SetControlHandBrake(1.0f);
    this->mIInput->SetControlNOS(false);
}
