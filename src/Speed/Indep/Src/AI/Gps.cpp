#include "Speed/Indep/Src/AI/AIBasics.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/Events/EGPSFinished.hpp"
#include "Speed/Indep/Src/Generated/Events/EGPSLost.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

// total size: 0x388
struct Gps : public Sim::Activity, public UTL::Collections::Singleton<Gps> {
    enum eGPSState {
        GPS_DOWN = 0,
        GPS_SEARCHING = 1,
        GPS_TRACKING = 2,
    };

    USE_FASTALLOC(Gps);

    static Sim::IActivity *Construct(Sim::Param params);

    Gps();

    ~Gps() override;

    bool OnTask(HSIMTASK htask, float dT) override;

    void Update(float dT);

    bool Engage(const UMath::Vector3 &target, float maxDeviation);

    void Render(eView *view);

    bool IsEngaged() const {
        return mState != GPS_DOWN;
    }

    void Disengage() {
        mState = GPS_DOWN;
    }

    UMath::Vector3 mTarget;    // offset 0x50, size 0xC
    HSIMTASK mTask;            // offset 0x5C, size 0x4
    UMath::Vector3 mPosition;  // offset 0x60, size 0xC
    eModel *mArrowModel;       // offset 0x6C, size 0x4
    UMath::Vector3 mDirection; // offset 0x70, size 0xC
    eGPSState mState;          // offset 0x7C, size 0x4
    WRoadNav mRoadNav;         // offset 0x80, size 0x2F0
    float mPathDist;           // offset 0x370, size 0x4
    bool mDrawn;               // offset 0x374, size 0x1
    float mAngle;              // offset 0x378, size 0x4
    float mScale;              // offset 0x37C, size 0x4
    float mMaxDeviation;       // offset 0x380, size 0x4
    float mDeviation;          // offset 0x384, size 0x4
};

IMPLEMENT_SINGLETON(Gps);

Sim::IActivity *Gps::Construct(Sim::Param params) {
    Gps *gps = Gps::Get();

    if (gps == NULL) {
        gps = new Gps();
    }

    return gps;
}

BIND_ACTIVITY_FACTORY(Gps);

// Los dos `stfs` finales del static-init usan el registro que YA tiene el 0.0f
// (f30, vivo desde el `fmr f30,f31` del indice 1128); con -3.0f haria falta un
// pseudo nuevo que ademas cruza el `bl stringhash32` del final, o sea un
// SALVADO, y eso corria la asignacion de f27/f28/f29 en toda la cola.
bVector2 Tweak_ForceGPSArrowTo(0.0f, 0.0f);

Gps::Gps() : Sim::Activity(0) {
    mArrowModel = new eModel;
    mArrowModel->Init(bStringHash("MARKER_DIRECTION_AID"));

    mTask = AddTask("WorldUpdate", 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(mTask, "GPS");
    mTarget = UMath::Vector3::kZero;
    mPosition = UMath::Vector3::kZero;
    mDirection = UMath::Vector3::kZero;
    mState = GPS_DOWN;
    mRoadNav.SetNavType(WRoadNav::kTypePath);
    mRoadNav.SetPathType(WRoadNav::kPathGPS);
    mRoadNav.SetLaneType(WRoadNav::kLaneRacing);
    mPathDist = 20.0f;
    mDrawn = false;
    mAngle = 0.0f;
    mScale = 0.0f;
    mMaxDeviation = 200.0f;
    mDeviation = 0.0f;
}

Gps::~Gps() {
    mRoadNav.CancelPathFinding();
    RemoveTask(mTask);
    delete mArrowModel;
    mArrowModel = NULL;
}

bool Gps::OnTask(HSIMTASK htask, float dT) {
    if (htask == mTask) {
        Update(dT);
        return true;
    }

    return false;
}

void Gps::Update(float dT) {
    if (mState == GPS_DOWN) {
        return;
    }

    IVehicle *vehicle = UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::First(VEHICLE_PLAYERS);
    IRigidBody *body;

    if (!vehicle || !vehicle->QueryInterface(&body)) {
        Disengage();
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
            Disengage();
            return;
        }
    } else {
        mState = GPS_TRACKING;
    }

    UMath::Vector3 path_dir = mRoadNav.GetForwardVector();

    UMath::Unit(path_dir);

    float speed = UMath::Dot(velocity, path_dir);

    float path_dist = UMath::Clamp(speed * 3.0f, 20.0f, 80.0f);

    if (path_dist > mPathDist) {
        mPathDist = UMath::Min(mPathDist + dT * 10.0f, path_dist);
    } else {
        mPathDist = UMath::Max(mPathDist - dT * 10.0f, path_dist);
    }

    mRoadNav.IncNavPosition(mPathDist, path_dir, 0.0f);

    mPosition = mRoadNav.GetPosition();
    mDirection = mRoadNav.GetForwardVector();

    UMath::Unit(mDirection);

    if (UMath::Distance(mTarget, vehicle->GetPosition()) < 30.0f) {
        new EGPSFinished();

        Disengage();
        return;
    }

    if (mMaxDeviation > 0.000001f && mState == GPS_SEARCHING) {
        float distance = UMath::Distance(mPosition, vehicle->GetPosition());

        if (distance > mMaxDeviation) {
            new EGPSLost();
            Disengage();
            return;
        }

        mDeviation = distance / mMaxDeviation;
    } else {
        mDeviation = 0.0f;
    }
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

    IVehicle *vehicle = UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::First(VEHICLE_PLAYERS);
    ICollisionBody *body;

    if (vehicle && vehicle->QueryInterface(&body)) {
        const UMath::Vector3 &direction = body->GetForwardVector();
        const UMath::Vector3 &position = body->GetPosition();

        mRoadNav.SetNavType(WRoadNav::kTypePath);
        mRoadNav.SetPathType(WRoadNav::kPathGPS);
        mRoadNav.SetLaneType(WRoadNav::kLaneRacing);
        mRoadNav.InitAtPoint(position, direction, true, 1.0f);

        if (!mRoadNav.IsValid()) {
            return false;
        }

        if (!mRoadNav.FindPathNow(&mTarget, NULL, NULL)) {
            return false;
        }

        if (mRoadNav.OnPath()) {
            mPosition = mRoadNav.GetPosition();
            mDirection = mRoadNav.GetForwardVector();
            UMath::Unit(mDirection);

            mState = GPS_TRACKING;
            mPathDist = 20.0f;

            return true;
        }
    }

    return false;
}

void Gps::Render(eView *view) {
    if (view->GetID() != 1) {
        return;
    }

    if (INIS::Exists()) {
        return;
    }

    if (FEManager::IsPaused()) {
        return;
    }

    eGPSState state = mState;

    if (state == GPS_DOWN) {
        return;
    }

    CameraMover *camera_mover = view->GetCameraMover();

    if (!camera_mover || !camera_mover->IsDriveCamera()) {
        return;
    }

    UMath::Vector3 nav_position = mPosition;
    float camera_speed = bLength(camera_mover->GetCamera()->GetVelocityPosition());

    if (state == GPS_SEARCHING) {
        mScale += RealTimeElapsed * (mDeviation * 2.0f + 1.5f);
        mScale = UMath::Mod(mScale, 1.0f);
    } else if (mScale > 0.0f) {
        mScale -= RealTimeElapsed * 1.5f;
        mScale = UMath::Max(mScale, 0.0f);
    }

    float extra_scale = UMath::Sina(mScale) * 0.1f + 1.0f;

    UMath::ScaleAdd(mDirection, camera_speed, nav_position, nav_position);

    bVector3 position_to_point_at;

    eSwizzleWorldVector(nav_position, position_to_point_at);

    Camera *camera = camera_mover->GetCamera();
    bMatrix4 *world_to_camera = camera->GetCameraMatrix();
    bMatrix4 camera_to_world;

    eInvertTransformationMatrix(&camera_to_world, world_to_camera);

    unsigned short half_fov = camera->GetFieldOfView() / 2;

    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        half_fov /= 2;
    }

    float alt_dist = 1.0f;

    if (eGetCurrentViewMode() == EVIEWMODE_ONE_RVM && FEDatabase->GetGameplaySettings()->RearviewOn) {
        alt_dist = 1.5f;
    }

    float scale = bCos(half_fov) * 2.0f;
    unsigned short angle = static_cast<unsigned short>(half_fov * 0.5f);
    float sin;
    float cos;

    bSinCos(&sin, &cos, -angle);

    bVector3 v_pos(0.0f, sin * scale, cos * scale * alt_dist);

    bMulMatrix(&v_pos, &camera_to_world, &v_pos);

    bVector3 v_ray = v_pos - position_to_point_at;

    v_ray.z = 0.0f;

    bNormalize(&v_ray, &v_ray);

    float desired_angle = UMath::Atan2r(v_ray.y, v_ray.x);

    if (!mDrawn) {
        mAngle = desired_angle;
    } else {
        float dist = UMath::Abs(mAngle - desired_angle);
        float circle_dist = UMath::Abs(dist - 6.2831855f);
        float rotation_scale = UMath::Min(dist, circle_dist);
        float roatation_speed = UMath::Lerp(0.125f, 1.0f, UMath::Ramp(rotation_scale, 0.0f, 3.1415927f));
        float rotation = RealTimeElapsed * roatation_speed * 6.2831855f;
        float abs_rotation = rotation;

        if (mAngle > desired_angle) {
            rotation = -rotation;
        }

        if (dist > 3.1415927f) {
            rotation = -rotation;
        }

        if (rotation_scale <= abs_rotation) {
            mAngle = desired_angle;
        } else {
            mAngle += rotation;
        }

        if (mAngle > 3.1415927f) {
            mAngle -= 6.2831855f;
        } else if (mAngle < -3.1415927f) {
            mAngle += 6.2831855f;
        }
    }

    v_ray.x = UMath::Cosr(mAngle);
    v_ray.y = UMath::Sinr(mAngle);

    bScale(&v_ray, &v_ray, 0.2f);

    bVector3 v_left;
    bVector3 v_up(0.0f, 0.0f, 1.0f);

    bCross(&v_left, &v_up, &v_ray);
    bNormalize(&v_left, &v_left, 0.2f);
    bCross(&v_up, &v_ray, &v_left);
    bNormalize(&v_up, &v_up, 0.2f);

    bMatrix4 *pMatrix = eFrameMallocMatrix(1);

    if (pMatrix) {
        v_left *= extra_scale;
        v_up *= extra_scale;
        v_ray *= extra_scale;

        bCopy(&pMatrix->v0, &v_left, 0.0f);
        bCopy(&pMatrix->v1, &v_up, 0.0f);
        bCopy(&pMatrix->v2, &v_ray, 0.0f);
        bCopy(&pMatrix->v3, &v_pos, 1.0f);

        view->Render(mArrowModel, pMatrix, NULL, 0, 0);

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

