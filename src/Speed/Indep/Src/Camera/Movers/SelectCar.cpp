#include "Speed/Indep/Src/Camera/Movers/SelectCar.hpp"

SelectCarCameraMover::~SelectCarCameraMover() {}

void SelectCarCameraMover::SetVRotateSpeed(float f) {
    OrbitVSpeed = f;
}

void SelectCarCameraMover::SetHRotateSpeed(float f) {
    OrbitHSpeed = f;
}

void SelectCarCameraMover::SetZoomSpeed(float f) {
    RadiusSpeed = f;
}

SelectCarCameraMover::SelectCarCameraMover(int view_id) : CameraMover(view_id) {
    // TODO
}

void SelectCarCameraMover::Update(float dT) {
    // TODO
}

void SelectCarCameraMover::SetCurrentOrientation(bVector3 &orbit, float roll, float fov, bVector3 &lookAt) {
    // TODO
}

void SelectCarCameraMover::SetDesiredOrientation(bVector3 &orbit, float roll, float fov, bVector3 &lookAt, float animSpeed, float damping,
                                                  int periods) {
    // TODO
}

float SelectCarCameraMover::FindBestAngleGoal(float start, float goal) {
    // TODO
    return 0.0f;
}

void SelectCarCameraMover::CreateCameraMatrix(bMatrix4 *camera_matrix, SelectCarCameraData *camera_data) {
    // TODO
}
