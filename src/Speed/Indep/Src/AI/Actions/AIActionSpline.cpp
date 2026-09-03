#include "Speed/Indep/Src/AI/AIAction.h"

// total size: 0x48
class AIActionSpline : public AIAction {
  public:
    AIActionSpline(AIActionParams *params, float score) : AIAction(params, score) {}
    ~AIActionSpline() override {}

    static AIAction *Construct(AIActionParams *params);

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

BIND_AIACTION_FACTORY(AIActionSpline);

AIAction *AIActionSpline::Construct(AIActionParams *params) {
    return new AIActionSpline(params, AIACTION_SCORE_LOW);
}

void AIActionSpline::Update(float dT) {}
