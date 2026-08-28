#include "Speed/Indep/Src/AI/AIVehiclePursuit.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"

float AIVehiclePursuit::mStagger = 0.0f;

AIVehiclePursuit::AIVehiclePursuit(const BehaviorParams &bp)
    : AIVehiclePid(bp, 0.125f, mStagger, Sim::TASK_FRAME_FIXED), //
      IPursuitAI(bp.fowner),                                     //
      mInPursuit(false),                                         //
      mBreaker(false),                                           //
      mChicken(false),                                           //
      mDamagedByPerp(false),                                     //
      mSirenState(Sound::SIREN_OFF),                                    //
      mSirenInit(false),                                         //
      mInFormation(false),                                       //
      mInPosition(false),                                        //
      mWithinEngagementRadius(false),                            //
      mPursuitOffset(UMath::Vector3::kZero) {
    mStagger += 0.125f;
    if (mStagger >= 1.0f) {
        mStagger = 0.0f;
    }
    this->mVisibiltyTestTimer = 0.0f;
    this->mTimeSinceTargetSeen = 99.0f;
    this->mSupportGoal = (const char *)nullptr;
    this->mT_siren[0] = this->mT_siren[1] = this->mT_siren[2] = WorldTimer;
}

AIVehiclePursuit::~AIVehiclePursuit() {}

void AIVehiclePursuit::ResetInternals() {
    this->AIVehicle::ResetInternals();
    this->mInPursuit = false;
    this->mBreaker = false;
    this->mChicken = false;
    this->mDamagedByPerp = false;
    this->mInFormation = false;
    this->mInPosition = false;

    this->mPursuitOffset = UMath::Vector3::kZero;

    this->mTimeSinceTargetSeen = 99.0f;
    this->mVisibiltyTestTimer = 0.25f;

    this->mWithinEngagementRadius = false;

    this->mT_siren[0] = this->mT_siren[1] = this->mT_siren[2] = WorldTimer;
    this->mSirenState = Sound::SIREN_OFF;
    this->mSirenInit = false;
}

void AIVehiclePursuit::StartPatrol() {
    this->SetInPursuit(false);
    this->GetTarget()->Clear();
    this->SetGoal("AIGoalPatrol");
}

void AIVehiclePursuit::StartFlee() {
    IVehicle *ivehicle;
    this->GetVehicle()->GlareOff(VehicleFX::LIGHT_COPS);

    UCrc32 goal("AIGoalFleePursuit");
    if (GetSimable()->QueryInterface(&ivehicle) && ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
        goal = "AIGoalHeliExit";
    }
    if (this->GetGoalName() != goal) {
        this->ClearGoal();
        this->SetGoal(goal);
    }
}

void AIVehiclePursuit::StartRoadBlock() {
    IVehicle *ivehicle;
    this->GetVehicle()->GlareOn(VehicleFX::LIGHT_COPS);
    this->SetInPursuit(true);
    this->GetTarget()->Clear();
    if (GetVehicle()->GetVehicleClass() == VehicleClass::CHOPPER) {
        this->SetGoal("AIGoalHeliRoadBlock");
    } else {
        this->SetGoal("AIGoalStaticRoadBlock");
    }
}

void AIVehiclePursuit::StartPursuit(AITarget *target, ISimable *itargetSimable) {
    this->GetVehicle()->GlareOn(VehicleFX::LIGHT_COPS);
    if (target) {
        this->GetTarget()->Aquire(target);
    } else if (itargetSimable) {
        this->GetTarget()->Aquire(itargetSimable);
    }
    this->UpdateTargeting();
    this->SetInPursuit(true);
    if (GetVehicle()->GetVehicleClass() == VehicleClass::CHOPPER) {
        this->SetGoal("AIGoalHeliPursuit");
    } else {
        this->SetGoal("AIGoalPursuit");
    }
}

void AIVehiclePursuit::DoInPositionGoal() {
    this->SetGoal(this->mInPositionGoal);
}

void AIVehiclePursuit::EndPursuit() {
    this->SetInPursuit(false);
    this->GetVehicle()->GlareOff(VehicleFX::LIGHT_COPS);
}

bool AIVehiclePursuit::StartSupportGoal() {
    if (this->mSupportGoal != (const char *)nullptr) {
        this->SetGoal(this->mSupportGoal);
        return true;
    }
    return false;
}

void AIVehiclePursuit::SetSupportGoal(UCrc32 sg) {
    this->mSupportGoal = sg;
}

AITarget *AIVehiclePursuit::GetPursuitTarget() {
    if (this->GetTarget()->IsValid()) {
        return this->GetTarget();
    } else {
        return nullptr;
    }
}

AITarget *AIVehiclePursuit::PursuitRequest() {
    if (!this->GetInPursuit() && this->GetTarget()->IsValid()) {
        return this->GetTarget();
    } else {
        return nullptr;
    }
}

void AIVehiclePursuit::Update(float dT) {
    ProfileNode profile_node("TODO", 0);
    this->AIVehicle::Update(dT);
    this->UpdateSiren(dT); // TODO

    if (!this->mInPursuit || !this->GetTarget()->IsValid()) {
        this->mTimeSinceTargetSeen = 0.25f;
        this->mVisibiltyTestTimer = 0.25f;
        return;
    }

    this->mVisibiltyTestTimer += dT;
    this->mTimeSinceTargetSeen += dT;
    if (this->mVisibiltyTestTimer >= 0.25f) {
        this->mVisibiltyTestTimer -= 0.25f;
        if (this->CanSeeTarget(this->GetTarget())) {
            this->mTimeSinceTargetSeen = -0.25f;
        }
    }
}
