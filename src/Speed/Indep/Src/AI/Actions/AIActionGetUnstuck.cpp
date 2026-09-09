#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"

// total size: 0x48
struct AIActionGetUnstuck : public AIAction {
  public:
    AIActionGetUnstuck(AIActionParams *params, float score);
    ~AIActionGetUnstuck() override {}

    static AIAction *Construct(AIActionParams *params);

    // AIAction
    bool CanBeAttempted(float dT) override;
    bool IsFinished() override;
    void BeginAction(float dT) override {}
    void FinishAction(float dT) override;
    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    UMath::Vector3 mStuckPos; // offset 0x48, size 0xC
    float mStuckTimer;        // offset 0x54, size 0x4
    IInput *mIInput;          // offset 0x58, size 0x4
};

BIND_AIACTION_FACTORY(AIActionGetUnstuck);

AIActionGetUnstuck::AIActionGetUnstuck(AIActionParams *params, float score) : AIAction(params, score) {
    this->mStuckTimer = 0.0f;
    params->mOwner->QueryInterface(&this->mIInput);
}

void AIActionGetUnstuck::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mIInput);
    }
}

AIAction *AIActionGetUnstuck::Construct(AIActionParams *params) {
    return new AIActionGetUnstuck(params, AIACTION_SCORE_HIGH);
}

static const float fGetUnstuckTooLong = 3.0f; // Decl: 57

static const float fGetUnstuckDistance = 3.0f; // Decl: 59
static const float fGetUnstuckDuration = 2.0f; // Decl: 60

bool AIActionGetUnstuck::CanBeAttempted(float dT) {
    if (this->GetVehicle() == nullptr || this->GetAI() == nullptr || this->mIInput == nullptr || this->GetAI()->GetReverseOverride()) {
        return false;
    }
    bool stuck = false;
    UMath::Vector3 position = this->GetActionParams().mOwner->GetPosition();
    if (this->mIInput->GetControls().fGas >= 0.5f || this->mIInput->GetControls().fSteeringVertical >= 0.5f) {
        if (this->GetVehicle()->IsStaging() || this->mStuckTimer <= 0.0f) {
            this->mStuckPos = position;
            this->mStuckTimer = dT;
        } else {
            this->mStuckTimer += dT;
            if (this->mStuckTimer >= fGetUnstuckTooLong) {
                float dist = UMath::Distance(this->mStuckPos, position);
                this->mStuckTimer = 0.0f;
                if (dist < fGetUnstuckDistance) {
                    stuck = true;
                    this->GetAI()->SetReverseOverride(fGetUnstuckDuration);
                    this->mStuckTimer = 0.0f;
                }
            }
        }
    } else {
        this->mStuckTimer = 0.0f;
    }
    if (stuck) {
        this->GetAI()->ResetDriveToNav(SELECT_CENTER_LANE);
    }
    return stuck;
}

void AIActionGetUnstuck::FinishAction(float dT) {
    WRoadNav *nav = this->GetAI()->GetDriveToNav();
    if (nav != nullptr) {
        this->GetAI()->ResetDriveToNav(SELECT_VALID_LANE);
    }
}

bool AIActionGetUnstuck::IsFinished() {
    return !this->GetAI()->GetReverseOverride();
}

void AIActionGetUnstuck::Update(float dT) {
    this->GetAI()->SetDriveSpeed(15.0f);
    this->GetAI()->DoSteering();
    this->GetAI()->DoGasBrake();
}
