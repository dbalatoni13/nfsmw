#include "Speed/Indep/Src/AI/AIAction.h"

// total size: 0x48
class AIActionSpline : public AIAction {
  public:
    static AIAction *Construct(AIActionParams *params);

    AIActionSpline(AIActionParams *params, float score) : AIAction(params, score) {}

    // Virtual overrides
    // IUnknown
    ~AIActionSpline() override {}

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

AIAction *AIActionSpline::Construct(AIActionParams *params) {
    return new AIActionSpline(params, 0.0f);
}

void AIActionSpline::Update(float dT) {}
