#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Behavior.h"

// total size: 0x48
class AIActionTooDamaged : public AIAction, public Debugable {
  public:
    static AIAction *Construct(struct AIActionParams *params);

    AIActionTooDamaged(AIActionParams *params, float score);

    // Virtual functions
    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AIActionTooDamaged() override {}

    // AIAction
    bool CanBeAttempted(float dT) override;
    void BeginAction(float dT) override;

    bool IsFinished() override {
        return false;
    }

    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    IVehicle *mIVehicle; // offset 0x4C, size 0x4
    IInput *mIInput;     // offset 0x50, size 0x4
};

AIActionTooDamaged::AIActionTooDamaged(AIActionParams *params, float score) : AIAction(params, score) {
    params->mOwner->QueryInterface(&mIVehicle);
    params->mOwner->QueryInterface(&mIInput);

    MakeDebugable(DBG_AI);
}

void AIActionTooDamaged::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
}

AIAction *AIActionTooDamaged::Construct(AIActionParams *params) {
    return new AIActionTooDamaged(params, 1.0f);
}

bool AIActionTooDamaged::CanBeAttempted(float dT) {
    if (mIVehicle && mIInput) {
        return mIVehicle->IsDestroyed();
    } else {
        return false;
    }
}

void AIActionTooDamaged::BeginAction(float dT) {
    IPursuitAI *iPursuitAI;
    if (GetOwner()->QueryInterface(&iPursuitAI)) {
        return iPursuitAI->EndPursuit();
    }
}

void AIActionTooDamaged::FinishAction(float dT) {}

void AIActionTooDamaged::Update(float dT) {
    mIInput->SetControlGas(0.0f);
    mIInput->SetControlBrake(0.0f);
    mIInput->SetControlSteering(0.0f);
    mIInput->SetControlSteeringVertical(0.0f);
    mIInput->SetControlHandBrake(0.0f);

    if (!mIVehicle->InShock() && GetActionParams().mOwner->GetRigidBody()->GetSpeedXZ() >= 2.5f) {
        mIInput->SetControlBrake(0.25f);
    }
}

void AIActionTooDamaged::OnDebugDraw() {}
