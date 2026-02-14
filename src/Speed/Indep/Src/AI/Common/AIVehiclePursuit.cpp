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
      mSirenState(SIREN_OFF),                                    //
      mSirenInit(false),                                         //
      mInFormation(false),                                       //
      mInPosition(false),                                        //
      mWithinEngagementRadius(false),                            //
      mPursuitOffset(UMath::Vector3::kZero) {
    mStagger += 0.125f;
    if (mStagger >= 1.0f) {
        mStagger = 0.0f;
    }
    mVisibiltyTestTimer = 0.0f;
    mTimeSinceTargetSeen = 99.0f;
    mSupportGoal = (const char *)nullptr;
    mT_siren[0] = mT_siren[1] = mT_siren[2] = WorldTimer;
}

AIVehiclePursuit::~AIVehiclePursuit() {}

void AIVehiclePursuit::ResetInternals() {
    AIVehicle::ResetInternals();
    mInPursuit = false;
    mBreaker = false;
    mChicken = false;
    mDamagedByPerp = false;
    mInFormation = false;
    mInPosition = false;

    mPursuitOffset = UMath::Vector3::kZero;

    mTimeSinceTargetSeen = 99.0f;
    mVisibiltyTestTimer = 0.25f;

    mWithinEngagementRadius = false;

    mT_siren[0] = mT_siren[1] = mT_siren[2] = WorldTimer;
    mSirenState = SIREN_OFF;
    mSirenInit = false;
}

void AIVehiclePursuit::StartPatrol() {
    SetInPursuit(false);
    GetTarget()->Clear();
    SetGoal("AIGoalPatrol");
}

void AIVehiclePursuit::StartFlee() {
    IVehicle *ivehicle;
    GetVehicle()->GlareOff(LIGHT_COPS);

    UCrc32 goal("AIGoalFleePursuit");
    if (GetSimable()->QueryInterface(&ivehicle) && ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
        goal = "AIGoalHeliExit";
    }
    if (GetGoalName() != goal) {
        ClearGoal();
        SetGoal(goal);
    }
}

void AIVehiclePursuit::StartRoadBlock() {
    IVehicle *ivehicle;
    GetVehicle()->GlareOn(LIGHT_COPS);
    SetInPursuit(true);
    GetTarget()->Clear();
    if (GetVehicle()->GetVehicleClass() == VehicleClass::CHOPPER) {
        SetGoal("AIGoalHeliRoadBlock");
    } else {
        SetGoal("AIGoalStaticRoadBlock");
    }
}

void AIVehiclePursuit::StartPursuit(AITarget *target, ISimable *itargetSimable) {
    GetVehicle()->GlareOn(LIGHT_COPS);
    if (target) {
        GetTarget()->Aquire(target);
    } else if (itargetSimable) {
        GetTarget()->Aquire(itargetSimable);
    }
    UpdateTargeting();
    SetInPursuit(true);
    if (GetVehicle()->GetVehicleClass() == VehicleClass::CHOPPER) {
        SetGoal("AIGoalHeliPursuit");
    } else {
        SetGoal("AIGoalPursuit");
    }
}

void AIVehiclePursuit::DoInPositionGoal() {
    SetGoal(mInPositionGoal);
}

void AIVehiclePursuit::EndPursuit() {
    SetInPursuit(false);
    GetVehicle()->GlareOff(LIGHT_COPS);
}

bool AIVehiclePursuit::StartSupportGoal() {
    if (mSupportGoal != (const char *)nullptr) {
        SetGoal(mSupportGoal);
        return true;
    }
    return false;
}

void AIVehiclePursuit::SetSupportGoal(UCrc32 sg) {
    mSupportGoal = sg;
}

AITarget *AIVehiclePursuit::GetPursuitTarget() {
    if (GetTarget()->IsValid()) {
        return GetTarget();
    } else {
        return nullptr;
    }
}

AITarget *AIVehiclePursuit::PursuitRequest() {
    if (!GetInPursuit() && GetTarget()->IsValid()) {
        return GetTarget();
    } else {
        return nullptr;
    }
}

void AIVehiclePursuit::Update(float dT) {
    ProfileNode profile_node("TODO", 0);
    AIVehicle::Update(dT);
    UpdateSiren(dT); // TODO

    if (!mInPursuit || !GetTarget()->IsValid()) {
        mTimeSinceTargetSeen = 0.25f;
        mVisibiltyTestTimer = 0.25f;
        return;
    }

    mVisibiltyTestTimer += dT;
    mTimeSinceTargetSeen += dT;
    if (mVisibiltyTestTimer >= 0.25f) {
        mVisibiltyTestTimer -= 0.25f;
        if (CanSeeTarget(GetTarget())) {
            mTimeSinceTargetSeen = -0.25f;
        }
    }
}
