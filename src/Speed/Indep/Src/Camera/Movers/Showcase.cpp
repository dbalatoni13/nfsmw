#include "Speed/Indep/Src/Camera/Movers/Showcase.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"

static float gPhoto_LatAng = 90.0f;
static float gPhoto_UpAng = 2.0f;
static float gPhoto_Dist = 5.0f;
static float gPhoto_DOF = 0.0f;
static bVector3 gPhoto_CarPosBias;

ShowcaseCameraMover::~ShowcaseCameraMover() {}

void ShowcaseCameraMover::ResetState() {
    GetCamera()->ClearVelocity();
}

ShowcaseCameraMover::ShowcaseCameraMover(int nView, CameraAnchor *p_car, bool flipSide)
    : CameraMover(nView, CM_SHOWCASE) {
    pCar = p_car;
    GetCamera()->ClearVelocity();
    SetFromTweakables();
    if (flipSide) {
        mLatAng = -mLatAng;
    }
    BuildPhotoCameraMatrix();
}

void ShowcaseCameraMover::SetFromTweakables() {
    mLatAng = gPhoto_LatAng;
    mUpAng = gPhoto_UpAng;
    mDist = gPhoto_Dist;
    mCarPosBias = gPhoto_CarPosBias;
    mFOV = gPhoto_LatAng;
    mFd = gPhoto_DOF;
    mDOF = gPhoto_DOF;
}

void ShowcaseCameraMover::SetShowcaseCameraParams(float lat_ang, float up_ang, float dist, float carpos_bias_x, float carpos_bias_y,
                                                   float carpos_bias_z, float fov, float fd, float dof) {
    mLatAng = lat_ang;
    mUpAng = up_ang;
    mDist = dist;
    mCarPosBias.x = carpos_bias_x;
    mCarPosBias.y = carpos_bias_y;
    mCarPosBias.z = carpos_bias_z;
    mFOV = fov;
    mFd = fd;
    mDOF = dof;
}

void ShowcaseCameraMover::BuildPhotoCameraMatrix() {
    bVector3 *car_position = pCar->GetGeometryPosition();
    bVector3 car_adj_position;
    bVector3 car_bias;
    bMatrix4 car_to_world;

    bCopy(&car_to_world, pCar->GetGeometryOrientation());

    bVector3 ground_normal(0.0f, 0.0f, 1.0f);
    bCopy(&car_to_world.v2, &ground_normal, 0.0f);
    car_to_world.v0.z = 0.0f;
    car_to_world.v1.z = 0.0f;

    bCross(reinterpret_cast<bVector3 *>(&car_to_world.v0), reinterpret_cast<bVector3 *>(&car_to_world.v1), reinterpret_cast<bVector3 *>(&car_to_world.v2));
    bCross(reinterpret_cast<bVector3 *>(&car_to_world.v1), reinterpret_cast<bVector3 *>(&car_to_world.v2), reinterpret_cast<bVector3 *>(&car_to_world.v0));

    bNormalize(&car_to_world.v0, &car_to_world.v0);
    bNormalize(&car_to_world.v1, &car_to_world.v1);

    bMulMatrix(&car_bias, &car_to_world, &mCarPosBias);

    bAdd(&car_adj_position, car_position, &car_bias);

    unsigned short lat_view_angle = bDegToAng(mLatAng);
    float lat_sin, lat_cos;
    bSinCos(&lat_sin, &lat_cos, lat_view_angle);

    if (mUpAng < 3.0f) {
        mUpAng = 3.0f;
    }

    unsigned short up_view_angle = bDegToAng(mUpAng);
    float up_sin, up_cos;
    bSinCos(&up_sin, &up_cos, up_view_angle);

    float view_dist = mDist;
    bVector3 camera_position;
    bVector3 yada(lat_cos * view_dist, lat_sin * view_dist, up_sin * view_dist);
    bMulMatrix(&camera_position, &car_to_world, &yada);

    bAdd(&camera_position, &camera_position, &car_adj_position);

    bMatrix4 matrix;
    bVector3 ref_up_vec(0.0f, 0.0f, 1.0f);
    eCreateLookAtMatrix(&matrix, camera_position, car_adj_position, ref_up_vec);

    MinGapTopology(&matrix, pCar->GetGeometryPosition());

    mCameraMatrix = matrix;
}

void ShowcaseCameraMover::Update(float dT) {
    BuildPhotoCameraMatrix();
    unsigned short fov = bDegToAng(mFOV);
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetFieldOfView(fov);
    }
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetDepthOfField(mDOF);
    }
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetFocalDistance(mFd);
    }
    GetCamera()->SetCameraMatrix(mCameraMatrix, dT);
}
