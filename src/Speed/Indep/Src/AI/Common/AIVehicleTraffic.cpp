
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"

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

float AIVehicleTraffic::mStagger = 0.0f;

Behavior *AIVehicleTraffic::Construct(const BehaviorParams &bp) {
    return new AIVehicleTraffic(bp);
}

AIVehicleTraffic::AIVehicleTraffic(const BehaviorParams &bp)
    : AIVehicle(bp, 0.1f, mStagger, Sim::TASK_FRAME_VARIABLE), //
      ITrafficAI(bp.fowner) {
    SetGoal("AIGoalNone");
    mStagger += 0.1f;
    if (mStagger >= 1.0f) {
        mStagger = 0.0f;
    }
}

AIVehicleTraffic::~AIVehicleTraffic() {}

void AIVehicleTraffic::Update(float dT) {
    AIVehicle::Update(dT);
    UpdateSpawnTimer(dT);
    if (GetGoal()) {
        GetGoal()->Update(dT);
    }
}

void AIVehicleTraffic::StartDriving(float speed) {
    ClearGoal();
    SetGoal("AIGoalTraffic");

    IDamageable *idamage;
    if (GetOwner()->QueryInterface(&idamage)) {
        idamage->ResetDamage();
    }

    IInput *input;
    if (GetOwner()->QueryInterface(&input)) {
        input->ClearInput();
    }

    IRBVehicle *vehiclebody;
    if (GetOwner()->QueryInterface(&vehiclebody)) {
        vehiclebody->SetInvulnerability(INVULNERABLE_NONE, 0.0f);
        vehiclebody->EnableObjectCollisions(true);
    }

    GetVehicle()->SetSpeed(speed);
    SetDriveSpeed(speed);

    WRoadNav *road_nav = GetDriveToNav();
    AITarget *target = GetTarget();

    if (target && target->IsValid()) {
        SetDriveTarget(target->GetPosition());
    } else if (road_nav && road_nav->IsValid()) {
        SetDriveTarget(road_nav->GetPosition());
    } else {
        UMath::Vector3 forward;
        GetVehicle()->ComputeHeading(&forward);
        UMath::Scale(forward, 10.0f, forward);
        SetDriveTarget(forward);
    }

    DoDriving(3);
}
