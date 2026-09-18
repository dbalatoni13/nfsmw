#include "Speed/Indep/Src/Camera/Movers/SelectCar.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"

extern bool CarGuysCamera;

// The original TU uses a 7-digit pi literal (0x40490FDA), not bMath's PI (M_PI -> 0x40490FDB).
static const float kSelectCarPi = 3.1415926f;

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
    Damping = 10.0f;
    Periods = 2;
}

SelectCarCameraMover::~SelectCarCameraMover() {}

void SelectCarCameraMover::Update(float dT) {
    SelectCarCameraData *camera_data = &CurrentCameraData;

    if (ControlMode != 2) {
        float animiation_amount = 1.0f;

        CurrentAnimationTime += dT;

        if (TotalAnimationTime > 0.0f && CurrentAnimationTime < TotalAnimationTime) {
            animiation_amount = CurrentAnimationTime / TotalAnimationTime;
        }

        float aa2 = animiation_amount * animiation_amount;
        float anim = 1.0f - expf(-Damping * aa2) * cosf((Periods + 0.5f) * aa2 * kSelectCarPi);

        if (ControlMode == 1) {
            float the_frame_rate = dT * 60.0f;

            CurrentCameraData.OrbitHAngle = OrbitHSpeed * the_frame_rate + CurrentCameraData.OrbitHAngle;

            float possibleNewOrbitV = OrbitVSpeed * the_frame_rate + CurrentCameraData.OrbitVAngle;

            if (!CarGuysCamera) {
                if (possibleNewOrbitV > 110.0f) {
                    possibleNewOrbitV = 110.0f;
                } else if (possibleNewOrbitV < 94.5f) {
                    possibleNewOrbitV = 94.5f;
                }
            }

            CurrentCameraData.OrbitVAngle = possibleNewOrbitV;

            float zoom_rate = the_frame_rate * 0.125f;
            float possibleNewRadius = RadiusSpeed * zoom_rate + CurrentCameraData.Radius;

            if (!CarGuysCamera) {
                if (possibleNewRadius > 6.65f) {
                    possibleNewRadius = 6.65f;
                } else if (possibleNewRadius < 4.65f) {
                    possibleNewRadius = 4.65f;
                }
            }

            CurrentCameraData.Radius = possibleNewRadius;

            CurrentCameraData.RollAngle =
                (1.0f - anim) * StartAnimCameraData.RollAngle + anim * GoalAnimCameraData.RollAngle;
            CurrentCameraData.FOV = (1.0f - anim) * StartAnimCameraData.FOV + anim * GoalAnimCameraData.FOV;

            bVector3 lookat_change = GoalAnimCameraData.LookAt - StartAnimCameraData.LookAt;
            lookat_change *= anim;
            CurrentCameraData.LookAt = StartAnimCameraData.LookAt + lookat_change;

            if (CurrentCameraData.OrbitHAngle > 360.0f) {
                CurrentCameraData.OrbitHAngle -= 360.0f;
            }
            if (CurrentCameraData.OrbitHAngle < 0.0f) {
                CurrentCameraData.OrbitHAngle += 360.0f;
            }
        } else if (ControlMode == 0) {
            CurrentCameraData.OrbitVAngle =
                (1.0f - anim) * StartAnimCameraData.OrbitVAngle + anim * GoalAnimCameraData.OrbitVAngle;
            CurrentCameraData.OrbitHAngle =
                (1.0f - anim) * StartAnimCameraData.OrbitHAngle + anim * GoalAnimCameraData.OrbitHAngle;
            CurrentCameraData.Radius = (1.0f - anim) * StartAnimCameraData.Radius + anim * GoalAnimCameraData.Radius;
            CurrentCameraData.RollAngle =
                (1.0f - anim) * StartAnimCameraData.RollAngle + anim * GoalAnimCameraData.RollAngle;
            CurrentCameraData.FOV = (1.0f - anim) * StartAnimCameraData.FOV + anim * GoalAnimCameraData.FOV;

            bVector3 lookat_change = GoalAnimCameraData.LookAt - StartAnimCameraData.LookAt;
            lookat_change *= anim;
            CurrentCameraData.LookAt = StartAnimCameraData.LookAt + lookat_change;

            if (CurrentCameraData.OrbitHAngle > 360.0f) {
                CurrentCameraData.OrbitHAngle -= 360.0f;
            }
            if (CurrentCameraData.OrbitHAngle < 0.0f) {
                CurrentCameraData.OrbitHAngle += 360.0f;
            }
            if (CurrentCameraData.RollAngle > 360.0f) {
                CurrentCameraData.RollAngle -= 360.0f;
            }
            if (CurrentCameraData.RollAngle < 0.0f) {
                CurrentCameraData.RollAngle += 360.0f;
            }

            if (ControlMode == 0 && animiation_amount >= 1.0f) {
                ControlMode = 2;
            }
        }
    }

    bMatrix4 camera_matrix;

    CreateCameraMatrix(&camera_matrix, camera_data);

    pCamera->SetFieldOfView(bDegToAng(camera_data->FOV));
    pCamera->SetTargetDistance(camera_data->Radius);

    pCamera->SetCameraMatrix(camera_matrix, dT);
}

void SelectCarCameraMover::CreateCameraMatrix(bMatrix4 *camera_matrix, SelectCarCameraData *camera_data) {
    bVector3 orbit_offset;
    bMatrix4 inverse_matrix;
    bVector3 eye;
    bVector3 up;

    eIdentity(camera_matrix);

    eRotateZ(camera_matrix, camera_matrix, bDegToAng(camera_data->OrbitHAngle));
    eRotateX(camera_matrix, camera_matrix, bDegToAng(camera_data->OrbitVAngle));

    bFill(&orbit_offset, 0.0f, 0.0f, 0.0f);
    orbit_offset.z += camera_data->Radius;

    eTranslate(camera_matrix, camera_matrix, &orbit_offset);

    eInvertTransformationMatrix(&inverse_matrix, camera_matrix);

    eye.x = inverse_matrix.v3.x;
    eye.y = inverse_matrix.v3.y;
    eye.z = inverse_matrix.v3.z;

    ComputeBankedUpVector(&up, &eye, &camera_data->LookAt, bDegToAng(camera_data->RollAngle));

    eCreateLookAtMatrix(camera_matrix, eye, camera_data->LookAt, up);
}

void SelectCarCameraMover::SetVRotateSpeed(float f) {
    if (ControlMode != 0) {
        if (ControlMode != 1) {
            StartAnimCameraData.RollAngle = CurrentCameraData.RollAngle;
            GoalAnimCameraData.RollAngle = 0.0f;

            StartAnimCameraData.FOV = CurrentCameraData.FOV;
            GoalAnimCameraData.FOV = 45.0f;

            StartAnimCameraData.LookAt.x = CurrentCameraData.LookAt.x;
            StartAnimCameraData.LookAt.y = CurrentCameraData.LookAt.y;
            StartAnimCameraData.LookAt.z = CurrentCameraData.LookAt.z;
            GoalAnimCameraData.LookAt.x = 0.0f;
            GoalAnimCameraData.LookAt.y = 0.0f;
            GoalAnimCameraData.LookAt.z = 0.75f;

            GoalAnimCameraData.RollAngle = FindBestAngleGoal(StartAnimCameraData.RollAngle, GoalAnimCameraData.RollAngle);

            CurrentAnimationTime = 0.0f;
            TotalAnimationTime = 0.555f;
        }

        OrbitVSpeed = f;
        ControlMode = 1;
    }
}

void SelectCarCameraMover::SetHRotateSpeed(float f) {
    if (ControlMode != 0) {
        if (ControlMode != 1) {
            StartAnimCameraData.RollAngle = CurrentCameraData.RollAngle;
            GoalAnimCameraData.RollAngle = 0.0f;

            StartAnimCameraData.FOV = CurrentCameraData.FOV;
            GoalAnimCameraData.FOV = 45.0f;

            StartAnimCameraData.LookAt.x = CurrentCameraData.LookAt.x;
            StartAnimCameraData.LookAt.y = CurrentCameraData.LookAt.y;
            StartAnimCameraData.LookAt.z = CurrentCameraData.LookAt.z;
            GoalAnimCameraData.LookAt.x = 0.0f;
            GoalAnimCameraData.LookAt.y = 0.0f;
            GoalAnimCameraData.LookAt.z = 0.75f;

            GoalAnimCameraData.RollAngle = FindBestAngleGoal(StartAnimCameraData.RollAngle, GoalAnimCameraData.RollAngle);

            CurrentAnimationTime = 0.0f;
            TotalAnimationTime = 0.555f;
        }

        OrbitHSpeed = f;
        ControlMode = 1;
    }
}

void SelectCarCameraMover::SetZoomSpeed(float f) {
    if (ControlMode != 0) {
        if (ControlMode != 1) {
            StartAnimCameraData.RollAngle = CurrentCameraData.RollAngle;
            GoalAnimCameraData.RollAngle = 0.0f;

            StartAnimCameraData.FOV = CurrentCameraData.FOV;
            GoalAnimCameraData.FOV = 45.0f;

            StartAnimCameraData.LookAt.x = CurrentCameraData.LookAt.x;
            StartAnimCameraData.LookAt.y = CurrentCameraData.LookAt.y;
            StartAnimCameraData.LookAt.z = CurrentCameraData.LookAt.z;
            GoalAnimCameraData.LookAt.x = 0.0f;
            GoalAnimCameraData.LookAt.y = 0.0f;
            GoalAnimCameraData.LookAt.z = 0.75f;

            GoalAnimCameraData.RollAngle = FindBestAngleGoal(StartAnimCameraData.RollAngle, GoalAnimCameraData.RollAngle);

            CurrentAnimationTime = 0.0f;
            TotalAnimationTime = 0.555f;
        }

        RadiusSpeed = f;
        ControlMode = 1;
    }
}

void SelectCarCameraMover::SetCurrentOrientation(bVector3 &orbit, float roll, float fov, bVector3 &lookAt) {
    CurrentCameraData.OrbitVAngle = orbit.x;
    CurrentCameraData.OrbitHAngle = orbit.y;
    CurrentCameraData.Radius = orbit.z;
    CurrentCameraData.RollAngle = roll;
    CurrentCameraData.FOV = fov;
    CurrentCameraData.LookAt = lookAt;
}

void SelectCarCameraMover::SetDesiredOrientation(bVector3 &orbit, float roll, float fov, bVector3 &lookAt, float animSpeed,
                                                 float damping, int periods) {
    ControlMode = 0;

    StartAnimCameraData = CurrentCameraData;
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
    float over_h_diff = bAbs(start - (goal + 360.0f));
    float under_h_diff = bAbs(start - (goal - 360.0f));
    float return_goal;

    if (over_h_diff < normal_h_diff && over_h_diff < under_h_diff) {
        return_goal = goal + 360.0f;
        return return_goal;
    }

    if (under_h_diff < normal_h_diff && under_h_diff < over_h_diff) {
        under_h_diff = 360.0f;
        return_goal = goal - under_h_diff;
        return return_goal;
    }

    return_goal = goal;
    return return_goal;
}
