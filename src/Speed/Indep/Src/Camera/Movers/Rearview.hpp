#ifndef CAMERA_MOVERS_REARVIEW_H
#define CAMERA_MOVERS_REARVIEW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraMover.hpp"

extern float RVMnearz;  // size: 0x4, address: 0x8041703C
extern float RVMfarz;   // size: 0x4, address: 0x80417040

// total size: 0x84
class RearViewMirrorCameraMover : public CameraMover {
  public:
    RearViewMirrorCameraMover(int view_id, CameraAnchor *car);

    ~RearViewMirrorCameraMover() override;

    void Update(float dT) override;

    CameraAnchor *GetAnchor() override {
        return pCar;
    }

  private:
    CameraAnchor *pCar; // offset 0x80, size: 0x4
};

#endif
