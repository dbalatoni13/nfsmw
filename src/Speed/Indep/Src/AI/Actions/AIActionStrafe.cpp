#include "Speed/Indep/Src/AI/AIAction.h"

// total size: 0x48
class AIActionStrafe : public AIAction {
  public:
    AIActionStrafe(AIActionParams *params, float score) : AIAction(params, score) {}
    ~AIActionStrafe() override {}

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

  private:
    void UpdateNavPos(UMath::Vector3 &dest, float lookAheadDistance);
};

BIND_AIACTION_FACTORY(AIActionStrafe);

AIAction *AIActionStrafe::Construct(AIActionParams *params) {
    return new AIActionStrafe(params, AIACTION_SCORE_LOW);
}

void AIActionStrafe::Update(float dT) {}
