#include "Speed/Indep/Src/Camera/Movers/SelectCar.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"

#include <cmath>

extern int CarGuysCamera;

static float kSelectCarFrameRate = 60.0f;
static float kSelectCarUpperOrbitV = 110.0f;
static float kSelectCarLowerOrbitV = 94.5f;
static float kSelectCarRadiusSpeedScale = 0.125f;
static float kSelectCarUpperRadius = 6.65f;
static float kSelectCarLowerRadius = 4.65f;
static float kSelectCarWrapAngle = 360.0f;

SelectCarCameraMover::~SelectCarCameraMover() {}

static float kSelectCarDefaultRollV = 0.0f;
static float kSelectCarDefaultFOVV = 45.0f;
static float kSelectCarDefaultLookAtZV = 0.75f;
static float kSelectCarDefaultAnimTimeV = 0.555f;

void SelectCarCameraMover::SetVRotateSpeed(float f) {
    if (ControlMode != 0) {
        if (ControlMode != 1) {
            StartAnimCameraData.RollAngle = CurrentCameraData.RollAngle;
            StartAnimCameraData.FOV = CurrentCameraData.FOV;
            StartAnimCameraData.LookAt = CurrentCameraData.LookAt;
            GoalAnimCameraData.RollAngle = kSelectCarDefaultRollV;
            GoalAnimCameraData.FOV = kSelectCarDefaultFOVV;
            bFill(&GoalAnimCameraData.LookAt, kSelectCarDefaultRollV, kSelectCarDefaultRollV, kSelectCarDefaultLookAtZV);
            GoalAnimCameraData.RollAngle = FindBestAngleGoal(CurrentCameraData.RollAngle, kSelectCarDefaultRollV);
            CurrentAnimationTime = kSelectCarDefaultRollV;
            TotalAnimationTime = kSelectCarDefaultAnimTimeV;
        }
        OrbitVSpeed = f;
        ControlMode = 1;
    }
}

static float kSelectCarDefaultRollH = 0.0f;
static float kSelectCarDefaultFOVH = 45.0f;
static float kSelectCarDefaultLookAtZH = 0.75f;
static float kSelectCarDefaultAnimTimeH = 0.555f;

void SelectCarCameraMover::SetHRotateSpeed(float f) {
    if (ControlMode != 0) {
        if (ControlMode != 1) {
            StartAnimCameraData.RollAngle = CurrentCameraData.RollAngle;
            StartAnimCameraData.FOV = CurrentCameraData.FOV;
            StartAnimCameraData.LookAt = CurrentCameraData.LookAt;
            GoalAnimCameraData.RollAngle = kSelectCarDefaultRollH;
            GoalAnimCameraData.FOV = kSelectCarDefaultFOVH;
            bFill(&GoalAnimCameraData.LookAt, kSelectCarDefaultRollH, kSelectCarDefaultRollH, kSelectCarDefaultLookAtZH);
            GoalAnimCameraData.RollAngle = FindBestAngleGoal(CurrentCameraData.RollAngle, kSelectCarDefaultRollH);
            CurrentAnimationTime = kSelectCarDefaultRollH;
            TotalAnimationTime = kSelectCarDefaultAnimTimeH;
        }
        OrbitHSpeed = f;
        ControlMode = 1;
    }
}

static float kSelectCarDefaultRollZ = 0.0f;
static float kSelectCarDefaultFOVZ = 45.0f;
static float kSelectCarDefaultLookAtZZ = 0.75f;
static float kSelectCarDefaultAnimTimeZ = 0.555f;

void SelectCarCameraMover::SetZoomSpeed(float f) {
    if (ControlMode != 0) {
        if (ControlMode != 1) {
            StartAnimCameraData.RollAngle = CurrentCameraData.RollAngle;
            StartAnimCameraData.FOV = CurrentCameraData.FOV;
            StartAnimCameraData.LookAt = CurrentCameraData.LookAt;
            GoalAnimCameraData.RollAngle = kSelectCarDefaultRollZ;
            GoalAnimCameraData.FOV = kSelectCarDefaultFOVZ;
            bFill(&GoalAnimCameraData.LookAt, kSelectCarDefaultRollZ, kSelectCarDefaultRollZ, kSelectCarDefaultLookAtZZ);
            GoalAnimCameraData.RollAngle = FindBestAngleGoal(CurrentCameraData.RollAngle, kSelectCarDefaultRollZ);
            CurrentAnimationTime = kSelectCarDefaultRollZ;
            TotalAnimationTime = kSelectCarDefaultAnimTimeZ;
        }
        RadiusSpeed = f;
        ControlMode = 1;
    }
}

SelectCarCameraMover::SelectCarCameraMover(int view_id) : CameraMover(view_id, CM_SELECT_CAR) {
    CurrentCameraData.OrbitVAngle = 0.0f;
    CurrentCameraData.OrbitHAngle = 0.0f;
    CurrentCameraData.Radius = 0.0f;
    CurrentCameraData.RollAngle = 0.0f;
    CurrentCameraData.FOV = 0.0f;
    bFill(&CurrentCameraData.LookAt, 0.0f, 0.0f, 0.0f);
    StartAnimCameraData = CurrentCameraData;
    GoalAnimCameraData = CurrentCameraData;
    RadiusSpeed = 0.0f;
    OrbitVSpeed = 0.0f;
    OrbitHSpeed = 0.0f;
    CurrentAnimationTime = 0.0f;
    ControlMode = 1;
    LookingAtParts = 0;
    TotalAnimationTime = 1.0f;
    Periods = 2;
    Damping = 5.0f;
}

void SelectCarCameraMover::Update(float dT) {
    int screen_print_x;
    int screen_print_y;
    SelectCarCameraData *camera_data = &CurrentCameraData;

    if (ControlMode != 2) {
        float animiation_amount = 1.0f;
        CurrentAnimationTime = CurrentAnimationTime + dT;
        if (0.0f < TotalAnimationTime && CurrentAnimationTime < TotalAnimationTime) {
            animiation_amount = CurrentAnimationTime / TotalAnimationTime;
        }
        float aa2 = animiation_amount * animiation_amount;
        float anim = 1.0f - static_cast<float>(expf(-Damping * aa2)) *
                                static_cast<float>(cosf((static_cast<float>(Periods) + 0.5f) * aa2 * 3.14159265f));
        if (ControlMode == 1) {
            float the_frame_rate = dT * kSelectCarFrameRate;

            camera_data->OrbitHAngle = OrbitHSpeed * the_frame_rate + camera_data->OrbitHAngle;
            float possibleNewOrbitV = OrbitVSpeed * the_frame_rate + camera_data->OrbitVAngle;
            if (CarGuysCamera == 0) {
                if (possibleNewOrbitV > kSelectCarUpperOrbitV) {
                    possibleNewOrbitV = kSelectCarUpperOrbitV;
                } else if (possibleNewOrbitV < kSelectCarLowerOrbitV) {
                    possibleNewOrbitV = kSelectCarLowerOrbitV;
                }
            }
            camera_data->OrbitVAngle = possibleNewOrbitV;
            float possibleNewRadius = RadiusSpeed * (the_frame_rate * kSelectCarRadiusSpeedScale) + camera_data->Radius;
            if (CarGuysCamera == 0) {
                if (possibleNewRadius > kSelectCarUpperRadius) {
                    possibleNewRadius = kSelectCarUpperRadius;
                } else if (possibleNewRadius < kSelectCarLowerRadius) {
                    possibleNewRadius = kSelectCarLowerRadius;
                }
            }
            float complement = 1.0f - anim;
            camera_data->Radius = possibleNewRadius;
            camera_data->RollAngle = complement * StartAnimCameraData.RollAngle + anim * GoalAnimCameraData.RollAngle;
            camera_data->FOV = complement * StartAnimCameraData.FOV + anim * GoalAnimCameraData.FOV;
            bVector3 lookat_change = GoalAnimCameraData.LookAt - StartAnimCameraData.LookAt;
            lookat_change *= anim;
            camera_data->LookAt = StartAnimCameraData.LookAt + lookat_change;
            if (kSelectCarWrapAngle < camera_data->OrbitHAngle) {
                camera_data->OrbitHAngle = camera_data->OrbitHAngle - kSelectCarWrapAngle;
            }
            if (camera_data->OrbitHAngle < 0.0f) {
                camera_data->OrbitHAngle = camera_data->OrbitHAngle + kSelectCarWrapAngle;
            }
        } else if (ControlMode == 0) {
            float complement = 1.0f - anim;
            camera_data->OrbitVAngle = complement * StartAnimCameraData.OrbitVAngle + anim * GoalAnimCameraData.OrbitVAngle;
            camera_data->OrbitHAngle = complement * StartAnimCameraData.OrbitHAngle + anim * GoalAnimCameraData.OrbitHAngle;
            camera_data->Radius = complement * StartAnimCameraData.Radius + anim * GoalAnimCameraData.Radius;
            camera_data->RollAngle = complement * StartAnimCameraData.RollAngle + anim * GoalAnimCameraData.RollAngle;
            camera_data->FOV = complement * StartAnimCameraData.FOV + anim * GoalAnimCameraData.FOV;
            bVector3 lookat_change = GoalAnimCameraData.LookAt - StartAnimCameraData.LookAt;
            lookat_change *= anim;
            camera_data->LookAt = StartAnimCameraData.LookAt + lookat_change;
            if (kSelectCarWrapAngle < camera_data->OrbitHAngle) {
                camera_data->OrbitHAngle = camera_data->OrbitHAngle - kSelectCarWrapAngle;
            }
            if (camera_data->OrbitHAngle < 0.0f) {
                camera_data->OrbitHAngle = camera_data->OrbitHAngle + kSelectCarWrapAngle;
            }
            if (kSelectCarWrapAngle < camera_data->RollAngle) {
                camera_data->RollAngle = camera_data->RollAngle - kSelectCarWrapAngle;
            }
            if (camera_data->RollAngle < 0.0f) {
                camera_data->RollAngle = camera_data->RollAngle + kSelectCarWrapAngle;
            }
            if (ControlMode == 0 && 1.0f <= animiation_amount) {
                ControlMode = 2;
            }
        }
    }
    bMatrix4 camera_matrix;
    CreateCameraMatrix(&camera_matrix, camera_data);
    screen_print_x = bDegToAng(camera_data->FOV);
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetFieldOfView(static_cast<unsigned short>(screen_print_x));
    }
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetTargetDistance(camera_data->Radius);
    }
    GetCamera()->SetCameraMatrix(camera_matrix, dT);
}

void SelectCarCameraMover::SetCurrentOrientation(bVector3 &orbit, float roll, float fov, bVector3 &lookAt) {
    CurrentCameraData.OrbitVAngle = orbit.x;
    CurrentCameraData.OrbitHAngle = orbit.y;
    CurrentCameraData.Radius = orbit.z;
    CurrentCameraData.RollAngle = roll;
    CurrentCameraData.FOV = fov;
    CurrentCameraData.LookAt = lookAt;
}

void SelectCarCameraMover::SetDesiredOrientation(bVector3 &orbit, float roll, float fov, bVector3 &lookAt, float animSpeed, float damping,
                                                  int periods) {
    StartAnimCameraData = CurrentCameraData;
    ControlMode = 0;
    GoalAnimCameraData = CurrentCameraData;
    GoalAnimCameraData.OrbitVAngle = orbit.x;
    GoalAnimCameraData.OrbitHAngle = orbit.y;
    GoalAnimCameraData.Radius = orbit.z;
    GoalAnimCameraData.RollAngle = roll;
    GoalAnimCameraData.FOV = fov;
    GoalAnimCameraData.LookAt = lookAt;
    Damping = damping;
    Periods = periods;
    GoalAnimCameraData.OrbitVAngle = FindBestAngleGoal(StartAnimCameraData.OrbitVAngle, GoalAnimCameraData.OrbitVAngle);
    GoalAnimCameraData.OrbitHAngle = FindBestAngleGoal(StartAnimCameraData.OrbitHAngle, GoalAnimCameraData.OrbitHAngle);
    GoalAnimCameraData.RollAngle = FindBestAngleGoal(StartAnimCameraData.RollAngle, GoalAnimCameraData.RollAngle);
    CurrentAnimationTime = 0.0f;
    TotalAnimationTime = animSpeed;
}

float SelectCarCameraMover::FindBestAngleGoal(float start, float goal) {
    float normal_h_diff = bAbs(start - goal);
    float over_h_diff = bAbs(start - (goal + kSelectCarWrapAngle));
    float under_h_diff = bAbs(start - (goal - kSelectCarWrapAngle));
    float return_goal;

    if (over_h_diff < normal_h_diff && over_h_diff < under_h_diff) {
        return_goal = goal + kSelectCarWrapAngle;
    } else if (under_h_diff < normal_h_diff && under_h_diff < over_h_diff) {
        return_goal = goal - kSelectCarWrapAngle;
    } else {
        return_goal = goal;
    }
    return return_goal;
}

void SelectCarCameraMover::CreateCameraMatrix(bMatrix4 *camera_matrix, SelectCarCameraData *camera_data) {
    bVector3 transpost(0.0f, 0.0f, camera_data->Radius);
    bMatrix4 camera_to_world;
    bVector3 eye;
    bVector3 up;

    bIdentity(camera_matrix);
    eRotateZ(camera_matrix, camera_matrix, bDegToAng(camera_data->OrbitHAngle));
    eRotateX(camera_matrix, camera_matrix, bDegToAng(camera_data->OrbitVAngle));
    eTranslate(camera_matrix, camera_matrix, &transpost);
    eInvertTransformationMatrix(&camera_to_world, camera_matrix);
    eye.x = camera_to_world.v3.x;
    eye.y = camera_to_world.v3.y;
    eye.z = camera_to_world.v3.z;
    ComputeBankedUpVector(&up, &eye, &camera_data->LookAt, bDegToAng(camera_data->RollAngle));
    eCreateLookAtMatrix(camera_matrix, eye, camera_data->LookAt, up);
}
