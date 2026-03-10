#ifndef CAMERA_MOVERS_SHOWCASE_H
#define CAMERA_MOVERS_SHOWCASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraMover.hpp"

// total size: 0xEC
class ShowcaseCameraMover : public CameraMover {
  public:
    ShowcaseCameraMover(int nView, CameraAnchor *p_car, bool flipSide);
    ~ShowcaseCameraMover() override;

    void SetFromTweakables();
    void SetShowcaseCameraParams(float lat_ang, float up_ang, float dist, float carpos_bias_x, float carpos_bias_y, float carpos_bias_z, float fov, float fd, float dof);
    void BuildPhotoCameraMatrix();
    void ResetState() override;
    void Update(float dT) override;

  private:
    CameraAnchor *pCar;       // offset 0x80, size 0x4
    float mLatAng;            // offset 0x84, size 0x4
    float mUpAng;             // offset 0x88, size 0x4
    float mDist;              // offset 0x8C, size 0x4
    bVector3 mCarPosBias;     // offset 0x90, size 0x10
    float mFOV;               // offset 0xA0, size 0x4
    float mFd;                // offset 0xA4, size 0x4
    float mDOF;               // offset 0xA8, size 0x4
    bMatrix4 mCameraMatrix;   // offset 0xAC, size 0x40
};

#endif
