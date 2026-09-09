#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"

// total size: 0x75C
class AIVehicleTraffic : public AIVehicle, public ITrafficAI {
  public:
    static Behavior *Construct(const BehaviorParams &bp);

    AIVehicleTraffic(const BehaviorParams &bp);

    // Overrides: IUnknown
    ~AIVehicleTraffic() override;

    // Overrides: AIVehicle
    void Update(float dT) override;

    // Overrides: ITrafficAI
    void StartDriving(float speed) override;

  private:
    static float mStagger; // size: 0x4
};

static const float Traffic_AIUpdateRate = 0.1f; // Decl: 24
float AIVehicleTraffic::mStagger = 0.0f;        // Decl: 25

Behavior *AIVehicleTraffic::Construct(const BehaviorParams &bp) {
    return new AIVehicleTraffic(bp);
}

BIND_BEHAVIOR_FACTORY(AIVehicleTraffic);

AIVehicleTraffic::AIVehicleTraffic(const BehaviorParams &bp)
    : AIVehicle(bp, Traffic_AIUpdateRate, mStagger, Sim::TASK_FRAME_VARIABLE), //
      ITrafficAI(bp.fowner) {
    this->SetGoal(UCrc32("AIGoalNone"));
    mStagger += Traffic_AIUpdateRate;
    if (mStagger >= 1.0f) {
        mStagger = 0.0f;
    }
}

AIVehicleTraffic::~AIVehicleTraffic() {}

void AIVehicleTraffic::Update(float dT) {
    this->AIVehicle::Update(dT);
    this->UpdateSpawnTimer(dT);
    if (this->GetGoal() != nullptr) {
        this->GetGoal()->Update(dT);
    }
}

void AIVehicleTraffic::StartDriving(float speed) {
    this->ClearGoal();
    this->SetGoal(UCrc32("AIGoalTraffic"));

    IDamageable *idamage;
    if (this->GetOwner()->QueryInterface(&idamage)) {
        idamage->ResetDamage();
    }

    IInput *input;
    if (this->GetOwner()->QueryInterface(&input)) {
        input->ClearInput();
    }

    IRBVehicle *vehiclebody;
    if (this->GetOwner()->QueryInterface(&vehiclebody)) {
        vehiclebody->SetInvulnerability(INVULNERABLE_NONE, 0.0f);
        vehiclebody->EnableObjectCollisions(true);
    }

    this->GetVehicle()->SetSpeed(speed);
    this->SetDriveSpeed(speed);

    WRoadNav *road_nav = this->GetDriveToNav();
    AITarget *target = this->GetTarget();

    if (target != nullptr && target->IsValid()) {
        this->SetDriveTarget(target->GetPosition());
    } else if (road_nav != nullptr && road_nav->IsValid()) {
        this->SetDriveTarget(road_nav->GetPosition());
    } else {
        UMath::Vector3 forward;
        this->GetVehicle()->ComputeHeading(&forward);
        UMath::Scale(forward, 10.0f, forward);
        this->SetDriveTarget(forward);
    }

    this->DoDriving(3);
}
