#ifndef CAMERA_MOVERS_TRACKCOP_H
#define CAMERA_MOVERS_TRACKCOP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Misc/Spline.hpp"

// total size: 0x44
class Bezier {
  public:
    Bezier();

    ~Bezier() {}

    void SetControlPoints(bMatrix4 *pPoints) {
        pControlPoints = pPoints;
    }

    void GetPoint(bVector3 *pOut, float t);

    bMatrix4 *pControlPoints; // offset 0x0, size: 0x4
    bMatrix4 mBasis;          // offset 0x4, size: 0x40
};

// total size: 0x250
class TrackCopCameraMover : public CameraMover {
  public:
    TrackCopCameraMover(int nView, CameraAnchor *pCar, bool focus_effects);

    ~TrackCopCameraMover() override;

    bool FindPursuitVehiclePosition(bVector3 *copPos);

    void Init();

    bVector3 *GetTarget() override;

    void Update(float dT) override;

    CameraAnchor *GetAnchor() override {
        return CarToFollow;
    }

    int RenderCarPOV() override {
        return bRenderCarPOV;
    }

    void SetRenderCarPOV(int render) {
        bRenderCarPOV = render;
    }

  protected:
    Bezier ZoomSpline;         // offset 0x80, size: 0x44
    bMatrix4 ZoomVerts;        // offset 0xC4, size: 0x40
    float ZoomSplineParam;     // offset 0x104, size: 0x4
    Bezier EyeSpline;          // offset 0x108, size: 0x44
    bMatrix4 EyeVerts;         // offset 0x14C, size: 0x40
    float EyeSplineParam;      // offset 0x18C, size: 0x4
    Bezier LookSpline;         // offset 0x190, size: 0x44
    bMatrix4 LookVerts;        // offset 0x1D4, size: 0x40
    float LookSplineParam;     // offset 0x214, size: 0x4
    CameraAnchor *CarToFollow; // offset 0x218, size: 0x4
    tCubic1D FocalDistCubic;   // offset 0x21C, size: 0x2C
    int FocusEffects;          // offset 0x248, size: 0x4
    int bRenderCarPOV;         // offset 0x24C, size: 0x4
};

#endif
