#include "Speed/Indep/Src/AI/Gps.h"

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/Events/EGPSFinished.hpp"
#include "Speed/Indep/Src/Generated/Events/EGPSLost.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

Sim::IActivity *Gps::Construct(Sim::Param params) {
    Gps *gps = Get();
    if (!gps) {
        gps = new Gps();
    }
    if (gps) {
        return static_cast<Sim::IActivity *>(gps);
    }
    return nullptr;
}

Gps::Gps()
    : Activity(0) //
    , mRoadNav() //
    , mState(GPS_DOWN) {
    mArrowModel = new eModel();
    mArrowModel->Init(bStringHash("gps_arrow"));
    mTask = AddTask(UCrc32("Gps"), 0.0f, 0.0f, Sim::TaskMode(1));
    Sim::ProfileTask(mTask, "Gps");
    mTarget = UMath::Vector3::kZero;
    mPosition = UMath::Vector3::kZero;
    mDirection = UMath::Vector3::kZero;
    mRoadNav.SetNavType(WRoadNav::kTypeNone);
    mRoadNav.SetLaneType(WRoadNav::kLaneAny);
    mPathDist = 0.0f;
    mDrawn = false;
    mMaxDeviation = 200.0f;
    mScale = 0.0f;
    mAngle = 0.0f;
}

Gps::~Gps() {
    mRoadNav.CancelPathFinding();
    RemoveTask(mTask);
    if (mArrowModel) {
        mArrowModel->UnInit();
        delete mArrowModel;
    }
    mArrowModel = nullptr;
}

bool Gps::OnTask(HSIMTASK htask, float dT) {
    bool matched = htask == mTask;
    if (matched) {
        Update(dT);
    }
    return matched;
}

bool Gps::Engage(const UMath::Vector3 &target, float maxDeviation) {
    mState = GPS_DOWN;
    mTarget = target;
    mRoadNav.CancelPathFinding();
    mDrawn = false;
    mDeviation = 0.0f;
    mScale = 0.0f;
    if (maxDeviation < 0.1f) {
        mMaxDeviation = 200.0f;
    } else {
        mMaxDeviation = maxDeviation;
    }
    IVehicle *vehicle = IVehicle::First(VEHICLE_PLAYERS);
    if (vehicle) {
        ICollisionBody *body;
        if (vehicle->QueryInterface(&body)) {
            const UMath::Vector3 &direction = body->GetForwardVector();
            const UMath::Vector3 &position = body->GetPosition();
            mRoadNav.SetNavType(WRoadNav::kTypePath);
            mRoadNav.SetPathType(WRoadNav::kPathGPS);
            mRoadNav.SetLaneType(WRoadNav::kLaneRacing);
            mRoadNav.InitAtPoint(position, direction, true, 1.0f);
            if (!mRoadNav.IsValid()) {
                return false;
            }
            if (!mRoadNav.FindPathNow(&mTarget, nullptr, nullptr)) {
                return false;
            }
            if (mRoadNav.OnPath()) {
                mPosition = mRoadNav.GetPosition();
                mDirection = mRoadNav.GetForwardVector();
                UMath::Unit(mDirection, mDirection);
                mState = GPS_TRACKING;
                mPathDist = 20.0f;
                return true;
            }
        }
    }
    return false;
}

void Gps::Update(float dT) {
    if (mState == GPS_DOWN) {
        return;
    }

    IVehicle *vehicle = IVehicle::First(VEHICLE_PLAYERS);
    if (!vehicle) {
        mState = GPS_DOWN;
        return;
    }

    IRigidBody *body;
    if (!vehicle->QueryInterface(&body)) {
        mState = GPS_DOWN;
        return;
    }

    const UMath::Vector3 &position = body->GetPosition();
    const UMath::Vector3 &velocity = body->GetLinearVelocity();
    UMath::Vector3 heading;

    vehicle->ComputeHeading(&heading);
    mRoadNav.SetNavType(WRoadNav::kTypePath);
    mRoadNav.InitAtPoint(position, heading, true, 1.0f);
    if (!mRoadNav.OnPath()) {
        mRoadNav.SetNavType(WRoadNav::kTypePath);
        mState = GPS_SEARCHING;
        mRoadNav.InitAtPath(position, true);
        if (!mRoadNav.IsValid()) {
            new EGPSLost();
            mState = GPS_DOWN;
            return;
        }
    } else {
        mState = GPS_TRACKING;
    }

    UMath::Vector3 path_dir;
    path_dir = mRoadNav.GetForwardVector();
    UMath::Unit(path_dir, path_dir);
    float speed = UMath::Dot(velocity, path_dir);
    speed = UMath::Clamp(speed * 3.0f, 20.0f, 80.0f);
    speed = UMath::Min(speed, mPathDist);
    speed = UMath::Max(speed, mPathDist);

    if (speed <= mPathDist) {
        float newDist = mPathDist - dT * 10.0f;
        if (speed < newDist) {
            speed = newDist;
        }
    } else {
        float newDist = dT * 10.0f + mPathDist;
        if (newDist < speed) {
            speed = newDist;
        }
    }

    mPathDist = speed;
    mRoadNav.IncNavPosition(mPathDist, heading, 0.0f);
    mPosition = mRoadNav.GetPosition();
    mDirection = mRoadNav.GetForwardVector();
    UMath::Unit(mDirection, mDirection);

    float distance = UMath::Distance(mTarget, vehicle->GetPosition());
    if (distance < 30.0f) {
        new EGPSFinished();
        mState = GPS_DOWN;
        return;
    }

    if (mMaxDeviation > 1e-06f && mState == GPS_SEARCHING) {
        float distance = UMath::Distance(mPosition, vehicle->GetPosition());
        if (distance > mMaxDeviation) {
            new EGPSLost();
            mState = GPS_DOWN;
            return;
        }
        mDeviation = distance / mMaxDeviation;
    } else {
        mDeviation = 0.0f;
    }
}

void GPS_Disengage() {
    Gps *gps = Gps::Get();
    if (gps) {
        gps->Disengage();
    }
}

void GPS_Engage(const UMath::Vector3 &target, float maxDeviation) {
    Gps *gps = Gps::Get();
    if (gps) {
        gps->Engage(target, maxDeviation);
    }
}

bool GPS_IsEngaged() {
    Gps *gps = Gps::Get();
    if (gps) {
        return gps->IsEngaged();
    }
    return false;
}

void RenderGpsArrows(eView *view) {
    Gps *gps = Gps::Get();
    if (gps) {
        gps->Render(view);
    }
}
