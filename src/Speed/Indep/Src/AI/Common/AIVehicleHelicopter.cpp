#include "Speed/Indep/Src/AI/AIVehicleHelicopter.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitlevels.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/PhysicsObject.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

bool HeliVehicleActive() {
    if (gHeliVehicle) {
        return true;
    } else {
        return false;
    }
}

AIVehicleHelicopter::AIVehicleHelicopter(const BehaviorParams &bp)
    : AIVehiclePursuit(bp),          //
      IAIHelicopter(bp.fowner),      //
      mPerpHiddenFromMe(false),      //
      mHeliFuelTimeRemaining(30.0f), //
      mDustStormIntensity(0.0f),     //
      mShadowScale(0.0f) {
    static float height = 0.0f;

    height += 10.0f;

    GetOwner()->QueryInterface(&mISimpleChopper);
    mStrafeToDest = false;

    mLookAtPosition = UMath::Vector3::kZero;
    mLastPlaceHeliSawPerp = UMath::Vector3::kZero;
    mDestinationVelocity = UMath::Vector3::kZero;
}

AIVehicleHelicopter::~AIVehicleHelicopter() {
    gHeliVehicle = nullptr;
}

Behavior *AIVehicleHelicopter::Construct(const BehaviorParams &bp) {
    return new AIVehicleHelicopter(bp);
}

void AIVehicleHelicopter::SetFuelFull() {
    gHeliVehicle = this;
    ISimable *simable = IPlayer::First(PLAYER_LOCAL)->GetSimable();
    IPerpetrator *perp;
    if (simable && simable->QueryInterface(&perp)) {
        Attrib::Gen::pursuitlevels *pursuitLevelAttrib = perp->GetPursuitLevelAttrib();
        if (pursuitLevelAttrib) {
            mHeliFuelTimeRemaining = pursuitLevelAttrib->HeliFuelTime();
        }
    }
}

void AIVehicleHelicopter::SetDestinationVelocity(const UMath::Vector3 &v) {
    mDestinationVelocity = v;
    UMath::Scale(mDestinationVelocity, 9.0f, mDestinationVelocity);
    UMath::AddScale(v, mDestinationVelocity, 0.1f, mDestinationVelocity);
}

void AIVehicleHelicopter::Update(float dT) {
    AIVehicle::Update(dT);
    GetTarget()->Aquire(IPlayer::First(PLAYER_LOCAL)->GetSimable());
    UpdateSpawnTimer(dT);
    UpdateTargeting();
    UpdateFuel(dT);
    if (GetGoal()) {
        GetGoal()->Update(dT);
    }
    const UMath::Vector3 &myPosition = GetOwner()->GetRigidBody()->GetPosition();
    WWorldPos pTopo = GetSimable()->GetWPos();
    float ground = pTopo.HeightAtPoint(myPosition);
    if (ground != 0.0f) {
        bClamp(0.0f, 0.0f, 1.0f); // unused bClamp
        float shadowSize = ground + 5.0f;
        SetShadowScale(1.0f - (myPosition.y - shadowSize) * 0.025f);
    }
}

void AIVehicleHelicopter::UpdateFuel(float dT) {
    mHeliFuelTimeRemaining -= dT;
    if (mHeliFuelTimeRemaining < 0.0f) {
        mHeliFuelTimeRemaining = 0.0f;
        if (!IsCurrentGoal("AIGoalHeliExit")) {
            SetGoal("AIGoalHeliExit");
        }
    }
}

bool AIVehicleHelicopter::StartPathToPoint(UMath::Vector3 &point) {
    ResetDriveToNav(SELECT_CENTER_LANE);
    WRoadNav *road_nav = GetDriveToNav();
    bool rv = road_nav->FindPath(&point, nullptr, nullptr);

    return rv;
}

void AIVehicleHelicopter::SteerToNav(WRoadNav *road_nav, float height, float speed, bool bStopAtDest) {
    UMath::Vector3 seekPosition = road_nav->GetPosition();
    seekPosition.y += height;
    SetDriveTarget(seekPosition);
    SetLookAtPosition(road_nav->GetPosition());
    SetDriveSpeed(speed);

    if (bStopAtDest || !road_nav->IsValid()) {
        SetDestinationVelocity(UMath::Vector3::kZero);
    } else {
        UMath::Vector3 navForward = road_nav->GetForwardVector();
        UMath::Unit(navForward, navForward);

        IRigidBody *irigidbody = GetOwner()->GetRigidBody();
        UMath::Vector3 currVel = irigidbody->GetLinearVelocity();
        float navDotVel = UMath::Dot(navForward, currVel);
        if (navDotVel < 0.0f) {
            SetDestinationVelocity(UMath::Vector3::kZero);
        } else {
            UMath::Scale(navForward, navDotVel, currVel);
            SetDestinationVelocity(currVel);
        }
    }
}

bool AIVehicleHelicopter::FilterHeliAltitude(UMath::Vector3 &point) {
    bool bValid;
    bVector2 point2D;
    point2D.x = point.z;
    point2D.y = -point.x;
    float elevation = mHeliSheetCoord.GetElevation(point2D, nullptr, &bValid);

    if (bValid) {
        if (point.y < elevation) {
            point.y = elevation;
        }
        return true;
    } else {
        return false;
    }
}

// TODO
extern bool bIgnoreHeliSheet;

// Functionally matching, but the stack is acting very weird
bool AIVehicleHelicopter::CheckHeliSheet(const UMath::Vector3 &myPosition, const UMath::Vector3 &LookAheadDest,
                                         const UMath::Vector3 &myWorkingPosition, UMath::Vector3 &dest, UMath::Vector3 &smoothingVel) {
    bool rv;
    if (bIgnoreHeliSheet == true) {
        rv = false;
        return rv;
    }
    UMath::Vector3 adjustedWorkingPosition = myWorkingPosition;
    FilterHeliAltitude(dest);

    bVector3 normal;
    float elevation;
    UMath::Vector3 lookAheadDest = LookAheadDest;

    bool bSmoothingValid;
    bVector2 point2Dsmooth;
    point2Dsmooth.x = lookAheadDest.z;
    point2Dsmooth.y = -lookAheadDest.x;
    elevation = mThirdHeliSheetCoord.GetElevation(point2Dsmooth, &normal, &bSmoothingValid);

    if (bSmoothingValid && elevation > lookAheadDest.y) {
        lookAheadDest.y = elevation;

        UMath::Vector3 me2lookAhead;
        UMath::Sub(lookAheadDest, myPosition, me2lookAhead);
        UMath::Unit(me2lookAhead, me2lookAhead);

        UMath::Vector3 normal2d;
        normal2d.x = -normal.y;
        normal2d.y = 0.0f;
        normal2d.z = normal.x;

        float cross = UMath::Abs(normal2d.x * me2lookAhead.z - normal2d.z * me2lookAhead.x);
        normal2d.x *= 0.4f * cross;
        normal2d.z *= 0.4f * cross;

        UMath::Add(me2lookAhead, normal2d, smoothingVel);
    }

    bool bAdjustedWorkingFiltered;
    bVector2 point2D;
    point2D.x = adjustedWorkingPosition.z;
    point2D.y = -adjustedWorkingPosition.x;
    elevation = mSecondaryHeliSheetCoord.GetElevation(point2D, &normal, &bAdjustedWorkingFiltered);

    if (bAdjustedWorkingFiltered && elevation > adjustedWorkingPosition.y) {
        adjustedWorkingPosition.y = elevation;

        UMath::Vector3 me2adjusted;
        UMath::Sub(adjustedWorkingPosition, myPosition, me2adjusted);
        float len_adjusted = UMath::Length(me2adjusted);

        UMath::Vector3 me2dest;
        UMath::Sub(dest, myPosition, me2dest);
        float len_dest = UMath::Length(me2dest);

        if (len_adjusted > 0.2f) {
            float scale = len_dest / len_adjusted;
            UMath::ScaleAdd(me2adjusted, scale, myPosition, adjustedWorkingPosition);
        }
        if (adjustedWorkingPosition.y > dest.y) {
            dest.y = adjustedWorkingPosition.y;
        }
        UMath::Unit(me2adjusted, me2adjusted);

        {
            UMath::Vector3 normal2d;
            normal2d.x = -normal.y;
            normal2d.y = 0.0f;
            normal2d.z = normal.x;

            {
                float cross = UMath::Abs(normal2d.x * me2adjusted.z - normal2d.z * me2adjusted.x);
                normal2d.x *= 0.3f * cross;
                normal2d.z *= 0.3f * cross;
            }

            UMath::Add(me2adjusted, normal2d, me2adjusted);
        }

        IRigidBody *irigidbody = GetOwner()->GetRigidBody();
        UMath::Scale(me2adjusted, irigidbody->GetSpeed(), me2adjusted);
        irigidbody->SetLinearVelocity(me2adjusted);
    }
    rv = bAdjustedWorkingFiltered;
    return rv;
}

void AIVehicleHelicopter::RestrictPointToRoadNet(UMath::Vector3 &seekPosition) {
    WRoadNav *roadNav = GetDriveToNav();
    roadNav->SetNavType(WRoadNav::kTypeTraffic);
    roadNav->SetLaneType(WRoadNav::kLaneTraffic);

    IRigidBody *player_rigid_body = IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody();
    UMath::Vector3 forwardVector;
    player_rigid_body->GetForwardVector(forwardVector);

    roadNav->InitAtPoint(seekPosition, forwardVector, false, 0.0f);
    if (roadNav->IsValid()) {
        seekPosition.x = roadNav->GetPosition().x;
        seekPosition.z = roadNav->GetPosition().z;
    }
}

void AIVehicleHelicopter::AvoidCamera(UMath::Vector3 &dest) {
    IRigidBody *irigidbody = GetOwner()->GetRigidBody();
    const UMath::Vector3 &myPosition = irigidbody->GetPosition();
    const UMath::Vector3 &Vlin = irigidbody->GetLinearVelocity();

    UMath::Vector3 myWorkingPosition;
    UMath::ScaleAdd(Vlin, 0.2f, myPosition, myWorkingPosition);

    bVector3 temp = eGetView(1, false)->GetCamera()->GetPositionSimSpace();
    UMath::Vector3 cameraPos = *reinterpret_cast<UMath::Vector3 *>(&temp);
    bVector3 cPos = *eGetView(1, false)->GetCamera()->GetDirection();
    UMath::Vector3 cameraForward;
    cameraForward.x = cPos.x;
    cameraForward.y = cPos.y;
    cameraForward.z = cPos.z;
    // ...
    eUnSwizzleWorldVector(*reinterpret_cast<bVector3 *>(&cameraForward), *reinterpret_cast<bVector3 *>(&cameraForward));

    UMath::ScaleAdd(cameraForward, 4.0f, cameraPos, cameraPos);
    UMath::Vector3 cam2me;
    UMath::Sub(myPosition, cameraPos, cam2me);

    float rad = UMath::Length(cam2me);
    float CamRad = 7.0f;
    if (rad < 7.0f && rad > 2.0f) {
        CamRad = rad - 0.5f;
    }
    UMath::Vector3 intersectPoint;
    bool hits = AI::Math::SegmentSphereIntersect(myPosition, myWorkingPosition, cameraPos, CamRad, intersectPoint);

    if (hits) {
        UMath::Vector3 normal;
        UMath::Direction(intersectPoint, cameraPos, normal);

        UMath::Vector3 p2ToIntersect;
        UMath::Sub(intersectPoint, myWorkingPosition, p2ToIntersect);

        float dot = UMath::Dot(p2ToIntersect, normal);
        UMath::Vector3 newWorkPos;
        UMath::ScaleAdd(normal, dot, myWorkingPosition, newWorkPos);
        dest = newWorkPos;

        dot = UMath::Dot(Vlin, normal);
        float pushOutScale = (7.0f - CamRad) * 0.5f + 1.2f;

        UMath::Vector3 newVel;
        UMath::ScaleAdd(normal, UMath::Abs(dot) * pushOutScale, Vlin, newVel);
        irigidbody->SetLinearVelocity(newVel);
    }
}

// TODO
extern float Max_Chopper_Accel;
extern float Min_Chopper_Accel;
extern float Chopper_Ratio;

void AIVehicleHelicopter::OnDriving(float dT) {
    IRigidBody *irigidbody = GetOwner()->GetRigidBody();
    const UMath::Vector3 &Vlin = irigidbody->GetLinearVelocity();
    const UMath::Vector3 &myPosition = irigidbody->GetPosition();

    UMath::Vector3 myWorkingPosition;
    UMath::ScaleAdd(Vlin, 0.45f, myPosition, myWorkingPosition);

    UMath::Vector3 lookAheadDest;
    UMath::Scale(Vlin, 0.65f, lookAheadDest);
    UMath::Add(lookAheadDest, myPosition, lookAheadDest);

    UMath::Vector3 dest;
    UMath::Vector3 compensationVelocity;

    compensationVelocity = mDestinationVelocity;
    compensationVelocity.y = 0.0f;
    UMath::Scale(compensationVelocity, 0.45f, dest);
    UMath::Add(dest, mDest, dest);

    AvoidCamera(dest);

    UMath::Vector3 smoothingVel = UMath::Vector3::kZero;
    CheckHeliSheet(myPosition, lookAheadDest, myWorkingPosition, dest, smoothingVel);

    UMath::Vector3 moveDirVector;
    UMath::Sub(dest, myWorkingPosition, moveDirVector);

    float moveLen = UMath::Length(moveDirVector);
    float rbSpeed = irigidbody->GetSpeed();
    float yDelt = moveDirVector.y;
    moveDirVector.y = 0.0f;

    UMath::Vector3 Vdif;
    UMath::Sub(Vlin, compensationVelocity, Vdif);

    UMath::Vector3 unitMoveDir;
    UMath::Unit(moveDirVector, unitMoveDir);

    float closingSpeed = UMath::Dot(Vdif, unitMoveDir);
    float closingSpeedUnadjusted = UMath::Dot(Vlin, unitMoveDir);
    mISimpleChopper->MaxDeceleration(false);

    if (closingSpeed > 3.0f) {
        float decelAvailable = rbSpeed * 0.2f + Min_Chopper_Accel;
        if (decelAvailable > Max_Chopper_Accel) {
            decelAvailable = Max_Chopper_Accel;
        }
        float decelToBeUsed = closingSpeed * Chopper_Ratio;
        if (decelToBeUsed > decelAvailable) {
            decelToBeUsed = decelAvailable;
        }
        float D = (closingSpeed * closingSpeed) / (1.55f * decelToBeUsed);
        if (moveLen <= D) {
            mDriveSpeed *= 0.4f;
            mISimpleChopper->MaxDeceleration(true);
        }
    } else {
        if (closingSpeedUnadjusted < -1.0f && moveLen < 15.0f) {
            mISimpleChopper->MaxDeceleration(true);
        }
    }

    if (bIgnoreHeliSheet) {
        mISimpleChopper->MaxDeceleration(true);
    }

    if (moveLen > 2.0f) {
        UMath::Scale(moveDirVector, mDriveSpeed / moveLen, moveDirVector);
    }

    if (yDelt >= 0.0f) {
        if (yDelt < 7.0f) {
            yDelt *= 5.0f;
            if (mDestinationVelocity.y > 0.0f) {
                yDelt += mDestinationVelocity.y * 2.0f;
            }
        } else if (yDelt < 0.0f) {
            goto block_19;
        }
    } else {
    block_19:
        if (yDelt > -5.0f) {
            yDelt *= 3.0f;
            if (mDestinationVelocity.y < 0.0f) {
                yDelt += mDestinationVelocity.y;
            }
        }
    }
    moveDirVector.y = yDelt;

    float lenSmooth = UMath::Length(smoothingVel);
    if (lenSmooth > 0.1f) {
        UMath::Scale(smoothingVel, mDriveSpeed / lenSmooth * 0.3f, smoothingVel);
        UMath::ScaleAdd(moveDirVector, 0.7f, smoothingVel, moveDirVector);
    }
    mISimpleChopper->SetDesiredVelocity(moveDirVector);

    UMath::Vector3 lookVector;
    UMath::Sub(mLookAtPosition, irigidbody->GetPosition(), lookVector);
    mISimpleChopper->SetDesiredFacingVector(lookVector);
}
