#include "Speed/Indep/Src/Camera/Movers/Showcase.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

static const bVector3 gPhoto_CarPosBias(2.0f, 0.0f, 0.0f); // size: 0x10, address: 0x8045AF68

ShowcaseCameraMover::ShowcaseCameraMover(int nView, CameraAnchor *p_car, bool flipSide)
    : CameraMover(nView, CM_SHOWCASE) {
    pCar = p_car;

    pCamera->ClearVelocity();

    SetFromTweakables();

    if (flipSide) {
        mLatAng = -mLatAng;
    }

    BuildPhotoCameraMatrix();
}

ShowcaseCameraMover::~ShowcaseCameraMover() {}

void ShowcaseCameraMover::SetFromTweakables() {
    mLatAng = 90.0f;
    mUpAng = 10.0f;
    mDist = 5.0f;
    mCarPosBias.x = gPhoto_CarPosBias.x;
    mCarPosBias.y = gPhoto_CarPosBias.y;
    mCarPosBias.z = gPhoto_CarPosBias.z;
    mFOV = 90.0f;
    mFd = 0.0f;
    mDOF = 0.0f;
}

void ShowcaseCameraMover::BuildPhotoCameraMatrix() {
    bVector3 *car_position = pCar->GetGeometryPosition();
    bVector3 car_adj_position;
    bVector3 car_bias;
    bMatrix4 car_to_world;
    bVector3 ground_normal;
    float lat_sin;
    float lat_cos;
    float up_sin;
    float up_cos;
    bVector3 camera_position;
    bVector3 yada;
    bMatrix4 matrix;
    bVector3 ref_up_vec;

    car_to_world = *pCar->GetGeometryOrientation();

    bFill(&ground_normal, 0.0f, 0.0f, 1.0f);
    bCopy(&car_to_world.v2, &ground_normal, 0.0f);
    car_to_world.v0.z = 0.0f;
    car_to_world.v1.z = 0.0f;

    bCross((bVector3 *)&car_to_world.v0, (const bVector3 *)&car_to_world.v1, (const bVector3 *)&car_to_world.v2);
    bCross((bVector3 *)&car_to_world.v1, (const bVector3 *)&car_to_world.v2, (const bVector3 *)&car_to_world.v0);
    bNormalize(&car_to_world.v0, &car_to_world.v0);
    bNormalize(&car_to_world.v1, &car_to_world.v1);

    bMulMatrix(&car_bias, &car_to_world, (const bVector3 *)&mCarPosBias);

    bAdd(&car_adj_position, car_position, &car_bias);

    bSinCos(&lat_sin, &lat_cos, bDegToAng(mLatAng));
    if (mUpAng < 3.0f) {
        mUpAng = 3.0f;
    }
    bSinCos(&up_sin, &up_cos, bDegToAng(mUpAng));

    float view_dist = mDist;

    yada.x = lat_cos * view_dist;
    yada.y = lat_sin * view_dist;
    yada.z = up_sin * view_dist;

    bMulMatrix(&camera_position, &car_to_world, &yada);

    bAdd(&camera_position, &camera_position, &car_adj_position);

    bFill(&ref_up_vec, 0.0f, 0.0f, 1.0f);

    eCreateLookAtMatrix(&matrix, camera_position, car_adj_position, ref_up_vec);

    MinGapTopology(&matrix, pCar->GetGeometryPosition());

    mCameraMatrix = matrix;
}

void ShowcaseCameraMover::ResetState() {
    pCamera->ClearVelocity();
}

void ShowcaseCameraMover::Update(float dT) {
    BuildPhotoCameraMatrix();

    pCamera->SetFieldOfView(bDegToAng(mFOV));
    pCamera->SetDepthOfField(mDOF);
    pCamera->SetFocalDistance(mFd);

    pCamera->SetCameraMatrix(mCameraMatrix, dT);
}
