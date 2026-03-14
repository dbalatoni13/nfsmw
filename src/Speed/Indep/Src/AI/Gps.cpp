#include "Speed/Indep/Src/AI/Gps.h"

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Generated/Events/EGPSFinished.hpp"
#include "Speed/Indep/Src/Generated/Events/EGPSLost.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern float RealTimeElapsed;
extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;

inline bMatrix4 *eFrameMallocMatrix(int num_matrices) {
    unsigned int size = num_matrices * sizeof(bMatrix4);
    unsigned char *address = CurrentBufferPos;
    unsigned char *new_pos = CurrentBufferPos + size;
    if (CurrentBufferEnd <= new_pos) {
        FrameMallocFailed = 1;
        FrameMallocFailAmount += size;
        address = nullptr;
        new_pos = CurrentBufferPos;
    }
    CurrentBufferPos = new_pos;
    return reinterpret_cast<bMatrix4 *>(address);
}

Sim::IActivity *Gps::Construct(Sim::Param params) {
    Gps *gps = Get();
    if (!gps) {
        gps = new Gps();
    }
    Sim::IActivity *result = nullptr;
    if (gps) {
        result = static_cast<Sim::IActivity *>(gps);
    }
    return result;
}

UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::Prototype _Gps("Gps", Gps::Construct);

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
    mRoadNav.SetNavType(WRoadNav::kTypePath);
    mRoadNav.SetPathType(WRoadNav::kPathGPS);
    mRoadNav.SetLaneType(WRoadNav::kLaneRacing);
    mPathDist = 0.0f;
    mDrawn = false;
    mMaxDeviation = 200.0f;
    mScale = 0.0f;
    mAngle = 0.0f;
    mDeviation = 0.0f;
}

Gps::~Gps() {
    mRoadNav.CancelPathFinding();
    RemoveTask(mTask);
    if (mArrowModel) {
        delete mArrowModel;
    }
    mArrowModel = nullptr;
}

bool Gps::OnTask(HSIMTASK htask, float dT) {
    if (htask != mTask) return false;
    Update(dT);
    return true;
}

bool Gps::Engage(const UMath::Vector3 &target, float maxDeviation) {
    mState = GPS_DOWN;
    mTarget = target;
    mRoadNav.CancelPathFinding();
    mDrawn = false;
    mScale = 0.0f;
    mDeviation = 0.0f;
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

void Gps::Render(eView *view) {
    if (view->GetID() != 1) return;
    if (UTL::Collections::Singleton<INIS>::Exists()) return;
    if (FEManager::IsPaused()) return;

    eGPSState state = mState;
    if (state == GPS_DOWN) return;

    CameraMover *camera_mover = view->GetCameraMover();
    if (!camera_mover) return;
    if (!camera_mover->IsDriveCamera()) return;

    UMath::Vector3 nav_position = mPosition;

    Camera *camera = camera_mover->GetCamera();
    float camera_speed = bLength(camera->GetVelocityPosition());

    if (state == GPS_SEARCHING) {
        mScale += RealTimeElapsed * (mDeviation + mDeviation + 1.5f);
        mScale = UMath::Mod(mScale, 1.0f);
    } else {
        if (mScale > 0.0f) {
            mScale -= RealTimeElapsed * 1.5f;
            mScale = UMath::Max(mScale, 0.0f);
        }
    }

    float extra_scale = UMath::Sina(mScale) * 0.1f + 1.0f;

    UMath::ScaleAdd(mDirection, camera_speed, nav_position, nav_position);

    bVector3 position_to_point_at;
    eSwizzleWorldVector(nav_position, position_to_point_at);

    camera = camera_mover->GetCamera();
    bMatrix4 *world_to_camera = camera->GetWorldToCameraMatrix();
    bMatrix4 camera_to_world;
    eInvertTransformationMatrix(&camera_to_world, world_to_camera);

    unsigned short half_fov = camera->GetFov() >> 1;
    if (Sim::GetUserMode() == 1) {
        half_fov >>= 1;
    }

    float alt_dist = 1.0f;
    if (eGetCurrentViewMode() == 2 && FEDatabase->GetGameplaySettings()->RearviewOn) {
        alt_dist = 1.5f;
    }

    float scale = bCos(half_fov);
    scale += scale;
    unsigned short angle = static_cast<unsigned short>(-static_cast<int>(static_cast<float>(half_fov) * 0.5f));
    float sin, cos;
    bSinCos(&sin, &cos, angle);

    bVector3 v_pos(0.0f, sin * scale, cos * scale * alt_dist);
    bMulMatrix(&v_pos, &camera_to_world, &v_pos);

    bVector3 v_ray(v_pos - position_to_point_at);
    v_ray.z = 0.0f;
    bNormalize(&v_ray, &v_ray);

    float desired_angle = UMath::Atan2r(v_ray.y, v_ray.x);

    if (!mDrawn) {
        mAngle = desired_angle;
    } else {
        float dist = UMath::Abs(mAngle - desired_angle);
        float circle_dist = UMath::Abs(dist - (float)M_TWOPI);
        float rotation_scale = UMath::Min(dist, circle_dist);
        float roatation_speed = UMath::Lerp(0.125f, 1.0f, UMath::Ramp(rotation_scale, 0.0f, (float)M_PI)) * RealTimeElapsed * (float)M_TWOPI;

        float rotation = roatation_speed;
        if (mAngle > desired_angle) {
            rotation = -roatation_speed;
        }
        float abs_rotation = rotation;
        if (dist > (float)M_PI) {
            abs_rotation = -rotation;
        }

        if (rotation_scale <= roatation_speed) {
            mAngle = desired_angle;
        } else {
            mAngle = mAngle + abs_rotation;
        }

        desired_angle = mAngle;
        if (desired_angle > (float)M_PI) {
            mAngle = desired_angle - (float)M_TWOPI;
        } else if (desired_angle < -(float)M_PI) {
            mAngle = desired_angle + (float)M_TWOPI;
        }
    }

    v_ray = bVector3(UMath::Cosr(mAngle), UMath::Sinr(mAngle), 0.0f);
    bScale(&v_ray, &v_ray, 0.2f);

    bVector3 v_left;
    bVector3 v_up(0.0f, 0.0f, 1.0f);
    bCross(&v_left, &v_up, &v_ray);
    bNormalize(&v_left, &v_left);

    bCross(&v_up, &v_ray, &v_left);
    bNormalize(&v_up, &v_up);

    bMatrix4 *pMatrix = eFrameMallocMatrix(1);

    if (pMatrix) {
        v_left *= extra_scale;
        v_up *= extra_scale;
        v_ray *= extra_scale;

        bCopy(&pMatrix->v0, &v_left, 0.0f);
        bCopy(&pMatrix->v1, &v_up, 0.0f);
        bCopy(&pMatrix->v2, &v_ray, 0.0f);
        bCopy(&pMatrix->v3, &v_pos, 1.0f);

        view->Render(mArrowModel, pMatrix, nullptr, 0, 0);
        mDrawn = true;
    }
}

void GPS_Disengage() {
    Gps *gps = Gps::Get();
    if (gps) {
        gps->Disengage();
    }
}

bool GPS_Engage(const UMath::Vector3 &target, float maxDeviation) {
    Gps *gps = Gps::Get();
    if (gps) {
        return gps->Engage(target, maxDeviation);
    }
    return false;
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
