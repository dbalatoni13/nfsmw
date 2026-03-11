#include "Speed/Indep/Src/Camera/Movers/Showcase.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"

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
    // TODO
}

void ShowcaseCameraMover::BuildPhotoCameraMatrix() {
    // TODO
}

void ShowcaseCameraMover::Update(float dT) {
    BuildPhotoCameraMatrix();
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetFieldOfView(bDegToAng(mFOV));
    }
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetDepthOfField(mDOF);
    }
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetFocalDistance(mFd);
    }
    GetCamera()->SetCameraMatrix(mCameraMatrix, dT);
}
