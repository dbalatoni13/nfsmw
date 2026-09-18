#ifndef CAMERA_MOVERS_TRACKCAR_H
#define CAMERA_MOVERS_TRACKCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Misc/Spline.hpp"

// total size: 0xD8
class TrackCarCameraMover : public CameraMover {
  public:
    TrackCarCameraMover(int nView, CameraAnchor *pCar, bool focus_effects);

    ~TrackCarCameraMover() override;

    void Init();

    bVector3 *GetTarget() override;

    void Update(float dT) override;

    CameraAnchor *GetAnchor() override {
        return CarToFollow;
    }

  private:
    bVector3 Eye;              // offset 0x80, size: 0x10
    bVector3 Look;             // offset 0x90, size: 0x10
    CameraAnchor *CarToFollow; // offset 0xA0, size: 0x4
    tCubic1D FocalDistCubic;   // offset 0xA4, size: 0x2C
    int FocusEffects;          // offset 0xD0, size: 0x4
    int CameraType;            // offset 0xD4, size: 0x4
};

#endif
