#include "Speed/Indep/Src/Camera/Movers/Showcase.hpp"

ShowcaseCameraMover::~ShowcaseCameraMover() {}

void ShowcaseCameraMover::ResetState() {
    // TODO - accesses ICEData members through pCar
}

ShowcaseCameraMover::ShowcaseCameraMover(int nView, CameraAnchor *p_car, bool flipSide)
    : CameraMover(nView) {
    // TODO
}

void ShowcaseCameraMover::SetFromTweakables() {
    // TODO
}

void ShowcaseCameraMover::SetShowcaseCameraParams(float lat_ang, float up_ang, float dist, float carpos_bias_x, float carpos_bias_y,
                                                   float carpos_bias_z, float fov, float fd, float dof) {
    // TODO
}

void ShowcaseCameraMover::BuildPhotoCameraMatrix() {
    // TODO
}

void ShowcaseCameraMover::Update(float dT) {
    // TODO
}
