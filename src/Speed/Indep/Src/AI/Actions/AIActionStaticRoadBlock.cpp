#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

// total size: 0x48
class AIActionStaticRoadBlock : public AIAction, public Debugable {
  public:
    static AIAction *Construct(AIActionParams *params);

    AIActionStaticRoadBlock(AIActionParams *params, float score);

    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AIActionStaticRoadBlock() override {}

    // AIAction
    bool CanBeAttempted(float dT) override;

    bool IsFinished() override {
        return false;
    }

    void BeginAction(float dT) override;
    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    IVehicle *mIVehicle; // offset 0x4C, size 0x4
    IInput *mIInput;     // offset 0x50, size 0x4
};

AIActionStaticRoadBlock::AIActionStaticRoadBlock(AIActionParams *params, float score) : AIAction(params, score) {
    params->mOwner->QueryInterface(&mIVehicle);
    params->mOwner->QueryInterface(&mIInput);
}

void AIActionStaticRoadBlock::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
}

AIAction *AIActionStaticRoadBlock::Construct(AIActionParams *params) {
    return new AIActionStaticRoadBlock(params, 1.0f);
}

bool AIActionStaticRoadBlock::CanBeAttempted(float dT) {
    if (!mIVehicle) {
        return false;
    }
    if (!mIInput) {
        return false;
    }
    return true;
}

void AIActionStaticRoadBlock::BeginAction(float dT) {}

void AIActionStaticRoadBlock::FinishAction(float dT) {}

void AIActionStaticRoadBlock::Update(float dT) {
    mIInput->SetControlGas(0.0f);
    mIInput->SetControlBrake(1.0f);
    mIInput->SetControlSteering(0.0f);
    mIInput->SetControlSteeringVertical(0.0f);
    mIInput->SetControlHandBrake(0.0f);
    mIInput->SetControlNOS(false);
}

void AIActionStaticRoadBlock::OnDebugDraw() {}
