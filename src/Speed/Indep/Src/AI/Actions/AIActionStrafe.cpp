#include "Speed/Indep/Src/AI/AIAction.h"

// total size: 0x48
class AIActionStrafe : public AIAction {
  public:
    static AIAction *Construct(AIActionParams *params);

    AIActionStrafe(AIActionParams *params, float score) : AIAction(params, score) {}

    void UpdateNavPos(struct Vector3 &dest, float lookAheadDistance);

    // Virtual overrides
    // IUnknown
    ~AIActionStrafe() override {}

    // AIAction
    bool CanBeAttempted(float dT) override {
        return false;
    }

    bool IsFinished() override {
        return true;
    }

    void BeginAction(float dT) override {}

    void FinishAction(float dT) override {}

    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override {}
};

AIAction *AIActionStrafe::Construct(AIActionParams *params) {
    return new AIActionStrafe(params, 0.0f);
}

void AIActionStrafe::Update(float dT) {}
