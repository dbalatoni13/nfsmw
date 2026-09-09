#include "Speed/Indep/Src/AI/AIAction.h"

// total size: 0x48
class AIActionNone : public AIAction {
  public:
    AIActionNone(AIActionParams *params, float score) : AIAction(params, score) {}
    ~AIActionNone() override {}

    static AIAction *Construct(AIActionParams *params);

    bool CanBeAttempted(float dT) override {
        return false;
    }

    bool IsFinished() override {
        return true;
    }

    void BeginAction(float dT) override {}

    void FinishAction(float dT) override {}

    void Update(float dT) override {}

    void OnBehaviorChange(const UCrc32 &mechanic) override {}
};

// Decl: 38
BIND_AIACTION_FACTORY(AIActionNone);

AIAction *AIActionNone::Construct(AIActionParams *params) {
    return new AIActionNone(params, AIACTION_SCORE_LOW);
}
